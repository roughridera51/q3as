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

typedef struct {
	void			(*fireFunction)(gentity_t *ent, as_weaponArgs_t args);
} as_fireMap_t;

void as_wf_gauntlet(gentity_t *ent, as_weaponArgs_t args);
void as_wf_mg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_cg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_sg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_gl(gentity_t *ent, as_weaponArgs_t args);
void as_wf_rl(gentity_t *ent, as_weaponArgs_t args);
void as_wf_lg(gentity_t *ent, as_weaponArgs_t args);
// USE as_wf_drg INSTEAD - void as_wf_rg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_pg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_bfg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_gh(gentity_t *ent, as_weaponArgs_t args);
void as_wf_ng(gentity_t *ent, as_weaponArgs_t args);
void as_wf_pl(gentity_t *ent, as_weaponArgs_t args);
void as_wf_egl(gentity_t *ent, as_weaponArgs_t args);
void as_wf_ebfg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_epg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_esg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_elg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_drg(gentity_t *ent, as_weaponArgs_t args);

static as_fireMap_t	as_fireMap[] = {
	as_wf_gauntlet,
	as_wf_mg,
	as_wf_sg,
	as_wf_gl,
	as_wf_rl,
	as_wf_lg,
	// as_wf_rg, - use as_wf_drg below instead!
	as_wf_drg,
	as_wf_pg,
	as_wf_bfg,
	as_wf_gh,
	as_wf_ng,
	as_wf_pl,
	as_wf_cg,
	as_wf_egl,
	as_wf_ebfg,
	as_wf_epg,
	as_wf_esg,
	as_wf_elg,
};


// from g_weapon
void CalcMuzzlePointOrigin ( gentity_t *ent, vec3_t origin, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );

void as_fireWeapon( gentity_t *ent ) {
	as_weaponArgs_t		args;
	as_modeWF_t			wf;

	if (ent->client->ps.powerups[PW_QUAD] ) {
		args.s_quadFactor = g_quadfactor.value;
	} else {
		args.s_quadFactor = 1;
	}
#ifdef MISSIONPACK
	if( ent->client->persistantPowerup && ent->client->persistantPowerup->item && ent->client->persistantPowerup->item->giTag == PW_DOUBLER ) {
		args.s_quadFactor *= 2;
	}
#endif

	// track shots taken for accuracy tracking.  Grapple is not a weapon and gauntet is just not tracked
	if( ent->s.weapon != WP_GRAPPLING_HOOK && ent->s.weapon != WP_GAUNTLET ) {
#ifdef MISSIONPACK
		if( ent->s.weapon == WP_NAILGUN ) {
			ent->client->accuracy_shots += as_mode.weapon.misc2[WP_NAILGUN]; // from NUM_NAILSHOTS in g_weapon
		} else {
			ent->client->accuracy_shots++;
		}
#else
		ent->client->accuracy_shots++;
#endif
	}

	// set aiming directions
	AngleVectors (ent->client->ps.viewangles, args.forward, args.right, args.up);

	CalcMuzzlePointOrigin ( ent, ent->client->oldOrigin, args.forward, args.right, args.up, args.muzzle );

	wf = as_mode.weapon.fire[ent->s.weapon];

	if (as_fireMap[wf].fireFunction) {
		as_fireMap[wf].fireFunction(ent, args);
	}
}

void as_runExplodeFunction(gentity_t *ent) {
	if (ent->q3as.explodeFunction)
		ent->q3as.explodeFunction(ent);
}

// "Excessive" weapons
void as_wf_egl(gentity_t *ent, as_weaponArgs_t args) {
	gentity_t	*m;

	// extra vertical velocity
	args.forward[2] += 0.2f;
	VectorNormalize( args.forward );

	m = fire_grenade (ent, args.muzzle, args.forward);
	m->damage *= args.s_quadFactor;
	m->splashDamage *= args.s_quadFactor;
	m->nextthink = level.time +  1000;
	m->s.eFlags = 0;	// explode on impact - don't bounce
}

