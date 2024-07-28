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


#include "g_local.h"
#include "q3as.h"

// Globals
as_locals_t			as_level;
as_cvars_t			as_cv;

char			spawnVars[AS_MAX_MAP_ENTSTRING];

static as_string_map_t	as_vote_map[] =
{
	{ "map_restart",	AS_VT_RESTARTMAP },
	{ "nextmap",		AS_VT_NEXTMAP },
	{ "map",			AS_VT_MAP },
	{ "g_gametype",		AS_VT_GAMETYPE },
	{ "kick",			AS_VT_KICK },
	{ "clientkick",		AS_VT_CLIENTKICK },
	{ "g_doWarmup",		AS_VT_DOWARMUP },
	{ "timelimit",		AS_VT_TIMELIMIT },
	{ "fraglimit",		AS_VT_FRAGLIMIT },
	{ "capturelimit",	AS_VT_CAPTURELIMIT },
	{ "leader",			AS_VT_TEAM_LEADER },
	{ "mode",			AS_VT_MODE },
	{ "togglebalance",	AS_VT_TOGGLEBALANCE },
	{ NULL }
};

static as_string_map_t	as_mt_map[] =
{
	{ "say:",					AS_MT_SAY },
	{ "tell:",					AS_MT_TELL },
	{ "vtell:",					AS_MT_VTELL },
	{ "Item:",					AS_MT_ITEM },
	{ "voice:",					AS_MT_VOICE },
	{ "sayteam:",				AS_MT_SAYTEAM },
	{ "ClientUserinfoChanged:",	AS_MT_INFOCHANGED },
	{ "ClientConnect:",			AS_MT_CLIENTCONNECT },
	{ "ClientBegin:",			AS_MT_CLIENTBEGIN },
	{ "ClientDisconnect:",		AS_MT_CLIENTDISCONNECT },
	{ NULL }
};

// CVARS

vmCvar_t	as_version;

vmCvar_t	as_debug_console;
vmCvar_t	as_debug_file;

vmCvar_t	as_maprotation_file;
vmCvar_t	as_maprotation_state_file;
vmCvar_t	as_maprotation_vote_preserve;
vmCvar_t	as_maprotation_type;
vmCvar_t	as_maprotation_spacing;
vmCvar_t	as_maprotation_autoload;
vmCvar_t	as_maprotation_gametype_mask;

vmCvar_t	as_vote_connected_percent;
vmCvar_t	as_vote_mask;
vmCvar_t	as_vote_gametype_mask;
vmCvar_t	as_vote_mintimelimit;
vmCvar_t	as_vote_maxtimelimit;
vmCvar_t	as_vote_minfraglimit;
vmCvar_t	as_vote_maxfraglimit;
vmCvar_t	as_vote_mincapturelimit;
vmCvar_t	as_vote_maxcapturelimit;
vmCvar_t	as_vote_maxcall;
vmCvar_t	as_vote_mode_mask;

vmCvar_t	as_adminlog_file;

vmCvar_t	as_ban_file;

vmCvar_t	as_nokick_file;

vmCvar_t	as_report_to_master;

vmCvar_t	as_referee_password;
vmCvar_t	as_referee_print_mask;

vmCvar_t	as_log_mask;
vmCvar_t	as_log_tick_interval;
vmCvar_t	as_log_tick_format;
vmCvar_t	as_log_rotate_interval;
vmCvar_t	as_log_rotate_format;
vmCvar_t	as_log_rotate_directory;
vmCvar_t	as_log_osp;

vmCvar_t	as_max_intermission;
vmCvar_t	as_time_during_warmup;

vmCvar_t	as_kick_spectator_bots;

vmCvar_t	as_motd_rotations;
vmCvar_t	as_motd_spectator_duration;
vmCvar_t	as_motd_firstspawn_duration;
vmCvar_t	as_motd_1;
vmCvar_t	as_motd_2;
vmCvar_t	as_motd_3;
vmCvar_t	as_motd_4;
vmCvar_t	as_motd_5;

vmCvar_t	as_team_balance;

vmCvar_t	as_spam_warn;
vmCvar_t	as_spam_kick;
vmCvar_t	as_spam_interval;
vmCvar_t	as_spam_message;

vmCvar_t	as_mode_number;
vmCvar_t	as_mode_file;

vmCvar_t	as_rollback_config;
vmCvar_t	as_rollback_timeout;

vmCvar_t	as_chat_filter_file;

vmCvar_t	as_gamestartup_format;

vmCvar_t	as_unbalanced_warning;

vmCvar_t	as_max_spectator_time;
vmCvar_t	as_max_spectator_warn;

vmCvar_t	as_preserve_gamename;

vmCvar_t	as_headhunter;

// don't forget these quasi-cvars
// as_ra_*
// as_alter_mod_*
// ...

static as_cvars_t	asCvarTable[] = {
	{ &as_version, "as_version", AS_VERSION, CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

	{ &as_debug_console, "as_debug_console", "0", 0, 0, qfalse },
	{ &as_debug_file, "as_debug_file", "", 0, 0, qfalse },

	{ &as_maprotation_file, "as_maprotation_file", "", 0, 0, qfalse },
	{ &as_maprotation_state_file, "as_maprotation_state_file", "", 0, 0, qfalse },
	{ &as_maprotation_vote_preserve, "as_maprotation_vote_preserve", "1", 0, 0, qfalse },
	{ &as_maprotation_type, "as_maprotation_type", "0", 0, 0, qfalse },
	{ &as_maprotation_spacing, "as_maprotation_spacing", "3", 0, 0, qfalse },
	{ &as_maprotation_autoload, "as_maprotation_autoload", "0", 0, 0, qfalse },
	{ &as_maprotation_gametype_mask, "as_maprotation_gametype_mask", "0", 0, 0, qfalse },

	{ &as_vote_mask, "as_vote_mask", "-1", CVAR_BITMAPPED, 0, qfalse },
	{ &as_vote_connected_percent, "as_vote_connected_percent", "51", 0, 0, qfalse },
	{ &as_vote_gametype_mask, "as_vote_gametype_mask", "-1", CVAR_BITMAPPED, 0, qfalse },
	{ &as_vote_mintimelimit, "as_vote_mintimelimit", "0", 0, 0, qfalse },
	{ &as_vote_maxtimelimit, "as_vote_maxtimelimit", "0", 0, 0, qfalse },
	{ &as_vote_minfraglimit, "as_vote_minfraglimit", "0", 0, 0, qfalse },
	{ &as_vote_maxfraglimit, "as_vote_maxfraglimit", "0", 0, 0, qfalse },
	{ &as_vote_mincapturelimit, "as_vote_mincapturelimit", "0", 0, 0, qfalse },
	{ &as_vote_maxcapturelimit, "as_vote_maxcapturelimit", "0", 0, 0, qfalse },
	{ &as_vote_maxcall, "as_vote_maxcall", "3", 0, 0, qfalse },
	{ &as_vote_mode_mask, "as_vote_mode_mask", "-1", CVAR_BITMAPPED, 0, qfalse },

	{ &as_adminlog_file, "as_adminlog_file", "", 0, 0, qfalse },
	{ &as_ban_file, "as_ban_file", "", 0, 0, qfalse },
	{ &as_nokick_file, "as_nokick_file", "", 0, 0, qfalse },

	{ &as_report_to_master, "as_report_to_master", "1", 0, 0, qfalse },

	{ &as_referee_password, "as_referee_password", "", 0, 0, qfalse },
	{ &as_referee_print_mask, "as_referee_print_mask", "1024", 0, 0, qfalse },

	{ &as_log_mask, "as_log_mask", "-1", CVAR_BITMAPPED, 0, qfalse },
	{ &as_log_tick_interval, "as_log_tick_interval", "600", 0, 0, qfalse },
	{ &as_log_tick_format, "as_log_tick_format", "MM/DD/YYYY hh:mm:ss", 0, 0, qfalse },
	{ &as_log_rotate_interval, "as_log_rotate_interval", "", 0, 0, qfalse },
	{ &as_log_rotate_format, "as_log_rotate_format", "YYYYMMDD", 0, 0, qfalse },
	{ &as_log_rotate_directory, "as_log_rotate_directory", "", 0, 0, qfalse },
	{ &as_log_osp, "as_log_osp", "0", 0, 0, qfalse },

	{ &as_max_intermission, "as_max_intermission", "0", 0, 0, qfalse },
	{ &as_time_during_warmup, "as_time_during_warmup", "0", 0, 0, qfalse },

	{ &as_kick_spectator_bots, "as_kick_spectator_bots", "1", 0, 0, qfalse },

	{ &as_motd_spectator_duration, "as_motd_spectator_duration", "20", 0, 0, qfalse },
	{ &as_motd_firstspawn_duration, "as_motd_firstspawn_duration", "5", 0, 0, qfalse },
	{ &as_motd_1, "as_motd_1", "", 0, 0, qfalse },
	{ &as_motd_2, "as_motd_2", "", 0, 0, qfalse },
	{ &as_motd_3, "as_motd_3", "", 0, 0, qfalse },
	{ &as_motd_4, "as_motd_4", "", 0, 0, qfalse },
	{ &as_motd_5, "as_motd_5", "", 0, 0, qfalse },

	{ &as_team_balance, "as_team_balance", "0", 0, 0, qfalse },

	{ &as_spam_warn, "as_spam_warn", "10", 0, 0, qfalse },
	{ &as_spam_kick, "as_spam_kick", "15", 0, 0, qfalse },
	{ &as_spam_interval, "as_spam_interval", "1000", 0, 0, qfalse },
	{ &as_spam_message, "as_spam_message", "Warning: stop spamming or be kicked!", 0, 0, qfalse },

	{ &as_mode_number, "as_mode_number", "0", CVAR_LATCH, 0, qfalse },
	{ &as_mode_file, "as_mode_file", "modes_2-1.dat", CVAR_LATCH, 0, qfalse },
	
	{ &as_rollback_config, "as_rollback_config", "", 0, 0, qfalse },
	{ &as_rollback_timeout, "as_rollback_timeout", "600", 0, 0, qfalse },

	{ &as_chat_filter_file, "as_chat_filter_file", "", 0, 0, qfalse },

	{ &as_gamestartup_format, "as_gamestartup_format", "MM/DD/YYYY hh:mm:ss.zz", 0, 0, qfalse },

	{ &as_unbalanced_warning, "as_unbalanced_warning", "2", 0, 0, qfalse },

	{ &as_max_spectator_time, "as_max_spectator_time", "0", 0, 0, qfalse },
	{ &as_max_spectator_warn, "as_max_spectator_warn", "15", 0, 0, qfalse },

	{ &as_preserve_gamename, "as_preserve_gamename", "0", 0, 0, qfalse },

	{ &as_headhunter, "as_headhunter", "0", CVAR_SERVERINFO, 0, qfalse },

	{ NULL }	// End of list marker

};

// Functions

// optionally debug to file and/or console
void as_debug(char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec;
	char		*p;
	static char	lastString[1024] = {'\n','0'};

	if (!( as_level.debugfile || as_debug_console.integer ))
		return;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	vsprintf( string + 7, fmt,argptr );
	va_end( argptr );

	if ( as_debug_console.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !as_level.debugfile )
		return;

	if (Q_strrchr(lastString, '\n') != 0 )
		trap_FS_Write( string, strlen( string ), as_level.debugfile );
	else {
		p = string + 7;
		trap_FS_Write( p, strlen( p ), as_level.debugfile );
	}
	memcpy(lastString, string, sizeof(lastString));
}

// Any bitmapped cvars set to -1 should be set to MAX_QINT, meaning
// ALL options in a bitmap environment (2^31 - 1)
void as_fixCvars( void ) {
	char		*m;
	as_cvars_t	*cv;

	m = va("%d", MAX_QINT);

	for ( cv = asCvarTable; cv->cvarName; cv++ ) {
		if ( (cv->cvarFlags & CVAR_BITMAPPED) && (cv->vmCvar->integer == -1) )
			trap_Cvar_Set(cv->cvarName, m);
	}
}

// q3as version of G_RegisterCvars()
void as_registerCvars( void ) {
	as_cvars_t	*cv;

	for ( cv = asCvarTable; cv->cvarName; cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar )
			cv->modificationCount = cv->vmCvar->modificationCount;
	}
	as_fixCvars();
}

