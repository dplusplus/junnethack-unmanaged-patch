/*	SCCS Id: @(#)teleport.c	3.4	2003/08/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2006
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009-2010
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"

STATIC_DCL boolean FDECL(tele_jump_ok, (int,int,int,int));
STATIC_DCL boolean FDECL(teleok, (int,int,BOOLEAN_P));
STATIC_DCL void NDECL(vault_tele);
STATIC_DCL boolean FDECL(rloc_pos_ok, (int,int,struct monst *));
STATIC_DCL void FDECL(mvault_tele, (struct monst *));

/*
 * Is (x, y) a bad position of mtmp?  If mtmp is NULL, then is (x, y) bad
 * for an object?
 *
 * Caller is responsible for checking (x, y) with isok() if required.
 *
 * Returns: -1: Inaccessible, 0: Good pos, 1: Temporally inacessible
 */
static int
badpos(x, y, mtmp, gpflags)
int x, y;
struct monst *mtmp;
unsigned gpflags;
{
	int is_badpos = 0, pool;
	struct permonst *mdat = NULL;
	boolean ignorewater = ((gpflags & MM_IGNOREWATER) != 0);
	struct monst *mtmp2;

	/* in many cases, we're trying to create a new monster, which
	 * can't go on top of the player or any existing monster.
	 * however, occasionally we are relocating engravings or objects,
	 * which could be co-located and thus get restricted a bit too much.
	 * oh well.
	 */
	if (mtmp != &youmonst && x == u.ux && y == u.uy
#ifdef STEED
			&& (!u.usteed || mtmp != u.usteed)
#endif
			)
	    is_badpos = 1;

	if (mtmp) {
	    mtmp2 = m_at(x,y);

	    /* Be careful with long worms.  A monster may be placed back in
	     * its own location.  Normally, if m_at() returns the same monster
	     * that we're trying to place, the monster is being placed in its
	     * own location.  However, that is not correct for worm segments,
	     * because all the segments of the worm return the same m_at().
	     * Actually we overdo the check a little bit--a worm can't be placed
	     * in its own location, period.  If we just checked for mtmp->mx
	     * != x || mtmp->my != y, we'd miss the case where we're called
	     * to place the worm segment and the worm's head is at x,y.
	     */
	    if (mtmp2 && (mtmp2 != mtmp || mtmp->wormno))
		is_badpos = 1;

	    mdat = mtmp->data;
	    pool = is_pool(x,y);
	    if (mdat->mlet == S_EEL && !pool && rn2(13) && !ignorewater)
		is_badpos = 1;

	    if (pool && !ignorewater) {
		if (mtmp == &youmonst)
			return (HLevitation || Flying || Wwalking ||
				    Swimming || Amphibious) ? is_badpos : -1;
		else	return (is_flyer(mdat) || is_swimmer(mdat) ||
				    is_clinger(mdat)) ? is_badpos : -1;
	    } else if (is_lava(x,y)) {
		if (mtmp == &youmonst)
		    return HLevitation ? is_badpos : -1;
		else
		    return (is_flyer(mdat) || likes_lava(mdat)) ?
			    is_badpos : -1;
	    }
	    if (passes_walls(mdat) && may_passwall(x,y)) return is_badpos;
	}
	if (!ACCESSIBLE(levl[x][y].typ)) {
		if (!(is_pool(x,y) && ignorewater)) return -1;
	}

	if (closed_door(x, y) && (!mdat || !amorphous(mdat)))
	    return mdat && (nohands(mdat) || verysmall(mdat)) ? -1 : 1;
	if (sobj_at(BOULDER, x, y) && (!mdat || !throws_rocks(mdat)))
	    return mdat ? -1 : 1;
	return is_badpos;
}

/*
 * Is (x,y) a good position of mtmp?  If mtmp is NULL, then is (x,y) good
 * for an object?
 *
 * This function will only look at mtmp->mdat, so makemon, mplayer, etc can
 * call it to generate new monster positions with fake monster structures.
 */
boolean
goodpos(x, y, mtmp, gpflags)
int x,y;
struct monst *mtmp;
unsigned gpflags;
{
	struct permonst *mdat = NULL;
	boolean ignorewater = ((gpflags & MM_IGNOREWATER) != 0);

	if (!isok(x, y)) return FALSE;

	/* in many cases, we're trying to create a new monster, which
	 * can't go on top of the player or any existing monster.
	 * however, occasionally we are relocating engravings or objects,
	 * which could be co-located and thus get restricted a bit too much.
	 * oh well.
	 */
	if (mtmp != &youmonst && x == u.ux && y == u.uy
#ifdef STEED
			&& (!u.usteed || mtmp != u.usteed)
#endif
			)
		return FALSE;

	if (mtmp) {
	    struct monst *mtmp2 = m_at(x,y);

	    /* Be careful with long worms.  A monster may be placed back in
	     * its own location.  Normally, if m_at() returns the same monster
	     * that we're trying to place, the monster is being placed in its
	     * own location.  However, that is not correct for worm segments,
	     * because all the segments of the worm return the same m_at().
	     * Actually we overdo the check a little bit--a worm can't be placed
	     * in its own location, period.  If we just checked for mtmp->mx
	     * != x || mtmp->my != y, we'd miss the case where we're called
	     * to place the worm segment and the worm's head is at x,y.
	     */
	    if (mtmp2 && (mtmp2 != mtmp || mtmp->wormno))
		return FALSE;

	    mdat = mtmp->data;
	    if (is_pool(x,y) && !ignorewater) {
		if (mtmp == &youmonst)
			return !!(HLevitation || Flying || Wwalking ||
					Swimming || Amphibious);
		else	return (is_flyer(mdat) || is_swimmer(mdat) ||
							is_clinger(mdat));
	    } else if (mdat->mlet == S_EEL && rn2(13) && !ignorewater) {
		return FALSE;
	    } else if (is_lava(x,y)) {
		if (mtmp == &youmonst)
		    return !!HLevitation;
		else
		    return (is_flyer(mdat) || likes_lava(mdat));
	    }
	    if (passes_walls(mdat) && may_passwall(x,y)) return TRUE;
	}
	if (!ACCESSIBLE(levl[x][y].typ)) {
		if (!(is_pool(x,y) && ignorewater)) return FALSE;
	}

	if (closed_door(x, y) && (!mdat || !amorphous(mdat)))
		return FALSE;
	if (sobj_at(BOULDER, x, y) && (!mdat || !throws_rocks(mdat)))
		return FALSE;
	return TRUE;
}

/*
 * "entity next to"
 *
 * Attempt to find a good place for the given monster type in the closest
 * position to (xx,yy).  Do so in successive square rings around (xx,yy).
 * If there is more than one valid positon in the ring, choose one randomly.
 * Return TRUE and the position chosen when successful, FALSE otherwise.
 */
boolean
enexto(cc, xx, yy, mdat)
coord *cc;
register xchar xx, yy;
struct permonst *mdat;
{
	return enexto_core(cc, xx, yy, mdat, 0);
}

boolean
enexto_core(cc, xx, yy, mdat, entflags)
coord *cc;
register xchar xx, yy;
struct permonst *mdat;
unsigned entflags;
{
	return enexto_core_range(cc, xx, yy, mdat, entflags, 1);
}

