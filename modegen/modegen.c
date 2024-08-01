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

#include "modegen.h"

static as_allModes_t	as_modes;

/***********************************************************************************
 *									FUNCTIONS
 ***********************************************************************************/
static void as_setDefaults(as_modeInfo_t *mi) {
	int			i;

	memcpy(&(mi->weapon), &as_defaultWeapons, sizeof(mi->weapon));
	for (i = 1; i < BG_NUMITEMS; i++) {
		mi->swap[i] = i;
		mi->value[i] = -1;
		mi->respawn[i] = -2;
	}
	mi->weapon.spawnMask = mi->powerup.spawnMask = mi->armor.spawnMask =
		mi->health.spawnMask = mi->holdable.spawnMask = MAX_QINT;
	for (i = 0; i < AS_NUM_GAMETYPES; i++) {
		mi->ammo[i].spawnMask = MAX_QINT;
		mi->ammo[i].startWith[WP_GAUNTLET] = -1;
		mi->ammo[i].startWith[WP_GRAPPLING_HOOK] = -1;
		mi->ammo[i].startWith[WP_MACHINEGUN] = 100;
	}
	mi->health.spawnBonus = 25;
	mi->ammo[GT_TEAM].startWith[WP_MACHINEGUN] = 50;	// TDM gets less ammo
}

static void as_initmode_default(void) {
	as_setDefaults(&as_modes.mi[AS_MODE_DEFAULT]);

	as_modes.mi[AS_MODE_DEFAULT].modeHandle = AS_MODE_DEFAULT;
	strncpy(as_modes.mi[AS_MODE_DEFAULT].fullname, "Default", sizeof(as_modes.mi[AS_MODE_DEFAULT].fullname));
	strncpy(as_modes.mi[AS_MODE_DEFAULT].gamename, "baseq3", sizeof(as_modes.mi[AS_MODE_DEFAULT].gamename));
}

static void as_initmode_instagib(void) {
	int				i;

	as_setDefaults(&as_modes.mi[AS_MODE_INSTAGIB]);

	as_modes.mi[AS_MODE_INSTAGIB].modeHandle = AS_MODE_INSTAGIB;
	strncpy(as_modes.mi[AS_MODE_INSTAGIB].fullname, "Instagib", sizeof(as_modes.mi[AS_MODE_INSTAGIB].fullname));
	strncpy(as_modes.mi[AS_MODE_INSTAGIB].gamename, "instagib", sizeof(as_modes.mi[AS_MODE_INSTAGIB].gamename));
	as_modes.mi[AS_MODE_INSTAGIB].mask = AS_MM_NODROPWEAPONS | AS_MM_NOFALLING;
	as_modes.mi[AS_MODE_INSTAGIB].health.spawnBonus = 0;
	as_modes.mi[AS_MODE_INSTAGIB].weapon.spawnMask = 0;
	as_modes.mi[AS_MODE_INSTAGIB].weapon.startWith[WP_GAUNTLET] = 0;
	as_modes.mi[AS_MODE_INSTAGIB].weapon.startWith[WP_MACHINEGUN] = 0;
	as_modes.mi[AS_MODE_INSTAGIB].weapon.startWith[WP_RAILGUN] = 1;
	as_modes.mi[AS_MODE_INSTAGIB].weapon.damage[WP_RAILGUN] = 500;
	for (i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_INSTAGIB].ammo[i].spawnMask = 0;
		as_modes.mi[AS_MODE_INSTAGIB].ammo[i].startWith[WP_RAILGUN] = 999;
	}
	as_modes.mi[AS_MODE_INSTAGIB].powerup.spawnMask = F(PW_HASTE) | F(PW_INVIS) | F(PW_FLIGHT);
	as_modes.mi[AS_MODE_INSTAGIB].armor.spawnMask = 0;
	as_modes.mi[AS_MODE_INSTAGIB].health.spawnMask = 0;
	as_modes.mi[AS_MODE_INSTAGIB].holdable.spawnMask = F(HI_TELEPORTER) | F(HI_KAMIKAZE) | F(HI_INVULNERABILITY);
}

