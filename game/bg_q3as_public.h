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

#define MAX_VSTRING 20

int as_vercmp (char *first, char *second);
int as_BGGetWeaponRate(int weapon);
void as_addTags(void);

typedef enum {
	HEALTH_NONE,
	HEALTH_5,
	HEALTH_25,
	HEALTH_50,
	HEALTH_100,
	HEALTH_NUM_HEALTHS,
} as_health_t;

typedef enum {
	ARMOR_NONE,
	ARMOR_5,
	ARMOR_50,
	ARMOR_100,
	ARMOR_NUM_ARMORS,
} as_armor_t;
