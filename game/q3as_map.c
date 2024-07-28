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

as_mapRotation_t	rotation;
as_mapRotation_t	backupRotation;

typedef struct {
	char						*name;
	int							ofs;
	int							size;
	as_mapRotationFieldTypes_t	type;
	int							flag;
} as_field_t;

as_field_t as_mapStanza_fields[] = {
	{"map", AS_FOFS(map), AS_SIZE(map), AS_MR_STRING, AS_MS_HAVEMAP},
	{"minplayers", AS_FOFS(minplayers), AS_SIZE(minplayers), AS_MR_INT, AS_MS_HAVEMIN},
	{"maxplayers", AS_FOFS(maxplayers), AS_SIZE(maxplayers), AS_MR_INT, AS_MS_HAVEMAX},
	{"disabled", AS_FOFS(disabled), AS_SIZE(disabled), AS_MR_BOOLEAN, AS_MS_HAVEDIS},
	{"notvoteable", AS_FOFS(notvoteable), AS_SIZE(notvoteable), AS_MR_BOOLEAN, AS_MS_HAVENOT},
	{"exec", AS_FOFS(exec), AS_SIZE(exec), AS_MR_STRING, AS_MS_HAVEEXE},
	{"id", AS_FOFS(id), AS_SIZE(id), AS_MR_STRING, AS_MS_HAVEID},
	{"execifvoted", AS_FOFS(execifvoted), AS_SIZE(execifvoted), AS_MR_BOOLEAN, AS_MS_HAVEEIF},
	{"template", AS_FOFS(templates), AS_SIZE(templates), AS_MR_ARRAYSTRINGS, AS_MS_HAVETEM},
	{"define", AS_FOFS(defTemplate), AS_SIZE(defTemplate), AS_MR_STRING, AS_MS_HAVEDEF},

	{NULL}
};

// Proto's we need from other .c files ...
const char *G_GetArenaInfoByMap( const char *map );
void G_LoadArenas( void );

/**************************************************************************************
 *	Private functions
 **************************************************************************************/

// Does this map (bsp) exist?
static qboolean as_mapExistsOnDisk(char *checkMap) {
	int			numMaps, i;
	int			dirlen;
	char		dirlist[8192];
	char		*dirptr;
	char		map[MAX_QPATH];

	numMaps = trap_FS_GetFileList("maps", ".bsp", dirlist, sizeof(dirlist));
	dirptr = dirlist;
	for (i = 0; i < numMaps; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		COM_StripExtension(dirptr, map);
		if (Q_stricmp(map, checkMap) == 0)
			return qtrue;
	}
	return qfalse;
}

// Set rotation system and all maps (in-memory copy) to
// reflect an unused state
static void as_initRotation(void) {
	as_debug("as_initRotation called\n");
	memset( &rotation, 0, sizeof( rotation ) );
	rotation.numMaps = -1;
	rotation.currentMapNum = -1;
	rotation.numTemplates = AS_TEMPLATE_USER - 1;	// Count the system templates!

	Q_strncpyz(rotation.templates[AS_TEMPLATE_AUTOLOAD].templateName, "autoload", sizeof(rotation.templates[AS_TEMPLATE_AUTOLOAD].templateName));
	Q_strncpyz(rotation.templates[AS_TEMPLATE_DEFAULT].templateName, "default", sizeof(rotation.templates[AS_TEMPLATE_DEFAULT].templateName));
}

// Return array index containing template name.  Return -1 if not found
static int as_findTemplateByName(char *name) {
	int				i;

	for (i = 0; i < AS_MAX_MAP_TEMPLATES; i++) {
		if (Q_stricmp(rotation.templates[i].templateName, name) == 0)
			return i;
	}

	G_Printf("WARNING: Unable to find template named '%s'\n", name);
	return -1;
}

