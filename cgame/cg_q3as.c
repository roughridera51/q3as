#include "cg_local.h"
#include "../ui/ui_shared.h"
#include "cg_q3as.h"

// Externals
extern float CG_DrawScores( float y );
extern float CG_DrawPowerups( float y );
extern int CG_DrawPickupItem( int y );
extern void CG_ScanForCrosshairEntity( void );
extern void CG_SetSelectedPlayerName();
extern int CG_GetSelectedPlayer();
extern void CG_DrawSelectedPlayerHead( rectDef_t *rect, qboolean draw2D, qboolean voice );
extern void CG_ForceModelChange( void );

// Globals
as_cvars_t		as_cv;
lagometer_t	lagometer;

// CVARS
vmCvar_t	sta_gJerseys;
vmCvar_t	sta_gSpectatorJerseys;
vmCvar_t	sta_hSpectatorOverlay;
vmCvar_t	sta_hSpectatorMessage;
vmCvar_t	sta_hCrosshairInfo;
vmCvar_t	sta_hTimerCountDown;
vmCvar_t	sta_hTimerPosition;
vmCvar_t	sta_hFPSPosition;
vmCvar_t	sta_hLagometerPosition;
vmCvar_t	sta_hAttackerPosition;
vmCvar_t	sta_hSpeedometerPosition;
vmCvar_t	sta_hTeamOverlayPosition;
vmCvar_t	sta_hTeamOverlayNameWidth;
vmCvar_t	sta_hTeamOverlayLocationWidth;
vmCvar_t	sta_hTeamOverlaySlots;
vmCvar_t	sta_hTeamOverlayInfo;
vmCvar_t	sta_hTeamOverlayCharHeight;
vmCvar_t	sta_hTeamOverlayCharWidth;
vmCvar_t	sta_hAmmo;
vmCvar_t	sta_mJumpSound;
vmCvar_t	sta_mFootsteps;
vmCvar_t	sta_gTeamColoredRails;
vmCvar_t	sta_gTeamColoredPlasma;
vmCvar_t	sta_gTeamColoredLightning;
vmCvar_t	sta_hSpeedometerUnits;
vmCvar_t	sta_hAutoTeamInfo;
vmCvar_t	sta_hCrosshairColor;
vmCvar_t	sta_gNoProjectileTrail;
vmCvar_t	sta_gNoPicMip;
vmCvar_t	sta_hMiscCharWidth;
vmCvar_t	sta_hMiscCharHeight;
vmCvar_t	sta_hChatMaxLines;
vmCvar_t	sta_hChatMaxColumns;
vmCvar_t	sta_hChatTime;
vmCvar_t	sta_hChatNewColors;
vmCvar_t	sta_hChatCharWidth;
vmCvar_t	sta_hChatCharHeight;
vmCvar_t	sta_gForceTeamModel;
vmCvar_t	sta_gTeamModel;
vmCvar_t	sta_gTeamHeadModel;
vmCvar_t	sta_gForceEnemyModel;
vmCvar_t	sta_gEnemyModel;
vmCvar_t	sta_gEnemyHeadModel;

// locals
int TeamModelModificationCount = -1;
int TeamHeadModelModificationCount = -1;
int EnemyModelModificationCount = -1;
int EnemyHeadModelModificationCount = -1;

static as_cvars_t	asCvarTable[] = {
	{ &sta_gJerseys, "sta_gJerseys", "1", CVAR_ARCHIVE | CVAR_LATCH },
	{ &sta_gSpectatorJerseys, "sta_gSpectatorJerseys", "1", CVAR_ARCHIVE},
	{ &sta_hSpectatorOverlay, "sta_hSpectatorOverlay", "1", CVAR_ARCHIVE},
	{ &sta_hSpectatorMessage, "sta_hSpectatorMessage", "1", CVAR_ARCHIVE},
	{ &sta_hCrosshairInfo, "sta_hCrosshairInfo", "1", CVAR_ARCHIVE},
	{ &sta_hTimerCountDown, "sta_hTimerCountDown", "0", CVAR_ARCHIVE},
	{ &sta_hTimerPosition, "sta_hTimerPosition", "1", CVAR_ARCHIVE},
	{ &sta_hFPSPosition, "sta_hFPSPosition", "1", CVAR_ARCHIVE},
	{ &sta_hLagometerPosition, "sta_hLagometerPosition", "1", CVAR_ARCHIVE},
	{ &sta_hAttackerPosition, "sta_hAttackerPosition", "1", CVAR_ARCHIVE},
	{ &sta_hSpeedometerPosition, "sta_hSpeedometerPosition", "1", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayPosition, "sta_hTeamOverlayPosition", "1", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayNameWidth, "sta_hTeamOverlayNameWidth", "12", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayLocationWidth, "sta_hTeamOverlayLocationWidth", "16", CVAR_ARCHIVE},
	{ &sta_hTeamOverlaySlots, "sta_hTeamOverlaySlots", "8", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayInfo, "sta_hTeamOverlayInfo", "pjtnslwha", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayCharHeight, "sta_hTeamOverlayCharHeight", "8", CVAR_ARCHIVE},
	{ &sta_hTeamOverlayCharWidth, "sta_hTeamOverlayCharWidth", "8", CVAR_ARCHIVE},
	{ &sta_hAmmo, "sta_hAmmo", "1", CVAR_ARCHIVE},
	{ &sta_mJumpSound, "sta_mJumpSound", "1", CVAR_ARCHIVE},
	{ &sta_mFootsteps, "sta_mFootsteps", "1", CVAR_ARCHIVE},
	{ &sta_gTeamColoredRails, "sta_gTeamColoredRails", "1", CVAR_ARCHIVE},
	{ &sta_gTeamColoredPlasma, "sta_gTeamColoredPlasma", "1", CVAR_ARCHIVE},
	{ &sta_gTeamColoredLightning, "sta_gTeamColoredLightning", "1", CVAR_ARCHIVE},
	{ &sta_hSpeedometerUnits, "sta_hSpeedometer_units", "1", CVAR_ARCHIVE},
	{ &sta_hAutoTeamInfo, "sta_hAutoTeamInfo", "1", CVAR_ARCHIVE},
	{ &sta_hCrosshairColor, "sta_hCrosshairColor", "", CVAR_ARCHIVE},
	{ &sta_gNoPicMip, "sta_gNoPicMip", "0", CVAR_ARCHIVE | CVAR_LATCH },
	{ &sta_gNoProjectileTrail, "sta_gNoProjectileTrail", "0", CVAR_ARCHIVE},
	{ &sta_hMiscCharWidth, "sta_hMiscCharWidth", "16", CVAR_ARCHIVE},
	{ &sta_hMiscCharHeight, "sta_hMiscCharHeight", "16", CVAR_ARCHIVE},
	{ &sta_hChatMaxLines, "sta_hChatMaxLines", "3", CVAR_ARCHIVE},
	{ &sta_hChatMaxColumns, "sta_hChatMaxColumns", "80", CVAR_ARCHIVE},
	{ &sta_hChatTime, "sta_hChatTime", "3000", CVAR_ARCHIVE},
	{ &sta_hChatNewColors, "sta_hChatNewColors", "1", CVAR_ARCHIVE},

	{ &sta_hChatCharWidth, "sta_hChatCharWidth", "8", CVAR_ARCHIVE},
	{ &sta_hChatCharHeight, "sta_hChatCharHeight", "16", CVAR_ARCHIVE},

	{ &sta_gForceTeamModel, "sta_gForceTeamModel", "0", CVAR_ARCHIVE},
	{ &sta_gTeamModel, "sta_gTeamModel", DEFAULT_TEAM_MODEL, CVAR_ARCHIVE},
	{ &sta_gTeamHeadModel, "sta_gTeamHeadModel", DEFAULT_TEAM_HEAD, CVAR_ARCHIVE},
	{ &sta_gForceEnemyModel, "sta_gForceEnemyModel", "0", CVAR_ARCHIVE},
	{ &sta_gEnemyModel, "sta_gEnemyModel", DEFAULT_TEAM_MODEL, CVAR_ARCHIVE},
	{ &sta_gEnemyHeadModel, "sta_gEnemyHeadModel", DEFAULT_TEAM_HEAD, CVAR_ARCHIVE}
};