void as_spewGrenades(gentity_t *ent) {
	int i;
	vec3_t dir;
	gentity_t	*m;
	vec3_t origin;
	trajectory_t t;

	// Step the original missile back a bit so the grenades we create
	// won't get stuck in the wall
	VectorCopy(ent->r.currentOrigin, t.trBase);		// from it's current position ..
	VectorNegate(ent->s.pos.trDelta, t.trDelta);	// move BACK the way it came ...
	VectorNormalize(t.trDelta);						// (fix vector)
	VectorScale(t.trDelta, 50, t.trDelta);			// at 50 units/s
	t.trTime = level.time - 100;					// pretend like it was at trBase 100ms ago
	t.trType = TR_LINEAR;							// straight line
	BG_EvaluateTrajectory(&t, level.time, origin);	// Do the math - where is it now at 50ups after 100ms?
	
	for (i=1; i<6; i++) {
		VectorCopy(ent->s.pos.trDelta, dir);
		VectorNormalize(dir);
		dir[0] += crandom();
		dir[1] += crandom();
		dir[2] += crandom();
		m = fire_grenade (ent->parent, origin, dir);
		m->nextthink = level.time + 50 + random() * 200;
		// These are all guesstimates
		m->damage = 125;
		m->splashDamage = 200;
		m->splashRadius = 100;
		// end guessimates
		m->q3as.noBounceNoise = qtrue;
		m->methodOfDeath = MOD_BFG;
		m->splashMethodOfDeath = MOD_BFG_SPLASH;
		VectorScale( dir, 1000, m->s.pos.trDelta );	// guesstimate on speed ...
		if ( (ent->parent) && (ent->parent->client) ) {
			if (ent->parent->client->ps.powerups[PW_QUAD] ) {
				m->damage *= g_quadfactor.value;
				m->splashDamage *= g_quadfactor.value;
			}
		}
	}
}

void as_wf_ebfg(gentity_t *ent, as_weaponArgs_t args) {
	gentity_t	*m;

	m = fire_bfg (ent, args.muzzle, args.forward);
	m->damage *= args.s_quadFactor;
	m->splashDamage *= args.s_quadFactor;
	m->q3as.explodeFunction = as_spewGrenades;
}

void as_wf_epg(gentity_t *ent, as_weaponArgs_t args) {
	gentity_t	*m;
	vec3_t		left, right;

	m = fire_plasma (ent, args.muzzle, args.forward);
	m->damage *= args.s_quadFactor;
	m->splashDamage *= args.s_quadFactor;

	VectorMA(args.forward, 0.34, args.right, right);
	m = fire_plasma (ent, args.muzzle, right);
	m->damage *= args.s_quadFactor;
	m->splashDamage *= args.s_quadFactor;

	VectorMA(args.forward, -0.34, args.right, left);
	m = fire_plasma (ent, args.muzzle, left);
	m->damage *= args.s_quadFactor;
	m->splashDamage *= args.s_quadFactor;
}

void as_wf_rg(gentity_t *ent, as_weaponArgs_t args);
void as_wf_esg(gentity_t *ent, as_weaponArgs_t args) {
	int			i;
	float		r, u;
	vec3_t		end;
	int			seed;
	as_weaponArgs_t	newArgs;

	seed = rand() & 255;

	for ( i = 0 ; i < DEFAULT_SHOTGUN_COUNT ; i++ ) {
		r = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		u = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		VectorMA( args.muzzle, 8192 * 16, args.forward, end);
		VectorMA (end, r, args.right, end);
		VectorMA (end, u, args.up, end);
		VectorNormalize(end);
		VectorCopy(end, newArgs.forward);
		VectorCopy(args.right, newArgs.right);
		VectorCopy(args.up, newArgs.up);
		VectorCopy(args.muzzle, newArgs.muzzle);
		newArgs.mod = MOD_SHOTGUN;
		newArgs.s_quadFactor = args.s_quadFactor;

		as_wf_rg(ent, newArgs);
	}
}

