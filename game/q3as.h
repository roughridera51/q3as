/*
 *  Portions Copyright (C) 2003 Ben Goodwin
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the OSML - Open Source Modification License v1.0 as
 *  described in the file COPYING which is distributed with this source
 *  code.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "q3as_mode.h"

// defines

// Version MUST be in the format of #.# or #.#x
// IE 1.0, 1.0f, 2.3, 2.3a, 1.12, 1.12d, 12.1, etc ...
#define AS_VERSION "1.99q"
#define AS_WEBSITE "http://www.q3as.com/"
#define AS_NAME "Q3AS"
#define AS_MASTER "master0.q3as.com"

#define AS_MAX_MAPROT			1024
#define AS_MAX_MAP_TEMPLATES	40 + AS_TEMPLATE_USER	// 40 userdefined, plus a couple system
#define AS_MAX_MAPFILE_SIZE		30000

#define AS_MAX_BANFILE_SIZE		16384
#define AS_MAX_NOKICKFILE_SIZE	8192

#define AS_MAX_HELPFILE_SIZE	1024

#define	AS_MAX_MAP_ENTSTRING	0x8000
#define	AS_MAX_SPAWNVAR_GROUPS	4000	// mpteam8 has 1838

// For the 'getcwd' call I use ...
#ifdef _WIN32
#include <direct.h>
#elif defined __linux__
#include <unistd.h>
#endif

// typedefs

typedef struct {
	fileHandle_t		debugfile;
	int					voteType;
	char				mapname[MAX_QPATH];
	int					curSpawnVarGroup;
	qboolean			spawnVarGroupDisabled[AS_MAX_SPAWNVAR_GROUPS];
	qboolean			skipRefPrint;	// skip printing next G_Printf to referees?
	int					lastClientTime;	// last level.time we had a client on the server
	int					timeSortedClients[MAX_CLIENTS];
	qboolean			nobalance;
	int					fps;
} as_locals_t;

typedef struct {
	vmCvar_t			*vmCvar;
	char				*cvarName;
	char				*defaultString;
	int					cvarFlags;
	int					modificationCount;  // for tracking changes
	qboolean			trackChange;	    // track this variable, and announce if changed
} as_cvars_t;

typedef enum {
	AS_RT_SEQUENTIAL,
	AS_RT_RANDOM
} as_rotationType_t;

typedef enum {
	AS_MST_UNKNOWN,
	AS_MST_MAP,
	AS_MST_TEMPLATE,
} as_mapStanzaType_t;

// For loadedMask (below)
#define AS_MS_HAVEMAP		0x00000001
#define AS_MS_HAVEMIN		0x00000002
#define AS_MS_HAVEMAX		0x00000004
#define	AS_MS_HAVEDIS		0x00000008
#define AS_MS_HAVENOT		0x00000010
#define AS_MS_HAVEEXE		0x00000020
#define AS_MS_HAVEID		0x00000040
#define AS_MS_HAVEEIF		0x00000080
#define AS_MS_HAVETEM		0x00000100
#define AS_MS_HAVEDEF		0x00000200

#define AS_MAX_TEMPLATES_PER_MAP 5

// This isn't SAVED or USED in the game.  This is a stepping-stone to loading
// things from file into memory.  The real stuff is stored in as_mapRotation_t and
// as_mapTemplate_t
typedef struct {
	char				map[MAX_QPATH];				// Define a map stanza (bsp name)
	int					minplayers;					// min players needed to load this map
	int					maxplayers;					// max players this map can have
	qboolean			disabled;					// Remove from normal rotation?
	qboolean			notvoteable;				// Can clients vote this map into play?
	char				exec[MAX_QPATH];			// Exec this config before map loads
	char				id[32];						// For as_map_id command
	qboolean			execifvoted;				// Exec 'exec' (above) if this map is voted on?
	char				defTemplate[MAX_QPATH];		// Define a template stanza
	char				templates[AS_MAX_TEMPLATES_PER_MAP][MAX_QPATH]; // If MAX_QPATH is changed, change it in as_ParseMapRotationField()!
	int					loadedMask;					// AS_MS_HAVE* flags
} as_mapStanza_t;

typedef struct {
	int					lastRotationNum;			// Last numRotation this map was used
	char				map[MAX_QPATH];				// Map (bsp) name
	char				id[32];
	int					templateIDs[AS_MAX_TEMPLATES_PER_MAP];	// References array index of templates to use
	int					numTemplates;				// -1 = no templates, 0 = 1 template, 1 = 2 templates, etc ...
	qboolean			autoLoaded;					// Was this map loaded via the autoloader?
} as_map_t;

typedef struct {
	char				templateName[MAX_QPATH];	// Template name
	int					minplayers;					// min players needed to load map
	int					maxplayers;					// max players map can be loaded with
	int					disabled;					// disable auto-rotation of this map?
	int					notvoteable;				// disable voting this map into play?
	char				exec[MAX_QPATH];			// exec this config before loading map
	int					execifvoted;				// exec config if map is voted into play?
	int					loadedMask;					// AS_MS_HAVE* flags
} as_mapTemplate_t;

typedef enum {
	AS_TEMPLATE_AUTOLOAD,
	AS_TEMPLATE_DEFAULT,
	AS_TEMPLATE_USER,	// ADD ABOVE THIS LINE!!!
} as_mapTemplates_t;

typedef struct {
	as_map_t			maps[AS_MAX_MAPROT];				// starts at 0
	int					numMaps;							// starts at 0 !!!
	int					numTemplates;						// starts at 0 !!!
	int					currentMapNum;						// starts at 0
	int					numRotations;						// # of rotations the system has seen thus far
	as_mapTemplate_t	templates[AS_MAX_MAP_TEMPLATES];	// Used for storing templates/defaults
} as_mapRotation_t;

typedef enum {
	AS_MR_INT,							// int
	AS_MR_STRING,						// char[]
	AS_MR_BOOLEAN,						// qboolean
	AS_MR_ARRAYSTRINGS,					// char[][]
} as_mapRotationFieldTypes_t;

#define	AS_FOFS(x) ((int)&(((as_mapStanza_t *)0)->x))	// field offset
#define	AS_SIZE(x) (sizeof(((as_mapStanza_t *)0)->x))	// field size

typedef enum {
	AS_MRE_NONE,				// No error
	AS_MRE_DONE,				// Done loading
	AS_MRE_FATAL,				// Fatal error
} as_mapRotationErrors_t;

typedef enum {
	AS_AL_NONE,					// Don't autoload anything
	AS_AL_NORMAL,				// Only autoload maps that have a corresponding arena file
	AS_AL_ALL,					// Load *all* maps (IE even those w/out arena files)
} as_mapRotationAutoLoad_t;

// vote type (used for as_vote_mask)
#define AS_VT_BAD			0x00000000
#define AS_VT_RESTARTMAP	0x00000001
#define AS_VT_NEXTMAP		0x00000002
#define AS_VT_MAP			0x00000004
#define AS_VT_GAMETYPE		0x00000008
#define AS_VT_KICK			0x00000010
#define AS_VT_DOWARMUP		0x00000020
#define AS_VT_CLIENTKICK	0x00000040
#define AS_VT_TEAM_LEADER	0x00000080
#define AS_VT_TIMELIMIT		0x00000100
#define AS_VT_FRAGLIMIT		0x00000200
#define AS_VT_CAPTURELIMIT	0x00000400
#define AS_VT_MODE			0x00000800
#define AS_VT_TOGGLEBALANCE	0x00001000

typedef struct {
	char		*string;
	int			type;
} as_string_map_t;

typedef enum {					// Team balancing - scramble players at start of each map
	AS_TB_NONE,					// no balancing
	AS_TB_SCORE,				// By score
	AS_TB_RANDOM,				// Random
	AS_TB_SCORETIME,			// by score / time ('frags'/second)
} as_tb_t;

// message type (used for as_log_mask and as_referee_print_mask)
#define AS_MT_SAY				0x00000001
#define AS_MT_SAYTEAM			0x00000002
#define AS_MT_TELL				0x00000004
#define AS_MT_VTELL				0x00000008
#define AS_MT_ITEM				0x00000010
#define AS_MT_VOICE				0x00000020
#define AS_MT_INFOCHANGED		0x00000040
#define AS_MT_CLIENTCONNECT		0x00000080
#define AS_MT_CLIENTBEGIN		0x00000100
#define AS_MT_CLIENTDISCONNECT	0x00000200
#define AS_MT_MISC				0x00000400

typedef struct {
	float				s_quadFactor;
	vec3_t				muzzle;
	vec3_t				forward;
	vec3_t				right;
	vec3_t				up;
	int					weaponNum;
	int					mod;
	int					splashMod;
} as_weaponArgs_t;

// Globals
extern	as_mapRotation_t	rotation;
extern	as_locals_t			as_level;
extern	as_cvars_t			as_cv;
extern	char				spawnVars[];

extern	as_modeInfo_t		as_mode;
extern	as_allModes_t		as_modes;

// CVARS

// From g_main.c
extern	vmCvar_t	g_log;
extern	vmCvar_t	g_logSync;

// From q3as.c
extern	vmCvar_t	as_version;

extern	vmCvar_t	as_debug_file;
extern	vmCvar_t	as_debug_console;

extern	vmCvar_t	as_vote_connected_percent;
extern	vmCvar_t	as_vote_mask;
extern	vmCvar_t	as_vote_gametype_mask;
extern	vmCvar_t	as_vote_mintimelimit;
extern	vmCvar_t	as_vote_maxtimelimit;
extern	vmCvar_t	as_vote_minfraglimit;
extern	vmCvar_t	as_vote_maxfraglimit;
extern	vmCvar_t	as_vote_mincapturelimit;
extern	vmCvar_t	as_vote_maxcapturelimit;
extern	vmCvar_t	as_vote_maxcall;
extern	vmCvar_t	as_vote_mode_mask;

extern	vmCvar_t	as_maprotation_file;
extern	vmCvar_t	as_maprotation_state_file;
extern	vmCvar_t	as_maprotation_vote_preserve;
extern	vmCvar_t	as_maprotation_type;
extern	vmCvar_t	as_maprotation_spacing;
extern	vmCvar_t	as_maprotation_autoload;
extern	vmCvar_t	as_maprotation_gametype_mask;

extern	vmCvar_t	as_adminlog_file;
extern	vmCvar_t	as_ban_file;
extern	vmCvar_t	as_nokick_file;

extern	vmCvar_t	as_report_to_master;

extern	vmCvar_t	as_referee_password;
extern	vmCvar_t	as_referee_print_mask;

extern	vmCvar_t	as_log_mask;
extern	vmCvar_t	as_log_timetick;
extern	vmCvar_t	as_log_tick_format;
extern	vmCvar_t	as_log_rotate_interval;
extern	vmCvar_t	as_log_rotate_format;
extern	vmCvar_t	as_log_rotate_directory;
extern	vmCvar_t	as_log_osp;

extern	vmCvar_t	as_max_intermission;
extern	vmCvar_t	as_time_during_warmup;

extern	vmCvar_t	as_kick_spectator_bots;

extern	vmCvar_t	as_motd_spectator_duration;
extern	vmCvar_t	as_motd_firstspawn_duration;
extern	vmCvar_t	as_motd_1;
extern	vmCvar_t	as_motd_2;
extern	vmCvar_t	as_motd_3;
extern	vmCvar_t	as_motd_4;
extern	vmCvar_t	as_motd_5;

extern	vmCvar_t	as_team_balance;

extern	vmCvar_t	as_spam_warn;
extern	vmCvar_t	as_spam_kick;
extern	vmCvar_t	as_spam_interval;
extern	vmCvar_t	as_spam_message;

extern	vmCvar_t	as_mode_number;
extern	vmCvar_t	as_mode_file;

extern	vmCvar_t	as_rollback_config;
extern	vmCvar_t	as_rollback_timeout;

extern	vmCvar_t	as_chat_filter_file;

extern	vmCvar_t	as_gamestartup_format;

extern	vmCvar_t	as_unbalanced_warning;

extern	vmCvar_t	as_max_spectator_time;
extern	vmCvar_t	as_max_spectator_warn;

extern	vmCvar_t	as_preserve_gamename;

extern	vmCvar_t	as_headhunter;

// Prototypes

int trap_RealTime( qtime_t *qtime );	// from g_syscalls

void as_debug(char *fmt, ... );
void as_registerCvars( void );
void as_updateCvars( void );
void as_postSpawnCleanup( void );
void as_stripIPPort (char *ip, char *newIP, int len);
qboolean as_isBanned (char *ip);
qboolean as_admin_log(char *fmt, ... );
void as_callVote_f( gentity_t *ent );
void as_callTeamVote_f( gentity_t *ent );
void as_svcmd_AS_Map_f( void );
void as_svcmd_AS_ReloadMapConfig_f( void );
void as_svcmd_Mute_f( void );
void as_svcmd_AddIP_f( void );
void as_svcmd_RemoveIP_f( void );
void as_svcmd_AS_MapID_f( void );
void as_openDebugLog( void );
void as_openDebugLog( void );
void as_serverNextmap( void );
void as_postInitInit( void );
qboolean as_checkToken( char *token );
void as_cmd_Help_f( gentity_t *ent );
void as_cmd_SetReferee_f( gentity_t *ent );
void as_cmd_Referee_f( gentity_t *ent );
void as_sendMessageToReferees(char *text);
void as_svcmd_status_f( void );
void as_printMOTD (gentity_t *ent, int duration);
void as_doLingerPrint (gentity_t *ent);
void as_balance_teams( void );
qboolean as_readExternalSVFile( void );
qboolean as_parseSpawnVars( qboolean getVersion );
void as_cmd_Players_f( gentity_t *ent );
void as_trackChat (int clientNum);
void as_checkConfigTimeout(void);
void as_setClientIP(int clientNum, qboolean init);
char *as_filterText(char *text);
qboolean as_canJoinTeam(gentity_t *ent, int team);
int QDECL as_sortTimeScore( const void *a, const void *b );
void as_updateChatStats(gentity_t *ent);
void as_cmd_Follow_FC_f( gentity_t *ent, int team );
void as_formatTime(char *format, char *out, int sizeOut, qtime_t t);
void as_logTick(void);
void as_checkTeams(void);
void as_checkLogRotation(void);
void as_logOSP(char *fmt, ... );
void as_clientSpectatorTimer( gclient_t *client );
int as_percentConnected (void);
void as_translateMOD(int *mod);

// q3as_map.c
qboolean as_isMapAllowed(int mapNum, qboolean voted);
int as_numForMap(char *map);
qboolean as_execMapByName(char *map, qboolean voted);
qboolean as_execMapByID(char *ID);
void as_loadQ3ASMapRotation( qboolean reload );
void as_nextMap(void);
void as_execScheduledMap(void);

// q3as_mode.c
void as_initMode(int mode);
qboolean as_itemDisabled (gitem_t *item);
gitem_t	*as_swapItem(gitem_t *item);
void as_modifyItem(gentity_t *ent, gitem_t *item);
void as_giveStartWeapons(gclient_t *client);
void as_giveStartOther(gentity_t *ent);
void as_setWRCS( void );
int as_getWeaponRate(int weapon);
qboolean as_shouldDecayHealth(gentity_t *ent);
qboolean as_shouldDecayArmor(gentity_t *ent);
void as_regenVitals(gentity_t *ent);
void as_regenAmmo(gentity_t *ent);
void as_modifyRespawn(gentity_t *ent, int *respawn);
void as_getVoteableModeList(char *buffer, int size);

// q3as_weapon.c
void as_fireWeapon( gentity_t *ent );
void as_runExplodeFunction(gentity_t *ent);