static int asCvarTableSize = sizeof( asCvarTable ) / sizeof( asCvarTable[0] );

void as_registerCvars( void ) {
	int			i;
	as_cvars_t	*cv;

	for ( i = 0, cv = asCvarTable ; i < asCvarTableSize; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
	}

	trap_Cvar_Register(NULL, "eui", va("%s", EUI_VERSION), CVAR_USERINFO | CVAR_ROM );
	TeamModelModificationCount = sta_gTeamModel.modificationCount;
	TeamHeadModelModificationCount = sta_gTeamHeadModel.modificationCount;
	EnemyModelModificationCount = sta_gEnemyModel.modificationCount;
	EnemyHeadModelModificationCount = sta_gEnemyHeadModel.modificationCount;
}

void as_updateCvars( void ) {
	int			i;
	as_cvars_t	*cv;

	for ( i = 0, cv = asCvarTable ; i < asCvarTableSize; i++, cv++ ) {
		trap_Cvar_Update( cv->vmCvar );
	}

	if ( TeamModelModificationCount != sta_gTeamModel.modificationCount ||
		TeamHeadModelModificationCount != sta_gTeamHeadModel.modificationCount )
	{
		TeamModelModificationCount = sta_gTeamModel.modificationCount;
		TeamHeadModelModificationCount = sta_gTeamHeadModel.modificationCount;
		CG_ForceModelChange();
	}

	if ( EnemyModelModificationCount != sta_gEnemyModel.modificationCount ||
		EnemyHeadModelModificationCount != sta_gEnemyHeadModel.modificationCount )
	{
		EnemyModelModificationCount = sta_gEnemyModel.modificationCount;
		EnemyHeadModelModificationCount = sta_gEnemyHeadModel.modificationCount;
		CG_ForceModelChange();
	}
}


// Modified version of CG_DrawLoadingIcons()
void as_drawLoadingIcons( int loadingPlayerIconCount, int loadingItemIconCount, qhandle_t *loadingPlayerIcons,
						 qhandle_t *loadingItemIcons) {
	int		n;
	int		x, y;

	for( n = 0; n < loadingPlayerIconCount; n++ ) {
		x = 8 + n * 39;
		y = 320;
		CG_DrawPic( x, y, 32, 32, loadingPlayerIcons[n] );
	}

	for( n = 0; n < loadingItemIconCount; n++ ) {
		y = 400-40;
		if( n >= 25 ) {
			y += 20;
		}
		x = 16 + n % 25 * 24;
		CG_DrawPic( x, y, 16, 16, loadingItemIcons[n] );
	}
}

// Remove special Q3AS colors (leaving only the stock ^x (0-7) values)
char *as_removeQ3ASColors( char *string ) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
				if ( *( s + 1 ) == 'X')
					s += 7;
				else if ( ( *( s + 1 ) > '7') || ( *( s + 1 ) < '0') )
					s++;
				else
					*d++ = c;
		} else {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

// Removes ALL color codes from string.
// Has the side effect of turning things like
// '^^0moo' into just 'moo' since we can't
// preserve a literal carat :-(
char *as_removeAllColors( char *string ) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( *s == '^') {
			if ( *(s + 1) && *(s + 1) != '^' && *(s + 1) != 'X')
				s += 2;
			else if ( strlen(s) > 7 && *(s + 1) == 'X')
				s += 8;
			else
				s++;
		} else {
			*d = c;
			d++;
			s++;
		}
	}
	*d = '\0';

	return string;
}

void as_printf( char *text, qboolean recursed ) {
	int		newline;
	int		len;
	char	string[TEAMCHAT_WIDTH*3+1];
	char	*d, *s;
	int		count = 0;
	char	currentColor[8];

	trap_Cvar_Set ("con_notifytime","-2");

	Q_strncpyz(string, text, sizeof(string));
	len = strlen(string);

	newline = ( cgs.q3as.bufferCurrentLine + cgs.q3as.bufferTotalLines ) % PRINTF_BUFFER;
	cgs.q3as.bufferTotalLines = ( cgs.q3as.bufferTotalLines + 1 ) % PRINTF_BUFFER;
	memset(cgs.q3as.bufferPrint[ newline ], 0, sizeof(cgs.q3as.bufferPrint[ newline ]));
	memset(currentColor, 0, sizeof(currentColor));

	s = string;
	d = cgs.q3as.bufferPrint[ newline ];

	if (Q_IsColorString(s) ) {
		if ( *(s + 1) != 'X' ) {
			currentColor[0] = *s; currentColor[1] = *(s + 1);
		} else {
			Q_strncpyz(currentColor, s, sizeof(currentColor));
		}
	} else {
		currentColor[0] = '^'; currentColor[1] = '7';
	}

	while (*s) {
		if (Q_IsColorString(s) ) {
			if ( *(s + 1) != 'X' ) {
				currentColor[0] = *s; currentColor[1] = *(s + 1);
				*d++ = *s++; *d++ = *s++;
			} else {
				Q_strncpyz(currentColor, s, sizeof(currentColor));
				*d++ = *s++; *d++ = *s++;
				*d++ = *s++; *d++ = *s++;
				*d++ = *s++; *d++ = *s++;
				*d++ = *s++; *d++ = *s++;
			}
		} else if ( count < ( sta_hChatMaxColumns.integer * 8 ) / sta_hChatCharWidth.integer ) {
				count++;
				*d++ = *s++;
		} else {
			s--;
			as_printf(va("%s%s", currentColor, s), qtrue);
			break;
		}
	}

	cgs.q3as.bufferPrint[ newline ][strlen(cgs.q3as.bufferPrint[ newline ]) - 1] = '\0';	// strip off trailing newline

	cgs.q3as.bufferExpireTime[ newline ] = cg.time + sta_hChatTime.integer;

	if (recursed == qfalse)
		trap_Print( as_removeQ3ASColors ( text ) );
}