// load the key/value pair into the appropriate spot in as_mapStanza_t
static int as_ParseMapRotationField( const char *key, const char *value, as_mapStanza_t *rot ) {
	as_field_t		*f;
	byte			*b;
	char			*ptr;

	as_debug("Loading key/value: %s -> %s\n", key, value);
	for ( f=as_mapStanza_fields ; f->name ; f++ ) {
		if ( !Q_stricmp(f->name, key) ) {
			b = (byte *)rot;

			switch( f->type ) {
			case AS_MR_STRING:
				Q_strncpyz((char *)(b+f->ofs), value, strlen(value) + 1);
				break;
			case AS_MR_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case AS_MR_BOOLEAN:
				*(qboolean *)(b+f->ofs) = atoi(value);
				break;
			case AS_MR_ARRAYSTRINGS:
				{
					int			numElements;
					int			count = 0;

					numElements = f->size / MAX_QPATH;
					while (count < numElements) {
						ptr = (char *)(b+f->ofs + MAX_QPATH * count);
						if (strlen(ptr) == 0) {
							Q_strncpyz(ptr, value, MAX_QPATH);
							break;
						}
						count++;
					}
					if (count == numElements) {
						COM_ParseError("Too many %s's found", key);
						return AS_MRE_FATAL;
					}
				}
				break;
			default:
				COM_ParseError("(INTERNAL) Invalid type (%d) for key (%s)!", key, f->type);
				return AS_MRE_FATAL;
			}
			rot->loadedMask |= f->flag;
			return AS_MRE_NONE;
		}
	}
	// Should only reach if key isn't found
	COM_ParseError("Invalid key name: '%s'", key);
	return AS_MRE_FATAL;
}

// From p, load a key/value pair into 'key' and 'val'
static int as_fetchKeypair( char **p, char *key, int keysize, char *val, int valsize ) {
	char		*token;

	*key = *val = 0;

	token = COM_Parse(p);
	if (Q_stricmp(token, "}") == 0)				// no key/value pair here; we've reached end of stanza
		return AS_MRE_DONE;

	if (!as_checkToken(token)) {				// no data for key = premature end
		COM_ParseError("EOF while searching for key");
		return AS_MRE_FATAL;
	}

	Q_strncpyz(key, token, keysize);

	token = COM_Parse(p);
	if (Q_stricmp(token, "}") == 0) {			// a "}" for value means premature end
		COM_ParseError("Found '}' while looking for a key's value");
		return AS_MRE_FATAL;
	}
	if (!as_checkToken(token)) {				// no data for val = premature end
		COM_ParseError("EOF while searching for value");
		return AS_MRE_FATAL;
	}

	Q_strncpyz(val, token, valsize);

	return AS_MRE_NONE;
}