void G_BounceProjectile( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout );
void as_wf_elg(gentity_t *ent, as_weaponArgs_t args) {
	trace_t		tr;
	vec3_t		end;
#ifdef MISSIONPACK
	vec3_t impactpoint, bouncedir;
#endif
	gentity_t	*traceEnt, *tent;
	int			damage, i, passent;
	gentity_t	*m;

	damage = as_mode.weapon.damage[WP_LIGHTNING] * args.s_quadFactor;

	passent = ent->s.number;
	for (i = 0; i < 10; i++) {
		VectorMA( args.muzzle, LIGHTNING_RANGE, args.forward, end );

		trap_Trace( &tr, args.muzzle, NULL, NULL, end, passent, MASK_SHOT );

#ifdef MISSIONPACK
		// if not the first trace (the lightning bounced of an invulnerability sphere)
		if (i) {
			// add bounced off lightning bolt temp entity
			// the first lightning bolt is a cgame only visual
			//
			tent = G_TempEntity( args.muzzle, EV_LIGHTNINGBOLT );
			VectorCopy( tr.endpos, end );
			SnapVector( end );
			VectorCopy( end, tent->s.origin2 );
		}
#endif
		if ( tr.entityNum == ENTITYNUM_NONE ) {
			return;
		}

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->takedamage) {
#ifdef MISSIONPACK
			if ( traceEnt->client && traceEnt->client->invulnerabilityTime > level.time ) {
				if (G_InvulnerabilityEffect( traceEnt, args.forward, tr.endpos, impactpoint, bouncedir )) {
					G_BounceProjectile( args.muzzle, impactpoint, bouncedir, end );
					VectorCopy( impactpoint, args.muzzle );
					VectorSubtract( end, impactpoint, args.forward );
					VectorNormalize(args.forward);
					// the player can hit him/herself with the bounced lightning
					passent = ENTITYNUM_NONE;
				}
				else {
					VectorCopy( tr.endpos, args.muzzle );
					passent = traceEnt->s.number;
				}
				continue;
			}
			else {
				G_Damage( traceEnt, ent, ent, args.forward, tr.endpos,
					damage, 0, MOD_LIGHTNING);
			}
#else
				G_Damage( traceEnt, ent, ent, args.forward, tr.endpos,
					damage, 0, MOD_LIGHTNING);
#endif
		}

		if ( traceEnt->takedamage && traceEnt->client ) {
			tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
			tent->s.otherEntityNum = traceEnt->s.number;
			tent->s.eventParm = DirToByte( tr.plane.normal );
			tent->s.weapon = ent->s.weapon;
			if( LogAccuracyHit( traceEnt, ent ) ) {
				ent->client->accuracy_hits++;
			}
		} else if ( !( tr.surfaceFlags & SURF_NOIMPACT ) ) {
			tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
			tent->s.eventParm = DirToByte( tr.plane.normal );
		}

		if ( (traceEnt->takedamage && traceEnt->client) || (!(tr.surfaceFlags & SURF_NOIMPACT) ) ) {
			m = fire_rocket(ent, tr.endpos, tv(0,0,0));
			m->nextthink = level.time;
			m->damage = 0;
			m->splashDamage = as_mode.weapon.splashDamage[WP_LIGHTNING];
			m->splashRadius = as_mode.weapon.splashRadius[WP_LIGHTNING];
			m->methodOfDeath = MOD_LIGHTNING;
			m->splashMethodOfDeath = MOD_LIGHTNING;
		}
		break;
	}
}

// END EXCESSIVE

// drg = default railgun.  Need this little helper function
// in order to properly set up 'mod'
void as_wf_drg(gentity_t *ent, as_weaponArgs_t args) {
	args.mod = MOD_RAILGUN;
	as_wf_rg(ent, args);
}

