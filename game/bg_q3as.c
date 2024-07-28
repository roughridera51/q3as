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


#include "bg_q3as_public.h"
#include "q_shared.h"
#include "bg_public.h"

// from bg_lib.c
#define to_digit(c)		((c) - '0')
#define is_digit(c)		((unsigned)to_digit(c) <= 9)

// convert #.#f (a version number) to a big integer of the form
// ######### where every three #'s represents each piece of the version
// (first #, second #, and the letter, if any).
// IE: 4.3c -> 004003099 (which becomes 4003099 since the leading 0's
// aren't important).  This enables version comparison.
static int as_vtoi(char *string) {
	int		i = 0;
	int		mode = 0;	// 0 = integral, 1 = fractional, 2 = letter
	char	tmpstr[4];	// 3 chars and a null
	char	outstr[MAX_VSTRING];
	char	orig[MAX_VSTRING];

	Q_strncpyz(orig, string, sizeof(orig));

	memset(tmpstr, 0, sizeof(tmpstr));

	while (*string) {
		switch (mode) {
		case 0:
			if (*string == '.') {
				Com_sprintf(outstr, sizeof(outstr), "%03d", atoi(tmpstr));
				memset(tmpstr, 0, sizeof(tmpstr));
				mode = 1;
				i = 0;
			} else {
				if ( i < 3 )
					tmpstr[i] = *string;
				i++;
			}
			break;
		case 1:
			if (!is_digit(*string)) {
				// Copy out the fractional part; prepare for copying out the letter
				Com_sprintf(outstr, sizeof(outstr), "%s%03d", outstr, atoi(tmpstr));
				// We found a letter, convert it NOW
				mode = 2;
				Com_sprintf(outstr, sizeof(outstr), "%s%03d", outstr, *string);
			} else {
				if ( i < 3 )
					tmpstr[i] = *string;
				i++;
			}
			break;
		default:
			// we should never reach here unless there's more than one non-digit at the end
			// of the version string, which is invalid anyway ...
			Com_Printf("WARNING: Error encountered parsing version string: %s\n", orig);
			break;
		}
		string++;
	}
	if (mode == 0) {	// String was "#" (didn't contain decimal)
		Com_sprintf(outstr, sizeof(outstr), "%03d000000", atoi(tmpstr));
	}
	if (mode == 1) {	// String was "#.#" (didn't contain a letter at the end)
		Com_sprintf(outstr, sizeof(outstr), "%s%03d000", outstr, atoi(tmpstr));
	}

	return atoi(outstr);
}

// return 1 if first > second, 0 if equal, -1 if less
int as_vercmp (char *first, char *second) {
	int		f, s;

	f = as_vtoi(first);
	s = as_vtoi(second);

	if (f > s)
		return 1;
	if (f < s)
		return -1;
	return 0;
}

#if (defined CGAME || defined QAGAME)
// This function is named the same in both game and cgame
extern		int as_getWeaponRate(int weapon);

int as_BGGetWeaponRate(int weapon) {
	int		rate;

	rate = as_getWeaponRate(weapon);
	// Com_Printf("Rate for %d = %d\n", weapon, rate);
	return (rate);
}
#endif

static int as_tagForHealthValue(int value) {
	switch (value) {
	case 5:
		return HEALTH_5;
	case 25:
		return HEALTH_25;
	case 50:
		return HEALTH_50;
	case 100:
		return HEALTH_100;
	default:
		Com_Error(ERR_DROP, "Bad health value: %d\n", value);
		return 0;
	}
}

static int as_tagForArmorValue(int value) {
	switch (value) {
	case 5:
		return ARMOR_5;
	case 50:
		return ARMOR_50;
	case 100:
		return ARMOR_100;
	default:
		Com_Error(ERR_DROP, "Bad armor value: %d\n", value);
		return 0;
	}
}

void as_addTags(void) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if (bg_itemlist[i].giType == IT_HEALTH)
			bg_itemlist[i].giTag = as_tagForHealthValue(bg_itemlist[i].quantity);
		if (bg_itemlist[i].giType == IT_ARMOR)
			bg_itemlist[i].giTag = as_tagForArmorValue(bg_itemlist[i].quantity);
	}
}