static void as_initmode_excessive(void) {
	int			i,j;

	as_setDefaults(&as_modes.mi[AS_MODE_EXCESSIVE]);

	as_modes.mi[AS_MODE_EXCESSIVE].modeHandle = AS_MODE_EXCESSIVE;
	strncpy(as_modes.mi[AS_MODE_EXCESSIVE].fullname, "Excessive", sizeof(as_modes.mi[AS_MODE_EXCESSIVE].fullname));
	strncpy(as_modes.mi[AS_MODE_EXCESSIVE].gamename, "Excessive", sizeof(as_modes.mi[AS_MODE_EXCESSIVE].gamename));
	as_modes.mi[AS_MODE_EXCESSIVE].mask = AS_MM_NODROPWEAPONS | AS_MM_NODROPPOWERUPS | AS_MM_NOSELF | AS_MM_NOARMORDECAY | AS_MM_REGENHEALTH | AS_MM_REGENAMMO | AS_MM_SHORTMINE;
	as_modes.mi[AS_MODE_EXCESSIVE].health.spawnBonus = 200;
	as_modes.mi[AS_MODE_EXCESSIVE].health.regenRate = 1000;
	as_modes.mi[AS_MODE_EXCESSIVE].health.regenInc = 5;
	as_modes.mi[AS_MODE_EXCESSIVE].health.regenMax = 300;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.spawnMask = 0;
	for (i = WP_GAUNTLET; i < WP_NUM_WEAPONS; i++)
		as_modes.mi[AS_MODE_EXCESSIVE].weapon.startWith[i] = 1;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.startWith[WP_GRAPPLING_HOOK] = 0;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_GAUNTLET] = 750;				// based off g_debugDamage
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_MACHINEGUN] = 45;				// based off g_debugDamage
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_RAILGUN] = 40;					// based off g_debugDamage
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_GRENADE_LAUNCHER] = 1000;		// based off g_debugDamage
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_PLASMAGUN] = 60;				// based off g_debugDamage
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_LIGHTNING] = 0;					// Guesstimate - all damage done via explosions
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_BFG] = 0;						// The BFG ball doesn't seem to have any effect ?

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_NAILGUN] = 15;					// NEW
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.damage[WP_CHAINGUN] = 20;					// NEW

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashRadius[WP_GRENADE_LAUNCHER] = 200;	// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashRadius[WP_PLASMAGUN] = 30;			// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashDamage[WP_GRENADE_LAUNCHER] = 200;	// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashDamage[WP_PLASMAGUN] = 60;			// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashDamage[WP_BFG] = 0;					// The BFG ball doesn't seem to have any effect ?
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashRadius[WP_BFG] = 0;					// The BFG ball doesn't seem to have any effect ?
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashDamage[WP_LIGHTNING] = 70;			// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashRadius[WP_LIGHTNING] = 80;			// Guesstimate

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashDamage[WP_PROX_LAUNCHER] = 200;		// NEW
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.splashRadius[WP_PROX_LAUNCHER] = 275;		// NEW
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.misc[WP_PROX_LAUNCHER] = 1000;			// NEW - lower the activation time

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_GAUNTLET] = 40;				// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_MACHINEGUN] = 40;			// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_SHOTGUN] = 500;				// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_GRENADE_LAUNCHER] = 400;	// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_ROCKET_LAUNCHER] = 100;		// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_LIGHTNING] = 100;			// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_RAILGUN] = 80;				// Guesstimate
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_BFG] = 750;					// Guesstimate

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_NAILGUN] = 400;				// NEW
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_PROX_LAUNCHER] = 500;		// NEW
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.firingRate[WP_CHAINGUN] = 20;				// NEW

	as_modes.mi[AS_MODE_EXCESSIVE].weapon.misc[WP_MACHINEGUN] = 45;					// Same dmg for team/nonteam modes
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.fire[WP_SHOTGUN] = AS_WF_ESG;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.fire[WP_GRENADE_LAUNCHER] = AS_WF_EGL;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.fire[WP_LIGHTNING] = AS_WF_ELG;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.fire[WP_PLASMAGUN] = AS_WF_EPG;
	as_modes.mi[AS_MODE_EXCESSIVE].weapon.fire[WP_BFG] = AS_WF_EBFG;
	for (i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].spawnMask = 0;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_MACHINEGUN] = 500;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_SHOTGUN] = 50;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_GRENADE_LAUNCHER] = 50;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_ROCKET_LAUNCHER] = 100;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_LIGHTNING] = 200;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_RAILGUN] = 100;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_PLASMAGUN] = 100;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_BFG] = 60;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_NAILGUN] = 30;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_PROX_LAUNCHER] = 30;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].startWith[WP_CHAINGUN] = 200;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_MACHINEGUN] = 200;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_SHOTGUN] = 2000;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_GRENADE_LAUNCHER] = 1500;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_ROCKET_LAUNCHER] = 400;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_LIGHTNING] = 400;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_RAILGUN] = 400;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_PLASMAGUN] = 400;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_BFG] = 4000;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_NAILGUN] = 1500;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_PROX_LAUNCHER] = 2000;
		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenRate[WP_CHAINGUN] = 150;

		as_modes.mi[AS_MODE_EXCESSIVE].ammo[i].regenInc[WP_GAUNTLET] = 0;
	}
	for (i = WP_MACHINEGUN; i < WP_NUM_WEAPONS; i++) {
		for (j = 0; j < AS_NUM_GAMETYPES; j++) {
			as_modes.mi[AS_MODE_EXCESSIVE].ammo[j].regenInc[i] = 1;
			as_modes.mi[AS_MODE_EXCESSIVE].ammo[j].regenMax[i] = as_modes.mi[AS_MODE_EXCESSIVE].ammo[j].startWith[i];
		}
	}
	as_modes.mi[AS_MODE_EXCESSIVE].powerup.spawnMask = F(PW_QUAD) | F(PW_BATTLESUIT) | F(PW_HASTE) | F(PW_INVIS) | F(PW_FLIGHT) | F(PW_SCOUT) | F(PW_DOUBLER);
	as_modes.mi[AS_MODE_EXCESSIVE].health.spawnMask = 0;
	as_modes.mi[AS_MODE_EXCESSIVE].holdable.spawnMask = F(HI_TELEPORTER) | F(HI_KAMIKAZE) | F(HI_INVULNERABILITY);
}

