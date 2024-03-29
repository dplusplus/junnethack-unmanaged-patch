/*	SCCS Id: @(#)dig.c	3.4	2003/03/23	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1996-2000
**	changing point is marked `JP' (94/7/16)
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2005
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"
#include "edog.h"
/* #define DEBUG */	/* turn on for diagnostics */

#ifdef OVLB

static NEARDATA boolean did_dig_msg;

STATIC_DCL boolean NDECL(rm_waslit);
STATIC_DCL void FDECL(mkcavepos, (XCHAR_P,XCHAR_P,int,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL void FDECL(mkcavearea, (BOOLEAN_P));
STATIC_DCL int FDECL(dig_typ, (struct obj *,XCHAR_P,XCHAR_P));
STATIC_DCL int NDECL(dig);
STATIC_DCL schar FDECL(fillholetyp, (int, int));
STATIC_DCL void NDECL(dig_up_grave);

/* Indices returned by dig_typ() */
#define DIGTYP_UNDIGGABLE 0
#define DIGTYP_ROCK       1
#define DIGTYP_STATUE     2
#define DIGTYP_BOULDER    3
#define DIGTYP_DOOR       4
#define DIGTYP_TREE       5


STATIC_OVL boolean
rm_waslit()
{
    register xchar x, y;

    if(levl[u.ux][u.uy].typ == ROOM && levl[u.ux][u.uy].waslit)
	return(TRUE);
    for(x = u.ux-2; x < u.ux+3; x++)
	for(y = u.uy-1; y < u.uy+2; y++)
	    if(isok(x,y) && levl[x][y].waslit) return(TRUE);
    return(FALSE);
}

/* Change level topology.  Messes with vision tables and ignores things like
 * boulders in the name of a nice effect.  Vision will get fixed up again
 * immediately after the effect is complete.
 */
STATIC_OVL void
mkcavepos(x, y, dist, waslit, rockit)
    xchar x,y;
    int dist;
    boolean waslit, rockit;
{
    register struct rm *lev;

    if(!isok(x,y)) return;
    lev = &levl[x][y];

    if(rockit) {
	register struct monst *mtmp;

	if(IS_ROCK(lev->typ)) return;
	if(t_at(x, y)) return; /* don't cover the portal */
	if ((mtmp = m_at(x, y)) != 0)	/* make sure crucial monsters survive */
	    if(!passes_walls(mtmp->data)) (void) rloc(mtmp, FALSE);
    } else if(lev->typ == ROOM) return;

    unblock_point(x,y);	/* make sure vision knows this location is open */

    /* fake out saved state */
    lev->seenv = 0;
    lev->doormask = 0;
    if(dist < 3) lev->lit = (rockit ? FALSE : TRUE);
    if(waslit) lev->waslit = (rockit ? FALSE : TRUE);
    lev->horizontal = FALSE;
    viz_array[y][x] = (dist < 3 ) ?
	(IN_SIGHT|COULD_SEE) : /* short-circuit vision recalc */
	COULD_SEE;
    lev->typ = (rockit ? STONE : ROOM);
    if(dist >= 3)
	impossible("mkcavepos called with dist %d", dist);
    if(Blind)
	feel_location(x, y);
    else newsym(x,y);
}

STATIC_OVL void
mkcavearea(rockit)
register boolean rockit;
{
    int dist;
    xchar xmin = u.ux, xmax = u.ux;
    xchar ymin = u.uy, ymax = u.uy;
    register xchar i;
    register boolean waslit = rm_waslit();

/*JP
    if(rockit) pline("Crash!  The ceiling collapses around you!");
*/
    if(rockit) pline("げげん！あなたのまわりの天井が崩れた！");
#if 0 /*JP:T*/
    else pline("A mysterious force %s cave around you!",
	     (levl[u.ux][u.uy].typ == CORR) ? "creates a" : "extends the");
#else
    else pline("神秘的な力によりあなたのまわり%sた！",
	     (levl[u.ux][u.uy].typ == CORR) ? "に洞窟ができ" : "の洞窟が広がっ");
#endif
    display_nhwindow(WIN_MESSAGE, TRUE);

    for(dist = 1; dist <= 2; dist++) {
	xmin--; xmax++;

	/* top and bottom */
	if(dist < 2) { /* the area is wider that it is high */
	    ymin--; ymax++;
	    for(i = xmin+1; i < xmax; i++) {
		mkcavepos(i, ymin, dist, waslit, rockit);
		mkcavepos(i, ymax, dist, waslit, rockit);
	    }
	}

	/* left and right */
	for(i = ymin; i <= ymax; i++) {
	    mkcavepos(xmin, i, dist, waslit, rockit);
	    mkcavepos(xmax, i, dist, waslit, rockit);
	}

	flush_screen(1);	/* make sure the new glyphs shows up */
	delay_output();
    }

    if(!rockit && levl[u.ux][u.uy].typ == CORR) {
	levl[u.ux][u.uy].typ = ROOM;
	if(waslit) levl[u.ux][u.uy].waslit = TRUE;
	newsym(u.ux, u.uy); /* in case player is invisible */
    }

    vision_full_recalc = 1;	/* everything changed */
}

/* When digging into location <x,y>, what are you actually digging into? */
STATIC_OVL int
dig_typ(otmp, x, y)
struct obj *otmp;
xchar x, y;
{
	boolean ispick = is_pick(otmp);

	return (ispick && sobj_at(STATUE, x, y) ? DIGTYP_STATUE :
		ispick && sobj_at(BOULDER, x, y) ? DIGTYP_BOULDER :
		closed_door(x, y) ? DIGTYP_DOOR :
		IS_TREES(levl[x][y].typ) ?
			(ispick ? DIGTYP_UNDIGGABLE : DIGTYP_TREE) :
		ispick && IS_ROCK(levl[x][y].typ) &&
			(!level.flags.arboreal || IS_WALL(levl[x][y].typ)) ?
			DIGTYP_ROCK : DIGTYP_UNDIGGABLE);
}

boolean
is_digging()
{
	if (occupation == dig) {
	    return TRUE;
	}
	return FALSE;
}

#define BY_YOU		(&youmonst)
#define BY_OBJECT	((struct monst *)0)

boolean
dig_check(madeby, verbose, x, y)
	struct monst	*madeby;
	boolean		verbose;
	int		x, y;
{
	struct trap *ttmp = t_at(x, y);
/*JP
	const char *verb = (madeby == BY_YOU && uwep && is_axe(uwep)) ? "chop" : "dig in";
*/
	const char *verb = (madeby == BY_YOU && uwep && is_axe(uwep)) ? "砕けない" : "掘れない";

	if (On_stairs(x, y)) {
	    if (x == xdnladder || x == xupladder) {
/*JP
		if(verbose) pline_The("ladder resists your effort.");
*/
		if(verbose) pline("はしごが邪魔をした。");
/*JP
	    } else if(verbose) pline_The("stairs are too hard to %s.", verb);
*/
	    } else if(verbose) pline("階段はとても固くて%s。", verb);
	    return(FALSE);
	} else if (IS_THRONE(levl[x][y].typ) && madeby != BY_OBJECT) {
/*JP
	    if(verbose) pline_The("throne is too hard to break apart.");
*/
	    if(verbose) pline("玉座はとても固くて砕けない。");
	    return(FALSE);
	} else if (IS_ALTAR(levl[x][y].typ) && (madeby != BY_OBJECT ||
				Is_astralevel(&u.uz) || Is_sanctum(&u.uz))) {
/*JP
	    if(verbose) pline_The("altar is too hard to break apart.");
*/
	    if(verbose) pline_The("祭壇はとても固くて砕けない。");
	    return(FALSE);
	} else if (Is_airlevel(&u.uz)) {
/*JP
	    if(verbose) You("cannot %s thin air.", verb);
*/
	    if(verbose) You("何もない空間は%s。", verb);
	    return(FALSE);
	} else if (Is_waterlevel(&u.uz)) {
/*JP
	    if(verbose) pline_The("water splashes and subsides.");
*/
	    if(verbose) pline("水がピシャッと跳ねた。");
	    return(FALSE);
	} else if ((IS_ROCK(levl[x][y].typ) && levl[x][y].typ != SDOOR &&
		      (levl[x][y].wall_info & W_NONDIGGABLE) != 0)
		|| (ttmp &&
		      (ttmp->ttyp == MAGIC_PORTAL || !Can_dig_down(&u.uz)))) {
#if 0 /*JP:T*/
	    if(verbose) pline_The("%s here is too hard to %s.",
				  surface(x,y), verb);
#else
	    if(verbose) pline_The("%sはとても固くて%s。",
				  surface(x,y), verb);
#endif
	    return(FALSE);
	} else if (sobj_at(BOULDER, x, y)) {
/*JP
	    if(verbose) There("isn't enough room to %s here.", verb);
*/
	    if(verbose) pline("十分な場所がないので%s。", verb);
	    return(FALSE);
	} else if (madeby == BY_OBJECT &&
		    /* the block against existing traps is mainly to
		       prevent broken wands from turning holes into pits */
		    (ttmp || is_pool(x,y) || is_lava(x,y))) {
	    /* digging by player handles pools separately */
	    return FALSE;
	}
	return(TRUE);
}

STATIC_OVL int
dig()
{
	register struct rm *lev;
	register xchar dpx = digging.pos.x, dpy = digging.pos.y;
	register boolean ispick = uwep && is_pick(uwep);
#if 0 /*JP:T*/
	const char *verb =
	    (!uwep || is_pick(uwep)) ? "dig into" : "chop through";
#else
	const char *verb =
	    (!uwep || is_pick(uwep)) ? "掘れない" : "砕けない";
#endif

	lev = &levl[dpx][dpy];
	/* perhaps a nymph stole your pick-axe while you were busy digging */
	/* or perhaps you teleported away */
	if (u.uswallow || !uwep || (!ispick && !is_axe(uwep)) ||
	    !on_level(&digging.level, &u.uz) ||
	    ((digging.down ? (dpx != u.ux || dpy != u.uy)
			   : (distu(dpx,dpy) > 2))))
		return(0);

	if (digging.down) {
	    if(!dig_check(BY_YOU, TRUE, u.ux, u.uy)) return(0);
	} else { /* !digging.down */
	    if (IS_TREES(lev->typ) && !may_dig(dpx,dpy) &&
			dig_typ(uwep, dpx, dpy) == DIGTYP_TREE) {
/*JP
		pline("This tree seems to be petrified.");
*/
		pline("この木は石化しているようだ。");
		return(0);
	    }
	    if (IS_ROCK(lev->typ) && !may_dig(dpx,dpy) &&
			dig_typ(uwep, dpx, dpy) == DIGTYP_ROCK) {
/*JP
		pline("This wall is too hard to %s.", verb);
*/
		pline("この壁はとても固くて%s。", verb);
		return(0);
	    }
	}
	if(Fumbling && !rn2(3)) {
	    switch(rn2(3)) {
	    case 0:
		if(!welded(uwep)) {
/*JP
		    You("fumble and drop your %s.", xname(uwep));
*/
		    You("手が滑り%sを落した。", xname(uwep));
		    dropx(uwep);
		} else {
#ifdef STEED
		    if (u.usteed)
#if 0 /*JP:T*/
			Your("%s %s and %s %s!",
			     xname(uwep),
			     otense(uwep, "bounce"), otense(uwep, "hit"),
			     mon_nam(u.usteed));
#else
			 pline("%sは跳ねかえり%sに命中した！",
			      xname(uwep), mon_nam(u.usteed));
#endif
		    else
#endif
#if 0 /*JP:T*/
			pline("Ouch!  Your %s %s and %s you!",
			      xname(uwep),
			      otense(uwep, "bounce"), otense(uwep, "hit"));
#else
			 pline("いてっ！%sは跳ねかえりあなたに命中した！",
			      xname(uwep));
#endif
		    set_wounded_legs(RIGHT_SIDE, 5 + rnd(5));
		}
		break;
	    case 1:
/*JP
		pline("Bang!  You hit with the broad side of %s!",
*/
	        pline("バン！%sの柄で打ってしまった！",
		      the(xname(uwep)));
		break;
/*JP
	    default: Your("swing misses its mark.");
*/
	    default: You("狙いを定めて振りおろしたがはずした。");
		break;
	    }
	    return(0);
	}

	digging.effort += 10 + rn2(5) + abon() +
			   uwep->spe - greatest_erosion(uwep) + u.udaminc;
	if (Race_if(PM_DWARF))
	    digging.effort *= 2;
	if (lev->typ == DEADTREE)
	    digging.effort *= 2;
	if (digging.down) {
		register struct trap *ttmp;

		if (digging.effort > 250) {
		    (void) dighole(FALSE);
		    (void) memset((genericptr_t)&digging, 0, sizeof digging);
		    return(0);	/* done with digging */
		}

		if (digging.effort <= 50 ||
		    ((ttmp = t_at(dpx,dpy)) != 0 &&
			(ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT ||
			 ttmp->ttyp == TRAPDOOR || ttmp->ttyp == HOLE)))
		    return(1);

		if (IS_ALTAR(lev->typ)) {
		    altar_wrath(dpx, dpy);
		    angry_priest();
		}

		if (dighole(TRUE)) {	/* make pit at <u.ux,u.uy> */
		    digging.level.dnum = 0;
		    digging.level.dlevel = -1;
		}
		return(0);
	}

	if (digging.effort > 100) {
		register const char *digtxt, *dmgtxt = (const char*) 0;
		register struct obj *obj;
		register boolean shopedge = *in_rooms(dpx, dpy, SHOPBASE);

		if ((obj = sobj_at(STATUE, dpx, dpy)) != 0) {
			if (break_statue(obj))
/*JP
				digtxt = "The statue shatters.";
*/
				digtxt = "彫像はこなごなになった。";
			else
				/* it was a statue trap; break_statue()
				 * printed a message and updated the screen
				 */
				digtxt = (char *)0;
		} else if ((obj = sobj_at(BOULDER, dpx, dpy)) != 0) {
			struct obj *bobj;

			fracture_rock(obj);
			if ((bobj = sobj_at(BOULDER, dpx, dpy)) != 0) {
			    /* another boulder here, restack it to the top */
			    obj_extract_self(bobj);
			    place_object(bobj, dpx, dpy);
			}
/*JP
			digtxt = "The boulder falls apart.";
*/
			digtxt = "岩はこなごなになった。";
		} else if (lev->typ == STONE || lev->typ == SCORR ||
				IS_TREES(lev->typ)) {
			if(Is_earthlevel(&u.uz)) {
			    if(uwep->blessed && !rn2(3)) {
				mkcavearea(FALSE);
				goto cleanup;
			    } else if((uwep->cursed && !rn2(4)) ||
					  (!uwep->blessed && !rn2(6))) {
				mkcavearea(TRUE);
				goto cleanup;
			    }
			}
			if (IS_TREES(lev->typ)) {
/*JP
			    digtxt = "You cut down the tree.";
*/
			    digtxt = "木を切り倒した。";
			    lev->typ = ROOM;
			    if (!rn2(5)) (void) rnd_treefruit_at(dpx, dpy);
			} else {
/*JP
			    digtxt = "You succeed in cutting away some rock.";
*/
			    digtxt = "岩を少し切りとった。";
			    lev->typ = CORR;
			}
		} else if(IS_WALL(lev->typ)) {
			if(shopedge) {
			    add_damage(dpx, dpy, 10L * ACURRSTR);
/*JP
			    dmgtxt = "damage";
*/
			    dmgtxt = "傷つける";
			}
			if (level.flags.is_maze_lev) {
			    lev->typ = ROOM;
			} else if (level.flags.is_cavernous_lev &&
				   !in_town(dpx, dpy)) {
			    lev->typ = CORR;
			} else {
			    lev->typ = DOOR;
			    lev->doormask = D_NODOOR;
			}
/*JP
			digtxt = "You make an opening in the wall.";
*/
			digtxt = "壁に穴を空けた。";
		} else if(lev->typ == SDOOR) {
			cvt_sdoor_to_door(lev);	/* ->typ = DOOR */
/*JP
			digtxt = "You break through a secret door!";
*/
			digtxt = "秘密の扉を通り抜けた！";
			if(!(lev->doormask & D_TRAPPED))
				lev->doormask = D_BROKEN;
		} else if(closed_door(dpx, dpy)) {
/*JP
			digtxt = "You break through the door.";
*/
			digtxt = "扉を通り抜けた。";
			if(shopedge) {
			    add_damage(dpx, dpy, 400L);
/*JP
			    dmgtxt = "break";
*/
			    dmgtxt = "壊す";
			}
			if(!(lev->doormask & D_TRAPPED))
				lev->doormask = D_BROKEN;
		} else return(0); /* statue or boulder got taken */

		if(!does_block(dpx,dpy,&levl[dpx][dpy]))
		    unblock_point(dpx,dpy);	/* vision:  can see through */
		if(Blind)
		    feel_location(dpx, dpy);
		else
		    newsym(dpx, dpy);
		if(digtxt && !digging.quiet) pline(digtxt); /* after newsym */
		if(dmgtxt)
		    pay_for_damage(dmgtxt, FALSE);

		if(Is_earthlevel(&u.uz) && !rn2(3)) {
		    register struct monst *mtmp;

		    switch(rn2(2)) {
		      case 0:
			mtmp = makemon(&mons[PM_EARTH_ELEMENTAL],
					dpx, dpy, NO_MM_FLAGS);
			break;
		      default:
			mtmp = makemon(&mons[PM_XORN],
					dpx, dpy, NO_MM_FLAGS);
			break;
		    }
/*JP
		    if(mtmp) pline_The("debris from your digging comes to life!");
*/
		    if(mtmp) pline("岩の破片が生命を帯びた！");
		}
		if(IS_DOOR(lev->typ) && (lev->doormask & D_TRAPPED)) {
			lev->doormask = D_NODOOR;
/*JP
			b_trapped("door", 0);
*/
			b_trapped("扉", 0);
			newsym(dpx, dpy);
		}
cleanup:
		digging.lastdigtime = moves;
		digging.quiet = FALSE;
		digging.level.dnum = 0;
		digging.level.dlevel = -1;
		return(0);
	} else {		/* not enough effort has been spent yet */
		static const char *const d_target[6] = {
/*JP
			"", "rock", "statue", "boulder", "door", "tree"
*/
			"", "石", "彫像", "岩", "扉", "木"
		};
		int dig_target = dig_typ(uwep, dpx, dpy);

		if (IS_WALL(lev->typ) || dig_target == DIGTYP_DOOR) {
		    if(*in_rooms(dpx, dpy, SHOPBASE)) {
#if 0 /*JP*/
			pline("This %s seems too hard to %s.",
			      IS_DOOR(lev->typ) ? "door" : "wall", verb);
#else
			pline("この%sはとても固くて掘れない。",
			      IS_DOOR(lev->typ) ? "扉" : "壁");
#endif
			return(0);
		    }
		} else if (!IS_ROCK(lev->typ) && dig_target == DIGTYP_ROCK)
		    return(0); /* statue or boulder got taken */
		if(!did_dig_msg) {
#if 0 /*JP:T*/
		    You("hit the %s with all your might.",
			d_target[dig_target]);
#else
		    You("%sを力一杯打ちつけた。",
			d_target[dig_target]);
#endif
		    did_dig_msg = TRUE;
		}
	}
	return(1);
}

/* When will hole be finished? Very rough indication used by shopkeeper. */
int
holetime()
{
	if(occupation != dig || !*u.ushops) return(-1);
	return ((250 - digging.effort) / 20);
}

/* Return typ of liquid to fill a hole with, or ROOM, if no liquid nearby */
STATIC_OVL
schar
fillholetyp(x,y)
int x, y;
{
    register int x1, y1;
    int lo_x = max(1,x-1), hi_x = min(x+1,COLNO-1),
	lo_y = max(0,y-1), hi_y = min(y+1,ROWNO-1);
    int pool_cnt = 0, moat_cnt = 0, lava_cnt = 0;

    for (x1 = lo_x; x1 <= hi_x; x1++)
	for (y1 = lo_y; y1 <= hi_y; y1++)
	    if (levl[x1][y1].typ == POOL)
		pool_cnt++;
	    else if (levl[x1][y1].typ == MOAT ||
		    (levl[x1][y1].typ == DRAWBRIDGE_UP &&
			(levl[x1][y1].drawbridgemask & DB_UNDER) == DB_MOAT))
		moat_cnt++;
	    else if (levl[x1][y1].typ == LAVAPOOL ||
		    (levl[x1][y1].typ == DRAWBRIDGE_UP &&
			(levl[x1][y1].drawbridgemask & DB_UNDER) == DB_LAVA))
		lava_cnt++;
    pool_cnt /= 3;		/* not as much liquid as the others */

    if (lava_cnt > moat_cnt + pool_cnt && rn2(lava_cnt + 1))
	return LAVAPOOL;
    else if (moat_cnt > 0 && rn2(moat_cnt + 1))
	return MOAT;
    else if (pool_cnt > 0 && rn2(pool_cnt + 1))
	return POOL;
    else
	return ROOM;
}

void
digactualhole(x, y, madeby, ttyp)
register int	x, y;
struct monst	*madeby;
int ttyp;
{
	struct obj *oldobjs, *newobjs;
	register struct trap *ttmp;
	char surface_type[BUFSZ];
	struct rm *lev = &levl[x][y];
	boolean shopdoor;
	struct monst *mtmp = m_at(x, y);	/* may be madeby */
	boolean madeby_u = (madeby == BY_YOU);
	boolean madeby_obj = (madeby == BY_OBJECT);
	boolean at_u = (x == u.ux) && (y == u.uy);
	boolean wont_fall = Levitation || Flying;

	if (u.utrap && u.utraptype == TT_INFLOOR) u.utrap = 0;

	/* these furniture checks were in dighole(), but wand
	   breaking bypasses that routine and calls us directly */
	if (IS_FOUNTAIN(lev->typ)) {
	    dogushforth(FALSE);
	    SET_FOUNTAIN_WARNED(x,y);		/* force dryup */
	    dryup(x, y, madeby_u);
	    return;
#ifdef SINKS
	} else if (IS_SINK(lev->typ)) {
	    breaksink(x, y);
	    return;
#endif
	} else if (lev->typ == DRAWBRIDGE_DOWN ||
		   (is_drawbridge_wall(x, y) >= 0)) {
	    int bx = x, by = y;
	    /* if under the portcullis, the bridge is adjacent */
	    (void) find_drawbridge(&bx, &by);
	    destroy_drawbridge(bx, by);
	    return;
	}

	if (ttyp != PIT && !Can_dig_down(&u.uz)) {
	    impossible("digactualhole: can't dig %s on this level.",
		       defsyms[trap_to_defsym(ttyp)].explanation);
	    ttyp = PIT;
	}

	/* maketrap() might change it, also, in this situation,
	   surface() returns an inappropriate string for a grave */
	if (IS_GRAVE(lev->typ))
/*JP
	    Strcpy(surface_type, "grave");
*/
	    Strcpy(surface_type, "墓");
	else
	    Strcpy(surface_type, surface(x,y));
	shopdoor = IS_DOOR(lev->typ) && *in_rooms(x, y, SHOPBASE);
	oldobjs = level.objects[x][y];
	ttmp = maketrap(x, y, ttyp);
	if (!ttmp) return;
	newobjs = level.objects[x][y];
	ttmp->tseen = (madeby_u || cansee(x,y));
	ttmp->madeby_u = madeby_u;
	newsym(ttmp->tx,ttmp->ty);

	if (ttyp == PIT) {

	    if(madeby_u) {
/*JP
		You("dig a pit in the %s.", surface_type);
*/
		You("%sに落し穴を掘った。", surface_type);
/*JP
		if (shopdoor) pay_for_damage("ruin", FALSE);
*/
		if (shopdoor) pay_for_damage("壊す", FALSE);
	    } else if (!madeby_obj && canseemon(madeby))
/*JP
		pline("%s digs a pit in the %s.", Monnam(madeby), surface_type);
*/
		pline("%sは%sに落し穴を掘った。", Monnam(madeby), surface_type);
	    else if (cansee(x, y) && flags.verbose)
/*JP
		pline("A pit appears in the %s.", surface_type);
*/
		pline("落し穴が%sに現われた。", surface_type);

	    if(at_u) {
		if (!wont_fall) {
		    if (!Passes_walls)
			u.utrap = rn1(4,2);
		    u.utraptype = TT_PIT;
		    vision_full_recalc = 1;	/* vision limits change */
		} else
		    u.utrap = 0;
		if (oldobjs != newobjs)	/* something unearthed */
			(void) pickup(1);	/* detects pit */
	    } else if(mtmp) {
		if(is_flyer(mtmp->data) || is_floater(mtmp->data)) {
		    if(canseemon(mtmp))
#if 0 /*JP:T*/
			pline("%s %s over the pit.", Monnam(mtmp),
						     (is_flyer(mtmp->data)) ?
						     "flies" : "floats");
#else
			pline("%sは%s落し穴を越えた。", Monnam(mtmp),
						     (is_flyer(mtmp->data)) ?
						     "飛んで" : "浮いて");
#endif
		} else if(mtmp != madeby)
		    (void) mintrap(mtmp);
	    }
	} else {	/* was TRAPDOOR now a HOLE*/

	    if(madeby_u)
/*JP
		You("dig a hole through the %s.", surface_type);
*/
		You("%sに穴を開けた。", surface_type);
	    else if(!madeby_obj && canseemon(madeby))
/*JP
		pline("%s digs a hole through the %s.",
*/
		pline("%sは%sに穴を開けた。",
		      Monnam(madeby), surface_type);
	    else if(cansee(x, y) && flags.verbose)
/*JP
		pline("A hole appears in the %s.", surface_type);
*/
		pline("%sに穴が現われた。", surface_type);

	    if (at_u) {
		if (!u.ustuck && !wont_fall && !next_to_u()) {
/*JP
		    You("are jerked back by your pet!");
*/
		    You("ペットによって引き戻された！");
		    wont_fall = TRUE;
		}

		/* Floor objects get a chance of falling down.  The case where
		 * the hero does NOT fall down is treated here.  The case
		 * where the hero does fall down is treated in goto_level().
		 */
		if (u.ustuck || wont_fall) {
		    if (newobjs)
			impact_drop((struct obj *)0, x, y, 0);
		    if (oldobjs != newobjs)
			(void) pickup(1);
/*JP
		    if (shopdoor && madeby_u) pay_for_damage("ruin", FALSE);
*/
		    if (shopdoor && madeby_u) pay_for_damage("めちゃめちゃにする", FALSE);

		} else {
		    d_level newlevel;

		    if (*u.ushops && madeby_u)
			shopdig(1); /* shk might snatch pack */
		    /* handle earlier damage, eg breaking wand of digging */
/*JP
		    else if (!madeby_u) pay_for_damage("dig into", TRUE);
*/
		    else if (!madeby_u) pay_for_damage("穴をあける", TRUE);

/*JP
		    You("fall through...");
*/
		    You("落ちた．．．");
		    /* Earlier checks must ensure that the destination
		     * level exists and is in the present dungeon.
		     */
		    newlevel.dnum = u.uz.dnum;
		    newlevel.dlevel = u.uz.dlevel + 1;
		    goto_level(&newlevel, FALSE, TRUE, FALSE);
		    /* messages for arriving in special rooms */
		    spoteffects(FALSE);
		}
	    } else {
/*JP
		if (shopdoor && madeby_u) pay_for_damage("ruin", FALSE);
*/
		if (shopdoor && madeby_u) pay_for_damage("めちゃめちゃにする", FALSE);
		if (newobjs)
		    impact_drop((struct obj *)0, x, y, 0);
		if (mtmp) {
		     /*[don't we need special sokoban handling here?]*/
		    if (is_flyer(mtmp->data) || is_floater(mtmp->data) ||
		        mtmp->data == &mons[PM_WUMPUS] ||
			(mtmp->wormno && count_wsegs(mtmp) > 5) ||
			mtmp->data->msize >= MZ_HUGE) return;
		    if (mtmp == u.ustuck)	/* probably a vortex */
			    return;		/* temporary? kludge */

		    if (teleport_pet(mtmp, FALSE)) {
			d_level tolevel;

			if (Is_stronghold(&u.uz)) {
			    assign_level(&tolevel, &valley_level);
			} else if (Is_botlevel(&u.uz)) {
			    if (canseemon(mtmp))
/*JP
				pline("%s avoids the trap.", Monnam(mtmp));
*/
				pline("%sは罠を避けた。", Monnam(mtmp));
			    return;
			} else {
			    get_level(&tolevel, depth(&u.uz) + 1);
			}
			if (mtmp->isshk) make_angry_shk(mtmp, 0, 0);
			migrate_to_level(mtmp, ledger_no(&tolevel),
					 MIGR_RANDOM, (coord *)0);
		    }
		}
	    }
	}
}

/* return TRUE if digging succeeded, FALSE otherwise */
boolean
dighole(pit_only)
boolean pit_only;
{
	register struct trap *ttmp = t_at(u.ux, u.uy);
	struct rm *lev = &levl[u.ux][u.uy];
	struct obj *boulder_here;
	schar typ;
	boolean nohole = !Can_dig_down(&u.uz);

	if ((ttmp && (ttmp->ttyp == MAGIC_PORTAL || nohole)) ||
	   (IS_ROCK(lev->typ) && lev->typ != SDOOR &&
	    (lev->wall_info & W_NONDIGGABLE) != 0)) {
/*JP
		pline_The("%s here is too hard to dig in.", surface(u.ux,u.uy));
*/
		pline("%sはとても固くて掘れない。", surface(u.ux,u.uy));

	} else if (is_pool(u.ux, u.uy) || is_lava(u.ux, u.uy)) {
#if 0 /*JP:T*/
		pline_The("%s sloshes furiously for a moment, then subsides.",
			is_lava(u.ux, u.uy) ? "lava" : "water");
#else
		pline("%sは激しく波うった。",
			is_lava(u.ux, u.uy) ? "溶岩" : "水");
#endif
		wake_nearby();	/* splashing */

	} else if (lev->typ == DRAWBRIDGE_DOWN ||
		   (is_drawbridge_wall(u.ux, u.uy) >= 0)) {
		/* drawbridge_down is the platform crossing the moat when the
		   bridge is extended; drawbridge_wall is the open "doorway" or
		   closed "door" where the portcullis/mechanism is located */
		if (pit_only) {
/*JP
		    pline_The("drawbridge seems too hard to dig through.");
*/
		    pline("跳ね橋はとても固くて掘れそうにない。");
		    return FALSE;
		} else {
		    int x = u.ux, y = u.uy;
		    /* if under the portcullis, the bridge is adjacent */
		    (void) find_drawbridge(&x, &y);
		    destroy_drawbridge(x, y);
		    return TRUE;
		}

	} else if ((boulder_here = sobj_at(BOULDER, u.ux, u.uy)) != 0) {
		if (ttmp && (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT) &&
		    rn2(2)) {
/*JP
			pline_The("boulder settles into the pit.");
*/
			pline("岩は落し穴を埋めた。");
			ttmp->ttyp = PIT;	 /* crush spikes */
		} else {
			/*
			 * digging makes a hole, but the boulder immediately
			 * fills it.  Final outcome:  no hole, no boulder.
			 */
/*JP
			pline("KADOOM! The boulder falls in!");
*/
			pline("どどーん！岩は落ちた！");
			(void) delfloortrap(ttmp);
		}
		delobj(boulder_here);
		return TRUE;

	} else if (IS_GRAVE(lev->typ)) {        
	    digactualhole(u.ux, u.uy, BY_YOU, PIT);
	    dig_up_grave();
	    return TRUE;
	} else if (lev->typ == DRAWBRIDGE_UP) {
		/* must be floor or ice, other cases handled above */
		/* dig "pit" and let fluid flow in (if possible) */
		typ = fillholetyp(u.ux,u.uy);

		if (typ == ROOM) {
			/*
			 * We can't dig a hole here since that will destroy
			 * the drawbridge.  The following is a cop-out. --dlc
			 */
/*JP
			pline_The("%s here is too hard to dig in.",
*/
			pline("%sはとても固くて掘れない。",
			      surface(u.ux, u.uy));
			return FALSE;
		}

		lev->drawbridgemask &= ~DB_UNDER;
		lev->drawbridgemask |= (typ == LAVAPOOL) ? DB_LAVA : DB_MOAT;

 liquid_flow:
		if (ttmp) (void) delfloortrap(ttmp);
		/* if any objects were frozen here, they're released now */
		unearth_objs(u.ux, u.uy);

#if 0 /*JP:T*/
		pline("As you dig, the hole fills with %s!",
		      typ == LAVAPOOL ? "lava" : "water");
#else
		pline("あなたが掘ると、%sが湧いてきた！",
		      typ == LAVAPOOL ? "溶岩" : "水");
#endif
		if (!Levitation && !Flying) {
		    if (typ == LAVAPOOL)
			(void) lava_effects();
		    else if (!Wwalking)
			(void) drown();
		}
		return TRUE;

	/* the following two are here for the wand of digging */
	} else if (IS_THRONE(lev->typ)) {
/*JP
		pline_The("throne is too hard to break apart.");
*/
		pline("玉座はとても固くて砕けない。");

	} else if (IS_ALTAR(lev->typ)) {
/*JP
		pline_The("altar is too hard to break apart.");
*/
		pline("祭壇はとても固くて砕けない。");

	} else {
		typ = fillholetyp(u.ux,u.uy);

		if (typ != ROOM) {
			lev->typ = typ;
			goto liquid_flow;
		}

		/* finally we get to make a hole */
		if (nohole || pit_only)
			digactualhole(u.ux, u.uy, BY_YOU, PIT);
		else
			digactualhole(u.ux, u.uy, BY_YOU, HOLE);

		return TRUE;
	}

	return FALSE;
}

STATIC_OVL void
dig_up_grave()
{
	struct obj *otmp;

	/* Grave-robbing is frowned upon... */
	exercise(A_WIS, FALSE);
	if (Role_if(PM_ARCHEOLOGIST)) {
	    adjalign(-sgn(u.ualign.type)*3);
/*JP
	    You_feel("like a despicable grave-robber!");
*/
	    pline("これではまるで墓泥棒だ！");
	} else if (Role_if(PM_SAMURAI)) {
	    adjalign(-sgn(u.ualign.type));
/*JP
	    You("disturb the honorable dead!");
*/
	    You("名誉ある死者の眠りを妨げだ！");
	} else if ((u.ualign.type == A_LAWFUL) && (u.ualign.record > -10)) {
	    adjalign(-sgn(u.ualign.type));
/*JP
	    You("have violated the sanctity of this grave!");
*/
	    You("聖なる墓地を犯した！");
	}

	switch (rn2(5)) {
	case 0:
	case 1:
/*JP
	    You("unearth a corpse.");
*/
	    You("死体を掘り起した。");
	    if (!!(otmp = mk_tt_object(CORPSE, u.ux, u.uy)))
	    	otmp->age -= 100;		/* this is an *OLD* corpse */;
	    break;
	case 2:
#if 0 /*JP:T*/
	    if (!Blind) pline(Hallucination ? "Dude!  The living dead!" :
 			"The grave's owner is very upset!");
#else
	    if (!Blind) pline(Hallucination ? "ゾンビがくるりと輪を描いた！" :
			      "墓の所有者はとても驚いた！");
#endif
 	    (void) makemon(mkclass(S_ZOMBIE,0), u.ux, u.uy, NO_MM_FLAGS);
	    break;
	case 3:
#if 0 /*JP:T*/
	    if (!Blind) pline(Hallucination ? "I want my mummy!" :
 			"You've disturbed a tomb!");
#else
	    if (!Blind) pline(Hallucination ? "マミーが必要だ！" :
 			"墓を荒してしまった！");
#endif
 	    (void) makemon(mkclass(S_MUMMY,0), u.ux, u.uy, NO_MM_FLAGS);
	    break;
	default:
	    /* No corpse */
/*JP
	    pline_The("grave seems unused.  Strange....");
*/
	    pline("この墓は未使用のようだ。奇妙だ．．．");
	    break;
	}
	levl[u.ux][u.uy].typ = ROOM;
	del_engr_at(u.ux, u.uy);
	newsym(u.ux,u.uy);
	return;
}

int
use_pick_axe(obj)
struct obj *obj;
{
	boolean ispick;
	char dirsyms[12];
	char qbuf[QBUFSZ];
	register char *dsp = dirsyms;
	register int rx, ry;
	int res = 0;
	register const char *sdp, *verb;

	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	/* Check tool */
	if (obj != uwep) {
	    if (!wield_tool(obj, "swing")) return 0;
	    else res = 1;
	}
	ispick = is_pick(obj);
/*JP
	verb = ispick ? "dig" : "chop";
*/
	verb = ispick ? "掘る" : "砕く";

	if (u.utrap && u.utraptype == TT_WEB) {
#if 0 /*JP:T*/
	    pline("%s you can't %s while entangled in a web.",
		  /* res==0 => no prior message;
		     res==1 => just got "You now wield a pick-axe." message */
		  !res ? "Unfortunately," : "But", verb);
#else
	    pline("%sくもの巣にひっかかっている間は%s。",
		  !res ? "残念ながら" : "しかし", jconj(verb, "ない"));
#endif
	    return res;
	}

	while(*sdp) {
		(void) movecmd(*sdp);	/* sets u.dx and u.dy and u.dz */
		rx = u.ux + u.dx;
		ry = u.uy + u.dy;
		/* Include down even with axe, so we have at least one direction */
		if (u.dz > 0 ||
		    (u.dz == 0 && isok(rx, ry) &&
		     dig_typ(obj, rx, ry) != DIGTYP_UNDIGGABLE))
			*dsp++ = *sdp;
		sdp++;
	}
	*dsp = 0;
/*JP
	Sprintf(qbuf, "In what direction do you want to %s? [%s]", verb, dirsyms);
*/
	Sprintf(qbuf, "どの方向を%s？[%s]", verb, dirsyms);
	if(!getdir(qbuf))
		return(res);

	return(use_pick_axe2(obj));
}

/* MRKR: use_pick_axe() is split in two to allow autodig to bypass */
/*       the "In what direction do you want to dig?" query.        */
/*       use_pick_axe2() uses the existing u.dx, u.dy and u.dz    */

int
use_pick_axe2(obj) 
struct obj *obj;
{
	register int rx, ry;
	register struct rm *lev;
	int dig_target;
	boolean ispick = is_pick(obj);
#if 0 /*JP*/
	const char *verbing = ispick ? "digging" : "chopping";
#else
	const char *verbing = ispick ? "掘る" : "砕く";
#endif

	if (u.uswallow && attack(u.ustuck)) {
		;  /* return(1) */
	} else if (Underwater) {
/*JP
		pline("Turbulence torpedoes your %s attempts.", verbing);
*/
		pline("%sとすると乱水流が起きた。", jconj(verbing, "よう"));
	} else if(u.dz < 0) {
		if(Levitation)
/*JP
			You("don't have enough leverage.");
*/
			You("浮いているのでふんばりがきかない。");
		else
/*JP
			You_cant("reach the %s.",ceiling(u.ux,u.uy));
*/
			You("%sに届かない。", ceiling(u.ux,u.uy));
	} else if(!u.dx && !u.dy && !u.dz) {
		char buf[BUFSZ];
		int dam;

		dam = rnd(2) + dbon() + obj->spe;
		if (dam <= 0) dam = 1;
#if 0 /*JP*/
		You("hit yourself with %s.", yname(uwep));
		Sprintf(buf, "%s own %s", uhis(),
				OBJ_NAME(objects[obj->otyp]));
#else
		You("自分自身を%sで叩いた。", yname(uwep));
		Sprintf(buf, "自分自身を%sで叩いて", yname(uwep));
#endif
		losehp(dam, buf, KILLED_BY);
		flags.botl=1;
		return(1);
	} else if(u.dz == 0) {
		if(Stunned || (Confusion && !rn2(5))) confdir();
		rx = u.ux + u.dx;
		ry = u.uy + u.dy;
		if(!isok(rx, ry)) {
/*JP
			pline("Clash!");
*/
			pline("ガラガラ！");
			return(1);
		}
		lev = &levl[rx][ry];
		if(MON_AT(rx, ry) && attack(m_at(rx, ry)))
			return(1);
		dig_target = dig_typ(obj, rx, ry);
		if (dig_target == DIGTYP_UNDIGGABLE) {
			/* ACCESSIBLE or POOL */
			struct trap *trap = t_at(rx, ry);

			if (trap && trap->ttyp == WEB) {
			    if (!trap->tseen) {
				seetrap(trap);
/*JP
				There("is a spider web there!");
*/
				pline("そこにはくもの巣がある！");
			    }
#if 0 /*JP*/
			    Your("%s entangled in the web.",
				aobjnam(obj, "become"));
#else
			    Your("%sはくもの巣にからまった。",
				xname(obj));
#endif
			    /* you ought to be able to let go; tough luck */
			    /* (maybe `move_into_trap()' would be better) */
/*JP
			    nomul(-d(2,2), "stuck in a spider web");
*/
			    nomul(-d(2,2), "くもの巣に捕まっている隙に");
/*JP
			    nomovemsg = "You pull free.";
*/
			    nomovemsg = "ひきはなした。";
			} else if (lev->typ == IRONBARS) {
/*JP
			    pline("Clang!");
*/
			    pline("ガツン！");
			    wake_nearby();
			} else if (IS_TREES(lev->typ))
/*JP
			    You("need an axe to cut down a tree.");
*/
			    You("木を切るには斧が必要だ。");
			else if (IS_ROCK(lev->typ))
/*JP
			    You("need a pick to dig rock.");
*/
			    You("掘るにはつるはしが必要だ。");
			else if (!ispick && (sobj_at(STATUE, rx, ry) ||
					     sobj_at(BOULDER, rx, ry))) {
			    boolean vibrate = !rn2(3);
#if 0 /*JP:T*/
			    pline("Sparks fly as you whack the %s.%s",
				sobj_at(STATUE, rx, ry) ? "statue" : "boulder",
				vibrate ? " The axe-handle vibrates violently!" : "");
#else
			    pline("%sを切ろうとしたら火花が散った！%s",
				sobj_at(STATUE, rx, ry) ? "彫像" : "岩",
				vibrate ? "斧は激しく振動した！" : "");
#endif
/*JP
			    if (vibrate) losehp(2, "axing a hard object", KILLED_BY);
*/
			    if (vibrate) losehp(2, "固いものに斧を使おうとして", KILLED_BY);
			}
			else
#if 0 /*JP:T*/
			    You("swing your %s through thin air.",
				aobjnam(obj, (char *)0));
#else
			    You("何もない空間で%sを振りまわした。",
				xname(obj));
#endif
		} else {
#if 0 /*JP*/
			static const char * const d_action[6] = {
						"swinging",
						"digging",
						"chipping the statue",
						"hitting the boulder",
						"chopping at the door",
						"cutting the tree"
			};
#else
			static const char * const d_action1[6] = {
						"振り",
						"掘り",
						"彫像を削り",
						"岩を打ちつけ",
						"扉を削り",
						"木を切り"
			};
			static const char * const d_action2[6] = {
						"振る",
						"掘る",
						"彫像を削る",
						"岩を打ちつける",
						"扉を削る",
						"木を切る"
			};
#endif
			did_dig_msg = FALSE;
			digging.quiet = FALSE;
			if (digging.pos.x != rx || digging.pos.y != ry ||
			    !on_level(&digging.level, &u.uz) || digging.down) {
			    if (flags.autodig &&
				dig_target == DIGTYP_ROCK && !digging.down &&
				digging.pos.x == u.ux &&
				digging.pos.y == u.uy &&
				(moves <= digging.lastdigtime+2 &&
				 moves >= digging.lastdigtime)) {
				/* avoid messages if repeated autodigging */
				did_dig_msg = TRUE;
				digging.quiet = TRUE;
			    }
			    digging.down = digging.chew = FALSE;
			    digging.warned = FALSE;
			    digging.pos.x = rx;
			    digging.pos.y = ry;
			    assign_level(&digging.level, &u.uz);
			    digging.effort = 0;
			    if (!digging.quiet)
/*JP
				You("start %s.", d_action[dig_target]);
*/
				You("%sはじめた。", d_action1[dig_target]);
			} else {
#if 0 /*JP*/
			    You("%s %s.", digging.chew ? "begin" : "continue",
					d_action[dig_target]);
#else
			    You("%sのを%sした。", d_action2[dig_target],
					digging.chew ? "開始" : "再開");
#endif
			    digging.chew = FALSE;
			}
			set_occupation(dig, verbing, 0);
		}
	} else if (Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)) {
		/* it must be air -- water checked above */
/*JP
		You("swing your %s through thin air.", aobjnam(obj, (char *)0));
*/
		You("%sを何もない空間で振りまわした。", xname(obj));
	} else if (!can_reach_floor()) {
/*JP
		You_cant("reach the %s.", surface(u.ux,u.uy));
*/
		You("%sに届かない。", surface(u.ux,u.uy));
	} else if (is_pool(u.ux, u.uy) || is_lava(u.ux, u.uy)) {
		/* Monsters which swim also happen not to be able to dig */
#if 0 /*JP:T*/
		You("cannot stay under%s long enough.",
				is_pool(u.ux, u.uy) ? "water" : " the lava");
#else
		You("%sには長時間いられない。",
				is_pool(u.ux, u.uy) ? "水面下" : "溶岩の中");
#endif
	} else if (!ispick) {
#if 0 /*JP:T*/
		Your("%s merely scratches the %s.",
				aobjnam(obj, (char *)0), surface(u.ux,u.uy));
#else
		Your("%sは%sにかすり傷をつけただけだった。",
				aobjnam(obj, (char *)0), surface(u.ux,u.uy));
#endif
		u_wipe_engr(3);
	} else {
		if (digging.pos.x != u.ux || digging.pos.y != u.uy ||
			!on_level(&digging.level, &u.uz) || !digging.down) {
		    digging.chew = FALSE;
		    digging.down = TRUE;
		    digging.warned = FALSE;
		    digging.pos.x = u.ux;
		    digging.pos.y = u.uy;
		    assign_level(&digging.level, &u.uz);
		    digging.effort = 0;
/*JP
		    You("start %s downward.", verbing);
*/
		    You("下向きに掘りはじめた。");
		    if (*u.ushops) shopdig(0);
		} else
/*JP
		    You("continue %s downward.", verbing);
*/
		    You("下向きに掘るのを再開した。");
		did_dig_msg = FALSE;
		set_occupation(dig, verbing, 0);
	}
	return(1);
}

/*
 * Town Watchmen frown on damage to the town walls, trees or fountains.
 * It's OK to dig holes in the ground, however.
 * If mtmp is assumed to be a watchman, a watchman is found if mtmp == 0
 * zap == TRUE if wand/spell of digging, FALSE otherwise (chewing)
 */
void
watch_dig(mtmp, x, y, zap)
    struct monst *mtmp;
    xchar x, y;
    boolean zap;
{
	struct rm *lev = &levl[x][y];

	if (in_town(x, y) &&
	    (closed_door(x, y) || lev->typ == SDOOR ||
	     IS_WALL(lev->typ) || IS_FOUNTAIN(lev->typ) || IS_TREE(lev->typ))) {
	    if (!mtmp) {
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if ((mtmp->data == &mons[PM_WATCHMAN] ||
			 mtmp->data == &mons[PM_WATCH_CAPTAIN]) &&
			mtmp->mcansee && m_canseeu(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) && mtmp->mpeaceful)
			break;
		}
	    }

	    if (mtmp) {
		if(zap || digging.warned) {
/*JP
		    verbalize("Halt, vandal!  You're under arrest!");
*/
		    verbalize("止まれ野蛮人！おまえを逮捕する！");
		    (void) angry_guards(!(flags.soundok));
		} else {
		    const char *str;

		    if (IS_DOOR(lev->typ))
/*JP
			str = "door";
*/
			str = "扉";
		    else if (IS_TREE(lev->typ))
/*JP
			str = "tree";
*/
			str = "木";
		    else if (IS_ROCK(lev->typ))
/*JP
			str = "wall";
*/
			str = "壁";
		    else
/*JP
			str = "fountain";
*/
			str = "泉";
/*JP
		    verbalize("Hey, stop damaging that %s!", str);
*/
		    verbalize("おい、%sを破壊するのをやめろ！", str);
		    digging.warned = TRUE;
		}
		if (is_digging())
		    stop_occupation();
	    }
	}
}