// Step through the next set of braces, loading that stanza into memory
// Then, depending on stanza type, copy into map rotation or template definitions
static int as_loadMapStanza( char **p ) {
	char			*token;
	char			key[128];
	char			value[128];
	as_map_t		*curmap;
	as_mapTemplate_t	*curtemplate;
	int				retVal;
	as_mapStanza_t	stanza;

	memset(&stanza, 0, sizeof(stanza));

	token = COM_Parse(p);

	if (token[0] != '{')						// Need opening brace to start
		return AS_MRE_DONE;						// This isn't the best error value...

	while ( 1 ) {
		retVal = as_fetchKeypair(p, key, sizeof(key), value, sizeof(value));
		if (retVal == AS_MRE_DONE)				// Done with this stanza
			break;

		if (retVal != AS_MRE_NONE)				// Some sort of error ocurred
			return retVal;

		// Load up the key/value pair
		retVal = as_ParseMapRotationField(key, value, &stanza);
		if (retVal != AS_MRE_NONE)
			return retVal;
	}

	// If this was a map, make sure nothing extraneous was defined
	if (stanza.loadedMask & AS_MS_HAVEMAP) {
		if (stanza.loadedMask & ( AS_MS_HAVEEXE | AS_MS_HAVEDEF | AS_MS_HAVEMIN | AS_MS_HAVEMAX |
								   AS_MS_HAVEDIS | AS_MS_HAVENOT | AS_MS_HAVEEIF )) {
			COM_ParseError("Map definitions can *only* contain 'map', 'template', and 'id'!\n");
			return AS_MRE_FATAL;
		}
	}

	// If this was a template, make sure nothing extraneous was defined
	if (stanza.loadedMask & AS_MS_HAVEDEF) {
		if (stanza.loadedMask & ( AS_MS_HAVEMAP | AS_MS_HAVEID | AS_MS_HAVETEM )) {
			COM_ParseError("Template definintions cannot contain 'map', 'id', or 'template'!\n");
			return AS_MRE_FATAL;
		}
	}

	// If this was a map, does map exist on disk?
	if (stanza.loadedMask & AS_MS_HAVEMAP && as_mapExistsOnDisk(stanza.map) == qfalse) {
		COM_ParseError("Map '%s' doesn't exist!\n", stanza.map);
		return AS_MRE_FATAL;
	}

	// Now that we've loaded the stanza, Load it into memory based upon it's type
	if (stanza.loadedMask & AS_MS_HAVEMAP) {			// This is a map
		int				i;

		rotation.numMaps++;
		if (rotation.numMaps >= AS_MAX_MAPROT) {		// numMaps starts at 0
			COM_ParseError("Too many maps defined.  MAX = %d", AS_MAX_MAPROT);
			return AS_MRE_FATAL;
		}
		curmap = &(rotation.maps[rotation.numMaps]);
		Q_strncpyz(curmap->map, stanza.map, sizeof(curmap->map));
		Q_strncpyz(curmap->id, stanza.id, sizeof(curmap->id));
		curmap->numTemplates = -1;
		for (i = 0; i < AS_MAX_TEMPLATES_PER_MAP; i++) {
			if (strlen(stanza.templates[i])) {
				curmap->templateIDs[i] = as_findTemplateByName(stanza.templates[i]);
				if (curmap->templateIDs[i] == -1) {
					COM_ParseError("Template '%s' doesn't exist!\n", stanza.templates[i]);
					return AS_MRE_FATAL;
				}
				curmap->numTemplates++;
			}
		}
		curmap->lastRotationNum = 0;
		curmap->autoLoaded = qfalse;
	} else if (stanza.loadedMask & AS_MS_HAVEDEF) {	// This is a template
		if (Q_stricmp(stanza.defTemplate, "autoload") == 0) {
			curtemplate = &(rotation.templates[AS_TEMPLATE_AUTOLOAD]);
		} else if (Q_stricmp(stanza.defTemplate, "default") == 0) {
			curtemplate = &(rotation.templates[AS_TEMPLATE_DEFAULT]);
		} else {
			rotation.numTemplates++;
			if (rotation.numTemplates >= AS_MAX_MAP_TEMPLATES) {	// numTemplates starts at 0
				COM_ParseError("Too many templates defined.  MAX = %d", AS_MAX_MAP_TEMPLATES);
				return AS_MRE_FATAL;
			}
			curtemplate = &(rotation.templates[rotation.numTemplates]);
		}
		
		curtemplate->disabled = stanza.disabled;
		curtemplate->execifvoted = stanza.execifvoted;
		curtemplate->maxplayers = stanza.maxplayers;
		curtemplate->minplayers = stanza.minplayers;
		curtemplate->notvoteable = stanza.notvoteable;
		Q_strncpyz(curtemplate->exec, stanza.exec, sizeof(curtemplate->exec));
		Q_strncpyz(curtemplate->templateName, stanza.defTemplate, sizeof(curtemplate->templateName));
		curtemplate->loadedMask = stanza.loadedMask;
	} else {											// Yikes! Neither template nor a map
		COM_ParseError("Stanza did not contain a 'define' or a 'map' key");
		return AS_MRE_FATAL;
	}
	return AS_MRE_NONE;
}