static void as_initmode_doublefun(void) {
	as_setDefaults(&as_modes.mi[AS_MODE_DOUBLEFUN]);

	as_modes.mi[AS_MODE_DOUBLEFUN].modeHandle = AS_MODE_DOUBLEFUN;
	strncpy(as_modes.mi[AS_MODE_DOUBLEFUN].fullname, "Doublefun", sizeof(as_modes.mi[AS_MODE_DOUBLEFUN].fullname));
	strncpy(as_modes.mi[AS_MODE_DOUBLEFUN].gamename, "doublefun", sizeof(as_modes.mi[AS_MODE_DOUBLEFUN].gamename));
	as_modes.mi[AS_MODE_DOUBLEFUN].mask = AS_MM_NOSELF;

	as_modes.mi[AS_MODE_DOUBLEFUN].swap[weapon_prox_launcher] = weapon_lightning;
	as_modes.mi[AS_MODE_DOUBLEFUN].swap[ammo_mines] = ammo_lightning;
	as_modes.mi[AS_MODE_DOUBLEFUN].swap[item_doubler] = item_health_mega;
	as_modes.mi[AS_MODE_DOUBLEFUN].swap[item_armor_combat] = item_health_large;
}

static void as_initmode_balanced(void) {
	int i;

	as_setDefaults(&as_modes.mi[AS_MODE_BALANCED]);

	as_modes.mi[AS_MODE_BALANCED].modeHandle = AS_MODE_BALANCED;
	strncpy(as_modes.mi[AS_MODE_BALANCED].fullname, "Balanced", sizeof(as_modes.mi[AS_MODE_BALANCED].fullname));
	strncpy(as_modes.mi[AS_MODE_BALANCED].gamename, "balanced", sizeof(as_modes.mi[AS_MODE_BALANCED].gamename));
	// chaingun changes
	as_modes.mi[AS_MODE_BALANCED].weapon.damage[WP_CHAINGUN] = 5;
	// prox mine changes
	as_modes.mi[AS_MODE_BALANCED].mask = AS_MM_LONGMINE;
	as_modes.mi[AS_MODE_BALANCED].weapon.splashDamage[WP_PROX_LAUNCHER] = 75;
	as_modes.mi[AS_MODE_BALANCED].weapon.splashRadius[WP_PROX_LAUNCHER] = 125;
	as_modes.mi[AS_MODE_BALANCED].weapon.misc[WP_PROX_LAUNCHER] = 1500; // activation time
	as_modes.mi[AS_MODE_BALANCED].weapon.misc2[WP_PROX_LAUNCHER] = 250; // trigger time
	as_modes.mi[AS_MODE_BALANCED].weapon.firingRate[WP_PROX_LAUNCHER] = 1000;
	// nailgun changes
	as_modes.mi[AS_MODE_BALANCED].weapon.misc2[WP_NAILGUN] = 10; // # nails
	// ammo regen changes and ammo box changes
	for ( i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_BALANCED].ammo[i].regenRate[WP_PROX_LAUNCHER] = 2500;
		//as_modes.mi[AS_MODE_BALANCED].ammo[i].pickup[WP_PROX_LAUNCHER] = 5;
		//as_modes.mi[AS_MODE_BALANCED].ammo[i].pickup[WP_NAILGUN] = 10;
	}
}