#endif /* OVLB */
#ifdef OVL0

/* Return TRUE if monster died, FALSE otherwise.  Called from m_move(). */
boolean
mdig_tunnel(mtmp)
register struct monst *mtmp;
{
	register struct rm *here;
	int pile = rnd(12);

	here = &levl[mtmp->mx][mtmp->my];
	if (here->typ == SDOOR)
	    cvt_sdoor_to_door(here);	/* ->typ = DOOR */

	/* Eats away door if present & closed or locked */
	if (closed_door(mtmp->mx, mtmp->my)) {
	    if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
		add_damage(mtmp->mx, mtmp->my, 0L);
	    unblock_point(mtmp->mx, mtmp->my);	/* vision */
	    if (here->doormask & D_TRAPPED) {
		here->doormask = D_NODOOR;
		if (mb_trapped(mtmp)) {	/* mtmp is killed */
		    newsym(mtmp->mx, mtmp->my);
		    return TRUE;
		}
	    } else {
		if (!rn2(3) && flags.verbose)	/* not too often.. */
/*JP
		    You_feel("an unexpected draft.");
*/
		    You("思いもよらず、すきま風を感じた。");
		here->doormask = D_BROKEN;
	    }
	    newsym(mtmp->mx, mtmp->my);
	    return FALSE;
	} else if (!IS_ROCK(here->typ) && !IS_TREE(here->typ)) /* no dig */
	    return FALSE;

	/* Only rock, trees, and walls fall through to this point. */
	if ((here->wall_info & W_NONDIGGABLE) != 0) {
	    impossible("mdig_tunnel:  %s at (%d,%d) is undiggable",
		       (IS_WALL(here->typ) ? "wall" : "stone"),
		       (int) mtmp->mx, (int) mtmp->my);
	    return FALSE;	/* still alive */
	}

	if (IS_WALL(here->typ)) {
	    /* KMH -- Okay on arboreal levels (room walls are still stone) */
	    if (flags.soundok && flags.verbose && !rn2(5))
/*JP
		You_hear("crashing rock.");
*/
		You_hear("岩のくだける音を聞いた。");
	    if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
		add_damage(mtmp->mx, mtmp->my, 0L);
	    if (level.flags.is_maze_lev) {
		here->typ = ROOM;
	    } else if (level.flags.is_cavernous_lev &&
		       !in_town(mtmp->mx, mtmp->my)) {
		here->typ = CORR;
	    } else {
		here->typ = DOOR;
		here->doormask = D_NODOOR;
	    }
	} else if (IS_TREE(here->typ)) {
	    here->typ = ROOM;
	    if (pile && pile < 5)
		(void) rnd_treefruit_at(mtmp->mx, mtmp->my);
	} else {
	    here->typ = CORR;
	    if (pile && pile < 5)
		(void) mksobj_at((pile == 1) ? BOULDER : ROCK,
			     mtmp->mx, mtmp->my, TRUE, FALSE);
	}
	newsym(mtmp->mx, mtmp->my);
	if (!sobj_at(BOULDER, mtmp->mx, mtmp->my))
	    unblock_point(mtmp->mx, mtmp->my);	/* vision */

	return FALSE;
}