// q3as version of G_UpdateCvars()
void as_updateCvars( void ) {
	as_cvars_t	*cv;

	as_fixCvars() ;
	for ( cv = asCvarTable ; cv->cvarName; cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", 
						cv->cvarName, cv->vmCvar->string ) );
				}
			}
		}
	}
}

void as_postSpawnCleanup( void ) {
	int			i;
	gentity_t	*e;

	// this is done to prevent the rare
	// BG_CanItemBeGrabbed crashes where ent->s.modelindex would be 0
	// because FinishSpawningItem() hadn't been called yet
	as_debug("Touching up item entities...\n");
	for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ) {
		if (e && e->inuse && e->item) {
			e->s.modelindex = e->item - bg_itemlist;
			e->s.eType = ET_ITEM;
		}
	}
}

// make sure token is valid
qboolean as_checkToken( char *token ) {
	if ( !token || token[0] == 0 )
		return qfalse;
	return qtrue;
}

qboolean as_admin_log(char *fmt, ... ) {
	va_list			argptr;
	char			string[1024];
	fileHandle_t	f;
	int				min,tens,sec;

	// as_debug("Writing admin log data ...");

	sec = level.time / 1000;
	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	vsprintf( string + 7, fmt, argptr );
	va_end( argptr );

	G_Printf("%s", string + 7);

	if ( (!as_adminlog_file.string) || (strlen(as_adminlog_file.string) == 0) ) {
		as_debug("Not admin logging, as_adminlog_file is null\n");
		return qfalse;
	}

	trap_FS_FOpenFile( as_adminlog_file.string, &f, FS_APPEND_SYNC);
	if (!f) {
		as_debug("Admin log failed - unable to open %s\n", as_adminlog_file.string);
		return qfalse;
	}

	trap_FS_Write( string, strlen( string ), f );
	trap_FS_FCloseFile(f);
	return qtrue;
}

qboolean as_writeStringToFile (char *file, const char *string, int len, int maxFileSize, int mode) {
	fileHandle_t	fh = 0;
	int				curLen;

	if ( !file )
		return qfalse;

	if ( !len )
		return qfalse;

	as_debug("Writing '%s' to file '%s' ...\n", string, file);

	curLen = trap_FS_FOpenFile( file, &fh, mode);
	if ( !fh ) {
		G_Printf("WARNING: Couldn't open file for writing: %s\n", file );
		as_debug("FAILED.  Unable to open file for writing: %s\n", file );
		return qfalse;
	}

	if ( (maxFileSize > 0 ) && (curLen > len + maxFileSize + 1) ) {
		G_Printf("WARNING: not enough room to write data to file: %s\n", file );
		trap_FS_FCloseFile(fh);
		as_debug("FAILED.  not enough room to write data to file: %s\n", file );
		return qfalse;
	}

	trap_FS_Write( string, len, fh );
	trap_FS_FCloseFile(fh);

	return qtrue;	
}

qboolean as_removeFromFile(char *file, char *string) {
	fileHandle_t	fh = 0;
	int				localLen;
	char			buffer[30000];
	char			*p;
	char			line[1024];
	int				i;
	qboolean		newline, eof;

	if (!file)
		return qfalse;
	if (!string)
		return qfalse;
	if (!string[0])
		return qfalse;

	as_debug("Removing '%s' from file '%s'...\n");

	// pull in contents of existing file
	localLen = trap_FS_FOpenFile(file, &fh, FS_READ);
	if ( !fh ) {
		as_debug("Unable to read file: %s\n", file);
		return qfalse;
	}

	if (localLen > sizeof(buffer)) {
		as_debug("File (%s) too big\n", file);
		trap_FS_FCloseFile(fh);
		return qtrue;
	}

	if (localLen == 0) {
		trap_FS_FCloseFile(fh);
		return qtrue;
	}

	memset(buffer, 0, sizeof(buffer));
	trap_FS_Read(buffer, sizeof(buffer), fh);
	trap_FS_FCloseFile(fh);

	// null out destination file
	trap_FS_FOpenFile(file, &fh, FS_WRITE);
	if ( !fh ) {
		as_debug("Unable to null out file '%s'\n", file);
		return qfalse;
	}

	trap_FS_FCloseFile(fh);

	// line by line, go through existing file, and write each line out, unless
	// it matches specified string
	p = buffer;

	eof = qfalse;
	// this loops per line
	while (!eof) {
		memset(line, 0, sizeof(line));
		i = 0;
		newline = qfalse;

		// this loops per character on a line
		while (1) {
			if (!*p)
				eof = qtrue;

			if (!eof && (*p == '\n' || *p == '\r')) {
				newline = qtrue;
				p++;
				continue;
			}

			// if eof or newline, write the line out
			if (eof || newline) {
				newline = qfalse;
				if (Q_strncmp(line, string, 1024) != 0) {
					as_writeStringToFile(file, va("%s\n",line) , strlen(line) + 1, 0, FS_APPEND);
				}
				break;
			}

			// otherwise continue building the line
			line[i++] = *p++;
		}
	}

	return qtrue;
}

