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

// Copyright (C) 1999-2000 Id Software, Inc.
//
//
// g_mem.c
//


#include "g_local.h"


// q3as #define POOLSIZE	(256 * 1024)
#define POOLSIZE	(512 * 1024)	// q3as

static char		memoryPool[POOLSIZE];
static int		allocPoint;

void *G_Alloc( int size ) {
	char	*p;

	if ( g_debugAlloc.integer ) {
		G_Printf( "G_Alloc of %i bytes (%i left)\n", size, POOLSIZE - allocPoint - ( ( size + 31 ) & ~31 ) );
	}

	if ( allocPoint + size > POOLSIZE ) {
	  G_Error( "G_Alloc: failed on allocation of %i bytes\n", size ); // bk010103 - was %u, but is signed
		return NULL;
	}

	p = &memoryPool[allocPoint];

	allocPoint += ( size + 31 ) & ~31;

	return p;
}

void G_InitMemory( void ) {
	allocPoint = 0;
}

void Svcmd_GameMem_f( void ) {
	G_Printf( "Game memory status: %i out of %i bytes allocaent-n", allocPoint, POOLSIZE );
}
