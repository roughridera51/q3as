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

// For things that g_local.h needs to reference
// Things in here cannot reference things that are defined in g_local.h!
// This is mostly to add/change the structs for g_entity and g_client ...

#define MAX_IPSTRING 32
#define MAX_SAY_TRACK 100

// From g_local.h, so we can use them here ...
typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

typedef struct {
	void		(*explodeFunction)(gentity_t *ent);
	qboolean	noBounceNoise;
} as_ent_t;

typedef struct {
	char		message[MAX_TOKEN_CHARS];	// message to centerprint to client
	int			endTime;					// what level.time the message should end
	int			llt;						// last level.time the centerprint command was sent
} as_linger_t;

typedef struct {
	qboolean	MOTDPrinted;				// Has this client had the MOTD sent yet?
	as_linger_t	linger;
	int			lastSayTime[MAX_SAY_TRACK];
	int			lastSayIndex;
	int			lastHealthRegenTime;
	int			lastArmorRegenTime;
	int			lastAmmoRegenTime[WP_NUM_WEAPONS];
	int			chatCount;
	int			lastChatCountReduce;
	int			lastSpecCheck;				// last level.time we counted spectator time for this client
} as_client_t;
