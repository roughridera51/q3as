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

#define AS_MAX_MODES	20
#define AS_MODE_INTERFACE_VERSION	2

// Use OUR own numbers so that this .h file can be used outside Q3
#define AS_NUM_WEAPONS		14
#define AS_NUM_POWERUPS		15
#define AS_NUM_ARMORS		4		
#define AS_NUM_HEALTHS		5
#define AS_NUM_HOLDABLES	6
#define AS_NUM_GAMETYPES	8

typedef enum {
	// Normal
	AS_WF_GAUNTLET,
	AS_WF_MG,
	AS_WF_SG,
	AS_WF_GL,
	AS_WF_RL,
	AS_WF_LG,
	AS_WF_DRG,
	AS_WF_PG,
	AS_WF_BFG,
	AS_WF_GH,
	AS_WF_NG,
	AS_WF_PL,
	AS_WF_CG,

	// Excessive
	AS_WF_EGL,
	AS_WF_EBFG,
	AS_WF_EPG,
	AS_WF_ESG,
	AS_WF_ELG,

	AS_WF_END,	// Add above this line!
} as_modeWF_t;

typedef struct {
	int				spawnMask;
	qboolean		startWith[AS_NUM_WEAPONS];
	// ----------------- //
	short			damage[AS_NUM_WEAPONS];
	short			splashRadius[AS_NUM_WEAPONS];
	short			splashDamage[AS_NUM_WEAPONS];
	short			missileSpeed[AS_NUM_WEAPONS];
	short			firingRate[AS_NUM_WEAPONS];
	short			spread[AS_NUM_WEAPONS];
	short			misc[AS_NUM_WEAPONS];
	as_modeWF_t		fire[AS_NUM_WEAPONS];
	short			misc2[AS_NUM_WEAPONS];
	
} as_modeWeapon_t;

typedef struct {
	int				spawnMask;
	short			startWith[AS_NUM_WEAPONS];
	// ----------------- //
	short			regenRate[AS_NUM_WEAPONS];
	short			regenInc[AS_NUM_WEAPONS];
	short			regenMax[AS_NUM_WEAPONS];
	//short			pickup[AS_NUM_WEAPONS];
	//short			max[AS_NUM_WEAPONS];
} as_modeAmmo_t;

typedef struct {
	int				spawnMask;
	short			startWith[AS_NUM_POWERUPS];
} as_modePowerup_t;

typedef struct {
	int				spawnMask;
	short			spawnBonus;
	short			regenRate;
	short			regenInc;
	short			regenMax;
} as_modeArmor_t;

typedef struct {
	int				spawnMask;
	short			spawnBonus;
	short			regenRate;
	short			regenInc;
	short			regenMax;
} as_modeHealth_t;

typedef struct {
	int				spawnMask;
	qboolean		startWith[AS_NUM_HOLDABLES];
} as_modeHoldable_t;

#define AS_MM_NODROPWEAPONS		0x00000001
#define AS_MM_NODROPPOWERUPS	0x00000002
#define AS_MM_NOFALLING			0x00000004
#define AS_MM_NOSELF			0x00000008
#define AS_MM_NOHEALTHDECAY		0x00000010
#define AS_MM_NOARMORDECAY		0x00000020
#define AS_MM_REGENHEALTH		0x00000040
#define AS_MM_REGENARMOR		0x00000080
#define AS_MM_REGENAMMO			0x00000100
#define AS_MM_SHORTMINE			0x00000200
#define AS_MM_LONGMINE			0x00000400
#define AS_MM_NOSELFWOPU		0x00000800

#define BG_NUMITEMS 52
typedef struct {
	short				modeHandle;
	char				fullname[50];
	char				gamename[25];
	int					mask;
	as_modeWeapon_t		weapon;
	as_modeAmmo_t		ammo[AS_NUM_GAMETYPES];
	as_modePowerup_t	powerup;
	as_modeArmor_t		armor;
	as_modeHealth_t		health;
	as_modeHoldable_t	holdable;
	short				swap[BG_NUMITEMS];
	short				value[BG_NUMITEMS];
	short				respawn[BG_NUMITEMS];
} as_modeInfo_t;

typedef struct {
	char				statement[30];		// So those attempting to edit file will see this message
	char				r1[40];				// Reserved for future use
	short				r2;					// Reserved for future use
	short				interfaceVersion;	// If ANYTHING about as_allModes_t changes, this number must change
	short				fileVersion;		// Minor revisions - IE addition or tuning of a mode
	short				numModes;			// counting starts at 1
} as_modeHeader_t;

typedef struct {
	as_modeHeader_t		header;
	as_modeInfo_t		mi[AS_MAX_MODES];
} as_allModes_t;