// convert "xxx.xxx.xxx.xxx" to a 32bit integer
// THIS IS NOT MSB/LSB safe!
qboolean inet_aton (char *s, unsigned *ret) {
	char		num[128];
	char		orig[MAX_IPSTRING];
	int			i, j;
	unsigned	tmp = 0;

	Q_strncpyz(orig, s, sizeof(orig));
	*ret = 0;

	for (i=3 ; i>=0 ; i--)
	{
		if (*s < '0' || *s > '9') {
			G_Printf( "inet_aton(): Bad address: %s\n", orig );
			return qfalse;
		}
		
		j = 0;
		while (*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		tmp = (atoi(num) << ( i * 8 ) );
		*ret += tmp;

		if (!*s)
			break;
		s++;
	}

	return qtrue;
}

// remove :yyyy from xxx.xxx.xxx.xxx:yyyy
void as_stripIPPort (char *ip, char *newIP, int len) {
	char		*p;
	int			i;
	char		ips[MAX_IPSTRING];

	p = ip;
	i = 0;
	while (*p) {
		if (*p == ':')
			break;
		ips[i++] = *p++;
	}
	ips[i] = '\0';
	Q_strncpyz(newIP, ips, len);
}

// convert a 32bit integer to "xxx.xxx.xxx.xxx"
// THIS IS NOT MSB/LSB safe!
void inet_ntoa(unsigned num, char *dest, int size) {
	int			i, tmpnum;
	char		str[MAX_IPSTRING];

	memset(str, 0,sizeof(str));
	for (i = 3; i >= 0; i--) {
		tmpnum = (num >> (i * 8)) & 0x000000ff;
		Com_sprintf(str, sizeof(str), "%s%d.", str, tmpnum);
	}
	str[strlen(str) - 1] = '\0';

	Q_strncpyz(dest, str, size);
}

qboolean as_checkNetmask(char *checkNet, int checkBits) {
	unsigned	mask;
	unsigned	cNet;

	inet_aton(checkNet, &cNet);
	mask = 0xffffffff << (32 - checkBits);
	if ( (cNet & ~mask) != 0) {
		as_debug("as_checkNetmask(): Bad net/mask pair: %s/%d\n", checkNet, checkBits);
		return qfalse;
	}

	return qtrue;
}

// does inputIP fall within checkNet/checkBits (CIDR)?
qboolean as_checkIP (char *inputIP, char *checkNet, int checkBits) {
	unsigned	mask;
	unsigned	iIP;
	unsigned	cNet;

	mask = 0xffffffff << (32 - checkBits);

	if (as_checkNetmask(checkNet, checkBits) == 0)
		return qfalse;

	inet_aton(checkNet, &cNet);
	inet_aton(inputIP, &iIP);
	if ( (iIP & mask) == cNet)
		return qtrue;
	
	return qfalse;
}

qboolean as_parseCIDR (char *cidr, char *ipOut, int *bitsOut) {
	char		IP[MAX_IPSTRING];
	char		tmp[MAX_IPSTRING];
	int			i;
	char		*p;

	p = cidr;
	i = 0;
	while (*p) {
		if (*p == '/') {
			p++;
			break;
		}
		IP[i++] = *p++;
	}
	IP[i] = '\0';
	Q_strncpyz(ipOut, IP, 32);
	i = 0;
	while (*p) {
		tmp[i++] = *p++;
	}
	tmp[i] = '\0';

	if (i == 0)
		Q_strncpyz(tmp, "32", sizeof(tmp));
	*bitsOut = atoi(tmp);

	if (as_checkNetmask(IP, *bitsOut) == 0)
		return qfalse;

	return qtrue;
}

qboolean as_writeIP (char *ip) {
	char			toWrite[33];

	if (as_ban_file.string && as_ban_file.string[0]) {
		as_debug("Adding '%s' to banfile...\n", ip);
		Com_sprintf(toWrite, sizeof(toWrite), "\n%s", ip);
		return(as_writeStringToFile(as_ban_file.string, toWrite, strlen(toWrite), AS_MAX_BANFILE_SIZE, FS_APPEND_SYNC));
	} else {
		return qfalse;
	}
}

qboolean as_removeIP (char *ip) {
	if (as_ban_file.string && as_ban_file.string[0])
		return(as_removeFromFile(as_ban_file.string, ip));
	else
		return qfalse;
}

qboolean as_isBanned (char *ip) {
	fileHandle_t	fh = 0;
	int				len;
	char			buf[AS_MAX_BANFILE_SIZE];
	char			*p, *token;
	char			fileIP[MAX_IPSTRING];
	int				fileBits;

	as_debug("Checking to see if '%s' is banned ...\n", ip);
	if ( as_ban_file.string[0] ) {
		len = trap_FS_FOpenFile( as_ban_file.string, &fh, FS_READ);
		if ( !fh ) {
			as_debug("FAILED. Couldn't open banfile: %s\n", as_ban_file.string);
			return qfalse;
		}
		if ( len > AS_MAX_BANFILE_SIZE ) {
			G_Printf("WARNING: banfile size too big: %d\n", len);
			trap_FS_FCloseFile(fh);
			as_debug("FAILED. banfile size too big: %d\n", len);
			return qfalse;
		}
	} else {
		as_debug("FAILED. banfile not set!\n");
		return qfalse;
	}

	if ( len == 0 ) {
		trap_FS_FCloseFile(fh);
		as_debug("0-byte banfile\n");
		return qfalse;
	}

	memset(buf, 0, sizeof(buf));
	trap_FS_Read(buf, sizeof(buf), fh);
	trap_FS_FCloseFile(fh);

	p = buf;
	while ( 1 ) {
		token = COM_Parse(&p);
		if (! as_checkToken(token) )
			break; // end of list

		if (as_parseCIDR (token, fileIP, &fileBits) ) {
			if (as_checkIP (ip, fileIP, fileBits) ) {
				as_debug("The IP, %s, is banned (matches %s)\n", ip, token);
				return qtrue;
			}
		} else {
			as_debug("WARNING: Bad CIDR value: %s\n", token);
		}
	}

	as_debug("The IP, %s, is NOT banned.\n", ip);
	return qfalse;

}

qboolean as_banIP (char *ip) {
	if (as_isBanned(ip))
		return qfalse;
	return (as_writeIP(ip));
}

qboolean as_isKickable( char *ip ) {
	fileHandle_t	fh = 0;
	int				len;
	char			buf[AS_MAX_NOKICKFILE_SIZE];
	char			*p, *token;
	char			fileIP[MAX_IPSTRING];
	int				fileBits;

	as_debug("Checking to see if '%s' is kickable ...\n", ip);
	if ( as_nokick_file.string[0] ) {
		len = trap_FS_FOpenFile( as_nokick_file.string, &fh, FS_READ);
		if ( !fh ) {
			as_debug("FAILED. Couldn't open nokickfile: %s\n", as_nokick_file.string);
			return qtrue;
		}
		if ( len > AS_MAX_NOKICKFILE_SIZE ) {
			G_Printf("WARNING: nokickfile size too big: %d\n", len);
			trap_FS_FCloseFile(fh);
			as_debug("FAILED. nokickfile size too big: %d\n", len);
			return qtrue;
		}
	} else {
		as_debug("FAILED. nokickfile not set!\n");
		return qtrue;
	}

	if ( len == 0 ) {
		trap_FS_FCloseFile(fh);
		as_debug("0-byte nokickfile\n");
		return qtrue;
	}

	memset(buf, 0, sizeof(buf));
	trap_FS_Read(buf, sizeof(buf), fh);
	trap_FS_FCloseFile(fh);

	p = buf;
	while ( 1 ) {
		token = COM_Parse(&p);
		if (! as_checkToken(token) )
			break; // end of list

		if (as_parseCIDR (token, fileIP, &fileBits) ) {
			if (as_checkIP (ip, fileIP, fileBits) ) {
				as_debug("The IP, %s, is not kickable (matches %s)\n", ip, token);
				return qfalse;
			}
		} else {
			as_debug("WARNING: Bad CIDR value: %s\n", token);
		}
	}

	as_debug("The IP, %s, *is* kickable.\n", ip);
	return qtrue;
}

static void as_setVoteType( char *vs ) {
	as_string_map_t		*type;

	as_level.voteType = AS_VT_BAD;
	for ( type = as_vote_map; type->string; type++ ) {
		if ( Q_stricmp( vs, type->string ) == 0 ) {
			as_level.voteType = type->type;
		}
	}
}

int FindClientByName(char *name);
// moved from g_cmds.c
static const char *gameNames[] = {
	"Free For All",
	"Tournament",
	"Single Player",
	"Team Deathmatch",
	"Capture the Flag",
	"One Flag CTF",
	"Overload",
	"Harvester"
};

static int as_numConnectedHumans(qboolean ignoreSpecs) {
	int	i, count;

	count = 0;
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED )
			continue;
		if ( g_entities[i].r.svFlags & SVF_BOT )
			continue;
		if ( ignoreSpecs && level.clients[i].sess.sessionTeam == TEAM_SPECTATOR )
			continue;

		count++;
	}
	return count;
}

static void as_getValidVoteList(char *buffer, int size) {
	as_string_map_t		*type;

	memset(buffer, 0, size);

	for ( type = as_vote_map; type->string; type++ ) {
		if ( as_vote_mask.integer & type->type )
			if (strlen(buffer))
				Com_sprintf(buffer, size, "%s, %s", buffer, type->string);
			else
				Com_sprintf(buffer, size, "%s", type->string);
	}

	if (!strlen(buffer))
		Com_sprintf(buffer, size, "NONE!");
}

void as_callVote_f( gentity_t *ent ) {
	int		i;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];
	char	userinfo[MAX_INFO_STRING];
	char	*ip;
	char	temp[1000];
	int		mask;

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	as_setVoteType(arg1);
	as_debug ("Vote type for '%s' set to '%d'\n", arg1, as_level.voteType);

	// (ALWAYS allow togglebalance vote)
	if ( g_allowVote.integer == 0 && as_level.voteType != AS_VT_TOGGLEBALANCE ) {
		trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
		return;
	}

	if ( as_percentConnected() < as_vote_connected_percent.integer ) {
		trap_SendServerCommand( ent-g_entities, "print \"Please wait for more people to finish connecting.\n\"" );
		return;
	}

	if ( level.voteTime ) {
		trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		return;
	}
	if ( ent->client->pers.voteCount >= as_vote_maxcall.integer ) {
		trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
		return;
	}

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		return;
	}

	if ( as_level.voteType == AS_VT_BAD ) {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		as_getValidVoteList(temp, sizeof(temp));
		// trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, fraglimit <frags>, capturelimit <caps>, mode <mode>, togglebalance.\n\"" );
		trap_SendServerCommand( ent-g_entities, va("print \"Vote commands are: %s\n\"", temp ));
		return;
	}

	// if there is still a vote to be executed
	if ( level.voteExecuteTime ) {
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}

	// ALWAYS allow togglebalance vote
	mask = as_vote_mask.integer | AS_VT_TOGGLEBALANCE;

	as_debug("Checking vote mask (%d) for the vote %s against %d\n", mask, arg1, as_level.voteType);
	if ( ! ( as_level.voteType & mask ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"The vote command, %s, has been disabled by the server administrator.  Type just /callvote for a list of enabled votes.\n\"", arg1 ) );
		return;
	}

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ) );
	ip = Info_ValueForKey (userinfo, "ip");

	switch (as_level.voteType) {
	case AS_VT_GAMETYPE:
		// special case for g_gametype, check for bad values
		i = atoi( arg2 );
		if( i == GT_SINGLE_PLAYER || i < GT_FFA || i >= GT_MAX_GAME_TYPE) {
			trap_SendServerCommand( ent-g_entities, "print \"Invalid gametype.\n\"" );
			return;
		}

		as_debug ("Checking gametype_mask (%d) against current gametype (%d -> %d)\n",
			as_vote_gametype_mask.integer, i, (1 << i));
		if (!(as_vote_gametype_mask.integer & (1 << i))) {
			trap_SendServerCommand( ent-g_entities, "print \"That gametype isn't allowed.\n\"" );
			return;
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d", arg1, i );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s %s", arg1, gameNames[i] );
		break;
	case AS_VT_MAP:
		if (rotation.numMaps == -1) {
			// special case for map changes, we want to reset the nextmap setting
			// this allows a player to change maps, but not upset the map rotation
			char	s[MAX_STRING_CHARS];

			as_debug("map vote called; doing normal rotation\n");
			trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
			if (*s) {
				Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
			} else {
				Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
			}
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		} else {
			as_debug("map vote called; checking to make sure it's in the rotation\n");
			if (as_isMapAllowed(as_numForMap(arg2), qtrue)) {
				as_debug("Yup!  Setting level.voteString to as_map %s true\n", arg2);
				Com_sprintf( level.voteString, sizeof( level.voteString ), "as_map %s true", arg2 );
				Com_sprintf( level.voteDisplayString, sizeof (level.voteDisplayString ), "map %s", arg2 );
			} else {
				as_debug("Nope, notifying user...\n");
				trap_SendServerCommand( ent-g_entities, "print \"That map isn't allowed.\n\"" );
				return;
			}
		}
		break;
	case AS_VT_NEXTMAP:
		if (rotation.numMaps == -1) {
			char	s[MAX_STRING_CHARS];
	
			as_debug("nextmap vote called; doing normal stuff\n");
			trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
			if (!*s) {
				trap_SendServerCommand( ent-g_entities, "print \"nextmap not set.\n\"" );
				return;
			}
			Com_sprintf( level.voteString, sizeof( level.voteString ), "vstr nextmap");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		} else {
			as_debug("nextmap vote called; using as_nextmap\n");
			Com_sprintf( level.voteString, sizeof( level.voteString ), "as_nextmap");
			Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "nextmap" );
		}
		break;
	case AS_VT_KICK:
	case AS_VT_CLIENTKICK:
		{
			int			clientNum;
			gentity_t	*other;
			gclient_t	*client;
			
			if (Q_stricmp(arg2, "allbots") == 0 ) {
				as_debug("Callvote kick 'allbots' called\n");
				Com_sprintf( level.voteString, sizeof( level.voteString ), "kick allbots" );
				Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
			} else {
				as_debug("Checking to make sure client isn't in as_noKickIPs\n");

				// get client number
				if (as_level.voteType == AS_VT_CLIENTKICK)
					clientNum = atoi(arg2);
				else {
					clientNum = FindClientByName(arg2);
					if (clientNum == -1) {
						trap_SendServerCommand( ent-g_entities, va("print \"Unable to locate '%s'\n\"", arg2 ));
						return;
					}
				}

				// check to see if this client's IP address isn't kickable
				other = g_entities + clientNum;
				client = other->client;
				if (!client) {
					as_debug("Woah! client = NULL during IP kickable test\n");
					return;
				} else {
					as_debug("Checking IP for client num %d\n", clientNum);
					if (strlen(ip) < 7)
						as_debug("Warning, while checking for kickability, no IP found (%s)!\n", ip);
					if (as_isKickable(ip) == qfalse) {
						trap_SendServerCommand( ent-g_entities, "print \"That person can't be kicked!!\n\"" );
						return;
					}
				}

				// do it
				as_debug("Moving forward with kicking\n");
				Com_sprintf( level.voteString, sizeof( level.voteString ), "clientkick %i",clientNum);
				Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s (%s)", level.voteString, client->pers.netname );
			}
		}
		break;
	case AS_VT_TIMELIMIT:
		as_debug("Checking to make sure timelimit vote is within acceptable range\n");
		if (as_vote_mintimelimit.integer != 0) {
			if (atoi(arg2) < as_vote_mintimelimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That timelimit is too low.\n\"" );
				return;
			}
		}
		if (as_vote_maxtimelimit.integer != 0) {
			if (atoi(arg2) > as_vote_maxtimelimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That timelimit is too high.\n\"" );
				return;
			}
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		break;
	case AS_VT_FRAGLIMIT:
		as_debug("Checking to make sure fraglimit vote is within acceptable range\n");
		if (as_vote_minfraglimit.integer != 0) {
			if (atoi(arg2) < as_vote_minfraglimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That fraglimit is too low.\n\"" );
				return;
			}
		}
		if (as_vote_maxfraglimit.integer != 0) {
			if (atoi(arg2) > as_vote_maxfraglimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That fraglimit is too high.\n\"" );
				return;
			}
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		break;
	case AS_VT_CAPTURELIMIT:
		as_debug("Checking to make sure capturelimit vote is within acceptable range\n");
		if (as_vote_mincapturelimit.integer != 0) {
			if (atoi(arg2) < as_vote_mincapturelimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That capturelimit is too low.\n\"" );
				return;
			}
		}
		if (as_vote_maxcapturelimit.integer != 0) {
			if (atoi(arg2) > as_vote_maxcapturelimit.integer) {
				trap_SendServerCommand( ent-g_entities, "print \"That capturelimit is too high.\n\"" );
				return;
			}
		}

		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		break;
	case AS_VT_MODE:
		if (!strlen(arg2)) {
			as_getVoteableModeList(temp, sizeof(temp));
			trap_SendServerCommand( ent-g_entities, va("print \"Valid modes are %s\n\"", temp));
			return;
		}
		i = atoi( arg2 );
		if ( i < 0 || i >= as_modes.header.numModes) {
			trap_SendServerCommand( ent-g_entities, "print \"Invalid mode!\n\"" );
			return;
		}
		if (!(as_vote_mode_mask.integer & (1 << i))) {
			trap_SendServerCommand( ent-g_entities, "print \"That mode isn't allowed.\n\"" );
			return;
		}
		Com_sprintf( level.voteString, sizeof( level.voteString ), "as_mode_number %d; map %s", i, as_level.mapname);
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "mode %d (%s); map_restart 0",
			i, as_modes.mi[i].fullname);

		break;
	case AS_VT_TOGGLEBALANCE:
		Com_sprintf( level.voteString, sizeof( level.voteString ), "as_togglebalance");
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "togglebalance" );
		break;

	default:
		Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
		break;
	}

	trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );

	// start the voting, the caller automatically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	ent->client->pers.voteCount++;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va("%i", level.voteTime ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
}

