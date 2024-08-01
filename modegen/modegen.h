/*
 *  Copyright (C) 2003 Ben Goodwin
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

/*******************************************************************************************
 * Possible interface spec changes:
 *  - Like ammo, make most everything else editable by gametype (huge increase in filesize)
 *******************************************************************************************/

#include <stdio.h>
#include <string.h>
// Order is really important here
#include "q3.h"
#define MISSIONPACK
#include "../source/game/q3as_mode.h"

#define FILEVERSION		1

typedef enum {
	AS_MODE_DEFAULT,
	AS_MODE_INSTAGIB,
	AS_MODE_EXCESSIVE,
	AS_MODE_DOUBLEFUN,
	AS_MODE_BALANCED,
	AS_MODE_ROCKETARENA,
	AS_MODE_HASTEINSTAGIB,
	AS_MODE_NUM_MODES,
} as_modeHandle_t;

#define F(x) (1<<x)

// Cleaner way to predefine the default weapon characteristics
as_modeWeapon_t	as_defaultWeapons = {
	MAX_QINT,
	//  gaunt,   mg,   sg,   gl,   rl,   lg,   rg,   pg,  bfg, grap,   ng,   pl,   cg
	{ 0,    1,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 }, // spawn with
	{ 0,    50,   7,   10,  100,  100,    8,  100,   20,  100,    0,   20,    0,    7 }, // damage
	{ 0,    0,    0,    0,  150,  120,    0,    0,   20,  120,    0,    0,  150,    0 }, // splash radius
	{ 0,    0,    0,    0,  100,  100,    0,    0,   15,  100,    0,    0,  100,    0 }, // splash damage
	{ 0,    0,    0,    0,  700,  900,    0,    0, 2000, 2000,    0,  555,  700,    0 }, // bolt speed
	{ 0,  400,  100, 1000,  800,  800,   50, 1500,  100,  200,  400, 1000,  800,   30 }, // firing rate
	{ 0,    0,  200,    0,    0,    0,    0,    0,    0,    0,    0,  500,    0,  600 }, // spread
	{ 0,    0,    5,    0,    0,    0,    0,    0,    0,    0,    0, 1800, 2000,    0 }, // misc
	{ 0,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12 }, // fire function
	{ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   10,  500,    0 }, // misc2
};

// An enumerated list of everything in bg_itemlist
typedef enum {
	item_none,
    item_armor_shard,
    item_armor_combat,
    item_armor_body,
    item_health_small,
    item_health,
    item_health_large,
    item_health_mega,
    weapon_gauntlet,
    weapon_shotgun,
    weapon_machinegun,
    weapon_grenadelauncher,
    weapon_rocketlauncher,
    weapon_lightning,
    weapon_railgun,
    weapon_plasmagun,
    weapon_bfg,
    weapon_grapplinghook,
    ammo_shells,
    ammo_bullets,
    ammo_grenades,
    ammo_cells,
    ammo_lightning,
    ammo_rockets,
    ammo_slugs,
    ammo_bfg,
    holdable_teleporter,
    holdable_medkit,
    item_quad,
    item_enviro,
    item_haste,
    item_invis,
    item_regen,
    item_flight,
    team_CTF_redflag,
    team_CTF_blueflag,
    holdable_kamikaze,
    holdable_portal,
    holdable_invulnerability,
    ammo_nails,
    ammo_mines,
    ammo_belt,
    item_scout,
    item_guard,
    item_doubler,
    item_ammoregen,
    team_CTF_neutralflag,
    item_redcube,
    item_bluecube,
    weapon_nailgun,
    weapon_prox_launcher,
    weapon_chaingun,
} as_items_t;