void as_Echo_f (void) {
	int		i, numArgs;
	char	text[1024];

	memset(text, 0, sizeof(text));
	numArgs = trap_Argc();

	if (numArgs >= 1)
		Q_strncpyz(text, CG_Argv(1), sizeof(text));

	for (i = 2; i < numArgs; i++) {
		Q_strncpyz(text, va("%s %s", text, CG_Argv(i)), sizeof(text));
	}
	text[strlen(text)] = '\n';
	as_printf(text, qfalse);
}

#ifdef MISSIONPACK
// Modified version of CG_LightningBoltBeam()
void as_lightningBoltBeam( vec3_t start, vec3_t end, clientInfo_t *ci ) {
	localEntity_t	*le;
	refEntity_t		*beam;

	le = CG_AllocLocalEntity();
	le->leFlags = 0;
	le->leType = LE_SHOWREFENTITY;
	le->startTime = cg.time;
	le->endTime = cg.time + 50;

	beam = &le->refEntity;

	VectorCopy( start, beam->origin );
	// this is the end point
	VectorCopy( end, beam->oldorigin );

	beam->reType = RT_LIGHTNING;
	if (sta_gTeamColoredLightning.integer && cgs.gametype >= GT_TEAM )
	{
		if ( ci->team == TEAM_RED )
			beam->customShader = cgs.q3as.media.lightningShaderR;
		else // TEAM_BLUE
			beam->customShader = cgs.q3as.media.lightningShaderB;
	}
	else
		beam->customShader = cgs.media.lightningShader;
}
#endif

static int as_char2hex( char c )
{
	if ( c >= '0' && c <= '9' )
		return ( c - '0' );
	if ( c >= 'A' && c <= 'F' )
		return ( c - 'A' + 10 ) ;
	if ( c >= 'a' && c <= 'f' )
		return ( c - 'a' + 10 );
	return 0;
}

void as_parseHTMLColors( vec4_t color, const char *s )
{
	if ( strlen( s ) < 6 )
	{
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}
	else
	{
		color[0] = (as_char2hex((char)s[0]) * 16 + as_char2hex((char)s[1])) / 255.0f;
		color[1] = (as_char2hex((char)s[2]) * 16 + as_char2hex((char)s[3])) / 255.0f;
		color[2] = (as_char2hex((char)s[4]) * 16 + as_char2hex((char)s[5])) / 255.0f;
	}
	color[3] = 1.0f;
}

void as_drawBigStringNoShadow( int x, int y, const char *s, float alpha ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	CG_DrawStringExt( x, y, s, color, qfalse, qfalse, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
}

#ifdef MISSIONPACK
void as_drawSelectedPlayerNameLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
	clientInfo_t *ci;
  ci = cgs.clientinfo + sortedTeamPlayers[CG_GetSelectedPlayer()]; 
  if (ci) {
	if ( ( cg.time / 1500 ) % 2 == 0 ) // alternate name/location every 1.5 seconds
	{
		const char *p = CG_ConfigString(CS_LOCATIONS + ci->location);
		if (!p || !*p) {
			p = "unknown";
		}
		CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle);
	}
	else
	{
		CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, ci->name, 0, 0, textStyle);
	}
  }
}

void as_drawSelectedPlayerNumber( rectDef_t *rect, float scale, vec4_t color, int textStyle, qboolean draw2D ) {
	clientInfo_t *ci;
	if ( sta_gJerseys.integer == 1) // default back to head if they choose the default triangle
		CG_DrawSelectedPlayerHead( rect, draw2D, qfalse );
	else
	{
		int selectedPlayer = sortedTeamPlayers[ CG_GetSelectedPlayer() ];
		ci = cgs.clientinfo + selectedPlayer;
		if (ci) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.q3as.media.friendShaders[ selectedPlayer ] );
		}
	}
}
#endif

void as_registerGraphics (void) {
	char	jerseyPath[MAX_QPATH];
	int		i;

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_SMOKE )) {
		cgs.media.smokePuffShader = trap_R_RegisterShader( "smokePuffnopicmip" );
		cgs.media.smokePuffRageProShader = trap_R_RegisterShader( "smokePuffRagePronopicmip" );
		cgs.media.shotgunSmokePuffShader = trap_R_RegisterShader( "shotgunSmokePuffnopicmip" );
	} else {
		cgs.media.smokePuffShader = trap_R_RegisterShader( "smokePuff" );
		cgs.media.smokePuffRageProShader = trap_R_RegisterShader( "smokePuffRagePro" );
		cgs.media.shotgunSmokePuffShader = trap_R_RegisterShader( "shotgunSmokePuff" );
	}
#ifdef MISSIONPACK
	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_SMOKE )) {
		cgs.media.nailPuffShader = trap_R_RegisterShader( "nailtrailnopicmip" );
	} else {
		cgs.media.nailPuffShader = trap_R_RegisterShader( "nailtrail" );
	}
#endif
	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_PG )) {
		cgs.media.plasmaBallShader = trap_R_RegisterShader( "sprites/plasma1nopicmip" );
		cgs.q3as.media.plasmaBallShaderBlue = trap_R_RegisterShader( "sprites/bplasmanopicmip" );
		cgs.q3as.media.plasmaBallShaderRed = trap_R_RegisterShader( "sprites/rplasmanopicmip" );
	} else {
		cgs.media.plasmaBallShader = trap_R_RegisterShader( "sprites/plasma1" );
		cgs.q3as.media.plasmaBallShaderBlue = trap_R_RegisterShader( "sprites/bplasma" );
		cgs.q3as.media.plasmaBallShaderRed = trap_R_RegisterShader( "sprites/rplasma" );
	}
	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_SMOKE ))
		cgs.media.waterBubbleShader = trap_R_RegisterShader( "waterBubblenopicmip" );
	else
		cgs.media.waterBubbleShader = trap_R_RegisterShader( "waterBubble" );
	for ( i = 0 ; i < NUM_CROSSHAIRS ; i++ ) {
		cgs.media.crosshairShader[i] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair2%c", 'a'+i) );
	}
#ifdef MISSIONPACK
	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_SMOKE ))
		cgs.media.dustPuffShader = trap_R_RegisterShader("hasteSmokePuffnopicmip" );
	else
		cgs.media.dustPuffShader = trap_R_RegisterShader("hasteSmokePuff" );