void as_callTeamVote_f( gentity_t *ent ) {
	int		i, team, cs_offset;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];

	team = ent->client->sess.sessionTeam;
	if ( team == TEAM_RED )
		cs_offset = 0;
	else if ( team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !g_allowVote.integer ) {
		trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
		return;
	}

	if ( level.teamVoteTime[cs_offset] ) {
		trap_SendServerCommand( ent-g_entities, "print \"A team vote is already in progress.\n\"" );
		return;
	}

	if ( ent->client->pers.teamVoteCount >= as_vote_maxcall.integer ) {
		trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of team votes.\n\"" );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
		return;
	}

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	arg2[0] = '\0';
	for ( i = 2; i < trap_Argc(); i++ ) {
		if (i > 2)
			strcat(arg2, " ");
		trap_Argv( i, &arg2[strlen(arg2)], sizeof( arg2 ) - strlen(arg2) );
	}

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		return;
	}

	as_setVoteType(arg1);
	as_debug ("Vote type for '%s' set to '%d'\n", arg1, as_level.voteType);
	
	if ( as_level.voteType == AS_VT_BAD ) {
		trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
		trap_SendServerCommand( ent-g_entities, "print \"Team vote commands are: leader <player>.\n\"" );
		return;
	}

	as_debug("Checking vote mask (%d) for the vote %s against %d\n", as_vote_mask.integer, arg1, as_level.voteType);
	if ( ! ( as_level.voteType & as_vote_mask.integer ) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"The vote command, %s, has been disabled by the server administrator\n\"", arg1 ) );
		return;
	}

	if ( as_level.voteType == AS_VT_TEAM_LEADER ) {
		char netname[MAX_NETNAME], leader[MAX_NETNAME];

		if ( !arg2[0] ) {
			i = ent->client->ps.clientNum;
		}
		else {
			// numeric values are just slot numbers
			for (i = 0; i < 3; i++) {
				if ( !arg2[i] || arg2[i] < '0' || arg2[i] > '9' )
					break;
			}
			if ( i >= 3 || !arg2[i]) {
				i = atoi( arg2 );
				if ( i < 0 || i >= level.maxclients ) {
					trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %i\n\"", i) );
					return;
				}

				if ( !g_entities[i].inuse ) {
					trap_SendServerCommand( ent-g_entities, va("print \"Client %i is not active\n\"", i) );
					return;
				}
			}
			else {
				Q_strncpyz(leader, arg2, sizeof(leader));
				Q_CleanStr(leader);
				for ( i = 0 ; i < level.maxclients ; i++ ) {
					if ( level.clients[i].pers.connected == CON_DISCONNECTED )
						continue;
					if (level.clients[i].sess.sessionTeam != team)
						continue;
					Q_strncpyz(netname, level.clients[i].pers.netname, sizeof(netname));
					Q_CleanStr(netname);
					if ( !Q_stricmp(netname, leader) ) {
						break;
					}
				}
				if ( i >= level.maxclients ) {
					trap_SendServerCommand( ent-g_entities, va("print \"%s is not a valid player on your team.\n\"", arg2) );
					return;
				}
			}
		}
		Com_sprintf(arg2, sizeof(arg2), "%d", i);
	} else {
		return;
	}

	Com_sprintf( level.teamVoteString[cs_offset], sizeof( level.teamVoteString[cs_offset] ), "%s %s", arg1, arg2 );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED )
			continue;
		if (level.clients[i].sess.sessionTeam == team)
			trap_SendServerCommand( i, va("print \"%s called a team vote.\n\"", ent->client->pers.netname ) );
	}

	// start the voting, the caller automatically votes yes
	level.teamVoteTime[cs_offset] = level.time;
	level.teamVoteYes[cs_offset] = 1;
	level.teamVoteNo[cs_offset] = 0;

	ent->client->pers.teamVoteCount++;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam == team)
			level.clients[i].ps.eFlags &= ~EF_TEAMVOTED;
	}
	ent->client->ps.eFlags |= EF_TEAMVOTED;

	trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, va("%i", level.teamVoteTime[cs_offset] ) );
	trap_SetConfigstring( CS_TEAMVOTE_STRING + cs_offset, level.teamVoteString[cs_offset] );
	trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset] ) );
	trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset] ) );
}

// exec a map by name.  Can be called from console/voting
void as_svcmd_AS_Map_f( void ) {
	char		map[MAX_TOKEN_CHARS];
	char		voted[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		as_debug("as_svcmd_AS_Map_f() called without an argument!\n");
		return;
	}

	trap_Argv( 1, map, sizeof( map ) );
	trap_Argv( 2, voted, sizeof( voted ) );

	if (!map) {
		as_debug("as_svcmd_AS_Map_f(): null map!\n");
		return;
	}

	if (!voted)
		as_debug("as_svcmd_AS_Map_f() arg1: '%s' arg2: NULL\n", map);
	else
		as_debug("as_svcmd_AS_Map_f() arg1: '%s' arg2: '%s'\n", map, voted);

	if (Q_stricmp(voted, "true") == 0)
		as_execMapByName(map, qtrue);
	else
		as_execMapByName(map, qfalse);
}

// exec a map by name.  Called from console
void as_svcmd_AS_MapID_f( void ) {
	char		ID[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		as_debug("as_svcmd_AS_Map_f() called without an argument!\n");
		return;
	}

	trap_Argv( 1, ID, sizeof( ID ) );

	if (!ID) {
		as_debug("as_svcmd_AS_MapID_f(): null ID!\n");
		return;
	}

	as_debug("as_svcmd_AS_MapID_f() arg1: '%s'\n", ID);

	as_execMapByID(ID);
}

void as_svcmd_AS_ReloadMapConfig_f( void ) {
	as_loadQ3ASMapRotation(qtrue);
}

void as_svcmd_AddIP_f( void ) {
	char	buf[MAX_IPSTRING];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage: as_addip <exact IP or CIDR IP & mask>\n");
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	if (as_writeIP(buf)) {
		G_Printf("IP added!\n");
	} else {
		G_Printf("Failed to add IP!\n");
	}
}

void as_svcmd_RemoveIP_f( void ) {
	char	buf[MAX_IPSTRING];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage: as_removeip <exact IP or CIDR IP & mask>\n");
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	if (as_removeIP(buf)) {
		G_Printf("IP removed (if it existed)!\n");
	} else {
		G_Printf("Failed to remove IP!\n");
	}
}