#endif /* OVL0 */
#ifdef OVL3

/* digging via wand zap or spell cast */
void
zap_dig()
{
	struct rm *room;
	struct monst *mtmp;
	struct obj *otmp;
	int zx, zy, digdepth;
	boolean shopdoor, shopwall, maze_dig;
	/*
	 * Original effect (approximately):
	 * from CORR: dig until we pierce a wall
	 * from ROOM: pierce wall and dig until we reach
	 * an ACCESSIBLE place.
	 * Currently: dig for digdepth positions;
	 * also down on request of Lennart Augustsson.
	 */

	if (u.uswallow) {
	    mtmp = u.ustuck;

	    if (!is_whirly(mtmp->data)) {
		if (is_animal(mtmp->data))
/*JP
		    You("pierce %s %s wall!",
*/
		    You("%sの%sの壁に穴を開けた！",
			s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH));

		/* Juiblex takes less damage from a wand of digging */
		if (mtmp->data == &mons[PM_JUIBLEX])
			mtmp->mhp = (mtmp->mhp + 1) / 2;
		else
			mtmp->mhp = 1;		/* almost dead */
		expels(mtmp, mtmp->data, !is_animal(mtmp->data));
	    }
	    return;
	} /* swallowed */

	if (u.dz) {
	    if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !Underwater) {
		if (u.dz < 0 || On_stairs(u.ux, u.uy)) {
		    if (On_stairs(u.ux, u.uy))
#if 0 /*JP:T*/
			pline_The("beam bounces off the %s and hits the %s.",
			      (u.ux == xdnladder || u.ux == xupladder) ?
			      "ladder" : "stairs", ceiling(u.ux, u.uy));
#else
			pline("光線は%sで反射し%sに命中した。",
			      (u.ux == xdnladder || u.ux == xupladder) ?
			      "はしご" : "階段", ceiling(u.ux, u.uy));
#endif
/*JP
		    You("loosen a rock from the %s.", ceiling(u.ux, u.uy));
*/
		    pline("%sの岩がガタガタしはじめた。", ceiling(u.ux, u.uy));
/*JP
		    pline("It falls on your %s!", body_part(HEAD));
*/
		    pline("それはあなたの%sに落ちてきた！", body_part(HEAD));
#if 0 /*JP:T*/
		    losehp(rnd((uarmh && is_metallic(uarmh)) ? 2 : 6),
			   "falling rock", KILLED_BY_AN);
#else
		    losehp(rnd((uarmh && is_metallic(uarmh)) ? 2 : 6),
			   "落岩で", KILLED_BY_AN);
#endif
		    otmp = mksobj_at(ROCK, u.ux, u.uy, FALSE, FALSE);
		    if (otmp) {
			(void)xname(otmp);	/* set dknown, maybe bknown */
			stackobj(otmp);
		    }
		    newsym(u.ux, u.uy);
		} else {
		    watch_dig((struct monst *)0, u.ux, u.uy, TRUE);
		    (void) dighole(FALSE);
		}
	    }
	    return;
	} /* up or down */

	/* normal case: digging across the level */
	shopdoor = shopwall = FALSE;
	maze_dig = level.flags.is_maze_lev && !Is_earthlevel(&u.uz);
	zx = u.ux + u.dx;
	zy = u.uy + u.dy;
	digdepth = rn1(18, 8);
	tmp_at(DISP_BEAM, cmap_to_glyph(S_digbeam));
	while (--digdepth >= 0) {
	    if (!isok(zx,zy)) break;
	    room = &levl[zx][zy];
	    tmp_at(zx,zy);
	    delay_output();	/* wait a little bit */
	    if (closed_door(zx, zy) || room->typ == SDOOR) {
		if (*in_rooms(zx,zy,SHOPBASE)) {
		    add_damage(zx, zy, 400L);
		    shopdoor = TRUE;
		}
		if (room->typ == SDOOR)
		    room->typ = DOOR;
		else if (cansee(zx, zy))
/*JP
		    pline_The("door is razed!");
*/
		    pline("扉は崩れ落ちた！");
		watch_dig((struct monst *)0, zx, zy, TRUE);
		room->doormask = D_NODOOR;
		unblock_point(zx,zy); /* vision */
		digdepth -= 2;
		if (maze_dig) break;
	    } else if (maze_dig) {
		if (IS_WALL(room->typ)) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			if (*in_rooms(zx,zy,SHOPBASE)) {
			    add_damage(zx, zy, 200L);
			    shopwall = TRUE;
			}
			room->typ = ROOM;
			unblock_point(zx,zy); /* vision */
		    } else if (!Blind)
/*JP
			pline_The("wall glows then fades.");
*/
			pline("壁は一瞬輝いた。");
		    break;
		} else if (IS_TREE(room->typ)) { /* check trees before stone */
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = ROOM;
			unblock_point(zx,zy); /* vision */
		    } else if (!Blind)