// Return space-separated list of gametype names the map
// in question allows based on arenas.txt or mapname.arena
// If no arena file/info is found, and if as_maprotation_autoload
// is set to AS_AL_ALL, return all gametypes
static char *as_getValidGametypesForMap(char *map) {
	const char		*arenainfo;
	char			*strValue;

	arenainfo = G_GetArenaInfoByMap( map );
	if ( !arenainfo ) {
		switch (as_maprotation_autoload.integer) {
		case AS_AL_ALL:
			return ("ffa tournament single team ctf oneflag obelisk harvester teamtournament");
			break;
		default:
			return "";
			break;
		}
	}

	strValue = Info_ValueForKey( arenainfo, "type" );
	return (strValue);

}

// Check 'gametypes' (space-separated list of gametypes) to see if any are allowed
// by as_maprotation_gametype_mask
static qboolean as_isMapGametypeCompatible (char *gametypes) {
	static char *names[] = {"ffa", "tournament", "single", "team", "ctf", "oneflag", "obelisk", "harvester", "teamtournament"};
	char		*name;
	int			i;
	char		*token, *p;

	for (i = GT_FFA; i < GT_MAX_GAME_TYPE; i++) {
		if (i == GT_SINGLE_PLAYER)
			continue;
		if (! ((1 << i) & as_maprotation_gametype_mask.integer))
			continue;

		name = names[i];
		p = gametypes;
		while (*p) {
			token = COM_Parse(&p);
			if (!as_checkToken(token))
				break;
			if (Q_stricmp(token, name) == 0)
				return qtrue;
		}
	}
	return qfalse;
}

// Grab a list of all .bsp files in the maps/ dir and load
// them into the in-memory 'rotation' structure.  Do this
// only if this feature is turned on.  Only load maps
// that haven't already been defined in as_maprotation_file
static int as_autoLoadMaps(void) {
	int			numMaps, i;
	int			dirlen;
	char		dirlist[8192];
	char		*dirptr;
	char		mapName[MAX_QPATH];
	int			count = 0;
	char		*gametypes;

	G_LoadArenas();		// This loads arenas.txt and *.arenas so we can see what
						// gametypes maps are designed for

	numMaps = trap_FS_GetFileList("maps", ".bsp", dirlist, sizeof(dirlist));
	dirptr = dirlist;
	for (i = 0; i < numMaps; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		COM_StripExtension(dirptr, mapName);
		if (Q_stricmp(mapName, "test_bigbox") == 0)
			continue;
		if (Q_stricmp(mapName, "texturegrab") == 0)
			continue;
		if (as_numForMap(mapName) == -1) {
			gametypes = as_getValidGametypesForMap(mapName);
			if (as_isMapGametypeCompatible(gametypes)) {
				as_debug("Autoloading: '%s'\n", mapName);
				rotation.numMaps++;
				if (rotation.numMaps >= AS_MAX_MAPROT) {
					G_Printf("WARNING: Unable to autoload any more maps.  Max (%d) reached\n", AS_MAX_MAPROT);
					break;
				}
				Q_strncpyz(rotation.maps[rotation.numMaps].map, mapName, sizeof(rotation.maps[rotation.numMaps].map));
				rotation.maps[rotation.numMaps].numTemplates = -1;
				rotation.maps[rotation.numMaps].autoLoaded = qtrue;
				count++;
			} else {
				as_debug("Autoload skipping '%s' - incompatible gametype\n", mapName);
			}
		} else {
			as_debug("Autoload skipping '%s' - already defined\n", mapName);
		}
	}
	G_Printf("%d maps autoloaded\n", count);
	return AS_MRE_NONE;
}