#endif
	if ( cgs.gametype >= GT_TEAM || cg_buildScript.integer ) {
		for ( i = 0; i < NUM_TEAM_JERSEYS; i++) // load 35 as #
			{
			switch (sta_gJerseys.integer) {
				case 1:
					Com_sprintf( jerseyPath, MAX_QPATH, "sprites/foe", i); // id default triangle
					break;
				case 2:
					Com_sprintf( jerseyPath, MAX_QPATH, "sprites/75_%i", i); // 75% opaque
					break;
				case 3:
					Com_sprintf( jerseyPath, MAX_QPATH, "sprites/100_%i", i); // 100% opaque
					break;
				default:
					Com_sprintf( jerseyPath, MAX_QPATH, "sprites/75_%i", i);
					break;
			}	
			cgs.q3as.media.friendShaders[i] = trap_R_RegisterShader( jerseyPath ); // 75% opaque
		} // load the rest as default triangle just to cover
		for ( i = NUM_TEAM_JERSEYS; i < MAX_CLIENTS; i ++)
			cgs.q3as.media.friendShaders[i] = trap_R_RegisterShader( "sprites/foe" );
	}

	cgs.media.botSkillShaders[0] = trap_R_RegisterShaderNoMip( "menu/art/skill1.tga" );
	cgs.media.botSkillShaders[1] = trap_R_RegisterShaderNoMip( "menu/art/skill2.tga" );
	cgs.media.botSkillShaders[2] = trap_R_RegisterShaderNoMip( "menu/art/skill3.tga" );
	cgs.media.botSkillShaders[3] = trap_R_RegisterShaderNoMip( "menu/art/skill4.tga" );
	cgs.media.botSkillShaders[4] = trap_R_RegisterShaderNoMip( "menu/art/skill5.tga" );

	cgs.media.balloonShader = trap_R_RegisterShader( "sprites/balloon3nopicmip" );
	
	cgs.media.noammoShader = trap_R_RegisterShaderNoMip( "icons/noammo" );

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_BLOOD ))
		cgs.media.bloodExplosionShader = trap_R_RegisterShader( "bloodExplosionnopicmip" );
	else
		cgs.media.bloodExplosionShader = trap_R_RegisterShader( "bloodExplosion" );

	cgs.q3as.media.armorShader = trap_R_RegisterShaderNoMip( "ui/assets/statusbar/selectedarmor.tga" );
	
	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_MG ))
		cgs.media.bulletMarkShader = trap_R_RegisterShader( "gfx/damage/bullet_mrknopicmip" );
	else
		cgs.media.bulletMarkShader = trap_R_RegisterShader( "gfx/damage/bullet_mrk" );

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_RL )) // changeme?
		cgs.media.burnMarkShader = trap_R_RegisterShader( "gfx/damage/burn_med_mrknopicmip" );
	else
		cgs.media.burnMarkShader = trap_R_RegisterShader( "gfx/damage/burn_med_mrk" );

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_LG )) // changeme?
		cgs.media.holeMarkShader = trap_R_RegisterShader( "gfx/damage/hole_lg_mrknopicmip" );
	else
		cgs.media.holeMarkShader = trap_R_RegisterShader( "gfx/damage/hole_lg_mrk" );

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_PG )) // changeme?
		cgs.media.energyMarkShader = trap_R_RegisterShader( "gfx/damage/plasma_mrknopicmip" );
	else
		cgs.media.energyMarkShader = trap_R_RegisterShader( "gfx/damage/plasma_mrk" );

	if ( sta_gNoPicMip.integer & ( 1 << NOPICMIP_PU )) {
		cgs.media.quadShader = trap_R_RegisterShader("powerups/quadnopicmip" );
 		cgs.media.quadWeaponShader = trap_R_RegisterShader("powerups/quadWeaponnopicmip" );
 		cgs.media.battleSuitShader = trap_R_RegisterShader("powerups/battleSuitnopicmip" );
 		cgs.media.battleWeaponShader = trap_R_RegisterShader("powerups/battleWeaponnopicmip" );
 		cgs.media.invisShader = trap_R_RegisterShader("powerups/invisibilitynopicmip" );
 		cgs.media.regenShader = trap_R_RegisterShader("powerups/regennopicmip" );
 		cgs.media.hastePuffShader = trap_R_RegisterShader("hasteSmokePuffnopicmip" );
	} else {
		cgs.media.quadShader = trap_R_RegisterShader("powerups/quad" );
		cgs.media.quadWeaponShader = trap_R_RegisterShader("powerups/quadWeapon" );
		cgs.media.battleSuitShader = trap_R_RegisterShader("powerups/battleSuit" );
		cgs.media.battleWeaponShader = trap_R_RegisterShader("powerups/battleWeapon" );
		cgs.media.invisShader = trap_R_RegisterShader("powerups/invisibility" );
		cgs.media.regenShader = trap_R_RegisterShader("powerups/regen" );
		cgs.media.hastePuffShader = trap_R_RegisterShader("hasteSmokePuff" );
	}

}

// modified version of CG_DrawAttacker
float as_drawAttacker( float y, qboolean right, qboolean upper ) {
	int			t, x;
	float		size;
	vec3_t		angles;
	const char	*info;
	const char	*name;
	int			clientNum;
	int ret_y;

	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return y;
	}

	if ( !cg.attackerTime ) {
		return y;
	}

	clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum ) {
		return y;
	}

	t = cg.time - cg.attackerTime;
	if ( t > ATTACKER_HEAD_TIME ) {
		cg.attackerTime = 0;
		return y;
	}

	size = ICON_SIZE * 1.25;

	// TA EUI - allow right/left
	if ( right )
		x = 640 - size;
	else
		x = 0;

	if ( upper ) {
		ret_y = y + size + 2;
	} else {
		y -= size + 2;
		ret_y = y;
	}
	// end TA EUI

	angles[PITCH] = 0;
	angles[YAW] = 180;
	angles[ROLL] = 0;
	CG_DrawHead( x, y, size, size, clientNum, angles );

	info = CG_ConfigString( CS_PLAYERS + clientNum );
	name = Info_ValueForKey(  info, "n" );
	y += size;
	CG_DrawBigString( x - ( Q_PrintStrlen( name ) * BIGCHAR_WIDTH), y, name, 0.5 );

	return ret_y;
}

// modified version of CG_DrawSnapshot
float as_drawSnapshot( float y, qboolean right, qboolean upper ) {
	char		*s;
	int			w, x;
	int ret_y;
	if ( upper ) {
		ret_y = y + BIGCHAR_HEIGHT + 4;
	} else {
		y -= BIGCHAR_HEIGHT + 4;
		ret_y = y;
	}

	s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime, 
		cg.latestSnapshotNum, cgs.serverCommandSequence );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	if ( right )
		x = 640 - w;
	else
		x = 0;

	CG_DrawBigString( x, y + 2, s, 1.0F);

	return ret_y;
}

#define	FPS_FRAMES	4
// modified version of CG_DrawFPS
float as_drawFPS( float y, qboolean right, qboolean upper ) {
	char		*s;
	int			w;
	static int	previousTimes[FPS_FRAMES];
	static int	index;
	int		i, total;
	int		fps;
	static	int	previous;
	int		t, frameTime;
	int x, ret_y;
	if ( upper ) {
		ret_y = y + sta_hMiscCharHeight.integer + 4;
	} else {
		y -= sta_hMiscCharHeight.integer + 4;
		ret_y = y;
	}

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if ( index > FPS_FRAMES ) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va( "%ifps", fps );
		w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;

		if ( right )
			x = 635 - w;
		else
			x = 0;

		CG_DrawStringExt( x , y + 2, s, colorWhite, qfalse, qtrue,
			sta_hMiscCharWidth.integer, sta_hMiscCharHeight.integer, 0);
	}

	return ret_y;
}