/*JP
			pline_The("tree shudders but is unharmed.");
*/
			pline("木はゆれたが傷つかなかった。");
		    break;
		} else if (room->typ == STONE || room->typ == SCORR) {
		    if (!(room->wall_info & W_NONDIGGABLE)) {
			room->typ = CORR;
			unblock_point(zx,zy); /* vision */
		    } else if (!Blind)
/*JP
			pline_The("rock glows then fades.");
*/
			pline("石は一瞬輝いた。");
		    break;
		}
	    } else if (IS_ROCK(room->typ)) {
		if (!may_dig(zx,zy)) break;
		if (IS_WALL(room->typ) || room->typ == SDOOR) {
		    if (*in_rooms(zx,zy,SHOPBASE)) {
			add_damage(zx, zy, 200L);
			shopwall = TRUE;
		    }
		    watch_dig((struct monst *)0, zx, zy, TRUE);
		    if (level.flags.is_cavernous_lev && !in_town(zx, zy)) {
			room->typ = CORR;
		    } else {
			room->typ = DOOR;
			room->doormask = D_NODOOR;
		    }
		    digdepth -= 2;
		} else if (IS_TREE(room->typ)) {
		    room->typ = ROOM;
		    digdepth -= 2;
		} else {	/* IS_ROCK but not IS_WALL or SDOOR */
		    room->typ = CORR;
		    digdepth--;
		}
		unblock_point(zx,zy); /* vision */
	    }
	    zx += u.dx;
	    zy += u.dy;
	} /* while */
	tmp_at(DISP_END,0);	/* closing call */
	if (shopdoor || shopwall)