void as_openDebugLog( void ) {
	if ( as_debug_file.string[0] ) {
		trap_FS_FOpenFile( as_debug_file.string, &as_level.debugfile, FS_APPEND_SYNC);
		if ( !as_level.debugfile ) {
			G_Printf("WARNING: Couldn't open debug logfile: %s\n", as_debug_file.string );
		}
	}
}

void as_serverNextmap( void ) {
	if (rotation.numMaps == -1) {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	} else {
		as_debug("Q3AS map exist - using as_nextmap to change maps\n");
		trap_SendConsoleCommand( EXEC_APPEND, "as_nextmap\n" );
	}
}

void as_postInitInit( void ) {
	int			num;
	qtime_t		t;
	char		ts[100];


	num = trap_Cvar_VariableIntegerValue("dedicated");
	if (num < 2)
		num = trap_Cvar_VariableIntegerValue("listen");
	if (as_report_to_master.integer && num > 1) {
		as_debug("Setting sv_master5 to \"%s\"\n", AS_MASTER);
		trap_SendConsoleCommand( EXEC_APPEND, va( "set sv_master5 \"%s\"\n", AS_MASTER) );
	}
	else {
		as_debug("Setting sv_master5 to \"\"\n");
		trap_SendConsoleCommand( EXEC_APPEND, va( "set sv_master5 \"\"\n") );
	}

	if (level.time == 0) {
		as_debug("Setting gamestartup\n");
		trap_RealTime(&t);
		as_formatTime(as_gamestartup_format.string, ts, sizeof(ts), t);
		trap_Cvar_Set("gamestartup", ts);
	}

	as_level.fps = trap_Cvar_VariableIntegerValue("sv_fps");

	if (level.warmupTime == 0)
		as_logOSP("Game_Start:\n");
}

void as_cmd_Help_f( gentity_t *ent ) {
	fileHandle_t		f;
	char				filename[MAX_QPATH];
	char				tmp[MAX_QPATH];
	int					len;
	char				buf[AS_MAX_HELPFILE_SIZE];

	if (trap_Argc() == 2) {
		trap_Argv( 1, tmp, sizeof(tmp) );
		Com_sprintf(filename, sizeof(filename), "help/%s.txt", tmp);
	} else {
		Com_sprintf(filename, sizeof(filename), "help/help.txt", tmp);
	}

	as_debug("as_cmd_Help_f() called: attempting to display contents of %s\n", filename);

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if (!f || len >= AS_MAX_HELPFILE_SIZE) {
		if (trap_Argc() == 1)
			trap_SendServerCommand( ent-g_entities, "print \"Sorry, no help available on this system\n\"" );
		else
			trap_SendServerCommand( ent-g_entities, "print \"Sorry, no help available on that topic\n\"" );
		if (f) {
			as_debug("File (%s) too big (>= %d bytes)\n", filename, AS_MAX_HELPFILE_SIZE);
			trap_FS_FCloseFile(f);
		} else {
			as_debug("File (%s) not found\n", filename);
		}
		return;
	}

	trap_FS_Read(buf, sizeof(buf), f);
	buf[len] = 0;
	trap_FS_FCloseFile(f);

	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", buf));
}

void as_cmd_SetReferee_f( gentity_t *ent ) {
	char		pass[32];
	char		userinfo[MAX_INFO_STRING];
	char		*ip;

	as_debug("as_cmd_SetReferee_f() called\n");

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ) );
	ip = Info_ValueForKey (userinfo, "ip");

	if (ent->client->sess.isReferee == qtrue) {
		trap_SendServerCommand( ent-g_entities, "print \"You are no longer a referee.\n\"" );
		as_admin_log("User '%s' disabled referee privs\n", ent->client->pers.netname);
		ent->client->sess.isReferee = qfalse;
		return;
	}

	if (trap_Argc() < 2) {
		trap_SendServerCommand( ent-g_entities, "print \"Usage: setref password\n\"" );
		as_admin_log("Bad setref command (missing password) by %s from IP %s\n", ent->client->pers.netname, ip);
		return;
	}

	if (strlen(as_referee_password.string) == 0) {
		trap_SendServerCommand( ent-g_entities, "print \"Server referee password not set\n\"" );
		as_admin_log("'setref' called by %s from IP %s, but as_referee_password isn't set!\n", ent->client->pers.netname, ip);
		return;
	}

	trap_Argv( 1, pass, sizeof(pass) );

	if (strcmp(pass, as_referee_password.string) != 0) {
		trap_SendServerCommand( ent-g_entities, "print \"Bad password\n\"" );
		as_admin_log("'setref' called by %s from IP %s: bad password!\n", ent->client->pers.netname, ip);
		return;
	}

	ent->client->sess.isReferee = qtrue;

	as_admin_log("Successful 'setref' by %s from IP %s\n", ent->client->pers.netname, ip);

	trap_SendServerCommand( ent-g_entities, "print \"You are now a referee.\n\"" );

}

void as_cmd_Referee_f( gentity_t *ent ) {
	char		command[128];
	char		args[256];
	char		tmp[256];
	char		check[128];
	char		userinfo[MAX_INFO_STRING];
	char		*ip;

	as_debug("as_cmd_Referee_f() called\n");

	trap_GetUserinfo( ent-g_entities, userinfo, sizeof( userinfo ) );
	ip = Info_ValueForKey (userinfo, "ip");

	if (ent->client->sess.isReferee != qtrue) {
		trap_SendServerCommand( ent-g_entities, "print \"Usage: ref command\n\"" );
		as_admin_log("Failed referee attempt (isn't a referee) by %s from IP %s\n", ent->client->pers.netname, ip);
		return;
	}

	if (trap_Argc() < 2) {
		trap_SendServerCommand( ent-g_entities, "print \"Usage: ref command\n\"" );
		as_admin_log("Bad referee command called by %s from IP %s\n", ent->client->pers.netname, ip);
		return;
	}

	// grab the command
	trap_Argv( 1, command, sizeof(command) );
	// grab up to 4 arguments to command
	trap_Argv( 2, args, sizeof(args) );
	trap_Argv( 3, tmp, sizeof(tmp) );
	Com_sprintf(args, sizeof(args), "%s %s", args, tmp);
	trap_Argv( 4, tmp, sizeof(tmp) );
	Com_sprintf(args, sizeof(args), "%s %s", args, tmp);
	trap_Argv( 5, tmp, sizeof(tmp) );
	Com_sprintf(args, sizeof(args), "%s %s", args, tmp);

	Com_sprintf(check, sizeof(check), "as_ra_%s", command);
	if (trap_Cvar_VariableIntegerValue( check ) ) {
		as_admin_log("Referee command called by %s from IP %s; command = %s %s\n", ent->client->pers.netname, ip, command, args);
		trap_SendConsoleCommand( EXEC_APPEND, va("%s %s\n", command, args) );
	} else {
		as_admin_log("Referee (%s from IP %s) tried to execute '%s %s'\n", ent->client->pers.netname, ip, command, args);
		trap_SendServerCommand( ent-g_entities, "print \"Referee's aren't allow to execute that command!\n\"" );
	}
}

static int as_getMessageType( char *text ) {
	as_string_map_t		*type;
	int					ret;

	ret = AS_MT_MISC;
	for ( type = as_mt_map; type->string; type++ ) {
		if ( Q_stricmpn( text, type->string, strlen(type->string) ) == 0 ) {
			ret = type->type;
		}
	}
	return ret;
}

void as_sendMessageToReferees(char *text) {
	gentity_t	*ent;
	int			type;
	int			i;

	if (as_level.skipRefPrint == qtrue) {
		// This can cause print loops if console debugging is on
		// as_debug("Skipping referee print (%s)\n", text);
		as_level.skipRefPrint = qfalse;
		return;
	}

	type = as_getMessageType(text);

	if (! (type & as_referee_print_mask.integer) )
		return;

	for ( i=0 ; i < level.maxclients ; i++ ) {
		ent = g_entities + i;
		if ( !ent->client )
			continue;

		if ( ent->client->pers.connected == CON_DISCONNECTED )
			continue;

		if ( ent->client->sess.isReferee )
			trap_SendServerCommand( ent-g_entities, va("print \"^1REFCON:^7 %s^7\"", text));
	}
}