// modified version of CG_DrawTimer
float as_drawTimer( float y, qboolean right, qboolean upper ) {
	char		*s;
	int			w, x;
	int			mins, seconds, tens;
	int			msec;
	int ret_y;
	if ( upper ) {
		ret_y = y + sta_hMiscCharHeight.integer + 4;
	} else {
		y -= sta_hMiscCharHeight.integer + 4;
		ret_y = y;
	}

	msec = cg.time - cgs.levelStartTime;
	if ( cgs.timelimit && sta_hTimerCountDown.integer )
	{
		msec = abs( (cgs.timelimit * 60 * 1000 ) - msec );
	}

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	s = va( "%i:%i%i", mins, tens, seconds );
	w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;

	if ( right )
		x = 635 - w;
	else
		x = 0;

	CG_DrawStringExt( x , y + 2, s, colorWhite, qfalse, qtrue,
			sta_hMiscCharWidth.integer, sta_hMiscCharHeight.integer, 0);

	return ret_y;
}

float as_drawTeamOverlay( float y, qboolean right, qboolean upper ) {
	int x, w, h;
	int i, j, len;
	const char *p;
	vec4_t		hcolor;
	int pwidth, lwidth;
	int plyrs;
	char st[16];
	clientInfo_t *ci;
	gitem_t	*item;
	int ret_y, count;

	qboolean drawJersey, drawTask, drawName, drawLocation, drawPowerups, drawWeapon, drawArmor, drawHealth;
	int startJersey, startTask, startName, startLocation, startPowerups, startWeapon, startArmor, startHealth;
	int startPU;
	int drawTeam, myTeam;
	int drawPlayers[TEAM_MAXOVERLAY];
	char info;
	int charWidth, charHeight;
	drawJersey = drawTask = drawName = drawLocation = drawPowerups = drawWeapon = drawArmor = drawHealth = qfalse;
	startJersey = startTask = startName = startLocation = startPowerups = startWeapon = startArmor = startHealth = 0;

	if ( !cg_drawTeamOverlay.integer ) {
		return y;
	}

	myTeam = cg.snap->ps.persistant[PERS_TEAM];
	if ( myTeam != TEAM_RED && myTeam != TEAM_BLUE &&
		!( sta_hSpectatorOverlay.integer && myTeam == TEAM_SPECTATOR )) {
		return y; // Not on any team
	}

	// max player name width
	plyrs = 0;
	pwidth = 0;
	if ( myTeam == TEAM_SPECTATOR )
	{
		y = 480;
		if ( upper )
			return y;

		if ( right )
		{
			drawTeam = TEAM_BLUE;
			hcolor[0] = 0.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 1.0f;
			hcolor[3] = 0.33f;
		}
		else // left
		{
			drawTeam = TEAM_RED;
			hcolor[0] = 1.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		}

		count =  cgs.maxclients;
		for ( i = 0; i < cgs.maxclients; i++ )
		{
			ci = cgs.clientinfo + i;
			if ( ci->infoValid && ci->team == drawTeam ) {
				drawPlayers[plyrs] = i;
				plyrs++;
				len = CG_DrawStrlen(ci->name);
				if (len > pwidth)
					pwidth = len;
				if ( plyrs == sta_hTeamOverlaySlots.integer )
					break;
			}
		}
	}
	else
	{
		drawTeam = myTeam;
		count = (numSortedTeamPlayers > sta_hTeamOverlaySlots.integer) ?  sta_hTeamOverlaySlots.integer: numSortedTeamPlayers;
		for (i = 0; i < count; i++) {
			ci = cgs.clientinfo + sortedTeamPlayers[i];
			drawPlayers[i] = sortedTeamPlayers[i];
			plyrs++;
			len = CG_DrawStrlen(ci->name);
			if (len > pwidth)
				pwidth = len;
		}

		if ( myTeam == TEAM_RED ) {
			hcolor[0] = 1.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 0.0f;
			hcolor[3] = 0.33f;
		} else { // if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
			hcolor[0] = 0.0f;
			hcolor[1] = 0.0f;
			hcolor[2] = 1.0f;
			hcolor[3] = 0.33f;
		}
	}

	if (!plyrs)
		return y;

	if (pwidth > sta_hTeamOverlayNameWidth.integer)
		pwidth = sta_hTeamOverlayNameWidth.integer;

	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_ConfigString(CS_LOCATIONS + i);
		if (p && *p) {
			len = CG_DrawStrlen(p);
			if (len > lwidth)
				lwidth = len;
		}
	}

	if (lwidth > sta_hTeamOverlayLocationWidth.integer)
		lwidth = sta_hTeamOverlayLocationWidth.integer;

	charWidth = sta_hTeamOverlayCharWidth.integer;
	charHeight = sta_hTeamOverlayCharHeight.integer;
	w = 0;
	for (i = 0; i < strlen( sta_hTeamOverlayInfo.string); i++)
	{
		info = sta_hTeamOverlayInfo.string[i];
		if (info == 'J' || info == 'j') // jersey
		{
			startJersey = w * charWidth;
			w += 2;
			drawJersey = qtrue;
		}
		else if (info == 'T' || info == 't') // team task
		{
			startTask = w * charWidth;
			w += 1;
			drawTask = qtrue;
		}
		else if (info == 'N' || info == 'n') // name
		{
			startName = w * charWidth;
			w += pwidth;
			drawName = qtrue;
		}
		else if ( myTeam != TEAM_SPECTATOR && (info == 'L' || info == 'l')) // location
		{
			startLocation = w * charWidth;
			w += lwidth;
			drawLocation = qtrue;
		}
		else if (info == 'P' || info == 'p') // powerups
		{
			startPowerups = w * charWidth;
			w += 1;
			drawPowerups = qtrue;
		}
		else if (myTeam != TEAM_SPECTATOR && (info == 'W' || info == 'w')) // weapon
		{
			startWeapon = w * charWidth;
			w += 1;
			drawWeapon = qtrue;
		}
		else if (myTeam != TEAM_SPECTATOR && (info == 'A' || info == 'a')) // armor
		{
			startArmor = w * charWidth;
			w += 4;
			drawArmor = qtrue;
		}
		else if (myTeam != TEAM_SPECTATOR && (info == 'H' || info == 'h')) // health
		{
			startHealth = w * charWidth;
			w += 4;
			drawHealth = qtrue;
		}
		else if (info == 'S' || info == 's') // space
		{
			w +=1;
		}
	}
	w *= charWidth;

	if ( right )
		x = 640 - w;
	else
		x = 0;

	h = plyrs * charHeight;

	if ( upper ) {
		ret_y = y + h;
	} else {
		y -= h;
		ret_y = y;
	}

	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );

	if ( myTeam == TEAM_SPECTATOR && cg.scoresRequestTime + 5000 < cg.time ) {
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );
	}

	for (i = 0; i < plyrs; i++) {
		ci = cgs.clientinfo + drawPlayers[i];
		//if ( ci->infoValid && ci->team == drawTeam ) { // q3as - do I even need this check?

			hcolor[0] = hcolor[1] = hcolor[2] = hcolor[3] = 1.0;

			if ( drawName == qtrue )
			{
				CG_DrawStringExt( x + startName, y,
					ci->name, hcolor, qfalse, qfalse, charWidth, charHeight,
					sta_hTeamOverlayNameWidth.integer);
			}

			if ( drawLocation == qtrue )
			{
				if (lwidth) {
					p = CG_ConfigString(CS_LOCATIONS + ci->location);
					if (!p || !*p)
						p = "unknown";
					len = CG_DrawStrlen(p);
					if (len > lwidth)
						len = lwidth;

					CG_DrawStringExt( x + startLocation, y,
						p, hcolor, qfalse, qfalse, charWidth, charHeight,
						sta_hTeamOverlayLocationWidth.integer);
				}
			}

			CG_GetColorForHealth( ci->health, ci->armor, hcolor );  // used for both health and armor
			if ( drawHealth == qtrue )
			{
				Com_sprintf (st, sizeof(st), "%3i", ci->health);
				CG_DrawStringExt( x + startHealth, y,
					st, hcolor, qfalse, qfalse,
					charWidth, charHeight, 0 );
				CG_DrawPic( x + startHealth + (3 * charWidth), y,
					charWidth, charHeight, cgs.media.heartShader );
			}

			if ( drawArmor == qtrue )
			{
				Com_sprintf (st, sizeof(st), "%3i", ci->armor);
				CG_DrawStringExt( x + startArmor, y,
					st, hcolor, qfalse, qfalse,
					charWidth, charHeight, 0 );
				CG_DrawPic( x + startArmor + (3 * charWidth), y,
					charWidth, charHeight, cgs.q3as.media.armorShader );
			}

			if ( drawWeapon == qtrue )
			{
				if ( cg_weapons[ci->curWeapon].weaponIcon ) {
					CG_DrawPic( x + startWeapon, y,
						charWidth, charHeight, cg_weapons[ci->curWeapon].weaponIcon );
				} else {
					CG_DrawPic( x + startWeapon, y,
						charWidth, charHeight, cgs.media.deferShader );
				}
			}

			if ( drawJersey == qtrue )
			{
				Com_sprintf (st, sizeof(st), "%2i", drawPlayers[i] );
				hcolor[0] = 1; hcolor[1] = 0.7f; hcolor[2] = 0.05f; hcolor[3] = 0.9f;
				CG_DrawStringExt( x + startJersey, y,
					st, hcolor , qfalse, qfalse,
					charWidth, charHeight, 0 );
			}

			// Draw powerup icons
			if ( drawPowerups == qtrue)
			{
				startPU = x + startPowerups;
				for (j = 0; j <= PW_NUM_POWERUPS; j++) {
					if (ci->powerups & (1 << j)) {
						item = BG_FindItemForPowerup( j );

						if (item) {
							CG_DrawPic( startPU, y, charWidth, charHeight, 
							trap_R_RegisterShader( item->icon ) );
							if (right) {
								startPU -= charWidth;
							} else {
								startPU += charWidth;
							}
						}
					}
				}
			}

#ifdef MISSIONPACK
			if ( drawTask == qtrue)
			{
				CG_DrawPic( x + startTask, y, charWidth, charHeight, 
						CG_StatusHandle(ci->teamTask));
			}
#endif

			y += charHeight;
		//}
	}

	return ret_y;
}