// Main custom map rotation loader
static int as_loadMapRotationFromFile( char *filename ) {
	int				len;
	fileHandle_t	f;
	char			buf[AS_MAX_MAPFILE_SIZE];
	char			*p;
	int				retVal;
	int				count = 0;

	as_debug("Loading map rotation from %s\n", filename);

	// load up the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		G_Printf("Unable to locate '%s'\n", filename);
		return AS_MRE_FATAL;
	}

	if ( len >= sizeof(buf) ) {
		trap_FS_FCloseFile( f );
		G_Printf("File '%s' too big.  MAX = %d\n", filename, AS_MAX_MAPFILE_SIZE);
		return AS_MRE_FATAL;
	}

	memset(buf, 0, sizeof(buf));
	trap_FS_Read( buf, len, f );
	trap_FS_FCloseFile( f );

	p = buf;

	COM_BeginParseSession(filename);
	// cycle through the file's stanza's
	while ( 1 ) {
		retVal = as_loadMapStanza(&p);
		if (retVal == AS_MRE_NONE) {
			count++;
			continue;
		}
		if (retVal == AS_MRE_DONE)
			break;
		// Otherwise, we got a problem...
		return retVal;
	}

	G_Printf("%d template(s) loaded from '%s'\n", rotation.numTemplates, as_maprotation_file.string);
	G_Printf("%d maps(s) loaded from '%s'\n", rotation.numMaps + 1, as_maprotation_file.string);

	return AS_MRE_NONE;
}

// saves rotation struct to disk
static void as_saveCurrentMapInfo(void) {
	fileHandle_t	f;

	as_debug("Saving rotation struct to disk\n");

	trap_FS_FOpenFile( as_maprotation_state_file.string, &f, FS_WRITE );
	if (!f) {
		G_Error("Unable to save current map state information to %s!\n", as_maprotation_state_file.string);
	} else {
		trap_FS_Write(&rotation, sizeof(rotation), f);
		trap_FS_FCloseFile(f);
	}
}

// reads rotation struct from disk
static void as_readCurrentMapInfo(void) {
	fileHandle_t		f;
	int					num;

	as_debug("levelTime != 0 (%d), pulling in contents of %s\n", level.time, as_maprotation_state_file.string);
	num = trap_FS_FOpenFile(as_maprotation_state_file.string, &f, FS_READ);
	if (!f) {
		G_Error("Unable to read current map information from %s!\n", as_maprotation_state_file.string);
	} else if (num != sizeof(rotation)) { 
		G_Error("Size of data read from %s does not match the struct size!\n", as_maprotation_state_file.string);
	} else {
		trap_FS_Read(&rotation, sizeof(rotation), f);
		trap_FS_FCloseFile(f);
	}
}

// Copy source to dest, but ONLY the fields outlined in the bitmapped value 'flags'
static void as_copyTemplate(const as_mapTemplate_t source, as_mapTemplate_t *dest, int flags) {
	int			i;
	int			curFlag;

	// Always copy loadedMask
	dest->loadedMask = source.loadedMask;

	for (i = 0; i < 32; i++) {	// run through every bitflag we could ever define in a 32-bit world ...
		curFlag = ( 1 << i );
		if ( flags & curFlag ) {
			switch ( curFlag ) {
			case AS_MS_HAVEMIN:
				dest->minplayers = source.minplayers;
				break;
			case AS_MS_HAVEMAX:
				dest->maxplayers = source.maxplayers;
				break;
			case AS_MS_HAVEDIS:
				dest->disabled = source.disabled;
				break;
			case AS_MS_HAVENOT:
				dest->notvoteable = source.notvoteable;
				break;
			case AS_MS_HAVEEXE:
				Q_strncpyz(dest->exec, source.exec, sizeof(dest->exec));
				break;
			case AS_MS_HAVEEIF:
				dest->execifvoted = source.execifvoted;
				break;
			case AS_MS_HAVEDEF:
				Q_strncpyz(dest->templateName, source.templateName, sizeof(dest->templateName));
				break;
			// None of these below should be in a template to be copied!
			case AS_MS_HAVEMAP:
			case AS_MS_HAVEID:
			case AS_MS_HAVETEM:
			default:
				break;
			}
		}
	}
}