// modified version of ClientCleanName - takes colors out of name
// FIXME: HTML colors
static void as_cleanName( const char *in, char *out, int outSize ) {
	int		len;
	char	ch;
	char	*p;
	int		spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while( 1 ) {
		ch = *in++;
		if( !ch ) {
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' ) {
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE ) {
			// solo trailing carat is not a color prefix
			if( !*in ) {
				break;
			}

			in++;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' ) {
			spaces++;
			if( spaces > 3 ) {
				continue;
			}
		}
		else {
			spaces = 0;
		}

		if( len > outSize - 1 ) {
			break;
		}

		*out++ = ch;
		len++;
	}
	*out = 0;

	// don't allow empty names
	if( *p == 0 ) {
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

// as_status
void as_svcmd_status_f( void ) {
	int			i, j, spaces;
	gclient_t	*client;
	char		name[128];
	char		tmp[10];
	char		line[256];
	char		message[MAX_STRING_CHARS];
	char		userinfo[MAX_INFO_STRING];
	char		*ip;

	trap_GetConfigstring(CS_MESSAGE, message, sizeof(message));

	G_Printf("Map '%s' (%s)\n", message, as_level.mapname);
	G_Printf("#  'Name'                IP\n");
	G_Printf("----------------------------------------------\n");
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if (!client)
			continue;
		if (client->pers.connected == CON_DISCONNECTED)
			continue;

		trap_GetUserinfo( i, userinfo, sizeof( userinfo ) );
		ip = Info_ValueForKey (userinfo, "ip");

		as_cleanName(client->pers.netname, name, sizeof(name));

		Com_sprintf(tmp, sizeof(tmp), "%d", i);
		
		Com_sprintf(line, sizeof(line), "%s", tmp);

		spaces = 3 - strlen(tmp);

		for (j = 0; j < spaces; j++)
			Com_sprintf(line, sizeof(line), "%s ", line);

		Com_sprintf(line, sizeof(line), "%s'%s'", line, name);

		spaces = 20 - strlen(name);
		
		for (j = 0; j < spaces; j++)
			Com_sprintf(line, sizeof(line), "%s ", line);

		Com_sprintf(line, sizeof(line), "%s%s\n", line, ip);

		G_Printf("%s", line);
	}
}

static void as_setupLingerPrint (gentity_t *ent, char *message, int duration) {
	if (! (ent && ent->client))
		return;

	as_debug("Setting up linger-print\n");
	Com_sprintf(ent->client->q3as.linger.message, sizeof(ent->client->q3as.linger.message), message);
	ent->client->q3as.linger.endTime = level.time + duration * 1000;
}

void as_doLingerPrint (gentity_t *ent) {
	int			remainder;

	if (! (ent && ent->client))
		return;

	// this can get called multiple times per frame; we don't want that
	if (level.time == ent->client->q3as.linger.llt)
		return;
	ent->client->q3as.linger.llt = level.time;

	if (ent->client->q3as.linger.endTime < level.time)
		return;

	remainder = ent->client->q3as.linger.endTime % 1000;

	if (level.time % 1000 != remainder)
		return;

	if (ent->client->q3as.linger.endTime - level.time < 3000)
		return;

	// as_debug("Sending linger-print message\n");
	if (as_vercmp(ent->client->sess.ecv, "1.0") < 0) {
		trap_SendServerCommand(ent-g_entities, va("cp \"%s\"", ent->client->q3as.linger.message));
	} else {
		trap_SendServerCommand(ent-g_entities, va("cpy 200 \"%s\"", ent->client->q3as.linger.message));
	}
}

// called inside SpectatorThink(); print the appropriate MOTD
// text to the client
void as_printMOTD (gentity_t *ent, int duration) {
	char	leader[] = "^0 |\n^0 |\n^0 |";
	char	pitch[80];

	if (! (ent && ent->client))
		return;

	Com_sprintf(pitch, sizeof(pitch), "^4=|^1Q3: Advanced Server v%s^4|=", AS_VERSION);

	as_debug("Sending MOTD to %s\n", ent->client->pers.netname);

	if (as_vercmp(ent->client->sess.ecv, "1.0") < 0) {
		as_setupLingerPrint(ent, va("%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
			leader, pitch, as_motd_1.string, as_motd_2.string, as_motd_3.string, as_motd_4.string, as_motd_5.string),
			duration);
	} else {
		as_setupLingerPrint(ent, va("%s\n%s\n%s\n%s\n%s\n%s\n",
			pitch, as_motd_1.string, as_motd_2.string, as_motd_3.string, as_motd_4.string, as_motd_5.string),
			duration);
	}
}

int QDECL as_sortTimeScore( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0 ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	// then sort by score / time
	if ( ca->ps.persistant[PERS_SCORE] / (float)(level.time - ca->pers.enterTime + 1)
		> cb->ps.persistant[PERS_SCORE] / (float)(level.time - cb->pers.enterTime + 1) ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE] / (float)(level.time - ca->pers.enterTime + 1)
		< cb->ps.persistant[PERS_SCORE] / (float)(level.time - cb->pers.enterTime + 1) ) {
		return 1;
	}
	return 0;
}