/*JP
	    pay_for_damage(shopdoor ? "destroy" : "dig into", FALSE);
*/
	    pay_for_damage(shopdoor ? "破壊する" : "穴をあける", FALSE);
	return;
}

/* move objects from fobj/nexthere lists to buriedobjlist, keeping position */
/* information */
struct obj *
bury_an_obj(otmp)
	struct obj *otmp;
{
	struct obj *otmp2;
	boolean under_ice;

#ifdef DEBUG
	pline("bury_an_obj: %s", xname(otmp));
#endif
	if (otmp == uball)
		unpunish();
	/* after unpunish(), or might get deallocated chain */
	otmp2 = otmp->nexthere;
	/*
	 * obj_resists(,0,0) prevents Rider corpses from being buried.
	 * It also prevents The Amulet and invocation tools from being
	 * buried.  Since they can't be confined to bags and statues,
	 * it makes sense that they can't be buried either, even though
	 * the real reason there (direct accessibility when carried) is
	 * completely different.
	 */
	if (otmp == uchain || obj_resists(otmp, 0, 0))
		return(otmp2);

	if (otmp->otyp == LEASH && otmp->leashmon != 0)
		o_unleash(otmp);

	if (otmp->lamplit && otmp->otyp != POT_OIL)
		end_burn(otmp, TRUE);