void as_drawDisconnect( int x, int y ) {
//	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;  // bk010215 - FIXME char message[1024];

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
		|| cmd.serverTime > cg.time ) {	// special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = "Connection Interrupted"; // bk 010215 - FIXME
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader("gfx/2d/net.tga" ) );
}

float as_drawLagometer( float y, qboolean right, qboolean upper ) {
	int		a, x, ret_y, i;
	float	v;
	float	ax, ay, aw, ah, mid, range;
	int		color;
	float	vscale;

	if ( right )
		x = 640 - 48;
	else 
		x = 0;

	if ( upper ) {
		ret_y = y + 48;
	} else {
		y -= 48;
		ret_y = y;
	}

	if ( !cg_lagometer.integer || cgs.localServer ) {
		as_drawDisconnect( x, y );
		return ret_y;
	}

	//
	// draw the graph
	//

	trap_R_SetColor( NULL );
	CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_BLUE)] );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;	// YELLOW for rate delay
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_YELLOW)] );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( g_color_table[ColorIndex(COLOR_GREEN)] );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;		// RED for dropped snapshots
				trap_R_SetColor( g_color_table[ColorIndex(COLOR_RED)] );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer || cg_synchronousClients.integer ) {
		CG_DrawBigString( ax, ay, "snc", 1.0 );
	}

	as_drawDisconnect( x, y );
	return ret_y;
}

float as_drawSpeedometer( float y, qboolean right, qboolean upper ) {
	char		*s;
	int			w;
	int x, ret_y;
	if ( upper ) {
		ret_y = y + sta_hMiscCharHeight.integer + 4;
	} else {
		y -= sta_hMiscCharHeight.integer + 4;
		ret_y = y;
	}

	switch ( sta_hSpeedometerUnits.integer )
	{
		case 1: // map units per second
			s = va( "%4.0fups", cg.xyspeed );
			w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;
			break;
		case 2: // miles per hour
			s = va( "%3.0fmph", cg.xyspeed / 8 * 3600 / 5280 );
			w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;
			break;
		case 3: // kilometers per hour
			s = va( "%3.0fkph", cg.xyspeed / 26.25 * 3600 / 1000 );
			w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;
			break;
		default:
			s = va( "%4.0fups", cg.xyspeed );
			w = CG_DrawStrlen( s ) * sta_hMiscCharWidth.integer;
			break;
	}

	if ( right )
		x = 635 - w;
	else
		x = 0;

	CG_DrawStringExt( x , y + 2, s, colorWhite, qfalse, qtrue,
			sta_hMiscCharWidth.integer, sta_hMiscCharHeight.integer, 0);

	return ret_y;
}


void as_drawUpperRight( void ) {
	float	y;

	y = 0;

	if ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer && sta_hTeamOverlayPosition.integer == 1 ) {
		y = as_drawTeamOverlay( y, qtrue, qtrue );
	} 
	if ( cg_drawSnapshot.integer == 1 ) {
		y = as_drawSnapshot( y, qtrue, qtrue );
	}
	if ( cg_drawFPS.integer && sta_hFPSPosition.integer == 1) {
		y = as_drawFPS( y, qtrue, qtrue );
	}
	if ( cg_drawTimer.integer && sta_hTimerPosition.integer == 1 ) {
		y = as_drawTimer( y, qtrue, qtrue );
	}
	if ( cg_lagometer.integer && sta_hLagometerPosition.integer == 1) {
		y = as_drawLagometer( y, qtrue, qtrue );
	}
	if ( cg_drawAttacker.integer && sta_hAttackerPosition.integer == 1 ) {
		y = as_drawAttacker( y, qtrue, qtrue );
	}
	if ( sta_hSpeedometerPosition.integer == 1) {
		y = as_drawSpeedometer(y, qtrue, qtrue);
	}
}