void as_balance_teams( void ) {

	int			i, team, clientNum, switches = 0;
	int			half = level.numPlayingClients / 2;
	char		userinfo[MAX_INFO_STRING];
	gclient_t	*client;
	gentity_t	*ent;

	// scramble teams according to as_team_balance, only if a team game
	if ( g_gametype.integer < GT_TEAM )
		return;

	// only balance at the end of maps, not after warmup period expires
	if ( level.previousTime < level.warmupTime ) // ( level.warmuptime ) is NOT sufficient
		return;
	
	switch ( as_team_balance.integer ) {
	case AS_TB_SCORE:
		{
			as_debug("Team balance by score\n");

			// swap highest scoring player to other team
			team = level.clients[level.sortedClients[0]].sess.sessionTeam == TEAM_BLUE ? TEAM_RED : TEAM_BLUE;

			for ( i = 0; i < level.numPlayingClients; i++ ) {

				clientNum = level.sortedClients[i];
				client = &level.clients[clientNum];
				ent = &g_entities[clientNum];

				as_debug("client: %d  oldteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);

				// make sure client is connected
				if ( client->pers.connected != CON_CONNECTED )
					continue;

				// team should be TEAM_RED or TEAM_BLUE, not TEAM_SPECTATOR
				if ( client->sess.sessionTeam == TEAM_SPECTATOR )
					continue;

				client->sess.sessionTeam = team;
				trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
				Info_SetValueForKey(userinfo, "team", va("%d", team));
				trap_SetUserinfo(clientNum, userinfo);

				// put 1st player on team1, 2nd and 3rd on team2, 4th and 5th on team1, 6th and 7th on team2, etc.
				switches++;
				if ( switches % 2 == 1 )
					team = team == TEAM_BLUE ? TEAM_RED : TEAM_BLUE;

				as_debug("client: %d  newteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);
			}
		}
		break;

	case AS_TB_RANDOM:
		{
			as_debug("Team balance random\n");

			// randomly choose which team gets assigned first
			// so you don't always end up with more on one team
			if ( random() < 0.5 )
				team = TEAM_RED;
			else
				team = TEAM_BLUE;

			// first switch everyone to one team
			for ( i = 0; i < level.maxclients; i++ ) {
				client = &level.clients[i];
				ent = &g_entities[i];

				// make sure client is connected
				if ( client->pers.connected != CON_CONNECTED )
					continue;

				// team should be TEAM_RED or TEAM_BLUE, not TEAM_SPECTATOR
				if ( client->sess.sessionTeam == TEAM_SPECTATOR )
					continue;

				client->sess.sessionTeam = team;
				trap_GetUserinfo(i, userinfo, sizeof(userinfo));
				Info_SetValueForKey(userinfo, "team", va("%d", team));
				trap_SetUserinfo(i, userinfo);
			}

			// now, put half of the players on the other team
			team = team == TEAM_BLUE ? TEAM_RED : TEAM_BLUE;

			switches = 0;
			while ( switches < half ) {
				clientNum = ( rand() % level.maxclients );
				client = &level.clients[clientNum];
				ent = &g_entities[clientNum];

				as_debug("client: %d  oldteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);
				//make sure client is connected
				if ( client->pers.connected == CON_DISCONNECTED ) // != CON_CONNECTED was an infinite loop
					continue;

				// team should be TEAM_RED or TEAM_BLUE, not TEAM_SPECTATOR
				if ( client->sess.sessionTeam == TEAM_SPECTATOR )
					continue;

				// check if we've already swapped this client
				if ( client->sess.sessionTeam == team )
					continue;

				// client passed
				switches++;

				client->sess.sessionTeam = team;
				trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
				Info_SetValueForKey(userinfo, "team", va("%d", team));
				trap_SetUserinfo(clientNum, userinfo);

				as_debug("client: %d  newteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);
			}
		}
		break;
	case AS_TB_SCORETIME:
			as_debug("Team balance scoretime\n");

			// swap highest timescoring player to other team
			team = level.clients[as_level.timeSortedClients[0]].sess.sessionTeam == TEAM_BLUE ? TEAM_RED : TEAM_BLUE;

			for ( i = 0; i < level.numPlayingClients; i++ ) {

				clientNum = as_level.timeSortedClients[i];
				client = &level.clients[clientNum];
				ent = &g_entities[clientNum];

				as_debug("client: %d  oldteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);

				// make sure client is connected
				if ( client->pers.connected != CON_CONNECTED )
					continue;

				// team should be TEAM_RED or TEAM_BLUE, not TEAM_SPECTATOR
				if ( client->sess.sessionTeam == TEAM_SPECTATOR )
					continue;

				client->sess.sessionTeam = team;
				trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
				Info_SetValueForKey(userinfo, "team", va("%d", team));
				trap_SetUserinfo(clientNum, userinfo);

				// put 1st player on team1, 2nd and 3rd on team2, 4th and 5th on team1, 6th and 7th on team2, etc.
				switches++;
				if ( switches % 2 == 1 )
					team = team == TEAM_BLUE ? TEAM_RED : TEAM_BLUE;

				as_debug("client: %d  newteam:  %d  switches: %d\n", clientNum, client->sess.sessionTeam, switches);
			}
		break;
	default:
		 return;
		 break;
	}
}

qboolean as_readExternalSVFile( void ) {
	char			filename[256];
	int				len;
	fileHandle_t	f;

	Com_sprintf(filename, sizeof(filename), "spawnvars/%s.sv", as_level.mapname);

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		as_debug("No external spawnvar file (%s) found.\n", filename);
		return qfalse;
	}
	if ( len >= AS_MAX_MAP_ENTSTRING ) {
		as_debug("External spawnvar file (%s) too big (%d > %d)\n", filename, len, AS_MAX_MAP_ENTSTRING);
		trap_FS_FCloseFile( f );
		return qfalse;
	}

	memset(spawnVars, 0, sizeof(spawnVars));
	trap_FS_Read( spawnVars, len, f );
	trap_FS_FCloseFile( f );
	as_debug("Loading additional spawnvars from %s (%d bytes)\n", filename, len);
	return qtrue;
}

char *G_AddSpawnVarToken( const char *string );
#define AS_SVF_VERSION 2
qboolean as_parseSpawnVars( qboolean getVersion ) {
	char			*token;
	char			key[MAX_TOKEN_CHARS];
	char			value[MAX_TOKEN_CHARS];
	static char		*p = spawnVars;

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
	
	// parse the opening brace
	token = COM_ParseExt(&p, qtrue);
	if ( !token || token[0] == 0 ) {
			as_debug ("Done loading external spawnvar file!\n" );
			return qfalse;
	}
	if ( token[0] != '{' ) {
		G_Error( "as_parseSpawnVars: found %s when expecting {",token );
		return qfalse;
	}

	if (getVersion) {
		qboolean	good = qfalse;

		token = COM_ParseExt(&p, qtrue);
		strcpy(key, token);
		if ( !key || key[0] == 0 ) {
			G_Error( "as_parseSpawnVars: EOF without closing brace" );
		}
		// parse value	
		token = COM_ParseExt(&p, qtrue);
		strcpy(value, token);

		if ( value[0] == '}' ) {
			G_Error( "as_parseSpawnVars: closing brace without data" );
		}
		if (!Q_stricmp(key, "version")) {
			if (strlen(value) > 0) {
				if (atoi(value) == AS_SVF_VERSION) {
					good = qtrue;;
				} else {
					G_Error( "as_parseSpawnVars: Bad version: %s. Expecting: %d\n", value, AS_SVF_VERSION);
				}
			} else {
				G_Error( "as_parseSpawnVars: NULL version!\n");
			}
		} else {
			G_Error( "as_parseSpawnVars: Version info not first entry!\n");
		}

		token = COM_ParseExt(&p, qtrue);
		strcpy(key, token);
		if ( !key || key[0] == 0 ) {
			G_Error( "as_parseSpawnVars: EOF without closing brace" );
		}
		if ( key[0] == '}' ) {
			if (good)
				return qtrue;
		}
		return qfalse;
	}

	while ( 1 ) {
		token = COM_ParseExt(&p, qtrue);
		strcpy(key, token);
		if ( !key || key[0] == 0 ) {
			G_Error( "as_parseSpawnVars: EOF without closing brace" );
		}
		if ( key[0] == '}' ) {
			break;
		}

		// parse value	
		token = COM_ParseExt(&p, qtrue);
		strcpy(value, token);

		if ( value[0] == '}' ) {
			G_Error( "as_parseSpawnVars: closing brace without data" );
		}

		if ( level.numSpawnVars == MAX_SPAWN_VARS ) {
			G_Error( "as_parseSpawnVars: MAX_SPAWN_VARS" );
			return qfalse;
		}
		if (!Q_stricmp(key, "disable")) {
			int		i;

			i = atoi(value);
			as_level.spawnVarGroupDisabled[i] = qtrue;
		} else {
			level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( key );
			level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( value );
			level.numSpawnVars++;
		}
	}

	return qtrue;
}

void as_cmd_Players_f( gentity_t *ent ) {
	int			i, j, spaces;
	gclient_t	*client;
	char		name[128];
	char		tmp[10];
	char		line[256];

	as_debug("as_cmd_Players_f() called\n");
	trap_SendServerCommand( ent-g_entities, "print \"#  'Name'\n\"");
	trap_SendServerCommand( ent-g_entities, "print \"----------------------------------------------\n\"");
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if (!client)
			continue;
		if (client->pers.connected == CON_DISCONNECTED)
			continue;

		as_cleanName(client->pers.netname, name, sizeof(name));

		Com_sprintf(tmp, sizeof(tmp), "%d", client->ps.clientNum);
		
		Com_sprintf(line, sizeof(line), "%s", tmp);

		spaces = 3 - strlen(tmp);

		for (j = 0; j < spaces; j++)
			Com_sprintf(line, sizeof(line), "%s ", line);

		Com_sprintf(line, sizeof(line), "%s'%s'", line, name);

		trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", line));
	}
}

// This is an overloaded function
// It increments the user's chat counter and
// then checks if the user is over the warn
// and/or kick limit.  If so, appropriate
// action is taken and logged ...
void as_trackChat (int clientNum) {
	gclient_t		*client;
	char			userinfo[MAX_INFO_STRING];
	char			*ip;

	client = &level.clients[clientNum];
	if (!client)
		return;

	if (g_entities[clientNum].r.svFlags & SVF_BOT)
		return;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
	ip = Info_ValueForKey(userinfo, "ip");

	client->q3as.chatCount++;

	if (as_spam_warn.integer && client->q3as.chatCount >= as_spam_warn.integer) {
		as_admin_log("User '%s' (IP = %s) warned for spamming\n", client->pers.netname, ip);
		trap_SendServerCommand(clientNum, va("cp \"%s\"", as_spam_message.string));
	}

	if (as_spam_kick.integer && client->q3as.chatCount >= as_spam_kick.integer) {
		as_admin_log("User '%s' (IP = %s) kicked for spamming\n", client->pers.netname, ip);
		trap_SendConsoleCommand( EXEC_INSERT, va("clientkick %d\n", clientNum));
	}
}

void as_updateChatStats(gentity_t *ent) {
	if (!(ent && ent->client))
		return;

	if (ent->client->q3as.lastChatCountReduce >= level.time)
		return;

	ent->client->q3as.lastChatCountReduce = level.time + as_spam_interval.integer;
	if (ent->client->q3as.chatCount > 0)
		ent->client->q3as.chatCount--;
}

void as_checkConfigTimeout(void){
	int		mapTime;

	if (strlen(as_rollback_config.string) == 0)
		return;

	mapTime = level.time - level.startTime;
	
	if (as_numConnectedHumans(qfalse) == 0) {
		// lastClientTime will be  -1 if we exec'ed default config and we're awaiting a new client
		if (as_level.lastClientTime != -1 && mapTime > as_level.lastClientTime + as_rollback_timeout.integer * 1000) {
			as_debug("Rolling back: %d %d %d\n", level.time, level.startTime, as_level.lastClientTime);
			as_level.lastClientTime = -1;
			as_admin_log("Executing rollback config (%s)\n", as_rollback_config.string);
			trap_SendConsoleCommand( EXEC_APPEND, va("exec %s\n", as_rollback_config.string) );
		}
	} else {
		as_level.lastClientTime = mapTime;
	}
}

// We use this to help make sure userinfo key=ip has a proper value in it.
// I've seen 'ip' not exist before, so we save it in a cvar if init is true.
// I've NEVER seen the IP be missing during first connect, but I have on
// subsequent connects.  Since the client is 'responsible' for sending this value
// There's no 'real' serverside fix for it except this function.
void as_setClientIP(int clientNum, qboolean init) {
	char		userinfo[MAX_INFO_STRING];
	char		*value;
	char		buf[MAX_IPSTRING];

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	if (g_entities[clientNum].r.svFlags & SVF_BOT) {	// bots get 127.0.0.1, period
		as_debug("Setting IP to 127.0.0.1 (bot)\n");
		Info_SetValueForKey(userinfo, "ip", "127.0.0.1");
		trap_SetUserinfo(clientNum, userinfo);
	} else if (init) {									// Store the userinfo value from key = 'ip' to cvar
		as_debug("Saving IP to cvar\n");
		value = Info_ValueForKey (userinfo, "ip");
		trap_Cvar_Set(va("as_ip%i", clientNum), value);
	} else {											// Read cvar created above
		as_debug("Setting IP from cvar\n");
		trap_Cvar_VariableStringBuffer(va("as_ip%i", clientNum), buf, sizeof(buf));
		Info_SetValueForKey(userinfo, "ip", buf);
		trap_SetUserinfo(clientNum, userinfo);
	}
}

static char *as_filterWord(char *text, const char *word) {
	char		orig[1024];	// unfettered copy of 'text'
	char		temp[1024];	// temp save buffer
	char		*p;			// to step through 'orig'
	int			i;			// loop var
	int			curWord;	// How far through 'word' we are
	qboolean	start;		// Have we matched the starting character of 'word' ?
	int			textSize;	// Size (storage) of 'text'
	int			wordLen;	// Length (strlen) of 'word'

	Q_strncpyz(orig, text, sizeof(orig));
	textSize = strlen(orig) + 1;
	memset(text, 0, textSize);
	wordLen = strlen(word);
	p = orig;

	start = qfalse;
	curWord = 0;
	memset(temp, 0, sizeof(temp));
	while (1) {
		if (curWord == wordLen) {	// Found a full match
			for (i = 0; i < wordLen; i++) {
				Com_sprintf(text, textSize, "%s%c", text, '*');
			}
			memset(temp, 0, sizeof(temp));
			curWord = 0;
			start = qfalse;
			continue;
		}

		if (!*p) {					// end of line; copy any remaining buffer out
			if (strlen(temp)) {
				Com_sprintf(text, textSize, "%s%s", text, temp);
				memset(temp, 0, sizeof(temp));
			}
			break;
		}

		if (!start) {				// Search for starting character
			if (tolower(*p) == word[0]) {	// got a match, save to buffer
				temp[0] = *p;
				start = qtrue;
				curWord++;
			} else {				// no match, write char out
				Com_sprintf(text, textSize, "%s%c", text, *p);
			}
			p++;
			continue;
		}

		// If we make it this far then we're in the midst of trying
		// to complete a match
		if (tolower(*p) == word[curWord]) {	// Matching character, save to buffer
			Com_sprintf(temp, sizeof(temp), "%s%c", temp, *p);
			curWord++;
			p++;
			continue;
		}

		if (*p == '^' && *(p + 1) && *(p + 1) != 'X') {	// skip (save directly to buffer) traditional colors
			Com_sprintf(temp, sizeof(temp), "%s%c%c", temp, *p, *(p + 1));
			p += 2;
			continue;
		}

		if (*p == '^' && strlen(p) >= 8 && *(p + 1) == 'X') {	// ... HTML
			Com_sprintf(temp, sizeof(temp), "%s%c%c%c%c%c%c%c%c", temp, *p, *(p + 1), *(p + 2), *(p + 3),
				*(p + 4), *(p + 5), *(p + 6), *(p + 7));
			p += 8;
			continue;
		}
	
		// All other conditions fail.  If we have a buffer, write buffer out.
		// We do NOT write current char out (or increment pointer) because
		// it might be the starting character of the word we're looking for
		if (strlen(temp)) {
			Com_sprintf(text, textSize, "%s%s", text, temp);
			memset(temp, 0, sizeof(temp));
			curWord = 0;
			start = qfalse;
		} else {  // All fail and no buffer, write this char out
			Com_sprintf(text, textSize, "%s%c", text, *p);
			p++;
		}
	}

	return text;
}

char *as_filterText(char *text) {
	fileHandle_t	f;
	int				len;
	char			buffer[32000];
	char			*p, *token;

	if (strlen(as_chat_filter_file.string) == 0)
		return text;

	as_debug("Running filter on chat text: %s\n", text);

	len = trap_FS_FOpenFile(as_chat_filter_file.string, &f, FS_READ);
	if ( !f ) {
		as_debug("Unable to read file: %s\n", as_chat_filter_file.string);
		return text;
	}

	if (len > sizeof(buffer)) {
		as_debug("File (%s) too big\n", as_chat_filter_file.string);
		trap_FS_FCloseFile(f);
		return text;
	}

	if (len == 0) {
		trap_FS_FCloseFile(f);
		as_debug("File (%s) is empty\n", as_chat_filter_file.string);
		return text;
	}

	memset(buffer, 0, sizeof(buffer));
	trap_FS_Read(buffer, sizeof(buffer), f);
	trap_FS_FCloseFile(f);

	p = buffer;
	while ( 1 ) {
		token = COM_Parse(&p);
		if (! as_checkToken(token) )
			break; // end of list
		text = as_filterWord(text, token);
	}
	return text;
}

// At some point I'll use this to help prevent
// 'stupid' team joins
qboolean as_canJoinTeam(gentity_t *ent, int team) {
	return qtrue;
}

static int as_whoIsFC(int team) {
	int			i;

	for (i = 0; i < level.maxclients; i++) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED )
			continue;
		if ( level.clients[i].sess.sessionTeam != team )
			continue;
		if ( level.clients[i].ps.powerups[PW_REDFLAG] || level.clients[i].ps.powerups[PW_BLUEFLAG] ||
			 level.clients[i].ps.powerups[PW_NEUTRALFLAG])
			 return i;
	}
	return -1;
}

void as_cmd_Follow_FC_f( gentity_t *ent, int team ) {
	int		i;
	char	arg[MAX_TOKEN_CHARS];


	if (!(g_gametype.integer == GT_CTF || g_gametype.integer == GT_1FCTF))
		return;

	trap_Argv( 1, arg, sizeof( arg ) );

	i = as_whoIsFC(team);
	if ( i == -1 )
		return;

	// can't follow self
	if ( &level.clients[ i ] == ent->client )
		return;

	// first set them to spectator
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR )
		SetTeam( ent, "spectator" );

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

#define as_isMonth(p)		( p && *(p) == 'M' && *((p)+1) && *((p)+1) == 'M' )
#define as_isDay(p)			( p && *(p) == 'D' && *((p)+1) && *((p)+1) == 'D' )
#define as_isYear2(p)		( p && *(p) == 'Y' && *((p)+1) && *((p)+1) == 'Y' && ((p)+2) && *((p)+2) != 'Y' )
#define as_isYear4(p)		( p && *(p) == 'Y' && *((p)+1) && *((p)+1) == 'Y' && ((p)+2) && *((p)+2) == 'Y' && ((p)+3) && *((p)+3) == 'Y' )
#define as_isHour(p)		( p && *(p) == 'h' && *((p)+1) && *((p)+1) == 'h' )
#define as_isMin(p)			( p && *(p) == 'm' && *((p)+1) && *((p)+1) == 'm' )
#define as_isSec(p)			( p && *(p) == 's' && *((p)+1) && *((p)+1) == 's' )
#define as_isHundredths(p)	( p && *(p) == 'z' && *((p)+1) && *((p)+1) == 'z' )

void as_formatTime(char *format, char *out, int sizeOut, qtime_t t) {
	char	*f;
	int		count;
	int		skip;

	as_debug("as_formatTime() called\n");

	memset(out, 0, sizeOut);

	f = format;
	count = 0;

	while (*f) {
		if (count + 4 > sizeOut) {
			printf("Overflow!\n");
			return;
		}

		skip = 2;
		if (as_isMonth(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_mon + 1);
		} else if (as_isDay(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_mday);
		} else if (as_isYear4(f)) {
			Com_sprintf(out, sizeOut, "%4d", t.tm_year + 1900);
			skip = 4;
		} else if (as_isYear2(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_year % 100);
		} else if (as_isHour(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_hour);
		} else if (as_isMin(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_min);
		} else if (as_isSec(f)) {
			Com_sprintf(out, sizeOut, "%02d", t.tm_sec);
		} else if (as_isHundredths(f)) {
			Com_sprintf(out, sizeOut, "00");
		} else {
			*out = *f;
			skip = 1;
		}
		count += skip;
		f += skip;
		out += skip;
	}
	as_debug("Exiting as_formatTime()\n");
}

void as_logTick(void) {
	qtime_t		t;
	char		ts[100];

	if (as_log_tick_interval.integer == 0)
		return;

	if (level.time % (as_log_tick_interval.integer * 1000) == 0 ) {
		trap_RealTime(&t);
		as_formatTime(as_log_tick_format.string, ts, sizeof(ts), t);
		G_LogPrintf("Q3AS: TICK: %s\n", ts);
		as_admin_log("TICK: %s\n", ts);
		as_debug("TICK: %s\n", ts);
	}
}

void as_checkTeams(void) {
	int			numRed = 0;
	int			numBlue = 0;
	int			i;
	int			rfc, bfc;

	if (g_gametype.integer < GT_TEAM)
		return;

	if (as_unbalanced_warning.integer == 0)
		return;

	if (as_level.nobalance == qtrue)
		return;

	if (level.warmupTime)
		return;

	// Allow this to run only once every 6 seconds
	if (level.time % 6000 != 0)
		return;

	for ( i = 0; i < level.maxclients; i++ ) {
		// make sure client is connected
		if ( level.clients[i].pers.connected != CON_CONNECTED )
			continue;

		// team should be TEAM_RED or TEAM_BLUE, not TEAM_SPECTATOR
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR )
			continue;

		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE)
			numBlue++;
		else
			numRed++;
	}

	rfc = as_whoIsFC(TEAM_RED);
	bfc = as_whoIsFC(TEAM_BLUE);

	if (abs(numBlue - numRed) >= as_unbalanced_warning.integer) {
		for ( i = 0; i < level.maxclients; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED )
				continue;
			if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR )
				continue;
			if ( i == rfc || i == bfc )
				continue;

			trap_SendServerCommand(i, va("cp \"Teams!\nUse\n/callvote togglebalance\nTo disable this message.\""));
		}
	}
}