	obj_extract_self(otmp);

	under_ice = is_ice(otmp->ox, otmp->oy);
	if (otmp->otyp == ROCK && !under_ice) {
		/* merges into burying material */
		obfree(otmp, (struct obj *)0);
		return(otmp2);
	}
	/*
	 * Start a rot on organic material.  Not corpses -- they
	 * are already handled.
	 */
	if (otmp->otyp == CORPSE) {
	    ;		/* should cancel timer if under_ice */
	} else if ((under_ice ? otmp->oclass == POTION_CLASS : is_organic(otmp))
		&& !obj_resists(otmp, 5, 95)) {
	    (void) start_timer((under_ice ? 0L : 250L) + (long)rnd(250),
			       TIMER_OBJECT, ROT_ORGANIC, (genericptr_t)otmp);
	}
	add_to_buried(otmp);
	return(otmp2);
}

void
bury_objs(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

#ifdef DEBUG
	if(level.objects[x][y] != (struct obj *)0)
		pline("bury_objs: at %d, %d", x, y);
#endif
	for (otmp = level.objects[x][y]; otmp; otmp = otmp2)
		otmp2 = bury_an_obj(otmp);

	/* don't expect any engravings here, but just in case */
	del_engr_at(x, y);
	newsym(x, y);
}

/* move objects from buriedobjlist to fobj/nexthere lists */
void
unearth_objs(x, y)
int x, y;
{
	struct obj *otmp, *otmp2;

#ifdef DEBUG
	pline("unearth_objs: at %d, %d", x, y);
#endif
	for (otmp = level.buriedobjlist; otmp; otmp = otmp2) {
		otmp2 = otmp->nobj;
		if (otmp->ox == x && otmp->oy == y) {
		    obj_extract_self(otmp);
		    if (otmp->timed)
			(void) stop_timer(ROT_ORGANIC, (genericptr_t)otmp);
		    place_object(otmp, x, y);
		    stackobj(otmp);
		}
	}
	del_engr_at(x, y);
	newsym(x, y);
}