void as_drawLowerRight( void ) {
	float	y;

	if ( cgs.q3as.largeHud )
		y = 480 - ICON_SIZE * 2;
	else
		y = 480 - ( ICON_SIZE / 2);

	if ( ( cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer && sta_hTeamOverlayPosition.integer == 2 ) ||
		( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR && sta_hSpectatorOverlay.integer )) {
		y = as_drawTeamOverlay( y, qtrue, qfalse );
	}
	if ( cg_drawSnapshot.integer == 2) {
		y = as_drawSnapshot( y, qtrue, qfalse );
	}
	if ( cg_drawFPS.integer && sta_hFPSPosition.integer == 2) {
		y = as_drawFPS( y, qtrue, qfalse );
	}
	if ( cg_drawTimer.integer && sta_hTimerPosition.integer == 2) {
		y = as_drawTimer( y, qtrue, qfalse );
	}
	if ( cg_lagometer.integer && sta_hLagometerPosition.integer == 2) {
		y = as_drawLagometer( y, qtrue, qfalse );
	}
	if ( cg_drawAttacker.integer && sta_hAttackerPosition.integer == 2) {
		y = as_drawAttacker( y, qtrue, qfalse );
	}
	if ( sta_hSpeedometerPosition.integer == 2) {
		y = as_drawSpeedometer(y, qtrue, qfalse);
	}

#ifndef MISSIONPACK
	y = CG_DrawScores( y );
	y = CG_DrawPowerups( y );
#endif // MISSIONPACK
}

void as_drawLowerLeft( void ) {
	float	y;

	if ( cgs.q3as.largeHud )
		y = 480 - ICON_SIZE * 2.5;
	else
		y = 480 - ICON_SIZE;

	if ( (cgs.gametype >= GT_TEAM && cg_drawTeamOverlay.integer && sta_hTeamOverlayPosition.integer == 3 ) ||
		( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR && sta_hSpectatorOverlay.integer )) {
		y = as_drawTeamOverlay( y, qfalse, qfalse );
	}
	if ( cg_drawSnapshot.integer == 3 ) {
		y = as_drawSnapshot( y, qfalse, qfalse );
	}
	if ( cg_drawFPS.integer && sta_hFPSPosition.integer == 3 ) {
		y = as_drawFPS( y, qfalse, qfalse );
	}
	if ( cg_drawTimer.integer && sta_hTimerPosition.integer == 3 ) {
		y = as_drawTimer( y, qfalse, qfalse );
	}
	if ( cg_lagometer.integer && sta_hLagometerPosition.integer == 3) {
		y = as_drawLagometer( y, qfalse, qfalse );
	}
	if ( cg_drawAttacker.integer && sta_hAttackerPosition.integer == 3 ) {
		y = as_drawAttacker( y, qfalse, qfalse );
	}
	if ( sta_hSpeedometerPosition.integer == 3) {
		y = as_drawSpeedometer( y, qfalse, qfalse );
	}

#ifndef MISSIONPACK
	y = CG_DrawPickupItem( y );
#endif
}

void as_drawCrosshair(void) {
	float		w, h;
	qhandle_t	hShader;
	float		f;
	float		x, y;
	int			ca;

	if ( !cg_drawCrosshair.integer ) {
		return;
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) {
		return;
	}

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if ( strlen( sta_hCrosshairColor.string ) == 6 )
	{
		vec4_t	hcolor;
		as_parseHTMLColors( hcolor, sta_hCrosshairColor.string );
		trap_R_SetColor( hcolor );
	}
	else

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor( NULL );
	}

	w = h = cg_crosshairSize.value;

	// pulse the size of the crosshair when picking up items
	f = cg.time - cg.itemPickupBlendTime;
	if ( f > 0 && f < ITEM_BLOB_TIME ) {
		f /= ITEM_BLOB_TIME;
		w *= ( 1 + f );
		h *= ( 1 + f );
	}

	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;
	CG_AdjustFrom640( &x, &y, &w, &h );

	ca = cg_drawCrosshair.integer;
	if (ca < 0) {
		ca = 0;
	}
	hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

	trap_R_DrawStretchPic( x + cg.refdef.x + 0.5 * (cg.refdef.width - w), 
		y + cg.refdef.y + 0.5 * (cg.refdef.height - h), 
		w, h, 0, 0, 1, 1, hShader );
}

void as_drawCrosshairNames( void ) {
	float		*color;
	char		*name;
	float		w;

#ifdef MISSIONPACK
	char printme[ MAX_NAME_LENGTH * 3 ];
	int health, armor;
	vec4_t hcolor;
#endif

	if ( !cg_drawCrosshair.integer ) {
		return;
	}
	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}
	if ( cg.renderingThirdPerson ) {
		return;
	}

	// scan the known entities to see if the crosshair is sighted on one
	CG_ScanForCrosshairEntity();

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );
	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	name = cgs.clientinfo[ cg.crosshairClientNum ].name;