// Since maps can use multiple templates, we must 'merge' the 
// templates into one and return it.
static void as_fetchTemplate(int mapNum, as_mapTemplate_t *t) {
	int		i;
	int		templateNum;

	as_debug("as_fetchTemplate called on map number %d (%s)\n", mapNum, rotation.maps[mapNum].map);

	as_debug("Starting with 'default' template\n");

	as_copyTemplate(rotation.templates[AS_TEMPLATE_DEFAULT], t, MAX_QINT);

	if (rotation.maps[mapNum].numTemplates == -1) {					// If this map has no templates ...
		if (rotation.maps[mapNum].autoLoaded == qtrue) {			// If autoloaded, ALWAYS use autoload template, period
			as_debug("Overriding with 'autoload' template\n");
			as_copyTemplate(rotation.templates[AS_TEMPLATE_AUTOLOAD], t, MAX_QINT);
		} 
	} else {
		for (i = 0; i <= rotation.maps[mapNum].numTemplates; i++) {	// For every template attached to this map ...
			templateNum = rotation.maps[mapNum].templateIDs[i];
			as_debug("Merging template '%s'\n", rotation.templates[templateNum].templateName);
			// Copy in only the settings defined in the template, overriding any existing settings
			as_copyTemplate(rotation.templates[templateNum], t, rotation.templates[templateNum].loadedMask);
		}
	}
	as_debug("Final settings: min %d max %d dis %d not %d exec '%s' eif %d\n",
		t->minplayers, t->maxplayers, t->disabled, t->notvoteable, t->exec, t->execifvoted);
}

// pick a new map number based on as_maprotation_type
// sets rotation.currentMapNum to picked number
static int as_newMapNum(void) {
	int		i;

	i = 0;

	switch (as_maprotation_type.integer) {
	case AS_RT_RANDOM:
		i = rand() % (rotation.numMaps);
		break;
	case AS_RT_SEQUENTIAL:
		i = rotation.currentMapNum + 1;
		if ( i > rotation.numMaps )
			i = 0;
		break;
	}
	rotation.currentMapNum = i;
	return i;
}

// return first matching map # for given map ID
static int as_numForMapID(char *ID) {
	int		i;

	if (!ID)
		return -1;

	for (i = 0; i < AS_MAX_MAPROT; i++) {
		if (rotation.maps[i].id) {
			if (Q_stricmp(rotation.maps[i].id, ID) == 0) {
				return i;
			}
		}
	}
	return -1;
}

// exec a map based on map number
// save state information
static qboolean as_execMapByNum(int mapNum, qboolean voted) {
	as_mapTemplate_t	t;

	as_debug("Attempting to execute map id %d\n", mapNum);

	as_fetchTemplate(mapNum, &t);

	if ( as_isMapAllowed(mapNum, voted) ) {
		// twiddle counters
		rotation.numRotations++;
		rotation.maps[mapNum].lastRotationNum = rotation.numRotations;
		// exec settings
		if (!voted || t.execifvoted == qtrue) {
			if (t.loadedMask & AS_MS_HAVEEXE) {
				trap_SendConsoleCommand( EXEC_APPEND, va("exec %s\n", t.exec) );
			}
		}
		// If preserve = no, move 'current map number' pointer to the voted map
		if (voted && as_maprotation_vote_preserve.integer == 0)
			rotation.currentMapNum = mapNum;
		// save current map info to disk
		as_saveCurrentMapInfo();
		// Schedule map for exec
		as_debug("Scheduling map \"%s\" to be executed ...\n", rotation.maps[mapNum].map);
		trap_Cvar_Set("as_scheduledMap", rotation.maps[mapNum].map);
		return qtrue;
	}
	return qfalse;
}

// Used to call first map from Q3AS rotation when server first starts
static void as_jumpstartQ3ASMapRotation(void) {
	if (rotation.numMaps != -1 && level.time == 0) {
		as_debug("Jumpstarting map rotation\n");
		rotation.currentMapNum = rotation.numMaps;
		as_nextMap();
	}
}