/*
 * The organic material has rotted away while buried.  As an expansion,
 * we could add add partial damage.  A damage count is kept in the object
 * and every time we are called we increment the count and reschedule another
 * timeout.  Eventually the object rots away.
 *
 * This is used by buried objects other than corpses.  When a container rots
 * away, any contents become newly buried objects.
 */
/* ARGSUSED */
void
rot_organic(arg, timeout)
genericptr_t arg;
long timeout;	/* unused */
{
	struct obj *obj = (struct obj *) arg;

	while (Has_contents(obj)) {
	    /* We don't need to place contained object on the floor
	       first, but we do need to update its map coordinates. */
	    obj->cobj->ox = obj->ox,  obj->cobj->oy = obj->oy;
	    /* Everything which can be held in a container can also be
	       buried, so bury_an_obj's use of obj_extract_self insures
	       that Has_contents(obj) will eventually become false. */
	    (void)bury_an_obj(obj->cobj);
	}
	obj_extract_self(obj);
	obfree(obj, (struct obj *) 0);
}

/*
 * Called when a corpse has rotted completely away.
 */
void
rot_corpse(arg, timeout)
genericptr_t arg;
long timeout;	/* unused */
{
	xchar x = 0, y = 0;
	struct obj *obj = (struct obj *) arg;
	boolean on_floor = obj->where == OBJ_FLOOR,
		in_invent = obj->where == OBJ_INVENT;

	if (on_floor) {
	    x = obj->ox;
	    y = obj->oy;
	} else if (in_invent) {
	    if (flags.verbose) {
		char *cname = corpse_xname(obj, FALSE);
#if 0 /*JP:T*/
		Your("%s%s %s away%c",
		     obj == uwep ? "wielded " : nul, cname,
		     otense(obj, "rot"), obj == uwep ? '!' : '.');
#else
		pline("あなたの%s%sは腐ってしまった%s",
		     obj == uwep ? "手にしている" : "", cname,
		     obj == uwep ? "！" : "。");
#endif
	    }
	    if (obj == uwep) {
		uwepgone();	/* now bare handed */
		stop_occupation();
	    } else if (obj == uswapwep) {
		uswapwepgone();
		stop_occupation();
	    } else if (obj == uquiver) {
		uqwepgone();
		stop_occupation();
	    }
	} else if (obj->where == OBJ_MINVENT && obj->owornmask) {
	    if (obj == MON_WEP(obj->ocarry)) {
		setmnotwielded(obj->ocarry,obj);
		MON_NOWEP(obj->ocarry);
	    }
	}
	rot_organic(arg, timeout);
	if (on_floor) newsym(x, y);
	else if (in_invent) update_inventory();
}