#ifdef MISSIONPACK
	color[3] *= 0.5f;

	if ( sta_hCrosshairInfo.integer )
	{
		if ( cgs.clientinfo[ cg.crosshairClientNum ].team == cg.snap->ps.persistant[PERS_TEAM] )
		{ // same team, draw health/armor
			//haw = CG_Text_Width( "999", 0.15f, 0); haw = 14
			w = CG_Text_Width(name, 0.3f, 0) + 14;
			CG_Text_Paint(  320 - w / 2 + 14, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
			health = cgs.clientinfo[ cg.crosshairClientNum ].health;
			armor = cgs.clientinfo[ cg.crosshairClientNum ].armor;
			CG_GetColorForHealth( health, armor, hcolor );
			CG_Text_Paint( 320 -  w / 2, 184, 0.15f, hcolor, va( "%i", health ), 0, 0, ITEM_TEXTSTYLE_NORMAL );
			CG_Text_Paint( 320 - w / 2, 190, 0.15f, hcolor, va( "%i", armor ), 0, 0, ITEM_TEXTSTYLE_NORMAL );
		}
		else
		{ // other team or spec, draw client #
			Com_sprintf(printme, sizeof(printme), "^3%i^7|%s", cg.crosshairClientNum, name );
			//w = CG_Text_Width( va("^3%i^7|%s", cg.crosshairClientNum, name ), 0.3f, 0);
			w = CG_Text_Width( printme, 0.3f, 0 );
			//CG_Text_Paint(  320 - w / 2, 190, 0.3f, color, va("^3%i^7|%s", cg.crosshairClientNum, name ), 0, 0, ITEM_TEXTSTYLE_SHADOWED);
			CG_Text_Paint(  320 - w / 2, 190, 0.3f, color, printme, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
		}
	}
	else
	{
		w = CG_Text_Width(name, 0.3f, 0);
		CG_Text_Paint(  320 - w / 2, 190, 0.3f, color, name, 0, 0, ITEM_TEXTSTYLE_SHADOWED);
	}


#else
	w = CG_DrawStrlen( name ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w / 2, 170, name, color[3] * 0.5f );
#endif
	trap_R_SetColor( NULL );
}

void as_drawSpectator(void) {
	if ( !sta_hSpectatorMessage.integer )
		return;

	as_drawBigStringNoShadow(320 - 9 * 8, 420, "SPECTATOR", 0.5);
	if ( cgs.gametype == GT_TOURNAMENT ) {
		CG_DrawBigString(320 - 15 * 8, 460, "waiting to play", 1.0F);
	}
	else if ( cgs.gametype >= GT_TEAM ) {
		as_drawBigStringNoShadow(320 - 17 * 8, 440, "press ESC and use", 0.5);
		as_drawBigStringNoShadow(320 - 21 * 8, 460, "the JOIN menu to play", 0.5);
	}
}

qboolean as_drawFollow( void ) {
	float		x;
	vec4_t		color;
	const char	*name;

	if ( !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) {
		return qfalse;
	}
	
	if ( !sta_hSpectatorMessage.integer )
		return qfalse;

	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

	as_drawBigStringNoShadow( 320 - 9 * 8, 24, "following", 0.5F );

	name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;
	x = 0.5 * ( 640 - BIGCHAR_WIDTH * CG_DrawStrlen( name ) );

	as_drawBigStringNoShadow( x, 24 + BIGCHAR_HEIGHT, name, 0.5F );

	return qtrue;
}

#ifdef MISSIONPACK
void as_findFlagCarrier(void) {
	int i, powerup;
	static int checkTime;

	if ( cgs.gametype != GT_CTF && cgs.gametype != GT_1FCTF )
		return;

	if (checkTime == 0) {
	    checkTime = cg.time + 1000;
	}

	if (cg.time < checkTime) {
		return;
	}

	checkTime = cg.time + 1000;

	for(i = 0; i < numSortedTeamPlayers; i++ )
	{
		// don't switch if I am the carrier
		if ( cg.snap->ps.clientNum == sortedTeamPlayers[i] )
			continue;

		powerup = cgs.clientinfo[sortedTeamPlayers[i]].powerups;
		if ((powerup & ( 1 << PW_NEUTRALFLAG)) ||
			(powerup & ( 1 << PW_BLUEFLAG)) ||
			(powerup & ( 1 << PW_REDFLAG)))
		{
			cg_currentSelectedPlayer.integer = i;
			CG_SetSelectedPlayerName();
			break;
		}
	}
}
#endif

// This one (instead of CG_DrawChar) doesn't resize
void as_drawChar( int x, int y, int width, int height, int ch ) {
	int row, col;
	float frow, fcol;
	float size;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	row = ch>>4;
	col = ch&15;

	frow = row*0.0625;
	fcol = col*0.0625;
	size = 0.0625;

	trap_R_DrawStretchPic( x, y, width, height,
					   fcol, frow, 
					   fcol + size, frow + size, 
					   cgs.media.charsetShader );
}

// calls as_drawChar instead of CG_DrawChar
void as_drawStringExt( int x, int y, const char *string, const float *setColor, 
		qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars ) {
	vec4_t		color;
	const char	*s;
	int			xx;
	int			cnt;

	if (maxChars <= 0)
		maxChars = 32767; // do them all!

	// draw the drop shadow
	if (shadow) {
		color[0] = color[1] = color[2] = 0;
		color[3] = setColor[3];
		trap_R_SetColor( color );
		s = string;
		xx = x;
		cnt = 0;
		while ( *s && cnt < maxChars) {
			if ( Q_IsColorString( s ) ) {
				if ( *( s + 1 ) == 'X')
					s += 8;
				else
					s += 2;
				continue;
			}
			as_drawChar( xx + 2, y + 2, charWidth, charHeight, *s );
			cnt++;
			xx += charWidth;
			s++;
		}
	}

	// draw the colored text
	s = string;
	xx = x;
	cnt = 0;
	trap_R_SetColor( setColor );
	while ( *s && cnt < maxChars) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				if ( *( s + 1) == 'X' )
				{
					as_parseHTMLColors( color, &*(s + 2) );
				}
				else
				{
					memcpy( color, g_color_table[ColorIndex(*(s+1))], sizeof( color ) );
				}
				color[3] = setColor[3];
				trap_R_SetColor( color );
			}
			if ( *( s + 1 ) == 'X')
				s += 8;
			else
				s += 2;
			continue;
		}
		as_drawChar( xx, y, charWidth, charHeight, *s );
		xx += charWidth;
		cnt++;
		s++;
	}
	trap_R_SetColor( NULL );
}

void as_drawPrintBuffer(void) {
	int i, x, y, line, currentLine, lines;
	char buf[10];

	if ( sta_hChatNewColors.integer == 0 )
		return;

	if ( cgs.q3as.bufferTotalLines == 0 )
		return;

	trap_Cvar_VariableStringBuffer( "cl_conXOffset", buf, sizeof(buf) );

	x = 2 + ( atoi(buf) * cgs.screenXScale );	// scale DOES matter here ...

	lines = cgs.q3as.bufferTotalLines < sta_hChatMaxLines.integer ? cgs.q3as.bufferTotalLines : sta_hChatMaxLines.integer;
	currentLine = cgs.q3as.bufferCurrentLine + cgs.q3as.bufferTotalLines - lines;

	for ( i = 0; i < lines; i++ ) {
		y = 36 + sta_hChatCharHeight.integer * i;	// We have to nudge it down some to allow room for 
													// 'say' and 'sayteam' prompts

		line = ( i + currentLine ) % PRINTF_BUFFER;

		// do NOT scale text here
		as_drawStringExt( x, y,	cgs.q3as.bufferPrint[ line ], colorWhite, qfalse, qfalse,
			sta_hChatCharWidth.integer, sta_hChatCharHeight.integer, strlen( cgs.q3as.bufferPrint[ line ] ));

		if ( cg.time > cgs.q3as.bufferExpireTime[ line ] ) {
			cgs.q3as.bufferCurrentLine = ( cgs.q3as.bufferCurrentLine + 1 ) % PRINTF_BUFFER;
			if ( cgs.q3as.bufferTotalLines > 0 )	
				cgs.q3as.bufferTotalLines--;
		}
	}
}

void as_setFeatureList(void) {
	if (as_vercmp(cgs.q3as.esv, "0.9") >= 0)
		cgs.q3as.features.teamColoredPlasma = qtrue;
	else
		cgs.q3as.features.teamColoredPlasma = qfalse;
}

static int defaultRates[] = { 0,  400,  100, 1000,  800,  800,   50, 1500,  100,  200,  400, 1000,  800,   30 } ;
void as_loadWeaponRates(void) {
	int				i;
	const char		*info;
	char			*t;

	info = CG_ConfigString(CS_AS_WR);

	for (i = 1; i < WP_NUM_WEAPONS; i++) {
		t = Info_ValueForKey(info, va("x%dx", i));
		cgs.q3as.weaponRates[i] = atoi(t);
		if (cgs.q3as.weaponRates[i] == 0) {
			// CG_Printf("No firing rate found for weapon #%d. Using %d\n", i, defaultRates[i]);
			cgs.q3as.weaponRates[i] = defaultRates[i];
		}
	}
}

int as_getWeaponRate(int weapon) {
	return cgs.q3as.weaponRates[weapon];
}