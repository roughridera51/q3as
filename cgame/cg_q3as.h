// Defines
#define NUM_TEAM_JERSEYS	35
#define EUI_VERSION			"1.0d"

#define	LAG_SAMPLES			128
#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

// typedef's
typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} as_cvars_t;

typedef enum {
	NOPICMIP_LG,	//    1
	NOPICMIP_PG,	//    2
	NOPICMIP_RL,	//    4
	NOPICMIP_GL,	//    8
	NOPICMIP_MG,	//   16
	NOPICMIP_RG,	//   32
	NOPICMIP_BFG,	//   64
	NOPICMIP_BLOOD,	//  128
	NOPICMIP_SMOKE,	//  256
	NOPICMIP_PU,	//  512
/*
	NOPICMIP_PL,	//  512
	NOPICMIP_NG,	// 1024
*/

	NOMIP_TOTAL
} nopicmip_t;

typedef enum {
	NOTRAIL_RL,		//  1
	NOTRAIL_GL,		//  2
	NOTRAIL_PL,		//  4
	NOTRAIL_NG,		//  8
	NOTRAIL_H20,	// 16

	NOTRAIL_TOTAL
} notrail_t;

typedef struct {
	int		frameSamples[LAG_SAMPLES];
	int		frameCount;
	int		snapshotFlags[LAG_SAMPLES];
	int		snapshotSamples[LAG_SAMPLES];
	int		snapshotCount;
} lagometer_t;

// globals from cg_q3as.c
extern	as_cvars_t	as_cv;
extern	lagometer_t	lagometer;

// CVARS
extern	vmCvar_t		sta_gJerseys;
extern	vmCvar_t		sta_gSpectatorJerseys;
extern	vmCvar_t		sta_hSpectatorOverlay;
extern	vmCvar_t		sta_hSpectatorMessage;
extern	vmCvar_t		sta_hTimerCountDown;
extern	vmCvar_t		sta_hTimerPosition;
extern	vmCvar_t		sta_hFPSPosition;
extern	vmCvar_t		sta_hLagometerPosition;
extern	vmCvar_t		sta_hAttackerPosition;
extern	vmCvar_t		sta_hSpeedometerPosition;
extern	vmCvar_t		sta_hTeamOverlayPosition;
extern	vmCvar_t		sta_hTeamOverlayNameWidth;
extern	vmCvar_t		sta_hTeamOverlayLocationWidth;
extern	vmCvar_t		sta_hTeamOverlaySlots;
extern	vmCvar_t		sta_hTeamOverlayInfo;
extern	vmCvar_t		sta_hTeamOverlayCharHeight;
extern	vmCvar_t		sta_hTeamOverlayCharWidth;
extern	vmCvar_t		sta_hAmmo;
extern	vmCvar_t		sta_mJumpSound;
extern	vmCvar_t		sta_mFootsteps;
extern	vmCvar_t		sta_hCrosshairInfo;
extern	vmCvar_t		sta_gTeamColoredRails;
extern	vmCvar_t		sta_gTeamColoredPlasma;
extern	vmCvar_t		sta_gTeamColoredLightning;
extern	vmCvar_t		sta_hSpeedometerUnits;
extern	vmCvar_t		sta_hAutoTeamInfo;
extern	vmCvar_t		sta_hCrosshairColor;
extern	vmCvar_t		sta_gNoProjectileTrail;
extern	vmCvar_t		sta_gNoPicMip;
extern	vmCvar_t		sta_hMiscCharWidth;
extern	vmCvar_t		sta_hMiscCharHeight;
extern	vmCvar_t		sta_hChatMaxLines;
extern	vmCvar_t		sta_hChatMaxColumns;
extern	vmCvar_t		sta_hChatTime;
extern	vmCvar_t		sta_hChatNewColors;
extern	vmCvar_t		sta_hChatCharWidth;
extern	vmCvar_t		sta_hChatCharHeight;
extern	vmCvar_t		sta_gForceTeamModel;
extern	vmCvar_t		sta_gTeamModel;
extern	vmCvar_t		sta_gTeamHeadModel;
extern	vmCvar_t		sta_gForceEnemyModel;
extern	vmCvar_t		sta_gEnemyModel;
extern	vmCvar_t		sta_gEnemyHeadModel;

// cg_q3as.c
void as_registerCvars( void );
void as_updateCvars( void );
void as_drawLoadingIcons( int loadingPlayerIconCount, int loadingItemIconCount, qhandle_t *loadingPlayerIcons,
						 qhandle_t *loadingItemIcons);
char *as_removeQ3ASColors( char *string );
char *as_removeAllColors( char *string );
void as_printf( char *text, qboolean recursed );
void as_Echo_f (void);
void as_lightningBoltBeam( vec3_t start, vec3_t end, clientInfo_t *ci );
void as_parseHTMLColors( vec4_t color, const char *s );
void as_drawBigStringNoShadow( int x, int y, const char *s, float alpha );
// void as_drawSelectedPlayerNameLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle );
// void as_drawSelectedPlayerNumber( rectDef_t *rect, float scale, vec4_t color, int textStyle, qboolean draw2D );
void as_registerGraphics (void);
void as_drawUpperRight(void);
void as_drawLowerRight(void);
void as_drawLowerLeft(void);
void as_drawPrintBuffer(void);
void as_findFlagCarrier(void);
void as_drawCrosshair(void);
void as_drawCrosshairNames(void);
void as_drawSpectator(void);
qboolean as_drawFollow(void);
void as_setFeatureList(void);
void as_loadWeaponRates(void);