boolean
enexto_core_range(cc, xx, yy, mdat, entflags, start_range)
coord *cc;
register xchar xx, yy;
struct permonst *mdat;
unsigned entflags;
int start_range; /**< Distance of checked tiles to begin with. Should be >=1. */
{
#define MAX_GOOD 15
    coord good[MAX_GOOD], *good_ptr;
    int x, y, range, i;
    int xmin, xmax, ymin, ymax;
    struct monst fakemon;	/* dummy monster */

    if (!mdat) {
#ifdef DEBUG
	pline("enexto() called with mdat==0");
#endif
	/* default to player's original monster type */
	mdat = &mons[u.umonster];
    }
    fakemon.data = mdat;	/* set up for goodpos */
    good_ptr = good;
    range = start_range;
    /*
     * Walk around the border of the square with center (xx,yy) and
     * radius range.  Stop when we find at least one valid position.
     */
    do {
	xmin = max(1, xx-range);
	xmax = min(COLNO-1, xx+range);
	ymin = max(0, yy-range);
	ymax = min(ROWNO-1, yy+range);

	for (x = xmin; x <= xmax; x++)
	    if (goodpos(x, ymin, &fakemon, entflags)) {
		good_ptr->x = x;
		good_ptr->y = ymin ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (x = xmin; x <= xmax; x++)
	    if (goodpos(x, ymax, &fakemon, entflags)) {
		good_ptr->x = x;
		good_ptr->y = ymax ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (y = ymin+1; y < ymax; y++)
	    if (goodpos(xmin, y, &fakemon, entflags)) {
		good_ptr->x = xmin;
		good_ptr-> y = y ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	for (y = ymin+1; y < ymax; y++)
	    if (goodpos(xmax, y, &fakemon, entflags)) {
		good_ptr->x = xmax;
		good_ptr->y = y ;
		/* beware of accessing beyond segment boundaries.. */
		if (good_ptr++ == &good[MAX_GOOD-1]) goto full;
	    }
	range++;

	/* return if we've grown too big (nothing is valid) */
	if (range > ROWNO && range > COLNO) return FALSE;
    } while (good_ptr == good);

full:
    i = rn2((int)(good_ptr - good));
    cc->x = good[i].x;
    cc->y = good[i].y;
    return TRUE;
}

/*
 * "entity path to"
 *
 * Attempt to find nc good places for the given monster type with the shortest
 * path to (xx,yy).  Where there is more than one valid set of positions, one
 * will be chosen at random.  Return the number of positions found.
 * Warning:  This routine is much slower than enexto and should be used
 * with caution.
 */

#define EPATHTO_UNSEEN		0x0
#define EPATHTO_INACCESSIBLE	0x1
#define EPATHTO_DONE		0x2
#define EPATHTO_TAIL(n)		(0x3 + ((n) & 7))

#define EPATHTO_XY(x,y)		(((y) + 1) * COLNO + (x))
#define EPATHTO_Y(xy)		((xy) / COLNO - 1)
#define EPATHTO_X(xy)		((xy) % COLNO)

#ifdef DEBUG
coord epathto_debug_cc[100];
#endif

int
epathto(cc, nc, xx, yy, mdat)
coord *cc;
int nc;
register xchar xx, yy;
struct permonst *mdat;
{
    int i, j, dir, ndirs, xy, x, y, r;
    int path_len, postype;
    int first_col, last_col;
    int nd, n;
    unsigned char *map;
    static const int dirs[8] =
      /* N, S, E, W, NW, NE, SE, SW */
      { -COLNO, COLNO, 1, -1, -COLNO-1, -COLNO+1, COLNO+1, COLNO-1};
    struct monst fakemon;	/* dummy monster */
    fakemon.data = mdat;	/* set up for badpos */
    map = (unsigned char *)alloc(COLNO * (ROWNO + 2));
    (void) memset((genericptr_t)map, EPATHTO_INACCESSIBLE, COLNO * (ROWNO + 2));
    for(i = 1; i < COLNO; i++)
	for(j = 0; j < ROWNO; j++)
	    map[EPATHTO_XY(i, j)] = EPATHTO_UNSEEN;
    map[EPATHTO_XY(xx, yy)] = EPATHTO_TAIL(0);
    if (badpos(xx, yy, &fakemon, 0) == 0) {
	cc[0].x = xx;
	cc[0].y = yy;
	nd = n = 1;
    }
    else
	nd = n = 0;
    for(path_len = 0; nd < nc; path_len++)
    {
	first_col = max(1, xx - path_len);
	last_col = min(COLNO - 1, xx + path_len);
	for(j = max(0, yy - path_len); j <= min(ROWNO - 1, yy + path_len); j++)
	    for(i = first_col; i <= last_col; i++)
		if (map[EPATHTO_XY(i, j)] == EPATHTO_TAIL(path_len)) {
		    map[EPATHTO_XY(i, j)] = EPATHTO_DONE;
		    ndirs = mdat == &mons[PM_GRID_BUG] ? 4 : 8;
		    for(dir = 0; dir < ndirs; dir++) {
			xy = EPATHTO_XY(i, j) + dirs[dir];
			if (map[xy] == EPATHTO_UNSEEN) {
			    x = EPATHTO_X(xy);
			    y = EPATHTO_Y(xy);
			    postype = badpos(x, y, &fakemon, 0);
			    map[xy] = postype < 0 ? EPATHTO_INACCESSIBLE :
				    EPATHTO_TAIL(path_len + 1);
			    if (postype == 0) {
				if (n < nc)
				{
				    cc[n].x = x;
				    cc[n].y = y;
				}
				else if (rn2(n - nd + 1) < nc - nd)
				{
				    r = rn2(nc - nd) + nd;
				    cc[r].x = x;
				    cc[r].y = y;
				}
				++n;
			    }
			}
		    }
		}
	if (nd == n)
	    break;	/* No more positions */
	else
	    nd = n;
    }
    if (nd > nc)
	nd = nc;
#ifdef DEBUG
    if (cc == epathto_debug_cc)
    {
	winid win;
	int glyph;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		xy = EPATHTO_XY(x, y);
		if (map[xy] == EPATHTO_INACCESSIBLE) {
		    glyph = back_to_glyph(x, y);
		    row[x] = showsyms[glyph_to_cmap(glyph)];
		}
		else
		    row[x] = ' ';
	    }
	    for (i = 0; i < nd; i++)
		if (cc[i].y == y)
		    row[cc[i].x] = i < 10 ? '0' + i :
			i < 36 ? 'a' + i - 10 :
			i < 62 ? 'A' + i - 36 :
			'?';
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
    }
#endif

    free((genericptr_t)map);
    return nd;
}

/*
 * Check for restricted areas present in some special levels.  (This might
 * need to be augmented to allow deliberate passage in wizard mode, but
 * only for explicitly chosen destinations.)
 */
STATIC_OVL boolean
tele_jump_ok(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	if (dndest.nlx > 0) {
	    /* if inside a restricted region, can't teleport outside */
	    if (within_bounded_area(x1, y1, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy) &&
		!within_bounded_area(x2, y2, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy))
		return FALSE;
	    /* and if outside, can't teleport inside */
	    if (!within_bounded_area(x1, y1, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy) &&
		within_bounded_area(x2, y2, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy))
		return FALSE;
	}
	if (updest.nlx > 0) {		/* ditto */
	    if (within_bounded_area(x1, y1, updest.nlx, updest.nly,
						updest.nhx, updest.nhy) &&
		!within_bounded_area(x2, y2, updest.nlx, updest.nly,
						updest.nhx, updest.nhy))
		return FALSE;
	    if (!within_bounded_area(x1, y1, updest.nlx, updest.nly,
						updest.nhx, updest.nhy) &&
		within_bounded_area(x2, y2, updest.nlx, updest.nly,
						updest.nhx, updest.nhy))
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL boolean
teleok(x, y, trapok)
register int x, y;
boolean trapok;
{
	if (!trapok && t_at(x, y)) return FALSE;
	if (!goodpos(x, y, &youmonst, 0)) return FALSE;
	if (!tele_jump_ok(u.ux, u.uy, x, y)) return FALSE;
	if (!in_out_region(x, y)) return FALSE;
	return TRUE;
}

void
teleds(nux, nuy, allow_drag)
register int nux,nuy;
boolean allow_drag;
{
	boolean ball_active = (Punished && uball->where != OBJ_FREE),
		ball_still_in_range = FALSE;

	/* If they have to move the ball, then drag if allow_drag is true;
	 * otherwise they are teleporting, so unplacebc().  
	 * If they don't have to move the ball, then always "drag" whether or
	 * not allow_drag is true, because we are calling that function, not
	 * to drag, but to move the chain.  *However* there are some dumb
	 * special cases:
	 *    0				 0
	 *   _X  move east       ----->  X_
	 *    @				  @
	 * These are permissible if teleporting, but not if dragging.  As a
	 * result, drag_ball() needs to know about allow_drag and might end
	 * up dragging the ball anyway.  Also, drag_ball() might find that
	 * dragging the ball is completely impossible (ball in range but there's
	 * rock in the way), in which case it teleports the ball on its own.
	 */
	if (ball_active) {
	    if (!carried(uball) && distmin(nux, nuy, uball->ox, uball->oy) <= 2)
		ball_still_in_range = TRUE; /* don't have to move the ball */
	    else {
		/* have to move the ball */
		if (!allow_drag || distmin(u.ux, u.uy, nux, nuy) > 1) {
		    /* we should not have dist > 1 and allow_drag at the same
		     * time, but just in case, we must then revert to teleport.
		     */
		    allow_drag = FALSE;
		    unplacebc();
		}
	    }
	}
	u.utrap = 0;
	u.ustuck = 0;
	u.ux0 = u.ux;
	u.uy0 = u.uy;

	if (hides_under(youmonst.data))
		u.uundetected = OBJ_AT(nux, nuy);
	else if (youmonst.data->mlet == S_EEL)
		u.uundetected = is_pool(nux, nuy);
	else {
		u.uundetected = 0;
		/* mimics stop being unnoticed */
		if (youmonst.data->mlet == S_MIMIC)
		    youmonst.m_ap_type = M_AP_NOTHING;
	}

	if (u.uswallow) {
		u.uswldtim = u.uswallow = 0;
		if (Punished && !ball_active) {
		    /* ensure ball placement, like unstuck */
		    ball_active = TRUE;
		    allow_drag = FALSE;
		}
		docrt();
	}
	if (ball_active) {
	    if (ball_still_in_range || allow_drag) {
		int bc_control;
		xchar ballx, bally, chainx, chainy;
		boolean cause_delay;

		if (drag_ball(nux, nuy, &bc_control, &ballx, &bally,
				    &chainx, &chainy, &cause_delay, allow_drag))
		    move_bc(0, bc_control, ballx, bally, chainx, chainy);
	    }
	}
	/* must set u.ux, u.uy after drag_ball(), which may need to know
	   the old position if allow_drag is true... */
	u.ux = nux;
	u.uy = nuy;
	fill_pit(u.ux0, u.uy0);
	if (ball_active) {
	    if (!ball_still_in_range && !allow_drag)
		placebc();
	}
	initrack(); /* teleports mess up tracking monsters without this */
	update_player_regions();
#ifdef STEED
	/* Move your steed, too */
	if (u.usteed) {
		u.usteed->mx = nux;
		u.usteed->my = nuy;
	}
#endif
	/*
	 *  Make sure the hero disappears from the old location.  This will
	 *  not happen if she is teleported within sight of her previous
	 *  location.  Force a full vision recalculation because the hero
	 *  is now in a new location.
	 */
	newsym(u.ux0,u.uy0);
	see_monsters();
	vision_full_recalc = 1;
	nomul(0, 0);
	vision_recalc(0);	/* vision before effects */
	spoteffects(TRUE);
	invocation_message();
}

boolean
safe_teleds(allow_drag)
boolean allow_drag;
{
	register int nux, nuy, tcnt = 0;

	do {
		nux = rnd(COLNO-1);
		nuy = rn2(ROWNO);
	} while (!teleok(nux, nuy, (boolean)(tcnt > 200)) && ++tcnt <= 400);

	if (tcnt <= 400) {
		teleds(nux, nuy, allow_drag);
		return TRUE;
	} else
		return FALSE;
}

STATIC_OVL void
vault_tele()
{
	register struct mkroom *croom = search_special(VAULT);
	coord c;

	if (croom && somexy(croom, &c) && teleok(c.x,c.y,FALSE)) {
		teleds(c.x,c.y,FALSE);
		return;
	}
	tele();
}

boolean
teleport_pet(mtmp, force_it)
register struct monst *mtmp;
boolean force_it;
{
	register struct obj *otmp;

#ifdef STEED
	if (mtmp == u.usteed)
		return (FALSE);
#endif

	if (mtmp->mleashed) {
	    otmp = get_mleash(mtmp);
	    if (!otmp) {
		warning("%s is leashed, without a leash.", Monnam(mtmp));
		goto release_it;
	    }
	    if (otmp->cursed && !force_it) {
		yelp(mtmp);
		return FALSE;
	    } else {
/*JP
		Your("leash goes slack.");
*/
		Your("%sはたるんだ。", jtrns_obj(TOOL_SYM,"leash"));
 release_it:
		m_unleash(mtmp, FALSE);
		return TRUE;
	    }
	}
	return TRUE;
}

void
tele()
{
	coord cc;

	/* Disable teleportation in stronghold && Vlad's Tower */
	if (level.flags.noteleport) {
#ifdef WIZARD
		if (!wizard) {
#endif
/*JP
		    pline("A mysterious force prevents you from teleporting!");
*/
		    pline("奇妙な力が瞬間移動を防いだ！");
		    return;
#ifdef WIZARD
		}
#endif
	}

	/* don't show trap if "Sorry..." */
	if (!Blinded) make_blinded(0L,FALSE);

	if
#ifdef WIZARD
	(
#endif
	 (u.uhave.amulet || On_W_tower_level(&u.uz)
#ifdef STEED
	  || (u.usteed && mon_has_amulet(u.usteed))
#endif
	 )
#ifdef WIZARD
	 && (!wizard) )
#endif
	{
/*JP
		You_feel("disoriented for a moment.");
*/
		You("一瞬方向感覚を失ったような気がした。");
		return;
	}
	if ((Teleport_control && !Stunned)
#ifdef WIZARD
			    || wizard
#endif
					) {
	    if (unconscious()) {
/*JP
		pline("Being unconscious, you cannot control your teleport.");
*/
		pline("意識がないので、あなたは瞬間移動を制御できない。");
	    } else {
#if 0 /*JP*/
#ifdef STEED
		    char buf[BUFSZ];
		    if (u.usteed) Sprintf(buf," and %s", mon_nam(u.usteed));
#endif
		    pline("To what position do you%s want to be teleported?",
#ifdef STEED
				u.usteed ? buf :
#endif
			   "");
#else /*JP*/
		    pline("どの位置に瞬間移動しますか？");
#endif /*JP*/
		    cc.x = u.ux;
		    cc.y = u.uy;
/*JP
		    if (getpos(&cc, TRUE, "the desired position") < 0)
*/
		    if (getpos(&cc, TRUE, "移動したい場所") < 0)
			return;	/* abort */
		    /* possible extensions: introduce a small error if
		       magic power is low; allow transfer to solid rock */
		    if (teleok(cc.x, cc.y, FALSE)) {
			teleds(cc.x, cc.y, FALSE);
			return;
		    }
/*JP
		    pline("Sorry...");
*/
		    pline("おっと．．．");
		}
	}

	(void) safe_teleds(FALSE);
}

int
dotele()
{
	struct trap *trap;

	trap = t_at(u.ux, u.uy);
	if (trap && (!trap->tseen || trap->ttyp != TELEP_TRAP))
		trap = 0;

	if (trap) {
		if (trap->once) {
/*JP
			pline("This is a vault teleport, usable once only.");
*/
			pline("一度かぎりの倉庫への瞬間移動の罠だ。");
/*JP
			if (yn("Jump in?") == 'n')
*/
			if (yn("飛び込む？") == 'n')
				trap = 0;
			else {
				deltrap(trap);
				newsym(u.ux, u.uy);
			}
		}
		if (trap)
#if 0 /*JP*/
			You("%s onto the teleportation trap.",
			    locomotion(youmonst.data, "jump"));
#else
			You("瞬間移動の罠に飛びこんだ。");
#endif
	}
	if (!trap) {
	    boolean castit = FALSE;
	    register int sp_no = 0, energy = 0;

	    if (!Teleportation || (u.ulevel < (Role_if(PM_WIZARD) ? 8 : 12)
					&& !can_teleport(youmonst.data))) {
		/* Try to use teleport away spell. */
		if (objects[SPE_TELEPORT_AWAY].oc_name_known && !Confusion)
		    for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
			if (spl_book[sp_no].sp_id == SPE_TELEPORT_AWAY) {
				castit = TRUE;
				break;
			}
#ifdef WIZARD
		if (!wizard) {
#endif
		    if (!castit) {
			if (!Teleportation)
/*JP
			    You("don't know that spell.");
*/
			    You("そんな呪文は知らない。");
/*JP
			else You("are not able to teleport at will.");
*/
			else You("自分の意思で瞬間移動できない。");
			return(0);
		    }
#ifdef WIZARD
		}
#endif
	    }

	    if (u.uhunger <= 100 || ACURR(A_STR) < 6) {
#ifdef WIZARD
		if (!wizard) {
#endif
#if 0 /*JP*/
			You("lack the strength %s.",
			    castit ? "for a teleport spell" : "to teleport");
#else
			You("%sだけの力がない。",
			    castit ? "瞬間移動の呪文を唱える" : "瞬間移動する");
#endif
			return 1;
#ifdef WIZARD
		}
#endif
	    }

	    energy = objects[SPE_TELEPORT_AWAY].oc_level * 7 / 2 - 2;
	    if (u.uen <= energy) {
#ifdef WIZARD
		if (wizard)
			energy = u.uen;
		else
#endif
		{
#if 0 /*JP*/
			You("lack the energy %s.",
			    castit ? "for a teleport spell" : "to teleport");
#else
			You("%sだけのエネルギーがない。",
			    castit ? "瞬間移動の呪文を唱える" : "瞬間移動する");
#endif
			return 1;
		}
	    }

	    if (check_capacity(
/*JP
			"Your concentration falters from carrying so much."))
*/
			"沢山ものを持ちすぎて集中できない。"))
		return 1;

	    if (castit) {
		exercise(A_WIS, TRUE);
		if (spelleffects(sp_no, TRUE))
			return(1);
		else
#ifdef WIZARD
		    if (!wizard)
#endif
			return(0);
	    } else {
		u.uen -= energy;
		flags.botl = 1;
	    }
	}

	if (next_to_u()) {
		if (trap && trap->once) vault_tele();
		else tele();
		(void) next_to_u();
	} else {
		You(shudder_for_moment);
		return(0);
	}
	if (!trap) morehungry(100);
	return(1);
}


void
level_tele()
{
	register int newlev;
	d_level newlevel;
	const char *escape_by_flying = 0;	/* when surviving dest of -N */
	char buf[BUFSZ];
	boolean force_dest = FALSE;

	if ((u.uhave.amulet || In_endgame(&u.uz) || In_sokoban(&u.uz))
#ifdef WIZARD
						&& !wizard
#endif
							) {
/*JP
	    You_feel("very disoriented for a moment.");
*/
	    You("一瞬方向感覚を大きく失ったような気がした。");
	    return;
	}
	if ((Teleport_control && !Stunned)
#ifdef WIZARD
	   || wizard
#endif
		) {
	    char qbuf[BUFSZ];
	    int trycnt = 0;

/*JP
	    Strcpy(qbuf, "To what level do you want to teleport?");
*/
	    Strcpy(qbuf, "何階に移動しますか？");
	    do {
		if (++trycnt == 2) {
#ifdef WIZARD
			if (wizard) Strcat(qbuf, " [type a number or ? for a menu]");
			else
#endif
/*JP
			Strcat(qbuf, " [type a number]");
*/
			Strcat(qbuf, " [数字をいれてね]");
		}
		getlin(qbuf, buf);
		if (!strcmp(buf,"\033")) {	/* cancelled */
		    if (Confusion && rnl(5)) {
/*JP
			pline("Oops...");
*/
			pline("おっと．．．");
			goto random_levtport;
		    }
		    return;
		} else if (!strcmp(buf,"*")) {
		    goto random_levtport;
		} else if (Confusion && rnl(5)) {
/*JP
		    pline("Oops...");
*/
		    pline("おっと．．．");
		    goto random_levtport;
		}
#ifdef WIZARD
		if (wizard && !strcmp(buf,"?")) {
		    schar destlev = 0;
		    xchar destdnum = 0;

		    if ((newlev = (int)print_dungeon(TRUE, &destlev, &destdnum))) {
			newlevel.dnum = destdnum;
			newlevel.dlevel = destlev;
			if (In_endgame(&newlevel) && !In_endgame(&u.uz)) {
#if 0 /*JP*/
				Sprintf(buf,
#ifdef RANDOMIZED_PLANES
				    "Destination is first elemental plane");
#else
				    "Destination is earth level");
#endif
#else /*JP*/
		        Strcpy(buf, "");
#endif /*JP*/
				if (!u.uhave.amulet) {
					struct obj *obj;
					obj = mksobj(AMULET_OF_YENDOR,
							TRUE, FALSE);
					if (obj) {
						obj = addinv(obj);
/*JP
						Strcat(buf, " with the amulet");
*/
						Strcat(buf, "魔除けを持った状態での");
					}
				}
#if 1 /*JP*/
#ifdef RANDOMIZED_PLANES
				Strcat(buf, "最初の精霊界");
#else
				Strcat(buf, "地の精霊界");
#endif
#endif /*JP*/
#ifdef RANDOMIZED_PLANES
				assign_level(&newlevel, get_first_elemental_plane());
#else
				assign_level(&newlevel, &earth_level);
#endif
/*JP
				pline("%s.", buf);
*/
				pline("目的地は%sになります.", buf);
			}
			force_dest = TRUE;
		    } else return;
		} else
#endif
		if ((newlev = lev_by_name(buf)) == 0) newlev = atoi(buf);
	    } while (!newlev && !digit(buf[0]) &&
		     (buf[0] != '-' || !digit(buf[1])) &&
		     trycnt < 10);

	    /* no dungeon escape via this route */
	    if (newlev == 0) {
		if (trycnt >= 10)
		    goto random_levtport;
/*JP
		if (ynq("Go to Nowhere.  Are you sure?") != 'y') return;
*/
		if(ynq("どことも知れぬ場所に行きます？よろしいですか？") != 'y') return;
#if 0 /*JP*/
		You("%s in agony as your body begins to warp...",
		    is_silent(youmonst.data) ? "writhe" : "scream");
#else
	        You("体が移動しはじめると、苦しみ%s．．．",
		    is_silent(youmonst.data) ? "で身もだえした" : "のあえぎ声を出した");
#endif
		display_nhwindow(WIN_MESSAGE, FALSE);
/*JP
		You("cease to exist.");
*/
		Your("存在は消滅した。");
#if 0 /*JP:T*/
		if (invent) Your("possessions land on the %s with a thud.",
				surface(u.ux, u.uy));
#else
		if (invent) Your("持ちものはドサッと%sに落ちた。",
				surface(u.ux, u.uy));
#endif
#if 0 /*JP*/
		killer_format = NO_KILLER_PREFIX;
		killer = "committed suicide";
#else
		killer_format = KILLED_BY;
		killer = "自殺行為で";
#endif
		done(DIED);
/*JP
		pline("An energized cloud of dust begins to coalesce.");
*/
		pline("エネルギーをもったほこりの渦が結合しはじめた。");
#if 0 /*JP:T*/
		Your("body rematerializes%s.", invent ?
			", and you gather up all your possessions" : "");
#else
		Your("体は再び実体化した。%s", invent ?
			"そして全ての持ち物を拾い上げた。" : "");
#endif
		return;
#ifdef WIZARD
	    /* allow only jump beyond the Dungeons of Doom branch */
	    } else if (!wizard && newlev > 0 && u.uz.dnum != 0) {
#else
	    } else if (newlev > 0 && u.uz.dnum != 0) {
#endif
		/* random teleport for destination level outside of the current dungeon branch */
		if (newlev > dungeons[u.uz.dnum].depth_start + dunlevs_in_dungeon(&u.uz)) {
/*JP
			You_feel("like bouncing off a solid wall!");
*/
			You_feel("まるで分厚い壁に跳ね返されたような気がした！");
			goto random_levtport;
		}
	    }

	    /* if in Knox and the requested level > 0, stay put.
	     * we let negative values requests fall into the "heaven" loop.
	     */
	    if ((Is_knox(&u.uz)
#ifdef BLACKMARKET
		    || Is_blackmarket(&u.uz)
#endif
	    ) && newlev > 0) {
		You(shudder_for_moment);
		return;
	    }
	    /* if in Quest, the player sees "Home 1", etc., on the status
	     * line, instead of the logical depth of the level.  controlled
	     * level teleport request is likely to be relativized to the
	     * status line, and consequently it should be incremented to
	     * the value of the logical depth of the target level.
	     *
	     * we let negative values requests fall into the "heaven" loop.
	     */
	    if (In_quest(&u.uz) && newlev > 0)
		newlev = newlev + dungeons[u.uz.dnum].depth_start - 1;
	} else { /* involuntary level tele */
 random_levtport:
	    newlev = random_teleport_level();
	    if (newlev == depth(&u.uz)) {
		You(shudder_for_moment);
		return;
	    }
	}

	if (!next_to_u()) {
		You(shudder_for_moment);
		return;
	}
#ifdef WIZARD
	if (In_endgame(&u.uz)) {	/* must already be wizard */
	    int llimit = dunlevs_in_dungeon(&u.uz);

	    if (newlev >= 0 || newlev <= -llimit) {
/*JP
		You_cant("get there from here.");
*/
		You("そこには行けない。");
		return;
	    }
	    newlevel.dnum = u.uz.dnum;
	    newlevel.dlevel = llimit + newlev;
	    schedule_goto(&newlevel, FALSE, FALSE, 0, (char *)0, (char *)0);
	    return;
	}
#endif

	killer = 0;		/* still alive, so far... */

	if (newlev < 0 && !force_dest) {
		if (*u.ushops0) {
		    /* take unpaid inventory items off of shop bills */
		    in_mklev = TRUE;	/* suppress map update */
		    u_left_shop(u.ushops0, TRUE);
		    /* you're now effectively out of the shop */
		    *u.ushops0 = *u.ushops = '\0';
		    in_mklev = FALSE;
		}
		if (newlev <= -10) {
/*JP
			You("arrive in heaven.");
*/
			You("天国に辿りついた。");
/*JP
			verbalize("Thou art early, but we'll admit thee.");
*/
			verbalize("汝、死ぬには早すぎるがそれもよかろう。");
#if 0 /*JP*/
			killer_format = NO_KILLER_PREFIX;
			killer = "went to heaven prematurely";
#else
			killer_format = KILLED_BY;
			killer = "若くして天国に行き";
#endif
		} else if (newlev == -9) {
/*JP
			You_feel("deliriously happy. ");
*/
			You("狂ったような幸せを感じた。");
/*JP
			pline("(In fact, you're on Cloud 9!) ");
*/
			pline("(本当に苦を乗り越えた場所にいる！) ");
			display_nhwindow(WIN_MESSAGE, FALSE);
		} else
/*JP
			You("are now high above the clouds...");
*/
			You("雲の遥か上にいる．．．");

		if (killer) {
		    ;		/* arrival in heaven is pending */
		} else if (Levitation) {
/*JP
		    escape_by_flying = "float gently down to earth";
*/
		    escape_by_flying = "ゆっくり地面に降りた。";
		} else if (Flying) {
/*JP
		    escape_by_flying = "fly down to the ground";
*/
		    escape_by_flying = "ゆっくり地面に降りた。";
		} else {
/*JP
		    pline("Unfortunately, you don't know how to fly.");
*/
		    pline("不幸にも、あなたは飛びかたを知らない。");
/*JP
		    You("plummet a few thousand feet to your death.");
*/
		    pline("数千フィートの死のダイビングだ！");
#if 0 /*JP:T*/
		    Sprintf(buf,
			  "teleported out of the dungeon and fell to %s death",
			    uhis());
		    killer = buf;
		    killer_format = NO_KILLER_PREFIX;
#else
		    killer = "迷宮を飛びだしダイビングして";
		    killer_format = KILLED_BY;
#endif
		}
	}

	if (killer) {	/* the chosen destination was not survivable */
	    d_level lsav;

	    /* set specific death location; this also suppresses bones */
	    lsav = u.uz;	/* save current level, see below */
	    u.uz.dnum = 0;	/* main dungeon */
	    u.uz.dlevel = (newlev <= -10) ? -10 : 0;	/* heaven or surface */
	    done(DIED);
	    /* can only get here via life-saving (or declining to die in
	       explore|debug mode); the hero has now left the dungeon... */
/*JP
	    escape_by_flying = "find yourself back on the surface";
*/
	    escape_by_flying = "気がついたら地上に戻っていた";
	    u.uz = lsav;	/* restore u.uz so escape code works */
	}

	/* calls done(ESCAPED) if newlevel==0 */
	if (escape_by_flying) {
/*JP
	    You("%s.", escape_by_flying);
*/
	    You("%s。", escape_by_flying);
	    newlevel.dnum = 0;		/* specify main dungeon */
	    newlevel.dlevel = 0;	/* escape the dungeon */
	    /* [dlevel used to be set to 1, but it doesn't make sense to
		teleport out of the dungeon and float or fly down to the
		surface but then actually arrive back inside the dungeon] */
	} else if (u.uz.dnum == medusa_level.dnum &&
	    newlev >= dungeons[u.uz.dnum].depth_start +
						dunlevs_in_dungeon(&u.uz)) {
#ifdef WIZARD
	    if (!(wizard && force_dest))
#endif
	    find_hell(&newlevel);
	} else {
	    /* if invocation did not yet occur, teleporting into
	     * the last level of Gehennom is forbidden.
	     */
#ifdef WIZARD
		if (!wizard)
#endif
	    if (Inhell && !u.uevent.invoked &&
			newlev >= (dungeons[u.uz.dnum].depth_start +
					dunlevs_in_dungeon(&u.uz) - 1)) {
		newlev = dungeons[u.uz.dnum].depth_start +
					dunlevs_in_dungeon(&u.uz) - 2;
/*JP
		pline("Sorry...");
*/
		pline("おっと．．．");
	    }
	    /* no teleporting out of quest dungeon */
	    if (In_quest(&u.uz) && newlev < depth(&qstart_level))
		newlev = depth(&qstart_level);
	    /* the player thinks of levels purely in logical terms, so
	     * we must translate newlev to a number relative to the
	     * current dungeon.
	     */
#ifdef WIZARD
	    if (!(wizard && force_dest))
#endif
	    get_level(&newlevel, newlev);
	}
	schedule_goto(&newlevel, FALSE, FALSE, 0, (char *)0, (char *)0);
	/* in case player just read a scroll and is about to be asked to
	   call it something, we can't defer until the end of the turn */
	if (u.utotype && !flags.mon_moving) deferred_goto();
}

void
domagicportal(ttmp)
register struct trap *ttmp;
{
	struct d_level target_level;

	if (!next_to_u()) {
		You(shudder_for_moment);
		return;
	}

	/* if landed from another portal, do nothing */
	/* problem: level teleport landing escapes the check */
	if (!on_level(&u.uz, &u.uz0)) return;

/*JP
	You("activated a magic portal!");
*/
	pline("魔法の入口が作動した！");

	/* prevent the poor shnook, whose amulet was stolen while in
	 * the endgame, from accidently triggering the portal to the
	 * next level, and thus losing the game
	 */
	if (In_endgame(&u.uz) && !u.uhave.amulet) {
/*JP
	    You_feel("dizzy for a moment, but nothing happens...");
*/
	    You("一瞬めまいを感じた、しかし何も起きなかった．．．");
	    return;
	}

	target_level = ttmp->dst;
	schedule_goto(&target_level, FALSE, FALSE, 1,
/*JP
		      "You feel dizzy for a moment, but the sensation passes.",
*/
		      "一瞬めまいを感じた、しかしその感覚は消えた。",
		      (char *)0);
}

void
tele_trap(trap)
struct trap *trap;
{
	if (In_endgame(&u.uz) || Antimagic) {
		if (Antimagic)
			shieldeff(u.ux, u.uy);
/*JP
		You_feel("a wrenching sensation.");
*/
		You("ねじられたような感覚を感じた。");
	} else if (!next_to_u()) {
		You(shudder_for_moment);
	} else if (trap->once) {
		deltrap(trap);
		newsym(u.ux,u.uy);	/* get rid of trap symbol */
		vault_tele();
	} else
		tele();
}

void
level_tele_trap(trap)
struct trap *trap;
{
#if 0 /*JP*/
	You("%s onto a level teleport trap!",
		      Levitation ? (const char *)"float" :
				  locomotion(youmonst.data, "step"));
#else
	You("別の階への瞬間移動の罠を%s！",
	    Levitation ? (const char *)"見下ろした" :
	    jpast(locomotion(youmonst.data, "踏む")));
#endif
	if (Antimagic) {
	    shieldeff(u.ux, u.uy);
	}
	if (Antimagic || In_endgame(&u.uz)) {
/*JP
	    You_feel("a wrenching sensation.");
*/
	    You("ねじられたような感覚を感じた。");
	    return;
	}
	if (!Blind)
/*JP
	    You("are momentarily blinded by a flash of light.");
*/
	    You("まばゆい光で一瞬目がくらんだ。");
	else
/*JP
	    You("are momentarily disoriented.");
*/
	    You("一瞬方向感覚を失った。");
	deltrap(trap);
	newsym(u.ux,u.uy);	/* get rid of trap symbol */
	level_tele();
}

/* check whether monster can arrive at location <x,y> via Tport (or fall) */
STATIC_OVL boolean
rloc_pos_ok(x, y, mtmp)
register int x, y;		/* coordinates of candidate location */
struct monst *mtmp;
{
	register int xx, yy;

	if (!goodpos(x, y, mtmp, 0)) return FALSE;
	/*
	 * Check for restricted areas present in some special levels.
	 *
	 * `xx' is current column; if 0, then `yy' will contain flag bits
	 * rather than row:  bit #0 set => moving upwards; bit #1 set =>
	 * inside the Wizard's tower.
	 */
	xx = mtmp->mx;
	yy = mtmp->my;
	if (!xx) {
	    /* no current location (migrating monster arrival) */
	    if (dndest.nlx && On_W_tower_level(&u.uz))
		return ((yy & 2) != 0) ^	/* inside xor not within */
		       !within_bounded_area(x, y, dndest.nlx, dndest.nly,
						  dndest.nhx, dndest.nhy);
	    if (updest.lx && (yy & 1) != 0)	/* moving up */
		return (within_bounded_area(x, y, updest.lx, updest.ly,
						  updest.hx, updest.hy) &&
		       (!updest.nlx ||
			!within_bounded_area(x, y, updest.nlx, updest.nly,
						   updest.nhx, updest.nhy)));
	    if (dndest.lx && (yy & 1) == 0)	/* moving down */
		return (within_bounded_area(x, y, dndest.lx, dndest.ly,
						  dndest.hx, dndest.hy) &&
		       (!dndest.nlx ||
			!within_bounded_area(x, y, dndest.nlx, dndest.nly,
						   dndest.nhx, dndest.nhy)));
	} else {
	    /* current location is <xx,yy> */
	    if (!tele_jump_ok(xx, yy, x, y)) return FALSE;
	}
	/* <x,y> is ok */
	return TRUE;
}

/*
 * rloc_to()
 *
 * Pulls a monster from its current position and places a monster at
 * a new x and y.  If oldx is 0, then the monster was not in the levels.monsters
 * array.  However, if oldx is 0, oldy may still have a value because mtmp is a
 * migrating_mon.  Worm tails are always placed randomly around the head of
 * the worm.
 */
void
rloc_to(mtmp, x, y)
struct monst *mtmp;
register int x, y;
{
	register int oldx = mtmp->mx, oldy = mtmp->my;
	boolean resident_shk = mtmp->isshk && inhishop(mtmp);

	if (x == mtmp->mx && y == mtmp->my)	/* that was easy */
		return;

	if (oldx) {				/* "pick up" monster */
	    if (mtmp->wormno)
		remove_worm(mtmp);
	    else {
		remove_monster(oldx, oldy);
		newsym(oldx, oldy);		/* update old location */
	    }
	}

	place_monster(mtmp, x, y);		/* put monster down */
	update_monster_region(mtmp);

	if (mtmp->wormno)			/* now put down tail */
		place_worm_tail_randomly(mtmp, x, y);

	if (u.ustuck == mtmp) {
		if (u.uswallow) {
			u.ux = x;
			u.uy = y;
			docrt();
		} else	u.ustuck = 0;
	}

	newsym(x, y);				/* update new location */
	set_apparxy(mtmp);			/* orient monster */

	/* shopkeepers will only teleport if you zap them with a wand of
	   teleportation or if they've been transformed into a jumpy monster;
	   the latter only happens if you've attacked them with polymorph */
	if (resident_shk && !inhishop(mtmp)) make_angry_shk(mtmp, oldx, oldy);
}

/* place a monster at a random location, typically due to teleport */
/* return TRUE if successful, FALSE if not */
boolean
rloc(mtmp, suppress_impossible)
struct monst *mtmp;	/* mx==0 implies migrating monster arrival */
boolean suppress_impossible;
{
	register int x, y, trycount;

#ifdef STEED
	if (mtmp == u.usteed) {
	    tele();
	    return TRUE;
	}
#endif

	if (mtmp->iswiz && mtmp->mx) {	/* Wizard, not just arriving */
	    if (!In_W_tower(u.ux, u.uy, &u.uz))
		x = xupstair,  y = yupstair;
	    else if (!xdnladder)	/* bottom level of tower */
		x = xupladder,  y = yupladder;
	    else
		x = xdnladder,  y = ydnladder;
	    /* if the wiz teleports away to heal, try the up staircase,
	       to block the player's escaping before he's healed
	       (deliberately use `goodpos' rather than `rloc_pos_ok' here) */
	    if (goodpos(x, y, mtmp, 0))
		goto found_xy;
	}

	trycount = 0;
	do {
	    x = rn1(COLNO-3,2);
	    y = rn2(ROWNO);
	    if ((trycount < 500) ? rloc_pos_ok(x, y, mtmp)
				 : goodpos(x, y, mtmp, 0))
		goto found_xy;
	} while (++trycount < 1000);

	/* last ditch attempt to find a good place */
	for (x = 2; x < COLNO - 1; x++)
	    for (y = 0; y < ROWNO; y++)
		if (goodpos(x, y, mtmp, 0))
		    goto found_xy;

	/* level either full of monsters or somehow faulty */
	if (!suppress_impossible)
		warning("rloc(): couldn't relocate monster");
	return FALSE;

 found_xy:
	rloc_to(mtmp, x, y);
	return TRUE;
}

STATIC_OVL void
mvault_tele(mtmp)
struct monst *mtmp;
{
	register struct mkroom *croom = search_special(VAULT);
	coord c;

	if (croom && somexy(croom, &c) &&
				goodpos(c.x, c.y, mtmp, 0)) {
		rloc_to(mtmp, c.x, c.y);
		return;
	}
	(void) rloc(mtmp, FALSE);
}

boolean
tele_restrict(mon)
struct monst *mon;
{
	if (level.flags.noteleport) {
		if (canseemon(mon))
#if 0 /*JP*/
		    pline("A mysterious force prevents %s from teleporting!",
			mon_nam(mon));
#else
		    pline("奇妙な力が%sの瞬間移動を防いだ！",
			mon_nam(mon));
#endif
		return TRUE;
	}
	return FALSE;
}

void
mtele_trap(mtmp, trap, in_sight)
struct monst *mtmp;
struct trap *trap;
int in_sight;
{
	char *monname;

	if (tele_restrict(mtmp)) return;
	if (teleport_pet(mtmp, FALSE)) {
	    /* save name with pre-movement visibility */
	    monname = Monnam(mtmp);

	    /* Note: don't remove the trap if a vault.  Other-
	     * wise the monster will be stuck there, since
	     * the guard isn't going to come for it...
	     */
	    if (trap->once) mvault_tele(mtmp);
	    else (void) rloc(mtmp, FALSE);

	    if (in_sight) {
		if (canseemon(mtmp))
/*JP
		    pline("%s seems disoriented.", monname);
*/
		    pline("%sは一瞬方向感覚を失ったようだ。", monname);
		else
/*JP
		    pline("%s suddenly disappears!", monname);
*/
		    pline("%sは突然消えた！", monname);
		seetrap(trap);
	    }
	}
}

/* return 0 if still on level, 3 if not */
int
mlevel_tele_trap(mtmp, trap, force_it, in_sight)
struct monst *mtmp;
struct trap *trap;
boolean force_it;
int in_sight;
{
	int tt = trap->ttyp;
	struct permonst *mptr = mtmp->data;

	if (mtmp == u.ustuck)	/* probably a vortex */
	    return 0;		/* temporary? kludge */
	if (teleport_pet(mtmp, force_it)) {
	    d_level tolevel;
	    int migrate_typ = MIGR_RANDOM;

	    if ((tt == HOLE || tt == TRAPDOOR)) {
		if (Is_stronghold(&u.uz)) {
		    assign_level(&tolevel, &valley_level);
		} else if (Is_botlevel(&u.uz)) {
		    if (in_sight && trap->tseen)
#if 0 /*JP*/
			pline("%s avoids the %s.", Monnam(mtmp),
			(tt == HOLE) ? "hole" : "trap");
#else
			pline("%sは%sを回避した。", Monnam(mtmp),
			(tt == HOLE) ? "穴" : "罠");
#endif
		    return 0;
		} else {
		    get_level(&tolevel, depth(&u.uz) + 1);
		}
	    } else if (tt == MAGIC_PORTAL) {
		if (In_endgame(&u.uz) &&
		    (mon_has_amulet(mtmp) || is_home_elemental(mptr))) {
		    if (in_sight && mptr->mlet != S_ELEMENTAL) {
/*JP
			pline("%s seems to shimmer for a moment.",
*/
			pline("%sが一瞬輝いたように見えた。",
							Monnam(mtmp));
			seetrap(trap);
		    }
		    return 0;
#ifdef BLACKMARKET
	      	} else if (mtmp->mtame &&
			(Is_blackmarket(&trap->dst) || Is_blackmarket(&u.uz))) {
	          if (in_sight) {
/*JP
		     pline("%s seems to shimmer for a moment.",
*/
		     pline("%sが一瞬輝いたように見えた。",
		     Monnam(mtmp));
		     seetrap(trap);
	          }
	          return 0;
#endif /* BLACKMARKET */
		} else {
		    assign_level(&tolevel, &trap->dst);
		    migrate_typ = MIGR_PORTAL;
		}
	    } else { /* (tt == LEVEL_TELEP) */
		int nlev;

		if (mon_has_amulet(mtmp) || In_endgame(&u.uz)) {
		    if (in_sight)
/*JP
			pline("%s seems very disoriented for a moment.",
*/
			pline("%sは一瞬方向感覚を大きく失ったようだ。",
				Monnam(mtmp));
		    return 0;
		}
		nlev = random_teleport_level();
		if (nlev == depth(&u.uz)) {
		    if (in_sight)
/*JP
			pline("%s shudders for a moment.", Monnam(mtmp));
*/
			pline("%sは一瞬震えた。", Monnam(mtmp));
		    return 0;
		}
		get_level(&tolevel, nlev);
	    }

	    if (in_sight) {
/*JP
		pline("Suddenly, %s disappears out of sight.", mon_nam(mtmp));
*/
		pline("突然%sが視界から消えた。", mon_nam(mtmp));
		seetrap(trap);
	    }
	    migrate_to_level(mtmp, ledger_no(&tolevel),
			     migrate_typ, (coord *)0);
	    return 3;	/* no longer on this level */
	}
	return 0;
}


void
rloco(obj)
register struct obj *obj;
{
	register xchar tx, ty, otx, oty;
	boolean restricted_fall;
	int try_limit = 4000;

	if (obj->otyp == CORPSE && is_rider(&mons[obj->corpsenm])) {
	    if (revive_corpse(obj)) return;
	}

	obj_extract_self(obj);
	otx = obj->ox;
	oty = obj->oy;
	restricted_fall = (otx == 0 && dndest.lx);
	do {
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	    if (!--try_limit) break;
	} while (!goodpos(tx, ty, (struct monst *)0, 0) ||
		/* bug: this lacks provision for handling the Wizard's tower */
		 (restricted_fall &&
		  (!within_bounded_area(tx, ty, dndest.lx, dndest.ly,
						dndest.hx, dndest.hy) ||
		   (dndest.nlx &&
		    within_bounded_area(tx, ty, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy)))));

/*JP
	if (flooreffects(obj, tx, ty, "fall")) {
*/
	if (flooreffects(obj, tx, ty, "落ちる")) {
	    return;
	} else if (otx == 0 && oty == 0) {
	    ;	/* fell through a trap door; no update of old loc needed */
	} else {
	    if (costly_spot(otx, oty)
	      && (!costly_spot(tx, ty) ||
		  !index(in_rooms(tx, ty, 0), *in_rooms(otx, oty, 0)))) {
		if (costly_spot(u.ux, u.uy) &&
			    index(u.urooms, *in_rooms(otx, oty, 0)))
		    addtobill(obj, FALSE, FALSE, FALSE);
		else (void)stolen_value(obj, otx, oty, FALSE, FALSE);
	    }
	    newsym(otx, oty);	/* update old location */
	}
	place_object(obj, tx, ty);
	newsym(tx, ty);
}

/* Returns an absolute depth */
int
random_teleport_level()
{
	int nlev, max_depth, min_depth,
	    cur_depth = (int)depth(&u.uz);

	if (!rn2(5) || Is_knox(&u.uz)
#ifdef BLACKMARKET
		|| Is_blackmarket(&u.uz)
#endif
		)
	    return cur_depth;

	/* What I really want to do is as follows:
	 * -- If in a dungeon that goes down, the new level is to be restricted
	 *    to [top of parent, bottom of current dungeon]
	 * -- If in a dungeon that goes up, the new level is to be restricted
	 *    to [top of current dungeon, bottom of parent]
	 * -- If in a quest dungeon or similar dungeon entered by portals,
	 *    the new level is to be restricted to [top of current dungeon,
	 *    bottom of current dungeon]
	 * The current behavior is not as sophisticated as that ideal, but is
	 * still better what we used to do, which was like this for players
	 * but different for monsters for no obvious reason.  Currently, we
	 * must explicitly check for special dungeons.  We check for Knox
	 * above; endgame is handled in the caller due to its different
	 * message ("disoriented").
	 * --KAA
	 * 3.4.2: explicitly handle quest here too, to fix the problem of
	 * monsters sometimes level teleporting out of it into main dungeon.
	 * Also prevent monsters reaching the Sanctum prior to invocation.
	 */
	min_depth = In_quest(&u.uz) ? dungeons[u.uz.dnum].depth_start : 1;
	max_depth = dunlevs_in_dungeon(&u.uz) +
			(dungeons[u.uz.dnum].depth_start - 1);
	/* can't reach the Sanctum if the invocation hasn't been performed */
	if (Inhell && !u.uevent.invoked) max_depth -= 1;

	/* Get a random value relative to the current dungeon */
	/* Range is 1 to current+3, current not counting */
	nlev = rn2(cur_depth + 3 - min_depth) + min_depth;
	if (nlev >= cur_depth) nlev++;

	if (nlev > max_depth) {
	    nlev = max_depth;
	    /* teleport up if already on bottom */
	    if (Is_botlevel(&u.uz)) nlev -= rnd(3);
	}
	if (nlev < min_depth) {
	    nlev = min_depth;
	    if (nlev == cur_depth) {
	        nlev += rnd(3);
	        if (nlev > max_depth)
		    nlev = max_depth;
	    }
	}
	return nlev;
}

/** You teleport a monster (via wand, spell, or poly'd q.mechanic attack).
   returns false iff the attempt fails */
boolean
u_teleport_mon(mtmp, give_feedback)
struct monst *mtmp;
boolean give_feedback;
{
	coord cc;

	if (mtmp->ispriest && *in_rooms(mtmp->mx, mtmp->my, TEMPLE)) {
	    if (give_feedback)
/*JP
		pline("%s resists your magic!", Monnam(mtmp));
*/
		pline("%sは魔法を防いだ！", Monnam(mtmp));
	    return FALSE;
	} else if (level.flags.noteleport && u.uswallow && mtmp == u.ustuck) {
	    if (give_feedback)
/*JP
		You("are no longer inside %s!", mon_nam(mtmp));
*/
		You("%sの内部から脱出した！", mon_nam(mtmp));
	    unstuck(mtmp);
	    (void) rloc(mtmp, FALSE);
#ifdef BLACKMARKET
	} else if (mtmp->data == &mons[PM_BLACK_MARKETEER] &&
	           rn2(13) &&
		   enexto_core_range(&cc, u.ux, u.uy, mtmp->data,0,
		                     rnf(1,10) ? 4 : 3)) {
	    rloc_to(mtmp, cc.x, cc.y);
#endif
	} else if (is_rider(mtmp->data) && rn2(13) &&
		   enexto(&cc, u.ux, u.uy, mtmp->data))
	    rloc_to(mtmp, cc.x, cc.y);
	else
	    (void) rloc(mtmp, FALSE);
	return TRUE;
}

/*teleport.c*/