#if 0
void
bury_monst(mtmp)
struct monst *mtmp;
{
#ifdef DEBUG
	pline("bury_monst: %s", mon_nam(mtmp));
#endif
	if(canseemon(mtmp)) {
	    if(is_flyer(mtmp->data) || is_floater(mtmp->data)) {
		pline_The("%s opens up, but %s is not swallowed!",
			surface(mtmp->mx, mtmp->my), mon_nam(mtmp));
		return;
	    } else
	        pline_The("%s opens up and swallows %s!",
			surface(mtmp->mx, mtmp->my), mon_nam(mtmp));
	}

	mtmp->mburied = TRUE;
	wakeup(mtmp);			/* at least give it a chance :-) */
	newsym(mtmp->mx, mtmp->my);
}

void
bury_you()
{
#ifdef DEBUG
	pline("bury_you");
#endif
    if (!Levitation && !Flying) {
	if(u.uswallow)
/*JP
	    You_feel("a sensation like falling into a trap!");
*/
	    You("罠に落ちるような感覚におそわれた！");
	else
/*JP
	    pline_The("%s opens beneath you and you fall in!",
*/
	    pline("%sが下方に開き、あなたは落ちた！",
		  surface(u.ux, u.uy));

	u.uburied = TRUE;
	if(!Strangled && !Breathless) Strangled = 6;
	under_ground(1);
    }
}

void
unearth_you()
{
#ifdef DEBUG
	pline("unearth_you");
#endif
	u.uburied = FALSE;
	under_ground(0);
	if(!uamul || uamul->otyp != AMULET_OF_STRANGULATION)
		Strangled = 0;
	vision_recalc(0);
}

void
escape_tomb()
{
#ifdef DEBUG
	pline("escape_tomb");
#endif
	if ((Teleportation || can_teleport(youmonst.data)) &&
	    (Teleport_control || rn2(3) < Luck+2)) {
/*JP
		You("attempt a teleport spell.");
*/
		You("瞬間移動の呪文を試みた。");
		(void) dotele();	/* calls unearth_you() */
	} else if(u.uburied) { /* still buried after 'port attempt */
		boolean good;

		if(amorphous(youmonst.data) || Passes_walls ||
		   noncorporeal(youmonst.data) || unsolid(youmonst.data) ||
		   (tunnels(youmonst.data) && !needspick(youmonst.data))) {

#if 0 /*JP:T*/
		    You("%s up through the %s.",
			(tunnels(youmonst.data) && !needspick(youmonst.data)) ?
			 "try to tunnel" : (amorphous(youmonst.data)) ?
			 "ooze" : "phase", surface(u.ux, u.uy));
#else
		    You("床に%sろうとした。",
			(tunnels(youmonst.data) && !needspick(youmonst.data)) ?
			 "トンネルを掘" : (amorphous(youmonst.data)) ?
			 "にじみのぼ" : "徐々にはいあが");
#endif

		    if(tunnels(youmonst.data) && !needspick(youmonst.data))
			good = dighole(TRUE);
		    else good = TRUE;
		    if(good) unearth_you();
		}
	}
}

void
bury_obj(otmp)
struct obj *otmp;
{

#ifdef DEBUG
	pline("bury_obj");
#endif
	if(cansee(otmp->ox, otmp->oy))
/*JP
	   pline_The("objects on the %s tumble into a hole!",
*/
	   pline_The("%sの物体は穴に転がった！",
		surface(otmp->ox, otmp->oy));

	bury_objs(otmp->ox, otmp->oy);
}
#endif

#ifdef DEBUG
int
wiz_debug_cmd() /* in this case, bury everything at your loc and around */
{
	int x, y;

	for (x = u.ux - 1; x <= u.ux + 1; x++)
	    for (y = u.uy - 1; y <= u.uy + 1; y++)
		if (isok(x,y)) bury_objs(x,y);
	return 0;
}

#endif /* DEBUG */
#endif /* OVL3 */

/*dig.c*/