static void as_initmode_rocketarena(void) {
	int i;

	as_setDefaults(&as_modes.mi[AS_MODE_ROCKETARENA]);

	as_modes.mi[AS_MODE_ROCKETARENA].modeHandle = AS_MODE_ROCKETARENA;
	strncpy(as_modes.mi[AS_MODE_ROCKETARENA].fullname, "RocketArena", sizeof(as_modes.mi[AS_MODE_ROCKETARENA].fullname));
	strncpy(as_modes.mi[AS_MODE_ROCKETARENA].gamename, "rocketarena", sizeof(as_modes.mi[AS_MODE_ROCKETARENA].gamename));

	// copied from "balanced" mode
	// chaingun changes
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.damage[WP_CHAINGUN] = 5;
	// prox mine changes
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.splashDamage[WP_PROX_LAUNCHER] = 75;
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.splashRadius[WP_PROX_LAUNCHER] = 125;
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.misc[WP_PROX_LAUNCHER] = 1500; // activation time
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.misc2[WP_PROX_LAUNCHER] = 250; // trigger time
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.firingRate[WP_PROX_LAUNCHER] = 1000;
	// nailgun changes
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.misc2[WP_NAILGUN] = 10; // # nails
	// ammo regen changes and ammo box changes
	for ( i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].regenRate[WP_PROX_LAUNCHER] = 2500;
		//as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].pickup[WP_PROX_LAUNCHER] = 5;
		//as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].pickup[WP_NAILGUN] = 10;
	}
	// end balanced

	// no self damage if not carring TA powerup
	as_modes.mi[AS_MODE_ROCKETARENA].mask = AS_MM_NOSELFWOPU | AS_MM_NOFALLING | AS_MM_LONGMINE;

	// start with 100 health, 100 armor
	as_modes.mi[AS_MODE_ROCKETARENA].health.spawnBonus = 0;
	as_modes.mi[AS_MODE_ROCKETARENA].armor.spawnBonus = 100;

	// start with all weapons
	for (i = WP_GAUNTLET; i < WP_NUM_WEAPONS; i++) {
		as_modes.mi[AS_MODE_ROCKETARENA].weapon.startWith[i] = 1;
	}
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.startWith[WP_GRAPPLING_HOOK] = 0;

	// no weapons, ammo, armor or heath spawns
	as_modes.mi[AS_MODE_ROCKETARENA].weapon.spawnMask = 0;
	as_modes.mi[AS_MODE_ROCKETARENA].armor.spawnMask = 0;
	as_modes.mi[AS_MODE_ROCKETARENA].health.spawnMask = 0;
	for (i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].spawnMask = 0;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_MACHINEGUN] = 200;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_SHOTGUN] = 100;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_GRENADE_LAUNCHER] = 10;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_ROCKET_LAUNCHER] = 50;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_LIGHTNING] = 200;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_RAILGUN] = 50;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_PLASMAGUN] = 200;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_BFG] = 1;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_NAILGUN] = 5;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_PROX_LAUNCHER] = 1;
		as_modes.mi[AS_MODE_ROCKETARENA].ammo[i].startWith[WP_CHAINGUN] = 100;
	}
}