/**************************************************************************************
 *	Exported functions
 **************************************************************************************/

// exec a map based on map name
// calls as_execMapByNum()
qboolean as_execMapByName(char *map, qboolean voted) {
	return as_execMapByNum(as_numForMap(map), voted);
}

// exec a map based on map ID
// calls as_execMapByNum()
qboolean as_execMapByID(char *ID) {
	return as_execMapByNum(as_numForMapID(ID), qfalse);
}

// try to find appropriate next map and exec it
void as_nextMap(void) {
	int			i, j, numTries, oldNum;
	as_mapTemplate_t	t;
	int			maxTries = rotation.numMaps + 1;

	oldNum = rotation.currentMapNum;
	as_debug("as_nextMap: old map number = %d\n", oldNum);

	j = as_newMapNum();
	numTries = 0;

	if (as_maprotation_type.integer > AS_RT_SEQUENTIAL)
		as_debug("Attemping to pick random map\n");
	else
		as_debug("Picking next sequential map\n");

	for (i = 0; i < maxTries; i++) {
		numTries++;
		as_fetchTemplate(j, &t);

		if (as_isMapAllowed(j, qfalse) == qfalse) {
			as_debug("Map picked (id = %d) is not allowed, trying again...\n", j);
			j = as_newMapNum();
			continue;
		}
		if (t.maxplayers != 0) {
			if ( level.numConnectedClients > t.maxplayers ) {
				as_debug("Too many players for map picked (id = %d), trying again...\n", j);
				j = as_newMapNum();
				continue;
			}
		}
		if (t.minplayers != 0) {
			if ( level.numConnectedClients < t.minplayers ) {
				as_debug("Not enough players for map picked (id = %d), trying again...\n", j);
				j = as_newMapNum();
				continue;
			}
		}
		if (as_maprotation_type.integer > AS_RT_SEQUENTIAL) {
			// try not to pick the same map ...
			if ( j == oldNum ) {
				as_debug("Map picked (id = %d) == last map played (%d), trying again...\n", j, oldNum);
				j = as_newMapNum();
				continue;
			}
			// try not to pick a map recently played...
			as_debug("rotation.numRotations = %d; rotation.maps[j].lastRotationNum = %d; j = %d\n",
				rotation.numRotations, rotation.maps[j].lastRotationNum, j);

			// rotation.numRotations					= total # of map changes since server started
			// j										= proposed map ID/#
			// rotation.maps[j].lastRotationNum			= last 'time' proposed map was played

			if ( rotation.numRotations - rotation.maps[j].lastRotationNum < as_maprotation_spacing.integer ) {
				if ( rotation.maps[j].lastRotationNum != 0 ) {
					as_debug("Map picked (id = %d, last was %d, cur is %d) was played too recently, trying again...\n",
							 j, rotation.maps[j].lastRotationNum, rotation.numRotations);
						j = as_newMapNum();
						continue;
				}
			}
		}
		// if we make it this far, we've got a good one
		break;
	}
	if (numTries == maxTries) {
		as_debug("Unable to find suitable next map. Continuing with current map\n");
		rotation.currentMapNum = oldNum;
		as_saveCurrentMapInfo();
		trap_SendConsoleCommand( EXEC_APPEND, va("map_restart 0\n") );
	} else {
		as_debug("New map number = %d\n", rotation.currentMapNum);
		as_execMapByNum(rotation.currentMapNum, qfalse);
	}
}