static void as_rotateLogs(void) {
	qtime_t		t;
	char		ts[100];
	char		cwd[200];
	char		game[MAX_QPATH];
	char		spath[200];
	char		source[300];
	char		tpath[200];
	char		target[300];

	as_debug("Rotating logs...\n");

	trap_RealTime(&t);
	as_formatTime(as_log_rotate_format.string, ts, sizeof(ts), t);

	getcwd(cwd, sizeof(cwd));
	trap_Cvar_VariableStringBuffer("fs_game", game, sizeof(game));
	Com_sprintf(spath, sizeof(spath), "%s/%s", cwd, game);
	Com_sprintf(tpath, sizeof(tpath), "%s/%s/%s", cwd, game, as_log_rotate_directory.string);

	if (level.logFile) {
		Com_sprintf(source, sizeof(source), "%s/%s", spath, g_log.string);
		Com_sprintf(target, sizeof(target), "%s/%s.%s", tpath, g_log.string, ts);

		G_LogPrintf("Rotating to %s\n", target);
		trap_FS_FCloseFile(level.logFile);
		rename(source, target);
		if ( g_logSync.integer ) {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
		} else {
			G_LogPrintf("Rotated to %s\n", target);
		}
	}

	if (as_level.debugfile) {
		Com_sprintf(source, sizeof(source), "%s/%s", spath, as_debug_file.string);
		Com_sprintf(target, sizeof(target), "%s/%s.%s", tpath, as_debug_file.string, ts);

		as_debug("Rotating to %s\n", target);
		trap_FS_FCloseFile(as_level.debugfile);
		rename(source, target);
		as_openDebugLog();
		as_debug("Rotated to %s\n", target);
	}

	if (as_adminlog_file.string && strlen(as_adminlog_file.string)) {
		Com_sprintf(source, sizeof(source), "%s/%s", spath, as_adminlog_file.string);
		Com_sprintf(target, sizeof(target), "%s/%s.%s", tpath, as_adminlog_file.string, ts);
		as_admin_log("Rotating to %s\n", target);
		rename(source, target);
		as_admin_log("Rotated to %s\n", target);
	}
}

void as_checkLogRotation(void) {
	qtime_t				t;
	int					val;
	static qboolean		rotated = qfalse;

	if (strlen(as_log_rotate_interval.string) == 0)
		return;

	// Check every 0.5 seconds (to make sure we don't miss a second)
	if (level.time % 500 != 0)
		return;

	trap_RealTime(&t);

	switch (as_log_rotate_interval.string[0]) {
	case 'm':
		val = t.tm_sec;
		break;
	case 'h':
		val = t.tm_min + t.tm_sec;
		break;
	case 'd':
		val = t.tm_hour + t.tm_min + t.tm_sec;
		break;
	case 'w':
		val = t.tm_wday + t.tm_min + t.tm_sec + t.tm_hour;
		break;
	default:
		val = 1;
		break;
	}

	if (val == 0) {
		if (rotated == qfalse) {
			as_rotateLogs();
			rotated = qtrue;	// So we don't try to rotate twice in a given second
		}
	} else {
		rotated = qfalse;
	}
}

// 
void as_logOSP(char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];

	if (as_log_osp.integer == qfalse)
		return;

	va_start( argptr, fmt );
	vsprintf( string, fmt,argptr );
	va_end( argptr );

	G_LogPrintf(string);
}

void as_clientSpectatorTimer( gclient_t *client ) {
	int			timeleft;
	int			milliseconds;

	if ( ! as_max_spectator_time.integer )
		return;

	// Don't try to drop the hosting (IE listen server) person
	if ( client->pers.localClient )
		return;

	// Don't count unless they're a spectator
	if ( client->sess.sessionTeam != TEAM_SPECTATOR )
		return;

	// Get around the fact that this seems to be called multiple times per frame
	if ( level.time == client->q3as.lastSpecCheck )
		return;
	client->q3as.lastSpecCheck = level.time;

	milliseconds = 1000 / as_level.fps;
	client->sess.totalSpecTime += milliseconds;
	timeleft = ( as_max_spectator_time.integer * 1000 ) - client->sess.totalSpecTime;

	if ( timeleft <= 0 ) {
		trap_DropClient( client - level.clients, "Dropped due to extended spectating" );
		return;
	}

	if ( timeleft <= as_max_spectator_warn.integer * 1000 ) {
		if (level.time % 1000 == 0) {
			client->q3as.linger.endTime = 0;	// Clear any MOTD that'll get in the way of the message below
			trap_SendServerCommand( client - level.clients, va("cp \"%d seconds until spectator drop!\n\"", timeleft / 1000 ));
		}
	}
}

// Percentage (0-100) of REAL people connectED versus connectING (connected/connected+connecting)
int as_percentConnected (void) {
	int				i;
	int				numConnected = 0;
	int				numConnecting = 0;

	for (i = 0; i < level.maxclients; i++) {
		if (level.clients[i].pers.connected == CON_DISCONNECTED)
			continue;
		if (level.gentities[i].r.svFlags & SVF_BOT)
			continue;
		if (level.clients[i].pers.connected == CON_CONNECTING)
			numConnecting++;
		if (level.clients[i].pers.connected == CON_CONNECTED)
			numConnected++;
	}

	if (numConnecting + numConnected == 0)
		return 0;

	return (100 * numConnected/(float)(numConnecting + numConnected));
}

// Change method of death 
void as_translateMOD(int *mod) {
	char		newMod[3];

	trap_Cvar_VariableStringBuffer(va("as_alter_mod_%d", *mod), newMod, sizeof(newMod));
	if (strlen(newMod)) {
		*mod = atoi(newMod);
	}
}

