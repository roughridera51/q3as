// This file is included in cg_local.h after any defines and enums, but
// before any typedefs so that we can effectively modify those structs
// without constantly editing cg_local.h.  Simply add a pointer in the
// struct in cg_local.h to a new struct here, and then just edit the
// struct here as needed ...

#define PRINTF_BUFFER		64

typedef struct {
	qhandle_t	lightningShaderR;
	qhandle_t	lightningShaderB;
	qhandle_t	friendShaders[MAX_CLIENTS];
	qhandle_t	armorShader;
	qhandle_t	plasmaBallShaderBlue;
	qhandle_t	plasmaBallShaderRed;
	qhandle_t	plasmaExplosionShaderB;
	qhandle_t	plasmaExplosionShaderR;
} as_media_t;

typedef struct {
	qboolean		teamColoredPlasma;
} as_features_t;

typedef struct {
	char			bufferPrint[PRINTF_BUFFER][TEAMCHAT_WIDTH*3+1];
	int				bufferExpireTime[PRINTF_BUFFER];
	int				bufferTotalLines;
	int				bufferCurrentLine;
	as_media_t		media;
	char			esv[MAX_VSTRING];
	qboolean		largeHud;
	as_features_t	features;
	int				weaponRates[WP_NUM_WEAPONS];
} as_cgs_t;