static void as_initmode_hasteinstagib(void) {
	int				i;

	as_setDefaults(&as_modes.mi[AS_MODE_HASTEINSTAGIB]);

	as_modes.mi[AS_MODE_HASTEINSTAGIB].modeHandle = AS_MODE_HASTEINSTAGIB;
	strncpy(as_modes.mi[AS_MODE_HASTEINSTAGIB].fullname, "Haste Instagib", sizeof(as_modes.mi[AS_MODE_HASTEINSTAGIB].fullname));
	strncpy(as_modes.mi[AS_MODE_HASTEINSTAGIB].gamename, "hasteinstagib", sizeof(as_modes.mi[AS_MODE_HASTEINSTAGIB].gamename));
	as_modes.mi[AS_MODE_HASTEINSTAGIB].mask = AS_MM_NODROPWEAPONS | AS_MM_NOFALLING | AS_MM_NODROPPOWERUPS;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].health.spawnBonus = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].weapon.spawnMask = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].weapon.startWith[WP_GAUNTLET] = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].weapon.startWith[WP_MACHINEGUN] = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].weapon.startWith[WP_RAILGUN] = 1;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].weapon.damage[WP_RAILGUN] = 500;
	for (i = 0; i < AS_NUM_GAMETYPES; i++) {
		as_modes.mi[AS_MODE_HASTEINSTAGIB].ammo[i].spawnMask = 0;
		as_modes.mi[AS_MODE_HASTEINSTAGIB].ammo[i].startWith[WP_RAILGUN] = 999;
	}
	as_modes.mi[AS_MODE_HASTEINSTAGIB].powerup.spawnMask = F(PW_INVIS) | F(PW_FLIGHT);
	as_modes.mi[AS_MODE_HASTEINSTAGIB].powerup.startWith[PW_HASTE] = 9999;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].armor.spawnMask = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].health.spawnMask = 0;
	as_modes.mi[AS_MODE_HASTEINSTAGIB].holdable.spawnMask = F(HI_TELEPORTER) | F(HI_KAMIKAZE) | F(HI_INVULNERABILITY);
}

/***********************************************************************************
 *									MAIN
 ***********************************************************************************/
void main (void) {
	FILE	*f;
	char	filename[100];

	printf("Initializing ...\n");
	memset(&as_modes, 0, sizeof(as_modes));

	as_modes.header.numModes = AS_MODE_NUM_MODES;
	as_modes.header.interfaceVersion = AS_MODE_INTERFACE_VERSION;
	as_modes.header.fileVersion = FILEVERSION;
	strncpy(as_modes.header.statement, "Please do not edit this file.", sizeof(as_modes.header.statement));

	as_initmode_default();
	as_initmode_instagib();
	as_initmode_excessive();
	as_initmode_doublefun();
	as_initmode_balanced();
	as_initmode_rocketarena();
	as_initmode_hasteinstagib();

	sprintf(filename, "modes_%d-%d.dat", AS_MODE_INTERFACE_VERSION, FILEVERSION);
	printf("Writing to %s ...\n", filename);
	f = fopen(filename, "wb");
	if (!f) {
		printf("ERROR: Unable to open %s for writing.\n", filename);
	} else {
		fwrite(&as_modes, sizeof(as_modes), 1, f);
		fclose(f);
		printf("Done!\n");
	}
}