// checks to make sure the map can be used (useable in voting or useable in
// normal rotation?)
qboolean as_isMapAllowed(int mapNum, qboolean voted) {
	as_mapTemplate_t	t;

	as_fetchTemplate(mapNum, &t);

	if (mapNum == -1) {
		as_debug("Map not valid: mapNum is -1\n");
		return qfalse;
	}

	if (mapNum <= rotation.numMaps) {
		if (voted) {
			if (t.notvoteable == qtrue) {
				as_debug("Map (%d: %s) not valid: notvoteable is set\n", mapNum, rotation.maps[mapNum].map);
				return qfalse;
			}
		} else {
			if (t.disabled == qtrue) {
				as_debug("Map (%d: %s) disabled.\n", mapNum, rotation.maps[mapNum].map);
				return qfalse;
			}
		}
		return qtrue;
	}
	as_debug("Map not valid (%d > %d)\n", mapNum, rotation.numMaps);
	return qfalse;
}

// return first matching map # for given map name
int as_numForMap(char *map) {
	int		i;

	if (!map)
		return -1;

	for (i = 0; i < AS_MAX_MAPROT; i++) {
		if (rotation.maps[i].map) {
			if (Q_stricmp(rotation.maps[i].map, map) == 0) {
				return i;
			}
		}
	}
	return -1;
}

// Load from file, or from memory, or reload
// Then run autoloader
void as_loadQ3ASMapRotation( qboolean reload ) {
	if (strlen(as_maprotation_file.string) == 0 && as_maprotation_autoload.integer == AS_AL_NONE) {
		as_initRotation();
		return;
	}

	if (strlen(as_maprotation_state_file.string) == 0)
		G_Error("You must set as_maprotation_state_file to a unique filename to use Q3AS rotation features");

	if (level.time != 0 && reload == qfalse) {
		as_readCurrentMapInfo();
		return;
	}

	if (reload)
		memcpy(&backupRotation, &rotation, sizeof(backupRotation));
	if (level.time == 0 || reload)
		as_initRotation();

	// Load from a file
	if (strlen(as_maprotation_file.string)) {	// We have a file to load from; load it
		as_debug("as_maprotation_file is set\n");
		if (level.time == 0) {					// Server startup
			as_debug("levelTime == 0\n");
			if (as_loadMapRotationFromFile(as_maprotation_file.string) != AS_MRE_NONE) {
				G_Error("Unable to load map rotation");
			}
		} else {								// NOT server startup
			as_debug("levelTime != 0\n");
			if (reload) {						// Reloading from config while server is active
				as_debug("reload = true\n");
				if (as_loadMapRotationFromFile(as_maprotation_file.string) != AS_MRE_NONE) {
					G_Printf("Reload failed\n");
					memcpy(&rotation, &backupRotation, sizeof(rotation));
					return;
				}
			}
		}
	}
	
	// Autoload
	if (as_maprotation_autoload.integer > AS_AL_NONE) {
		as_debug("Autoload is set\n");
		if (level.time == 0) {					// Server startup
			if (as_autoLoadMaps() != AS_MRE_NONE) {
				G_Error("Unable to autoload maps");
			}
		} else {								// NOT server startup
			as_debug("levelTime != 0\n");
			if (reload) {						// Reloading from autoload while server is active
				as_debug("reload = true\n");
				if (as_autoLoadMaps() != AS_MRE_NONE) {
					G_Printf("Reload failed\n");
					memcpy(&rotation, &backupRotation, sizeof(rotation));
					return;
				}
			}
		}
	}

	// Final check & report + jumpstart
	if (rotation.numMaps == -1) {
		if (reload) {
			G_Printf("Reload failed: No maps defined!\n");
			return;
		} else {
			G_Error("No maps defined!");
		}
	} else {
		G_Printf("%d maps loaded\n", rotation.numMaps + 1);
		as_jumpstartQ3ASMapRotation();
	}
}

void as_execScheduledMap(void) {
	char		s[MAX_QPATH];

	trap_Cvar_VariableStringBuffer("as_scheduledMap", s, sizeof(s));
	if (strlen(s)) {
		as_debug("Executing scheduled map change ...\n");
		trap_SendConsoleCommand( EXEC_APPEND, va("map %s\n", s) );
		trap_Cvar_Set("as_scheduledMap", "");
	}
}
