/*	SCCS Id: @(#)trap.c	3.4	2003/10/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2009
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"

extern const char * const destroy_strings[];	/* from zap.c */

STATIC_DCL void FDECL(dofiretrap, (struct obj *));
STATIC_DCL void NDECL(domagictrap);
STATIC_DCL boolean FDECL(emergency_disrobe,(boolean *));
STATIC_DCL int FDECL(untrap_prob, (struct trap *ttmp));
STATIC_DCL void FDECL(cnv_trap_obj, (int, int, struct trap *));
STATIC_DCL void FDECL(move_into_trap, (struct trap *));
STATIC_DCL int FDECL(try_disarm, (struct trap *,BOOLEAN_P));
STATIC_DCL void FDECL(reward_untrap, (struct trap *, struct monst *));
STATIC_DCL int FDECL(disarm_holdingtrap, (struct trap *));
STATIC_DCL int FDECL(disarm_landmine, (struct trap *));
STATIC_DCL int FDECL(disarm_squeaky_board, (struct trap *));
STATIC_DCL int FDECL(disarm_shooting_trap, (struct trap *, int));
STATIC_DCL int FDECL(try_lift, (struct monst *, struct trap *, int, BOOLEAN_P));
STATIC_DCL int FDECL(help_monster_out, (struct monst *, struct trap *));
STATIC_DCL boolean FDECL(thitm, (int,struct monst *,struct obj *,int,BOOLEAN_P));
STATIC_DCL int FDECL(mkroll_launch,
			(struct trap *,XCHAR_P,XCHAR_P,SHORT_P,long));
STATIC_DCL boolean FDECL(isclearpath,(coord *, int, SCHAR_P, SCHAR_P));
#ifdef STEED
STATIC_OVL int FDECL(steedintrap, (struct trap *, struct obj *));
STATIC_OVL boolean FDECL(keep_saddle_with_steedcorpse,
			(unsigned, struct obj *, struct obj *));
#endif

#ifndef OVLB
STATIC_VAR const char *a_your[2];
STATIC_VAR const char *A_Your[2];
STATIC_VAR const char tower_of_flame[];
STATIC_VAR const char *A_gush_of_water_hits;
STATIC_VAR const char * const blindgas[6];
#if 1 /*JP*/
STATIC_VAR const char *dig_you[2];
STATIC_VAR const char *web_you[2];
STATIC_VAR const char * const blindgas[6];
#endif

#else

#if 0 /*JP*/
STATIC_VAR const char * const a_your[2] = { "a", "your" };
STATIC_VAR const char * const A_Your[2] = { "A", "Your" };
STATIC_VAR const char tower_of_flame[] = "tower of flame";
STATIC_VAR const char * const A_gush_of_water_hits = "A gush of water hits";
STATIC_VAR const char * const blindgas[6] = 
	{"humid", "odorless", "pungent", "chilling", "acrid", "biting"};
#else
const char *set_you[2] = { "", "あなたの仕掛けた" };
STATIC_VAR const char *dig_you[2] = { "", "あなたが掘った" };
STATIC_VAR const char *web_you[2] = { "", "あなたが張った" };
STATIC_VAR const char * const blindgas[6] = 
	{"むしむしする", "無臭の", "刺激臭のする", "冷たい", "ツンとしたにおいの", "ひりひりする"};
#endif

#endif /* OVLB */

#ifdef OVLB

/* called when you're hit by fire (dofiretrap,buzz,zapyourself,explode) */
boolean			/* returns TRUE if hit on torso */
burnarmor(victim)
struct monst *victim;
{
    struct obj *item;
    char buf[BUFSZ];
    int mat_idx;
    
    if (!victim) return 0;
#define burn_dmg(obj,descr) rust_dmg(obj, descr, 0, FALSE, victim)
    while (1) {
	switch (rn2(5)) {
	case 0:
	    item = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	    if (item) {
		mat_idx = objects[item->otyp].oc_material;
/*JP
	    	Sprintf(buf,"%s helmet", materialnm[mat_idx] );
*/
	    	Sprintf(buf,"%sの兜", materialnm[mat_idx] );
	    }
/*JP
	    if (!burn_dmg(item, item ? buf : "helmet")) continue;
*/
	    if (!burn_dmg(item, item ? buf : "兜")) continue;
	    break;
	case 1:
	    item = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	    if (item) {
		(void) burn_dmg(item, cloak_simple_name(item));
		return TRUE;
	    }
	    item = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
	    if (item) {
		(void) burn_dmg(item, xname(item));
		return TRUE;
	    }
#ifdef TOURIST
	    item = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
	    if (item)
/*JP
		(void) burn_dmg(item, "shirt");
*/
		(void) burn_dmg(item, "シャツ");
#endif
	    return TRUE;
	case 2:
	    item = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
/*JP
	    if (!burn_dmg(item, "wooden shield")) continue;
*/
	    if (!burn_dmg(item, "木の盾")) continue;
	    break;
	case 3:
	    item = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
/*JP
	    if (!burn_dmg(item, "gloves")) continue;
*/
	    if (!burn_dmg(item, "手袋")) continue;
	    break;
	case 4:
	    item = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
/*JP
	    if (!burn_dmg(item, "boots")) continue;
*/
	    if (!burn_dmg(item, "靴")) continue;
	    break;
	}
	break; /* Out of while loop */
    }
    return FALSE;
#undef burn_dmg
}

/* Generic rust-armor function.  Returns TRUE if a message was printed;
 * "print", if set, means to print a message (and thus to return TRUE) even
 * if the item could not be rusted; otherwise a message is printed and TRUE is
 * returned only for rustable items.
 */
boolean
rust_dmg(otmp, ostr, type, print, victim)
register struct obj *otmp;
register const char *ostr;
int type;
boolean print;
struct monst *victim;
{
/*JP
	static NEARDATA const char * const action[] = { "smoulder", "rust", "rot", "corrode" };
*/
	static NEARDATA const char * const action[] = { "くすぶった","錆びた","腐った","腐食した" };
/*JP
	static NEARDATA const char * const msg[] =  { "burnt", "rusted", "rotten", "corroded" };
*/
	static NEARDATA const char * const msg[] =  { "焦げた", "錆びた", "腐った", "腐食した" };
	boolean vulnerable = FALSE;
	boolean grprot = FALSE;
	boolean is_primary = TRUE;
	boolean vismon = (victim != &youmonst) && canseemon(victim);
	int erosion;

	if (!otmp) return(FALSE);
	switch(type) {
		case 0: vulnerable = is_flammable(otmp);
			break;
		case 1: vulnerable = is_rustprone(otmp);
			grprot = TRUE;
			break;
		case 2: vulnerable = is_rottable(otmp);
			is_primary = FALSE;
			break;
		case 3: vulnerable = is_corrodeable(otmp);
			grprot = TRUE;
			is_primary = FALSE;
			break;
	}
	erosion = is_primary ? otmp->oeroded : otmp->oeroded2;

	if (!print && (!vulnerable || otmp->oerodeproof || erosion == MAX_ERODE))
		return FALSE;

	if (!vulnerable) {
	    if (flags.verbose) {
		if (victim == &youmonst)
/*JP
		    Your("%s %s not affected.", ostr, vtense(ostr, "are"));
*/
		    Your("%sは影響を受けなかった。",ostr);
		else if (vismon)
#if 0 /*JP*/
		    pline("%s's %s %s not affected.", Monnam(victim), ostr,
			  vtense(ostr, "are"));
#else
		    pline("%sの%sは影響を受けなかった。", Monnam(victim), ostr);
#endif
	    }
	} else if (erosion < MAX_ERODE) {
	    if (grprot && otmp->greased) {
		grease_protect(otmp,ostr,victim);
	    } else if (otmp->oerodeproof || (otmp->blessed && !rnl(4))) {
		if (flags.verbose) {
		    if (victim == &youmonst)
#if 0 /*JP*/
			pline("Somehow, your %s %s not affected.",
			      ostr, vtense(ostr, "are"));
#else
			pline("なぜか、%sは影響を受けなかった。",ostr);
#endif
		    else if (vismon)
#if 0 /*JP*/
			pline("Somehow, %s's %s %s not affected.",
			      mon_nam(victim), ostr, vtense(ostr, "are"));
#else
			pline("なぜか、%sの%sは影響を受けなかった。",
			      mon_nam(victim), ostr);
#endif
		}
	    } else {
		if (victim == &youmonst)
#if 0 /*JP*/
		    Your("%s %s%s!", ostr,
			 vtense(ostr, action[type]),
			 erosion+1 == MAX_ERODE ? " completely" :
			    erosion ? " further" : "");
#else
		    Your("%sは%s%s！", ostr, 
			 otmp->oeroded+1 == MAX_ERODE ? "完全に" :
			 otmp->oeroded ? "さらに" : "",
			 action[type]);
#endif
		else if (vismon)
#if 0 /*JP*/
		    pline("%s's %s %s%s!", Monnam(victim), ostr,
			vtense(ostr, action[type]),
			erosion+1 == MAX_ERODE ? " completely" :
			  erosion ? " further" : "");
#else
		    pline("%sの%sは%s%s！", Monnam(victim), ostr,
			  otmp->oeroded+1 == MAX_ERODE ? "完全に" :
			  otmp->oeroded ? "さらに" : "",
			  action[type]);
#endif
		if (is_primary)
		    otmp->oeroded++;
		else
		    otmp->oeroded2++;
		update_inventory();
	    }
	} else {
	    if (flags.verbose) {
		if (victim == &youmonst)
#if 0 /*JP*/
		    Your("%s %s completely %s.", ostr,
			 vtense(ostr, Blind ? "feel" : "look"),
			 msg[type]);
#else
		    Your("%sは完全に%s%s。", ostr, msg[type],
			 Blind ? "ようだ" : "");
#endif
		else if (vismon)
#if 0 /*JP*/
		    pline("%s's %s %s completely %s.",
			  Monnam(victim), ostr,
			  vtense(ostr, "look"), msg[type]);
#else
		    pline("%sの%sは完全に%s%s。", 
			  Monnam(victim), ostr, msg[type],
			  Blind ? "ようだ" : "");
#endif
	    }
	    destroy_arm(otmp);
	}
	return(TRUE);
}

void
grease_protect(otmp,ostr,victim)
register struct obj *otmp;
register const char *ostr;
struct monst *victim;
{
/*JP
	static const char txt[] = "protected by the layer of grease!";
*/
	static const char txt[] = "油の塗りこみによって守られている！";
	boolean vismon = victim && (victim != &youmonst) && canseemon(victim);

	if (ostr) {
	    if (victim == &youmonst)
/*JP
		Your("%s %s %s", ostr, vtense(ostr, "are"), txt);
*/
		Your("%sは%s", ostr, txt);
	    else if (vismon)
#if 0 /*JP*/
		pline("%s's %s %s %s", Monnam(victim),
		    ostr, vtense(ostr, "are"), txt);
#else
		pline("%sの%sは%s", Monnam(victim), ostr, txt);
#endif
	} else {
	    if (victim == &youmonst)
/*JP
		Your("%s %s",aobjnam(otmp,"are"), txt);
*/
		Your("%sは%s",xname(otmp),txt);
	    else if (vismon)
/*JP
		pline("%s's %s %s", Monnam(victim), aobjnam(otmp,"are"), txt);
*/
		pline("%sの%sは%s",Monnam(victim), xname(otmp), txt);
	}
	if (!rn2(2)) {
	    otmp->greased = 0;
	    if (carried(otmp)) {
/*JP
		pline_The("grease dissolves.");
*/
		pline("油ははげてしまった。");
		update_inventory();
	    }
	}
}

struct trap *
maketrap(x,y,typ)
register int x, y, typ;
{
	register struct trap *ttmp;
	register struct rm *lev;
	register boolean oldplace;

	if ((ttmp = t_at(x,y)) != 0) {
	    if (ttmp->ttyp == MAGIC_PORTAL) return (struct trap *)0;
	    oldplace = TRUE;
	    if (u.utrap && (x == u.ux) && (y == u.uy) &&
	      ((u.utraptype == TT_BEARTRAP && typ != BEAR_TRAP) ||
	      (u.utraptype == TT_WEB && typ != WEB) ||
	      (u.utraptype == TT_PIT && typ != PIT && typ != SPIKED_PIT)))
		    u.utrap = 0;
	} else {
	    oldplace = FALSE;
	    ttmp = newtrap();
	    ttmp->tx = x;
	    ttmp->ty = y;
	    ttmp->launch.x = -1;	/* force error if used before set */
	    ttmp->launch.y = -1;
	}
	ttmp->ttyp = typ;
	switch(typ) {
	    case STATUE_TRAP:	    /* create a "living" statue */
	      { struct monst *mtmp;
		struct obj *otmp, *statue;

		statue = mkcorpstat(STATUE, (struct monst *)0,
					&mons[rndmonnum()], x, y, FALSE);
		mtmp = makemon(&mons[statue->corpsenm], 0, 0, NO_MM_FLAGS);
		if (!mtmp) break; /* should never happen */
		while(mtmp->minvent) {
		    otmp = mtmp->minvent;
		    otmp->owornmask = 0;
		    obj_extract_self(otmp);
		    (void) add_to_container(statue, otmp);
		}
		statue->owt = weight(statue);
		mongone(mtmp);
		break;
	      }
	    case ROLLING_BOULDER_TRAP:	/* boulder will roll towards trigger */
		(void) mkroll_launch(ttmp, x, y, BOULDER, 1L);
		break;
	    case HOLE:
	    case PIT:
	    case SPIKED_PIT:
	    case TRAPDOOR:
		lev = &levl[x][y];
		if (*in_rooms(x, y, SHOPBASE) &&
			((typ == HOLE || typ == TRAPDOOR) ||
			 IS_DOOR(lev->typ) || IS_WALL(lev->typ)))
		    add_damage(x, y,		/* schedule repair */
			       ((IS_DOOR(lev->typ) || IS_WALL(lev->typ))
				&& !flags.mon_moving) ? 200L : 0L);
		lev->doormask = 0;	/* subsumes altarmask, icedpool... */
		if (IS_ROOM(lev->typ)) /* && !IS_AIR(lev->typ) */
		    lev->typ = ROOM;

		/*
		 * some cases which can happen when digging
		 * down while phazing thru solid areas
		 */
		else if (lev->typ == STONE || lev->typ == SCORR)
		    lev->typ = CORR;
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    lev->typ = level.flags.is_maze_lev ? ROOM :
			       level.flags.is_cavernous_lev ? CORR : DOOR;

		unearth_objs(x, y);
		break;
	}
	if (ttmp->ttyp == HOLE) ttmp->tseen = 1;  /* You can't hide a hole */
	else ttmp->tseen = 0;
	ttmp->once = 0;
	ttmp->madeby_u = 0;
	ttmp->dst.dnum = -1;
	ttmp->dst.dlevel = -1;
	if (!oldplace) {
	    ttmp->ntrap = ftrap;
	    ftrap = ttmp;
	}
	return(ttmp);
}

void
fall_through(td)
boolean td;	/* td == TRUE : trap door or hole */
{
	d_level dtmp;
	char msgbuf[BUFSZ];
	const char *dont_fall = 0;
	int currentlevel = dunlev(&u.uz);
	int newlevel = currentlevel;

	/* KMH -- You can't escape the Sokoban level traps */
	if(Blind && Levitation && !In_sokoban(&u.uz)) return;

	do {
	    newlevel++;
	} while(!rn2(4) && newlevel < dunlevs_in_dungeon(&u.uz));

	if(td) {
	    struct trap *t=t_at(u.ux,u.uy);
	    seetrap(t);
	    if (!In_sokoban(&u.uz)) {
		if (t->ttyp == TRAPDOOR)
/*JP
			pline("A trap door opens up under you!");
*/
			pline("落し扉があなたの足元に開いた！");
		else 
/*JP
			pline("There's a gaping hole under you!");
*/
			pline("あなたの足下にぽっかりと穴が開いている！");
	    }
/*JP
	} else pline_The("%s opens up under you!", surface(u.ux,u.uy));
*/
	} else pline("足元の%sに穴が開いた！", surface(u.ux,u.uy));

	if (In_sokoban(&u.uz) && Can_fall_thru(&u.uz))
	    ;	/* KMH -- You can't escape the Sokoban level traps */
	else if(Levitation || u.ustuck || !Can_fall_thru(&u.uz)
	   || Flying || is_clinger(youmonst.data)
	   || (Inhell && !u.uevent.invoked &&
					newlevel == dunlevs_in_dungeon(&u.uz))
		) {
/*JP
	    dont_fall = "don't fall in.";
*/
	    dont_fall = "しかしあなたは落ちなかった。";
	} else if (youmonst.data->msize >= MZ_HUGE) {
/*JP
	    dont_fall = "don't fit through.";
*/
	    dont_fall = "通り抜けるにはサイズが合わない。";
	} else if (!next_to_u()) {
/*JP
	    dont_fall = "are jerked back by your pet!";
*/
	    dont_fall = "あなたはペットによって引っぱりもどされた！";
	}
	if (dont_fall) {
/*JP
	    You(dont_fall);
*/
	    pline(dont_fall);
	    /* hero didn't fall through, but any objects here might */
	    impact_drop((struct obj *)0, u.ux, u.uy, 0);
	    if (!td) {
		display_nhwindow(WIN_MESSAGE, FALSE);
/*JP
		pline_The("opening under you closes up.");
*/
		pline_The("足下に開いていたものは閉じた。");
	    }
	    return;
	}

	if(*u.ushops) shopdig(1);
	if (Is_stronghold(&u.uz)) {
	    find_hell(&dtmp);
	} else {
		static const char * const falling_down_msgs[] = {
/*JP
			"fall down a shaft!",
*/
			"縦穴に落ちた！",
/*JP
			"fall down a deep shaft!",
*/
			"深い縦穴に落ちた！",
/*JP
			"keep falling down a really deep shaft!",
*/
			"実に深い縦穴を落ち続けた！",
/*JP
			"keep falling down an unbelievable deep shaft!",
*/
			"信じられないほど深い縦穴を落ち続けた！",
		};
		/* TODO: Hallucination messages */

	    dtmp.dnum = u.uz.dnum;
	    dtmp.dlevel = newlevel;
	    switch (newlevel-currentlevel) {
	    case 1:
		    /* no message when falling to the next level */
		    break;
	    case 2:
	    case 3:
	    case 4:
	    case 5:
		    You(falling_down_msgs[newlevel-currentlevel-2]);
		    break;
	    default:
/*JP
		    You("are falling down an unbelievable deep shaft!");
*/
		    You("信じられないほど深い縦穴を落ちている！");
#if 0 /*JP*/
		    pline("While falling you wonder how unlikely it is to find such a deep shaft."); /* (1/4)^5 ~= 0.1% */
#else
		    pline("落ち続けている間、あなたはここまで深い縦穴は"
		        "そうそう見つかるものではないと思った。"); /* (1/4)^5 ~= 0.1% */
#endif
		    break;
	    }
	}
	if (!td)
/*JP
	    Sprintf(msgbuf, "The hole in the %s above you closes up.",
*/
	    Sprintf(msgbuf, "%sに開いた穴は閉じた。",
		    ceiling(u.ux,u.uy));
	schedule_goto(&dtmp, FALSE, TRUE, 0,
		      (char *)0, !td ? msgbuf : (char *)0);
}

/*
 * Animate the given statue.  May have been via shatter attempt, trap,
 * or stone to flesh spell.  Return a monster if successfully animated.
 * If the monster is animated, the object is deleted.  If fail_reason
 * is non-null, then fill in the reason for failure (or success).
 *
 * The cause of animation is:
 *
 *	ANIMATE_NORMAL  - hero "finds" the monster
 *	ANIMATE_SHATTER - hero tries to destroy the statue
 *	ANIMATE_SPELL   - stone to flesh spell hits the statue
 *
 * Perhaps x, y is not needed if we can use get_obj_location() to find
 * the statue's location... ???
 */
struct monst *
animate_statue(statue, x, y, cause, fail_reason)
struct obj *statue;
xchar x, y;
int cause;
int *fail_reason;
{
	struct permonst *mptr;
	struct monst *mon = 0;
	struct obj *item;
	coord cc;
	boolean historic = (Role_if(PM_ARCHEOLOGIST) && !flags.mon_moving && (statue->spe & STATUE_HISTORIC));
	char statuename[BUFSZ];

	Strcpy(statuename,the(xname(statue)));

	if (statue->oxlth && statue->oattached == OATTACHED_MONST) {
	    cc.x = x,  cc.y = y;
	    mon = montraits(statue, &cc);
	    if (mon && mon->mtame && !mon->isminion)
		wary_dog(mon, TRUE);
	} else {
	    /* statue of any golem hit with stone-to-flesh becomes flesh golem */
	    if (is_golem(&mons[statue->corpsenm]) && cause == ANIMATE_SPELL)
	    	mptr = &mons[PM_FLESH_GOLEM];
	    else
		mptr = &mons[statue->corpsenm];
	    /*
	     * Guard against someone wishing for a statue of a unique monster
	     * (which is allowed in normal play) and then tossing it onto the
	     * [detected or guessed] location of a statue trap.  Normally the
	     * uppermost statue is the one which would be activated.
	     */
	    if ((mptr->geno & G_UNIQ) && cause != ANIMATE_SPELL) {
	        if (fail_reason) *fail_reason = AS_MON_IS_UNIQUE;
	        return (struct monst *)0;
	    }
	    if (cause == ANIMATE_SPELL &&
		((mptr->geno & G_UNIQ) || mptr->msound == MS_GUARDIAN)) {
		/* Statues of quest guardians or unique monsters
		 * will not stone-to-flesh as the real thing.
		 */
		mon = makemon(&mons[PM_DOPPELGANGER], x, y,
			NO_MINVENT|MM_NOCOUNTBIRTH|MM_ADJACENTOK);
		if (mon) {
			/* makemon() will set mon->cham to
			 * CHAM_ORDINARY if hero is wearing
			 * ring of protection from shape changers
			 * when makemon() is called, so we have to
			 * check the field before calling newcham().
			 */
			if (mon->cham == CHAM_DOPPELGANGER)
				(void) newcham(mon, mptr, FALSE, FALSE);
		}
	    } else
		mon = makemon(mptr, x, y, (cause == ANIMATE_SPELL) ?
			(NO_MINVENT | MM_ADJACENTOK) : NO_MINVENT);
	}

	if (!mon) {
	    if (fail_reason) *fail_reason = AS_NO_MON;
	    return (struct monst *)0;
	}

	/* in case statue is wielded and hero zaps stone-to-flesh at self */
	if (statue->owornmask) remove_worn_item(statue, TRUE);

	/* allow statues to be of a specific gender */
	if (statue->spe & STATUE_MALE)
	    mon->female = FALSE;
	else if (statue->spe & STATUE_FEMALE)
	    mon->female = TRUE;
	/* if statue has been named, give same name to the monster */
	if (statue->onamelth)
	    mon = christen_monst(mon, ONAME(statue));
	/* transfer any statue contents to monster's inventory */
	while ((item = statue->cobj) != 0) {
	    obj_extract_self(item);
	    (void) add_to_minv(mon, item);
	}
	m_dowear(mon, TRUE);
	delobj(statue);

	/* mimic statue becomes seen mimic; other hiders won't be hidden */
	if (mon->m_ap_type) seemimic(mon);
	else mon->mundetected = FALSE;
	if ((x == u.ux && y == u.uy) || cause == ANIMATE_SPELL) {
#if 0 /*JP*/
	    const char *comes_to_life = nonliving(mon->data) ?
					"moves" : "comes to life"; 
#else
	    const char *comes_to_life = nonliving(mon->data) ?
					"動い" : "生命を帯び";
#endif
	    if (cause == ANIMATE_SPELL)
#if 0 /*JP*/
	    	pline("%s %s!", upstart(statuename),
	    		canspotmon(mon) ? comes_to_life : "disappears");
#else
	    	pline("%sは%sた！", upstart(statuename),
	    		canspotmon(mon) ? comes_to_life : "消え");
#endif
	    else
#if 0 /*JP*/
		pline_The("statue %s!",
			canspotmon(mon) ? comes_to_life : "disappears");
#else
		pline("彫像は%sた！",
			canspotmon(mon) ? comes_to_life : "消え");
#endif
	    if (historic) {
/*JP
		    You_feel("guilty that the historic statue is now gone.");
*/
		    You("歴史的な彫像がなくなってしまったことに罪を感じた。");
		    adjalign(-1);
	    }
	} else if (cause == ANIMATE_SHATTER)
#if 0 /*JP*/
	    pline("Instead of shattering, the statue suddenly %s!",
		canspotmon(mon) ? "comes to life" : "disappears");
#else
	    pline("砕けるかわりに、彫像は%sた！",
		canspotmon(mon) ? "生命を帯び" : "消え");
#endif
	else { /* cause == ANIMATE_NORMAL */
#if 0 /*JP*/
	    You("find %s posing as a statue.",
		canspotmon(mon) ? a_monnam(mon) : something);
#else
	    You("%sが彫像のふりをしているのを見つけた。",
		canspotmon(mon) ? a_monnam(mon) : something);
#endif
	    stop_occupation();
	}
	/* avoid hiding under nothing */
	if (x == u.ux && y == u.uy &&
		Upolyd && hides_under(youmonst.data) && !OBJ_AT(x, y))
	    u.uundetected = 0;

	if (fail_reason) *fail_reason = AS_OK;
	return mon;
}

/*
 * You've either stepped onto a statue trap's location or you've triggered a
 * statue trap by searching next to it or by trying to break it with a wand
 * or pick-axe.
 */
struct monst *
activate_statue_trap(trap, x, y, shatter)
struct trap *trap;
xchar x, y;
boolean shatter;
{
	struct monst *mtmp = (struct monst *)0;
	struct obj *otmp = sobj_at(STATUE, x, y);
	int fail_reason;

	/*
	 * Try to animate the first valid statue.  Stop the loop when we
	 * actually create something or the failure cause is not because
	 * the mon was unique.
	 */
	deltrap(trap);
	while (otmp) {
	    mtmp = animate_statue(otmp, x, y,
		    shatter ? ANIMATE_SHATTER : ANIMATE_NORMAL, &fail_reason);
	    if (mtmp || fail_reason != AS_MON_IS_UNIQUE) break;

	    while ((otmp = otmp->nexthere) != 0)
		if (otmp->otyp == STATUE) break;
	}

	if (Blind) feel_location(x, y);
	else newsym(x, y);
	return mtmp;
}

#ifdef STEED
STATIC_OVL boolean
keep_saddle_with_steedcorpse(steed_mid, objchn, saddle)
unsigned steed_mid;
struct obj *objchn, *saddle;
{
	if (!saddle) return FALSE;
	while(objchn) {
		if(objchn->otyp == CORPSE &&
		   objchn->oattached == OATTACHED_MONST && objchn->oxlth) {
			struct monst *mtmp = (struct monst *)objchn->oextra;
			if (mtmp->m_id == steed_mid) {
				/* move saddle */
				xchar x,y;
				if (get_obj_location(objchn, &x, &y, 0)) {
					obj_extract_self(saddle);
					place_object(saddle, x, y);
					stackobj(saddle);
				}
				return TRUE;
			}
		}
		if (Has_contents(objchn) &&
		    keep_saddle_with_steedcorpse(steed_mid, objchn->cobj, saddle))
			return TRUE;
		objchn = objchn->nobj;
	}
	return FALSE;
}
#endif /*STEED*/

void
dotrap(trap, trflags)
register struct trap *trap;
unsigned trflags;
{
	register int ttype = trap->ttyp;
	register struct obj *otmp;
	boolean already_seen = trap->tseen;
	boolean webmsgok = (!(trflags & NOWEBMSG));
	boolean forcebungle = (trflags & FORCEBUNGLE);

	nomul(0, 0);

	/* KMH -- You can't escape the Sokoban level traps */
	if (In_sokoban(&u.uz) &&
			(ttype == PIT || ttype == SPIKED_PIT || ttype == HOLE ||
			ttype == TRAPDOOR)) {
	    /* The "air currents" message is still appropriate -- even when
	     * the hero isn't flying or levitating -- because it conveys the
	     * reason why the player cannot escape the trap with a dexterity
	     * check, clinging to the ceiling, etc.
	     */
#if 0 /*JP:T*/
	    pline("Air currents pull you down into %s %s!",
	    	a_your[trap->madeby_u],
	    	defsyms[trap_to_defsym(ttype)].explanation);
#else
	    pline("空気の流れがあなたを%sに引き戻した！",
	    	jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation));
#endif
	    /* then proceed to normal trap effect */
	} else if (already_seen) {
	    if ((Levitation || Flying) &&
		    (ttype == PIT || ttype == SPIKED_PIT || ttype == HOLE ||
		    ttype == BEAR_TRAP)) {
#if 0 /*JP:T*/
		You("%s over %s %s.",
		    Levitation ? "float" : "fly",
		    a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
#else
		You("%s%sの上%s。",
		    set_you[trap->madeby_u],
		    jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation),
		    Levitation ? "を見下ろした" : "を飛んでいる");
#endif
		return;
	    }
	    if(!Fumbling && ttype != MAGIC_PORTAL &&
		ttype != ANTI_MAGIC && !forcebungle &&
		(!rn2(5) ||
	    ((ttype == PIT || ttype == SPIKED_PIT) && is_clinger(youmonst.data)))) {
#if 0 /*JP:T*/
		You("escape %s %s.",
		    (ttype == ARROW_TRAP && !trap->madeby_u) ? "an" :
			a_your[trap->madeby_u],
		    defsyms[trap_to_defsym(ttype)].explanation);
#else
		You("%s%sをするりと避けた。",
		    set_you[trap->madeby_u],
		    jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation));
#endif
		return;
	    }
	}

#ifdef STEED
	if (u.usteed) u.usteed->mtrapseen |= (1 << (ttype-1));
#endif

	switch(ttype) {
	    case ARROW_TRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
/*JP
		    You_hear("a loud click!");
*/
		    You_hear("ガチャッという大きな音を聞いた！");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
/*JP
		pline("An arrow shoots out at you!");
*/
		pline("矢が飛んできた！");
		otmp = mksobj(ARROW, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		otmp->opoisoned = 0;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
/*JP
		if (thitu(8, dmgval(otmp, &youmonst), otmp, "arrow")) {
*/
		if (thitu(8, dmgval(otmp, &youmonst), otmp, "矢")) {
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
		}
		break;
	    case DART_TRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
/*JP
		    You_hear("a soft click.");
*/
		    You_hear("プチッという音を聞いた。");
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		    break;
		}
		trap->once = 1;
		seetrap(trap);
/*JP
		pline("A little dart shoots out at you!");
*/
		pline("小さな投げ矢が飛んできた！");
		otmp = mksobj(DART, TRUE, FALSE);
		otmp->quan = 1L;
		otmp->owt = weight(otmp);
		if (!rn2(6)) otmp->opoisoned = 1;
#ifdef STEED
		if (u.usteed && !rn2(2) && steedintrap(trap, otmp)) /* nothing */;
		else
#endif
/*JP
		if (thitu(7, dmgval(otmp, &youmonst), otmp, "little dart")) {
*/
		if (thitu(7, dmgval(otmp, &youmonst), otmp, "投げ矢")) {
		    if (otmp->opoisoned)
/*JP
			poisoned("dart", A_CON, "little dart", -10);
*/
			poisoned("投げ矢", A_CON, "小さな投げ矢", -10);
		    obfree(otmp, (struct obj *)0);
		} else {
		    place_object(otmp, u.ux, u.uy);
		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux, u.uy);
		}
		break;
	    case ROCKTRAP:
		if (trap->once && trap->tseen && !rn2(15)) {
#if 0 /*JP*/
		    pline("A trap door in %s opens, but nothing falls out!",
			  the(ceiling(u.ux,u.uy)));
#else
		    pline("落し扉が%sに開いたが、何も落ちてこなかった！",
			  ceiling(u.ux,u.uy));
#endif
		    deltrap(trap);
		    newsym(u.ux,u.uy);
		} else {
		    int dmg = d(2,6); /* should be std ROCK dmg? */

		    trap->once = 1;
		    seetrap(trap);
		    otmp = mksobj_at(ROCK, u.ux, u.uy, TRUE, FALSE);
		    otmp->quan = 1L;
		    otmp->owt = weight(otmp);

/*JP
		    pline("A trap door in %s opens and %s falls on your %s!",
*/
		    pline("落し扉が%sに開き、%sがあなたの%sに落ちてきた！",
			  the(ceiling(u.ux,u.uy)),
			  an(xname(otmp)),
			  body_part(HEAD));

		    if (uarmh) {
			if(is_metallic(uarmh)) {
/*JP
			    pline("Fortunately, you are wearing a hard helmet.");
*/
			    pline("幸運にも、あなたは固い兜を身につけていた。");
			    dmg = 2;
			} else if (flags.verbose) {
/*JP
			    Your("%s does not protect you.", xname(uarmh));
*/
			    Your("%sでは防げない。", xname(uarmh));
			}
		    }

		    if (!Blind) otmp->dknown = 1;
		    stackobj(otmp);
		    newsym(u.ux,u.uy);	/* map the rock */

/*JP
		    losehp(dmg, "falling rock", KILLED_BY_AN);
*/
		    losehp(dmg, "落岩で", KILLED_BY_AN);
		    exercise(A_STR, FALSE);
		}
		break;

	    case SQKY_BOARD:	    /* stepped on a squeaky board */
		if (Levitation || Flying) {
		    if (!Blind) {
			seetrap(trap);
			if (Hallucination)
/*JP
				You("notice a crease in the linoleum.");
*/
				You("床の仕上材のしわに気がついた。");
			else
/*JP
				You("notice a loose board below you.");
*/
				You("足元の緩んだ板に気がついた。");
		    }
		} else {
		    seetrap(trap);
/*JP
		    pline("A board beneath you squeaks loudly.");
*/
		    pline("足元の板が大きくきしんだ。");
		    wake_nearby();
		}
		break;

	    case BEAR_TRAP:
		if(Levitation || Flying) break;
		seetrap(trap);
		if(amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
#if 0 /*JP*/
		    pline("%s bear trap closes harmlessly through you.",
			    A_Your[trap->madeby_u]);
#else
		    pline("%s熊の罠は噛みついたが、するっと通り抜けた。",
			    set_you[trap->madeby_u]);
#endif
		    break;
		}
		if(
#ifdef STEED
		   !u.usteed &&
#endif
		   youmonst.data->msize <= MZ_SMALL) {
#if 0 /*JP*/
		    pline("%s bear trap closes harmlessly over you.",
			    A_Your[trap->madeby_u]);
#else
		    pline("%s熊の罠は噛みついたが、遥か上方だった。",
			    set_you[trap->madeby_u]);
#endif
		    break;
		}
		u.utrap = rn1(4, 4);
		u.utraptype = TT_BEARTRAP;
#ifdef STEED
		if (u.usteed) {
#if 0 /*JP*/
		    pline("%s bear trap closes on %s %s!",
			A_Your[trap->madeby_u], s_suffix(mon_nam(u.usteed)),
			mbodypart(u.usteed, FOOT));
#else
		    pline("%s熊の罠は%sの%sに噛みついた！",
			set_you[trap->madeby_u], mon_nam(u.usteed),
			mbodypart(u.usteed, FOOT));
#endif
		} else
#endif
		{
#if 0 /*JP*/
		    pline("%s bear trap closes on your %s!",
			    A_Your[trap->madeby_u], body_part(FOOT));
#else
		    pline("%s熊の罠があなたの%sに噛みついた！",
			  set_you[trap->madeby_u], body_part(FOOT));
#endif
		    if(u.umonnum == PM_OWLBEAR || u.umonnum == PM_BUGBEAR)
/*JP
			You("howl in anger!");
*/
			You("怒りの咆哮をあげた！");
		}
		exercise(A_DEX, FALSE);
		break;

	    case SLP_GAS_TRAP:
		seetrap(trap);
		if(Sleep_resistance || breathless(youmonst.data)) {
/*JP
		    You("are enveloped in a cloud of gas!");
*/
		    You("ガス雲につつまれた！");
		    break;
		}
/*JP
		pline("A cloud of gas puts you to sleep!");
*/
		pline("あなたはガス雲で眠ってしまった！");
		fall_asleep(-rnd(25), TRUE);
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case RUST_TRAP:
		seetrap(trap);
		if (u.umonnum == PM_IRON_GOLEM) {
		    int dam = u.mhmax;

/*JP
		    pline("%s you!", A_gush_of_water_hits);
*/
		    pline("水が噴出してあなたに命中した！");
/*JP
		    You("are covered with rust!");
*/
		    You("錆に覆われた！");
		    if (Half_physical_damage) dam = (dam+1) / 2;
/*JP
		    losehp(dam, "rusting away", KILLED_BY);
*/
		    losehp(dam, "完全に錆びて" , KILLED_BY);
		    break;
		} else if (u.umonnum == PM_GREMLIN && rn2(3)) {
/*JP
		    pline("%s you!", A_gush_of_water_hits);
*/
		    pline("水が噴出してあなたに命中した！");
		    (void)split_mon(&youmonst, (struct monst *)0);
		    break;
		}

	    /* Unlike monsters, traps cannot aim their rust attacks at
	     * you, so instead of looping through and taking either the
	     * first rustable one or the body, we take whatever we get,
	     * even if it is not rustable.
	     */
		switch (rn2(5)) {
		    case 0:
/*JP
			pline("%s you on the %s!", A_gush_of_water_hits,
*/
			pline("水が噴出してあなたの%sに命中した！",
				    body_part(HEAD));
/*JP
			(void) rust_dmg(uarmh, "helmet", 1, TRUE, &youmonst);
*/
			(void) rust_dmg(uarmh, "兜", 1, TRUE, &youmonst);
			break;
		    case 1:
/*JP
			pline("%s your left %s!", A_gush_of_water_hits,
*/
			pline("水が噴出してあなたの左%sに命中した！",
				    body_part(ARM));
/*JP
			if (rust_dmg(uarms, "shield", 1, TRUE, &youmonst))
*/
			if (rust_dmg(uarms, "盾", 1, TRUE, &youmonst))
			    break;
			if (u.twoweap || (uwep && bimanual(uwep)))
			    erode_obj(u.twoweap ? uswapwep : uwep, FALSE, TRUE);
/*JP
glovecheck:		(void) rust_dmg(uarmg, "gauntlets", 1, TRUE, &youmonst);
*/
glovecheck:		(void) rust_dmg(uarmg, "小手", 1, TRUE, &youmonst);
			/* Not "metal gauntlets" since it gets called
			 * even if it's leather for the message
			 */
			break;
		    case 2:
/*JP
			pline("%s your right %s!", A_gush_of_water_hits,
*/
			pline("水が噴出してあなたの右%sに命中した！",
				    body_part(ARM));
			erode_obj(uwep, FALSE, TRUE);
			goto glovecheck;
		    default:
/*JP
			pline("%s you!", A_gush_of_water_hits);
*/
			pline("水が噴出してあなたに命中した！");
			for (otmp=invent; otmp; otmp = otmp->nobj)
				    (void) snuff_lit(otmp);
			if (uarmc)
			    (void) rust_dmg(uarmc, cloak_simple_name(uarmc),
						1, TRUE, &youmonst);
			else if (uarm)
/*JP
			    (void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst);
*/
			    (void) rust_dmg(uarm, "鎧", 1, TRUE, &youmonst);
#ifdef TOURIST
			else if (uarmu)
/*JP
			    (void) rust_dmg(uarmu, "shirt", 1, TRUE, &youmonst);
*/
			    (void) rust_dmg(uarmu, "シャツ", 1, TRUE, &youmonst);
#endif
		}
		update_inventory();
		break;

	    case FIRE_TRAP:
		seetrap(trap);
		dofiretrap((struct obj *)0);
		break;

	    case PIT:
	    case SPIKED_PIT:
		/* KMH -- You can't escape the Sokoban level traps */
		if (!In_sokoban(&u.uz) && (Levitation || Flying)) break;
		seetrap(trap);
		if (!In_sokoban(&u.uz) && is_clinger(youmonst.data)) {
		    if(trap->tseen) {
#if 0 /*JP:T*/
			You("see %s %spit below you.", a_your[trap->madeby_u],
			    ttype == SPIKED_PIT ? "spiked " : "");
#else
			pline("足元に%s%s落し穴を発見した。",
			    dig_you[trap->madeby_u],
			    ttype == SPIKED_PIT ? "トゲだらけの" : "");
#endif
		    } else {
#if 0 /*JP:T*/
			pline("%s pit %sopens up under you!",
			    A_Your[trap->madeby_u],
			    ttype == SPIKED_PIT ? "full of spikes " : "");
#else
			pline("%s%s落し穴が足元に開いた！",
			      dig_you[trap->madeby_u],
			      ttype == SPIKED_PIT ? "トゲだらけの" : "");
#endif
/*JP
			You("don't fall in!");
*/
			pline("しかし、あなたは落ちなかった！");
		    }
		    break;
		}
		if (!In_sokoban(&u.uz)) {
		    char verbbuf[BUFSZ];
#ifdef STEED
		    if (u.usteed) {
		    	if ((trflags & RECURSIVETRAP) != 0)
/*JP
			    Sprintf(verbbuf, "and %s fall",
*/
			    Sprintf(verbbuf, "と%s",
				x_monnam(u.usteed,
				    u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				    (char *)0, SUPPRESS_SADDLE, FALSE));
			else
#if 0 /*JP*/
			    Sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
#else
			    Sprintf(verbbuf,"と%s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "かわいそうな", SUPPRESS_SADDLE, FALSE));
#endif
		    } else
#endif
/*JP
		    Strcpy(verbbuf,"fall");
*/
		    Strcpy(verbbuf,"");
#if 0 /*JP*/
		    You("%s into %s pit!", verbbuf, a_your[trap->madeby_u]);
#else
		    pline("あなた%sは%s落し穴に落ちた！",
			verbbuf, dig_you[trap->madeby_u]);
#endif
		}
		/* wumpus reference */
		if (Role_if(PM_RANGER) && !trap->madeby_u && !trap->once &&
			In_quest(&u.uz) && Is_qlocate(&u.uz)) {
/*JP
		    pline("Fortunately it has a bottom after all...");
*/
		    pline("幸い、結局は底があった．．．");
		    trap->once = 1;
		} else if (u.umonnum == PM_PIT_VIPER ||
			u.umonnum == PM_PIT_FIEND)
/*JP
		    pline("How pitiful.  Isn't that the pits?");
*/
		    pline("この落し穴はいい仕事をしている。");
		if (ttype == SPIKED_PIT) {
/*JP
		    const char *predicament = "on a set of sharp iron spikes";
*/
		    const char *predicament = "鋭い鉄のトゲトゲの上に落ちた";
#ifdef STEED
		    if (u.usteed) {
#if 0 /*JP:T*/
			pline("%s lands %s!",
				upstart(x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
					 "poor", SUPPRESS_SADDLE, FALSE)),
			      predicament);
#else
			pline("%sは%s！",
				upstart(x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
					 "かわいそうな", SUPPRESS_SADDLE, FALSE)),
			      predicament);
#endif
		    } else
#endif
/*JP
		    You("land %s!", predicament);
*/
		    You("%s！", predicament);
		}
		if (!Passes_walls)
		    u.utrap = rn1(6,2);
		u.utraptype = TT_PIT;
#ifdef STEED
		if (!steedintrap(trap, (struct obj *)0)) {
#endif
		if (ttype == SPIKED_PIT) {
#if 0 /*JP:T*/
		    losehp(rnd(10),"fell into a pit of iron spikes",
			NO_KILLER_PREFIX);
#else
		    losehp(rnd(10),"トゲだらけの落し穴に落ちて",
			KILLED_BY);
#endif
		    if (!rn2(6))
/*JP
			poisoned("spikes", A_STR, "fall onto poison spikes", 8);
*/
			poisoned("トゲ", A_STR, "落し穴のトゲ", 8);
		} else
/*JP
		    losehp(rnd(6),"fell into a pit", NO_KILLER_PREFIX);
*/
		    losehp(rnd(6),"落し穴に落ちて", KILLED_BY);
		if (Punished && !carried(uball)) {
		    unplacebc();
		    ballfall();
		    placebc();
		}
/*JP
		selftouch("Falling, you");
*/
		selftouch("落下中、あなたは");
		vision_full_recalc = 1;	/* vision limits change */
		exercise(A_STR, FALSE);
		exercise(A_DEX, FALSE);
#ifdef STEED
		}
#endif
		break;
	    case HOLE:
	    case TRAPDOOR:
		if (!Can_fall_thru(&u.uz)) {
		    seetrap(trap);	/* normally done in fall_through */
		    warning("dotrap: %ss cannot exist on this level.",
			       defsyms[trap_to_defsym(ttype)].explanation);
		    break;		/* don't activate it after all */
		}
		fall_through(TRUE);
		break;

	    case TELEP_TRAP:
		seetrap(trap);
		tele_trap(trap);
		break;
	    case LEVEL_TELEP:
		seetrap(trap);
		level_tele_trap(trap);
		break;

	    case WEB: /* Our luckless player has stumbled into a web. */
		seetrap(trap);
		if (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
						    unsolid(youmonst.data)) {
		    if (acidic(youmonst.data) || u.umonnum == PM_GELATINOUS_CUBE ||
			u.umonnum == PM_FIRE_ELEMENTAL) {
			if (webmsgok)
#if 0 /*JP*/
			    You("%s %s spider web!",
				(u.umonnum == PM_FIRE_ELEMENTAL) ? "burn" : "dissolve",
				a_your[trap->madeby_u]);
#else
			    You("%sくもの巣を%s！",
				web_you[trap->madeby_u],
				(u.umonnum == PM_FIRE_ELEMENTAL) ? "焼いた" : "こなごなにした");
#endif
			deltrap(trap);
			newsym(u.ux,u.uy);
			break;
		    }
#if 0 /*JP:T*/
		    if (webmsgok) You("flow through %s spider web.",
			    a_your[trap->madeby_u]);
#else
		    if (webmsgok) You("%sくもの巣をするりと通り抜けた。",
			    web_you[trap->madeby_u]);
#endif
		    break;
		}
		if (webmaker(youmonst.data)) {
		    if (webmsgok)
#if 0 /*JP:T*/
		    	pline(trap->madeby_u ? "You take a walk on your web."
					 : "There is a spider web here.");
#else
			pline(trap->madeby_u ? "自分で張ったくもの巣の上を歩いた。"
			      : "ここにはくもの巣がある。");
#endif
		    break;
		}
		if (webmsgok) {
		    char verbbuf[BUFSZ];
		    verbbuf[0] = '\0';
#ifdef STEED
		    if (u.usteed)
#if 0 /*JP*/
		   	Sprintf(verbbuf,"lead %s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "poor", SUPPRESS_SADDLE, FALSE));
#else
		   	Sprintf(verbbuf, "あなたと%s",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 "かわいそうな", SUPPRESS_SADDLE, FALSE));
#endif
		    else
#endif
			
#if 0 /*JP*/
		    Sprintf(verbbuf, "%s", Levitation ? (const char *)"float" :
		      		locomotion(youmonst.data, "stumble"));
#else
		    Sprintf(verbbuf, "%s", Levitation ? (const char *)"浮きながら" :
		      		jconj(locomotion(youmonst.data, "つまずく"), "て"));
#endif
#if 0 /*JP*/
		    You("%s into %s spider web!",
			verbbuf, a_your[trap->madeby_u]);
#else
		    You("%s%sくもの巣にひっかかった！",
			verbbuf, web_you[trap->madeby_u]);
#endif
		}
		u.utraptype = TT_WEB;

		/* Time stuck in the web depends on your/steed strength. */
		{
		    register int str = ACURR(A_STR);

#ifdef STEED
		    /* If mounted, the steed gets trapped.  Use mintrap
		     * to do all the work.  If mtrapped is set as a result,
		     * unset it and set utrap instead.  In the case of a
		     * strongmonst and mintrap said it's trapped, use a
		     * short but non-zero trap time.  Otherwise, monsters
		     * have no specific strength, so use player strength.
		     * This gets skipped for webmsgok, which implies that
		     * the steed isn't a factor.
		     */
		    if (u.usteed && webmsgok) {
			/* mtmp location might not be up to date */
			u.usteed->mx = u.ux;
			u.usteed->my = u.uy;

			/* mintrap currently does not return 2(died) for webs */
			if (mintrap(u.usteed)) {
			    u.usteed->mtrapped = 0;
			    if (strongmonst(u.usteed->data)) str = 17;
			} else {
			    break;
			}

			webmsgok = FALSE; /* mintrap printed the messages */
		    }
#endif
		    if (str <= 3) u.utrap = rn1(6,6);
		    else if (str < 6) u.utrap = rn1(6,4);
		    else if (str < 9) u.utrap = rn1(4,4);
		    else if (str < 12) u.utrap = rn1(4,2);
		    else if (str < 15) u.utrap = rn1(2,2);
		    else if (str < 18) u.utrap = rnd(2);
		    else if (str < 69) u.utrap = 1;
		    else {
			u.utrap = 0;
			if (webmsgok)
/*JP
			    You("tear through %s web!", a_your[trap->madeby_u]);
*/
			    You("%sくもの巣を切り裂いた！", web_you[trap->madeby_u]);
			deltrap(trap);
			newsym(u.ux,u.uy);	/* get rid of trap symbol */
		    }
		}
		break;

	    case STATUE_TRAP:
		(void) activate_statue_trap(trap, u.ux, u.uy, FALSE);
		break;

	    case MAGIC_TRAP:	    /* A magic trap. */
		seetrap(trap);
		if (!rn2(30)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* update position */
/*JP
		    You("are caught in a magical explosion!");
*/
		    You("魔法の爆発を浴びた！");
/*JP
		    losehp(rnd(10), "magical explosion", KILLED_BY_AN);
*/
		    losehp(rnd(10), "魔法の爆発を浴びて", KILLED_BY_AN);
/*JP
		    Your("body absorbs some of the magical energy!");
*/
		    Your("体は魔法のエネルギーを少し吸いとった！");
		    u.uen = (u.uenmax += 2);
		} else domagictrap();
#ifdef STEED
		(void) steedintrap(trap, (struct obj *)0);
#endif
		break;

	    case ANTI_MAGIC:
		seetrap(trap);
		if(Antimagic) {
		    shieldeff(u.ux, u.uy);
/*JP
		    You_feel("momentarily lethargic.");
*/
		    You("一瞬無気力感を感じた。");
		} else drain_en(rnd(u.ulevel) + 1);
		break;

	    case POLY_TRAP: {
	        char verbbuf[BUFSZ];
		seetrap(trap);
#ifdef STEED
		if (u.usteed)
/*JP
			Sprintf(verbbuf, "lead %s",
*/
			Sprintf(verbbuf, "%sとともに飛び込んだ",
				x_monnam(u.usteed,
					 u.usteed->mnamelth ? ARTICLE_NONE : ARTICLE_THE,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
		else
#endif
#if 0 /*JP*/
		 Sprintf(verbbuf,"%s",
		    Levitation ? (const char *)"float" :
		    locomotion(youmonst.data, "step"));
#else
		 Sprintf(verbbuf,"%s",
		    jpast(Levitation ? (const char *)"浮きながら飛びこむ" : 
		    locomotion(youmonst.data, "踏み込む")));
#endif
/*JP
		You("%s onto a polymorph trap!", verbbuf);
*/
		You("変化の罠に%s！", verbbuf);
		if(Antimagic || Unchanging) {
		    shieldeff(u.ux, u.uy);
/*JP
		    You_feel("momentarily different.");
*/
		    You("一瞬違った感じがした。");
		    /* Trap did nothing; don't remove it --KAA */
		} else {
#ifdef STEED
		    (void) steedintrap(trap, (struct obj *)0);
#endif
		    deltrap(trap);	/* delete trap before polymorph */
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
/*JP
		    You_feel("a change coming over you.");
*/
		    You("変化が訪れたような気がした。");
		    polyself(FALSE);
		}
		break;
	    }
	    case LANDMINE: {
#ifdef STEED
		unsigned steed_mid = 0;
		struct obj *saddle = 0;
#endif
		if (Levitation || Flying) {
		    if (!already_seen && rn2(3)) break;
		    seetrap(trap);
#if 0 /*JP*/
		    pline("%s %s in a pile of soil below you.",
			    already_seen ? "There is" : "You discover",
			    trap->madeby_u ? "the trigger of your mine" :
					     "a trigger");
#else
		    if(already_seen)
		      pline("ここには%s地雷の起爆スイッチがある。",
			    set_you[trap->madeby_u]);
		    else
		      You("足下の土の山に%s地雷の起爆スイッチをみつけた。",
			    set_you[trap->madeby_u]);
#endif
		    if (already_seen && rn2(3)) break;
#if 0 /*JP*/
		    pline("KAABLAMM!!!  %s %s%s off!",
			  forcebungle ? "Your inept attempt sets" :
					"The air currents set",
			    already_seen ? a_your[trap->madeby_u] : "",
			    already_seen ? " land mine" : "it");
#else
		    pline("ちゅどーん！！%s%s地雷の起爆スイッチが発動した！",
			    forcebungle ? "不器用なせいで" :
			    		  "空気の流れで",
			    set_you[trap->madeby_u]);
#endif
		} else {
#ifdef STEED
		    /* prevent landmine from killing steed, throwing you to
		     * the ground, and you being affected again by the same
		     * mine because it hasn't been deleted yet
		     */
		    static boolean recursive_mine = FALSE;

		    if (recursive_mine) break;
#endif
		    seetrap(trap);
#if 0 /*JP*/
		    pline("KAABLAMM!!!  You triggered %s land mine!",
					    a_your[trap->madeby_u]);
#else
		    pline("ちゅどーん！！%s地雷の起爆スイッチを踏んだ！",
			    set_you[trap->madeby_u]);
#endif
#ifdef STEED
		    if (u.usteed) steed_mid = u.usteed->m_id;
		    recursive_mine = TRUE;
		    (void) steedintrap(trap, (struct obj *)0);
		    recursive_mine = FALSE;
		    saddle = sobj_at(SADDLE,u.ux, u.uy);
#endif
		    set_wounded_legs(LEFT_SIDE, rn1(35, 41));
		    set_wounded_legs(RIGHT_SIDE, rn1(35, 41));
		    exercise(A_DEX, FALSE);
		}
		blow_up_landmine(trap);
#ifdef STEED
		if (steed_mid && saddle && !u.usteed)
			(void)keep_saddle_with_steedcorpse(steed_mid, fobj, saddle);
#endif
		newsym(u.ux,u.uy);		/* update trap symbol */
/*JP
		losehp(rnd(16), "land mine", KILLED_BY_AN);
*/
		losehp(rnd(16), "地雷を踏んで", KILLED_BY_AN);
		/* fall recursively into the pit... */
		if ((trap = t_at(u.ux, u.uy)) != 0) dotrap(trap, RECURSIVETRAP);
		fill_pit(u.ux, u.uy);
		break;
	    }
	    case ROLLING_BOULDER_TRAP: {
		int style = ROLL | (trap->tseen ? LAUNCH_KNOWN : 0);

		seetrap(trap);
#if 0 /*JP*/
		pline("Click! You trigger a rolling boulder trap!");
#else
		pline("カチッ！あなたは%sのスイッチを踏んだ！",
		      jtrns_obj('^', "rolling boulder trap"));
#endif
		if(!launch_obj(BOULDER, trap->launch.x, trap->launch.y,
		      trap->launch2.x, trap->launch2.y, style)) {
		    deltrap(trap);
		    newsym(u.ux,u.uy);	/* get rid of trap symbol */
/*JP
		    pline("Fortunately for you, no boulder was released.");
*/
		    pline("運のよいことに岩は転がってこなかった。");
		}
		break;
	    }
	    case MAGIC_PORTAL:
		seetrap(trap);
#if defined(BLACKMARKET) && defined(STEED)
		if (u.usteed &&
			(Is_blackmarket(&trap->dst) || Is_blackmarket(&u.uz)))
/*JP
		    pline("%s seems to shimmer for a moment.",
*/
		    pline("%sが一瞬輝いたように見えた。",
			  Monnam(u.usteed));
		else
#endif
		domagicportal(trap);
		break;

	    default:
		seetrap(trap);
		warning("You hit a trap of type %u", trap->ttyp);
	}
}

#ifdef STEED
STATIC_OVL int
steedintrap(trap, otmp)
struct trap *trap;
struct obj *otmp;
{
	struct monst *mtmp = u.usteed;
	struct permonst *mptr;
	int tt;
	boolean in_sight;
	boolean trapkilled = FALSE;
	boolean steedhit = FALSE;

	if (!u.usteed || !trap) return 0;
	mptr = mtmp->data;
	tt = trap->ttyp;
	mtmp->mx = u.ux;
	mtmp->my = u.uy;

	in_sight = !Blind;
	switch (tt) {
		case ARROW_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant arrow?");
				return 0;
			}
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case DART_TRAP:
			if(!otmp) {
				impossible("steed hit by non-existant dart?");
				return 0;
			}
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case SLP_GAS_TRAP:
		    if (!resists_sleep(mtmp) && !breathless(mptr) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
/*JP
				pline("%s suddenly falls asleep!",
*/
				pline("%sはとつぜん眠ってしまった！",
				      Monnam(mtmp));
			    }
			}
			steedhit = TRUE;
			break;
		case LANDMINE:
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case PIT:
		case SPIKED_PIT:
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : 10), FALSE))
			    trapkilled = TRUE;
			steedhit = TRUE;
			break;
		case POLY_TRAP: 
		    if (!resists_magm(mtmp)) {
			if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
			(void) newcham(mtmp, (struct permonst *)0,
				       FALSE, FALSE);
			if (!can_saddle(mtmp) || !can_ride(mtmp)) {
				dismount_steed(DISMOUNT_POLY);
			} else {
/*JP
				You("have to adjust yourself in the saddle on %s.",
*/
				You("%sの鞍の上で座りなおした。",
					x_monnam(mtmp,
					 mtmp->mnamelth ? ARTICLE_NONE : ARTICLE_A,
				 	 (char *)0, SUPPRESS_SADDLE, FALSE));
			}
				
		    }
		    steedhit = TRUE;
		    break;
		default:
			return 0;
	    }
	}
	if(trapkilled) {
		dismount_steed(DISMOUNT_POLY);
		return 2;
	}
	else if(steedhit) return 1;
	else return 0;
}
#endif /*STEED*/

/* some actions common to both player and monsters for triggered landmine */
void
blow_up_landmine(trap)
struct trap *trap;
{
	(void)scatter(trap->tx, trap->ty, 4,
		MAY_DESTROY | MAY_HIT | MAY_FRACTURE | VIS_EFFECTS,
		(struct obj *)0);
	del_engr_at(trap->tx, trap->ty);
	wake_nearto(trap->tx, trap->ty, 400);
	if (IS_DOOR(levl[trap->tx][trap->ty].typ))
	    levl[trap->tx][trap->ty].doormask = D_BROKEN;
	/* TODO: destroy drawbridge if present */
	/* caller may subsequently fill pit, e.g. with a boulder */
	trap->ttyp = PIT;		/* explosion creates a pit */
	trap->madeby_u = FALSE;		/* resulting pit isn't yours */
	seetrap(trap);			/* and it isn't concealed */
}

#endif /* OVLB */
#ifdef OVL3

/*
 * Move obj from (x1,y1) to (x2,y2)
 *
 * Return 0 if no object was launched.
 *        1 if an object was launched and placed somewhere.
 *        2 if an object was launched, but used up.
 */
int
launch_obj(otyp, x1, y1, x2, y2, style)
short otyp;
register int x1,y1,x2,y2;
int style;
{
	register struct monst *mtmp;
	register struct obj *otmp, *otmp2;
	register int dx,dy;
	struct obj *singleobj;
	boolean used_up = FALSE;
	boolean otherside = FALSE;
	int dist;
	int tmp;
	int delaycnt = 0;

	otmp = sobj_at(otyp, x1, y1);
	/* Try the other side too, for rolling boulder traps */
	if (!otmp && otyp == BOULDER) {
		otherside = TRUE;
		otmp = sobj_at(otyp, x2, y2);
	}
	if (!otmp) return 0;
	if (otherside) {	/* swap 'em */
		int tx, ty;

		tx = x1; ty = y1;
		x1 = x2; y1 = y2;
		x2 = tx; y2 = ty;
	}

	if (otmp->quan == 1L) {
	    obj_extract_self(otmp);
	    singleobj = otmp;
	    otmp = (struct obj *) 0;
	} else {
	    singleobj = splitobj(otmp, 1L);
	    obj_extract_self(singleobj);
	}
	newsym(x1,y1);
	/* in case you're using a pick-axe to chop the boulder that's being
	   launched (perhaps a monster triggered it), destroy context so that
	   next dig attempt never thinks you're resuming previous effort */
	if ((otyp == BOULDER || otyp == STATUE) &&
	    singleobj->ox == digging.pos.x && singleobj->oy == digging.pos.y)
	    (void) memset((genericptr_t)&digging, 0, sizeof digging);

	dist = distmin(x1,y1,x2,y2);
	bhitpos.x = x1;
	bhitpos.y = y1;
	dx = sgn(x2 - x1);
	dy = sgn(y2 - y1);
	switch (style) {
	    case ROLL|LAUNCH_UNSEEN:
			if (otyp == BOULDER) {
#if 0 /*JP:T*/
			    You_hear(Hallucination ?
				     "someone bowling." :
				     "rumbling in the distance.");
#else
			    You_hear(Hallucination ?
				     "誰かがボーリングをしている音を聞いた。" :
				     "遠くのゴロゴロという音を聞いた。");
#endif
			}
			style &= ~LAUNCH_UNSEEN;
			goto roll;
	    case ROLL|LAUNCH_KNOWN:
			/* use otrapped as a flag to ohitmon */
			singleobj->otrapped = 1;
			style &= ~LAUNCH_KNOWN;
			/* fall through */
	    roll:
	    case ROLL:
			delaycnt = 2;
			/* fall through */
	    default:
			if (!delaycnt) delaycnt = 1;
			if (!cansee(bhitpos.x,bhitpos.y)) curs_on_u();
			tmp_at(DISP_FLASH, obj_to_glyph(singleobj));
			tmp_at(bhitpos.x, bhitpos.y);
	}

	/* Set the object in motion */
	while(dist-- > 0 && !used_up) {
		struct trap *t;
		tmp_at(bhitpos.x, bhitpos.y);
		tmp = delaycnt;

		/* dstage@u.washington.edu -- Delay only if hero sees it */
		if (cansee(bhitpos.x, bhitpos.y))
			while (tmp-- > 0) delay_output();

		bhitpos.x += dx;
		bhitpos.y += dy;
		t = t_at(bhitpos.x, bhitpos.y);
		
		if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
			if (otyp == BOULDER && throws_rocks(mtmp->data)) {
			    if (rn2(3)) {
/*JP
				pline("%s snatches the boulder.",
*/
				pline("%sは岩をつかみ取った。",
					Monnam(mtmp));
				singleobj->otrapped = 0;
				(void) mpickobj(mtmp, singleobj);
				used_up = TRUE;
				break;
			    }
			}
			if (ohitmon(mtmp,singleobj,
					(style==ROLL) ? -1 : dist, FALSE)) {
				used_up = TRUE;
				break;
			}
		} else if (bhitpos.x == u.ux && bhitpos.y == u.uy) {
			if (multi) nomul(0, 0);
			if (thitu(9 + singleobj->spe,
				  dmgval(singleobj, &youmonst),
				  singleobj, (char *)0))
			    stop_occupation();
		}
		if (style == ROLL) {
		    if (down_gate(bhitpos.x, bhitpos.y) != -1) {
		        if(ship_object(singleobj, bhitpos.x, bhitpos.y, FALSE)){
				used_up = TRUE;
				break;
			}
		    }
		    if (t && otyp == BOULDER) {
			switch(t->ttyp) {
			case LANDMINE:
			    if (rn2(10) > 2) {
#if 0 /*JP*/
			  	pline(
				  "KAABLAMM!!!%s",
				  cansee(bhitpos.x, bhitpos.y) ?
					" The rolling boulder triggers a land mine." : "");
#else
			  	pline(
				  "ちゅどーん！！%s",
				  cansee(bhitpos.x, bhitpos.y) ?
					"転がってきた岩が地雷の起爆スイッチを踏んだ。" : "");
#endif
				deltrap(t);
				del_engr_at(bhitpos.x,bhitpos.y);
				place_object(singleobj, bhitpos.x, bhitpos.y);
				singleobj->otrapped = 0;
				fracture_rock(singleobj);
				(void)scatter(bhitpos.x,bhitpos.y, 4,
					MAY_DESTROY|MAY_HIT|MAY_FRACTURE|VIS_EFFECTS,
					(struct obj *)0);
				if (cansee(bhitpos.x,bhitpos.y))
					newsym(bhitpos.x,bhitpos.y);
			        used_up = TRUE;
			    }
			    break;		
			case LEVEL_TELEP:
			case TELEP_TRAP:
			    if (cansee(bhitpos.x, bhitpos.y))
/*JP
			    	pline("Suddenly the rolling boulder disappears!");
*/
			    	pline("転がってきた岩がとつぜん消えた！");
			    else
/*JP
			    	You_hear("a rumbling stop abruptly.");
*/
			    	pline("ゴロゴロという音が突然止まった。");
			    singleobj->otrapped = 0;
			    if (t->ttyp == TELEP_TRAP)
				rloco(singleobj);
			    else {
				int newlev = random_teleport_level();
				d_level dest;

				if (newlev == depth(&u.uz) || In_endgame(&u.uz))
				    continue;
				add_to_migration(singleobj);
				get_level(&dest, newlev);
				singleobj->ox = dest.dnum;
				singleobj->oy = dest.dlevel;
				singleobj->owornmask = (long)MIGR_RANDOM;
			    }
		    	    seetrap(t);
			    used_up = TRUE;
			    break;
			case PIT:
			case SPIKED_PIT:
			case HOLE:
			case TRAPDOOR:
			    /* the boulder won't be used up if there is a
			       monster in the trap; stop rolling anyway */
			    x2 = bhitpos.x,  y2 = bhitpos.y;  /* stops here */
/*JP
			    if (flooreffects(singleobj, x2, y2, "fall"))
*/
			    if (flooreffects(singleobj, x2, y2, "落ちる"))
				used_up = TRUE;
			    dist = -1;	/* stop rolling immediately */
			    break;
			}
			if (used_up || dist == -1) break;
		    }
/*JP
		    if (flooreffects(singleobj, bhitpos.x, bhitpos.y, "fall")) {
*/
		    if (flooreffects(singleobj, bhitpos.x, bhitpos.y, "落ちる")) {
			used_up = TRUE;
			break;
		    }
		    if (otyp == BOULDER &&
		       (otmp2 = sobj_at(BOULDER, bhitpos.x, bhitpos.y)) != 0) {
			const char *bmsg =
/*JP
				     " as one boulder sets another in motion";
*/
				     "ひとつの岩が他の岩を動かしたかのような、";

			if (!isok(bhitpos.x + dx, bhitpos.y + dy) || !dist ||
			    IS_ROCK(levl[bhitpos.x + dx][bhitpos.y + dy].typ))
/*JP
			    bmsg = " as one boulder hits another";
*/
			    bmsg = "ひとつの岩が他の岩に当たったような";

/*JP
			You_hear("a loud crash%s!",
*/
			You_hear("%s大きなゴンという音を聞いた！",
				cansee(bhitpos.x, bhitpos.y) ? bmsg : "");
			obj_extract_self(otmp2);
			/* pass off the otrapped flag to the next boulder */
			otmp2->otrapped = singleobj->otrapped;
			singleobj->otrapped = 0;
			place_object(singleobj, bhitpos.x, bhitpos.y);
			singleobj = otmp2;
			otmp2 = (struct obj *)0;
			wake_nearto(bhitpos.x, bhitpos.y, 10*10);
		    }
		}
		if (otyp == BOULDER && closed_door(bhitpos.x,bhitpos.y)) {
			if (cansee(bhitpos.x, bhitpos.y))
/*JP
				pline_The("boulder crashes through a door.");
*/
				pline("岩は扉を破壊した。");
			levl[bhitpos.x][bhitpos.y].doormask = D_BROKEN;
			if (dist) unblock_point(bhitpos.x, bhitpos.y);
		}

		/* if about to hit iron bars, do so now */
		if (dist > 0 && isok(bhitpos.x + dx,bhitpos.y + dy) &&
			levl[bhitpos.x + dx][bhitpos.y + dy].typ == IRONBARS) {
		    x2 = bhitpos.x,  y2 = bhitpos.y;	/* object stops here */
		    if (hits_bars(&singleobj, x2, y2, !rn2(20), 0)) {
			if (!singleobj) used_up = TRUE;
			break;
		    }
		}
	}
	tmp_at(DISP_END, 0);
	if (!used_up) {
		singleobj->otrapped = 0;
		place_object(singleobj, x2,y2);
		newsym(x2,y2);
		return 1;
	} else
		return 2;
}

#endif /* OVL3 */
#ifdef OVLB

void
seetrap(trap)
	register struct trap *trap;
{
	if(!trap->tseen) {
	    trap->tseen = 1;
	    newsym(trap->tx, trap->ty);
	}
}

#endif /* OVLB */
#ifdef OVL3

STATIC_OVL int
mkroll_launch(ttmp, x, y, otyp, ocount)
struct trap *ttmp;
xchar x,y;
short otyp;
long ocount;
{
	struct obj *otmp;
	register int tmp;
	schar dx,dy;
	int distance;
	coord cc;
	coord bcc;
	int trycount = 0;
	boolean success = FALSE;
	int mindist = 4;

	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) mindist = 2;
	distance = rn1(5,4);    /* 4..8 away */
	tmp = rn2(8);		/* randomly pick a direction to try first */
	while (distance >= mindist) {
		dx = xdir[tmp];
		dy = ydir[tmp];
		cc.x = x; cc.y = y;
		/* Prevent boulder from being placed on water */
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP
				&& is_pool(x+distance*dx,y+distance*dy))
			success = FALSE;
		else success = isclearpath(&cc, distance, dx, dy);
		if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
			boolean success_otherway;
			bcc.x = x; bcc.y = y;
			success_otherway = isclearpath(&bcc, distance,
						-(dx), -(dy));
			if (!success_otherway) success = FALSE;
		}
		if (success) break;
		if (++tmp > 7) tmp = 0;
		if ((++trycount % 8) == 0) --distance;
	}
	if (!success) {
	    /* create the trap without any ammo, launch pt at trap location */
		cc.x = bcc.x = x;
		cc.y = bcc.y = y;
	} else {
		otmp = mksobj(otyp, TRUE, FALSE);
		otmp->quan = ocount;
		otmp->owt = weight(otmp);
		place_object(otmp, cc.x, cc.y);
		stackobj(otmp);
	}
	ttmp->launch.x = cc.x;
	ttmp->launch.y = cc.y;
	if (ttmp->ttyp == ROLLING_BOULDER_TRAP) {
		ttmp->launch2.x = bcc.x;
		ttmp->launch2.y = bcc.y;
	} else
		ttmp->launch_otyp = otyp;
	newsym(ttmp->launch.x, ttmp->launch.y);
	return 1;
}

STATIC_OVL boolean
isclearpath(cc,distance,dx,dy)
coord *cc;
int distance;
schar dx,dy;
{
	uchar typ;
	xchar x, y;

	x = cc->x;
	y = cc->y;
	while (distance-- > 0) {
		x += dx;
		y += dy;
		typ = levl[x][y].typ;
		if (!isok(x,y) || !ZAP_POS(typ) || closed_door(x,y))
			return FALSE;
	}
	cc->x = x;
	cc->y = y;
	return TRUE;
}
#endif /* OVL3 */
#ifdef OVL1

int
mintrap(mtmp)
register struct monst *mtmp;
{
	register struct trap *trap = t_at(mtmp->mx, mtmp->my);
	boolean trapkilled = FALSE;
	struct permonst *mptr = mtmp->data;
	struct obj *otmp;
#ifdef WEBB_DISINT
	boolean can_disint =(touch_disintegrates(mtmp->data) && 
	                     !mtmp->mcan &&
	                      mtmp->mhp>6 &&
	                      rn2(20));
#endif

	if (!trap) {
	    mtmp->mtrapped = 0;	/* perhaps teleported? */
	} else if (mtmp->mtrapped) {	/* is currently in the trap */
	    if (!trap->tseen &&
		cansee(mtmp->mx, mtmp->my) && canseemon(mtmp) &&
		(trap->ttyp == SPIKED_PIT || trap->ttyp == BEAR_TRAP ||
		 trap->ttyp == HOLE || trap->ttyp == PIT ||
		 trap->ttyp == WEB)) {
		/* If you come upon an obviously trapped monster, then
		 * you must be able to see the trap it's in too.
		 */
		seetrap(trap);
	    }
		
	    if (!rn2(40)) {
		if (sobj_at(BOULDER, mtmp->mx, mtmp->my) &&
			(trap->ttyp == PIT || trap->ttyp == SPIKED_PIT)) {
		    if (!rn2(2)) {
			mtmp->mtrapped = 0;
			if (canseemon(mtmp))
/*JP
			    pline("%s pulls free...", Monnam(mtmp));
*/
			    pline("%sは助け上げられた。", Monnam(mtmp));
			fill_pit(mtmp->mx, mtmp->my);
		    }
		} else {
		    mtmp->mtrapped = 0;
		}
	    } else if (metallivorous(mptr)) {
		if (trap->ttyp == BEAR_TRAP) {
		    if (canseemon(mtmp))
/*JP
			pline("%s eats a bear trap!", Monnam(mtmp));
*/
		    pline("%sは熊の罠を食べた！", Monnam(mtmp));
		    deltrap(trap);
		    mtmp->meating = 5;
		    mtmp->mtrapped = 0;
		} else if (trap->ttyp == SPIKED_PIT) {
		    if (canseemon(mtmp))
/*JP
			pline("%s munches on some spikes!", Monnam(mtmp));
*/
			pline("%sはトゲトゲを食べた！", Monnam(mtmp));
		    trap->ttyp = PIT;
		    mtmp->meating = 5;
		}
	    }
	} else {
	    register int tt = trap->ttyp;
	    boolean in_sight, tear_web, see_it,
#ifdef WEBB_DISINT
	    trap_visible = (trap->tseen && cansee(trap->tx,trap->ty)),
#endif
		    inescapable = ((tt == HOLE || tt == PIT) &&
				   In_sokoban(&u.uz) && !trap->madeby_u);
	    const char *fallverb;

#ifdef STEED
	    /* true when called from dotrap, inescapable is not an option */
	    if (mtmp == u.usteed) inescapable = TRUE;
#endif
	    if (!inescapable &&
		    ((mtmp->mtrapseen & (1 << (tt-1))) != 0 ||
			(tt == HOLE && !mindless(mtmp->data)))) {
		/* it has been in such a trap - perhaps it escapes */
		if(rn2(4)) return(0);
	    } else {
		mtmp->mtrapseen |= (1 << (tt-1));
	    }
	    /* Monster is aggravated by being trapped by you.
	       Recognizing who made the trap isn't completely
	       unreasonable; everybody has their own style. */
	    if (trap->madeby_u && rnl(5)) setmangry(mtmp);

	    in_sight = canseemon(mtmp);
	    see_it = cansee(mtmp->mx, mtmp->my);
#ifdef STEED
	    /* assume hero can tell what's going on for the steed */
	    if (mtmp == u.usteed) in_sight = TRUE;
#endif
	    switch (tt) {
		case ARROW_TRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
#if 0 /*JP:T*/
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
#else
				pline("%sがわなを起動させたが何も起こらなかった。",
				      Monnam(mtmp));
#endif
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ARROW, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			otmp->opoisoned = 0;
			if (in_sight) seetrap(trap);
			if (thitm(8, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			break;
		case DART_TRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
#if 0 /*JP:T*/
				pline("%s triggers a trap but nothing happens.",
				      Monnam(mtmp));
#else
				pline("%sがわなを起動させたが何も起こらなかった。",
				      Monnam(mtmp));
#endif
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(DART, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (!rn2(6)) otmp->opoisoned = 1;
			if (in_sight) seetrap(trap);
			if (thitm(7, mtmp, otmp, 0, FALSE)) trapkilled = TRUE;
			break;
		case ROCKTRAP:
			if (trap->once && trap->tseen && !rn2(15)) {
			    if (in_sight && see_it)
#if 0 /*JP:T*/
				pline("A trap door above %s opens, but nothing falls out!",
				      mon_nam(mtmp));
#else
				pline("%sの上の扉が開いたが、何も落ちてこなかった！",
				      mon_nam(mtmp));
#endif
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			    break;
			}
			trap->once = 1;
			otmp = mksobj(ROCK, TRUE, FALSE);
			otmp->quan = 1L;
			otmp->owt = weight(otmp);
			if (in_sight) seetrap(trap);
			if (thitm(0, mtmp, otmp, d(2, 6), FALSE))
			    trapkilled = TRUE;
			break;

		case SQKY_BOARD:
			if(is_flyer(mptr)) break;
			/* stepped on a squeaky board */
			if (in_sight) {
/*JP
			    pline("A board beneath %s squeaks loudly.", mon_nam(mtmp));
*/
			    pline("%sの足元の板が大きくきしんだ。", mon_nam(mtmp));
			    seetrap(trap);
			} else
/*JP
			   You_hear("a distant squeak.");
*/
			   You_hear("遠くできしむ音を聞いた。");
			/* wake up nearby monsters */
			wake_nearto(mtmp->mx, mtmp->my, 40);
			break;

		case BEAR_TRAP:
			if(mptr->msize > MZ_SMALL &&
				!amorphous(mptr) && !is_flyer(mptr) &&
				!is_whirly(mptr) && !unsolid(mptr)) {
#ifdef WEBB_DISINT
				if (can_disint){
					if (in_sight)
#if 0 /*JP*/
						pline("%s beartrap disintegrates on %s leg!",
								A_Your[trap->madeby_u], s_suffix(mon_nam(mtmp)));
#else
						pline("%s熊の罠は%sの足で粉砕された！",
								set_you[trap->madeby_u], mon_nam(mtmp));
#endif
					else if (trap_visible)
#if 0 /*JP*/
						pline("%s beartrap disintegrates!",
								A_Your[trap->madeby_u]);
#else
						pline("%s熊の罠は粉砕された！",
								set_you[trap->madeby_u]);
#endif
					deltrap(trap);
					newsym(mtmp->mx,mtmp->my);
					mtmp->mhp -= rnd(2); /* beartrap weighs 200 */
				} else {
#endif
			    mtmp->mtrapped = 1;
			    if(in_sight) {
#if 0 /*JP*/
				pline("%s is caught in %s bear trap!",
				      Monnam(mtmp), a_your[trap->madeby_u]);
#else
				pline("%sは%s熊の罠につかまった！",
				      Monnam(mtmp), set_you[trap->madeby_u]);
#endif
				seetrap(trap);
			    } else {
				if((mptr == &mons[PM_OWLBEAR]
				    || mptr == &mons[PM_BUGBEAR])
				   && flags.soundok)
/*JP
				    You_hear("the roaring of an angry bear!");
*/
				    You_hear("怒り狂う熊の咆哮を聞いた！");
			    }
			}
		   }
		   break;

		case SLP_GAS_TRAP:
		    if (!resists_sleep(mtmp) && !breathless(mptr) &&
				!mtmp->msleeping && mtmp->mcanmove) {
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = rnd(25);
			    if (in_sight) {
/*JP
				pline("%s suddenly falls asleep!",
*/
				pline("%sは突然眠りに落ちた！",
				      Monnam(mtmp));
				seetrap(trap);
			    }
			}
			break;

		case RUST_TRAP:
		    {
			struct obj *target;

			if (in_sight)
			    seetrap(trap);
			switch (rn2(5)) {
			case 0:
			    if (in_sight)
#if 0 /*JP*/
				pline("%s %s on the %s!", A_gush_of_water_hits,
				    mon_nam(mtmp), mbodypart(mtmp, HEAD));
#else
				pline("水が噴出して、%sの%sに命中した！", 
				    mon_nam(mtmp), mbodypart(mtmp, HEAD));
#endif
			    target = which_armor(mtmp, W_ARMH);
/*JP
			    (void) rust_dmg(target, "helmet", 1, TRUE, mtmp);
*/
			    (void) rust_dmg(target, "兜", 1, TRUE, mtmp);
			    break;
			case 1:
			    if (in_sight)
/*JP
				pline("%s %s's left %s!", A_gush_of_water_hits,
*/
				pline("水が噴出して、%sの左%sに命中した！",
				    mon_nam(mtmp), mbodypart(mtmp, ARM));
			    target = which_armor(mtmp, W_ARMS);
/*JP
			    if (rust_dmg(target, "shield", 1, TRUE, mtmp))
*/
			    if (rust_dmg(target, "盾", 1, TRUE, mtmp))
				break;
			    target = MON_WEP(mtmp);
			    if (target && bimanual(target))
				erode_obj(target, FALSE, TRUE);
glovecheck:		    target = which_armor(mtmp, W_ARMG);
/*JP
			    (void) rust_dmg(target, "gauntlets", 1, TRUE, mtmp);
*/
			    (void) rust_dmg(target, "小手", 1, TRUE, mtmp);
			    break;
			case 2:
			    if (in_sight)
/*JP
				pline("%s %s's right %s!", A_gush_of_water_hits,
*/
				pline("水が噴出して、%sの右%sに命中した！",
				    mon_nam(mtmp), mbodypart(mtmp, ARM));
			    erode_obj(MON_WEP(mtmp), FALSE, TRUE);
			    goto glovecheck;
			default:
			    if (in_sight)
/*JP
				pline("%s %s!", A_gush_of_water_hits,
*/
				pline("水が噴出して、%sに命中した！", 
				    mon_nam(mtmp));
			    for (otmp=mtmp->minvent; otmp; otmp = otmp->nobj)
				(void) snuff_lit(otmp);
			    target = which_armor(mtmp, W_ARMC);
			    if (target)
				(void) rust_dmg(target, cloak_simple_name(target),
						 1, TRUE, mtmp);
			    else {
				target = which_armor(mtmp, W_ARM);
				if (target)
/*JP
				    (void) rust_dmg(target, "armor", 1, TRUE, mtmp);
*/
				    (void) rust_dmg(target, "鎧", 1, TRUE, mtmp);
#ifdef TOURIST
				else {
				    target = which_armor(mtmp, W_ARMU);
/*JP
				    (void) rust_dmg(target, "shirt", 1, TRUE, mtmp);
*/
				    (void) rust_dmg(target, "シャツ", 1, TRUE, mtmp);
				}
#endif
			    }
			}
			if (mptr == &mons[PM_IRON_GOLEM]) {
				if (in_sight)
/*JP
				    pline("%s falls to pieces!", Monnam(mtmp));
*/
				    pline("%sはくだけちった！", Monnam(mtmp));
				else if(mtmp->mtame)
/*JP
				    pline("May %s rust in peace.",
*/
				    pline("%sよ、安らかに錆びん事を。",
								mon_nam(mtmp));
				mondied(mtmp);
				if (mtmp->mhp <= 0)
					trapkilled = TRUE;
#ifdef WEBB_DISINT
			} else if (can_disint) {
/*JP
				pline("The water vanishes in a green twinkling.");
*/
				pline("%s輝きとともに水が消えた。", hcolor("緑色の"));
#endif
			} else if (mptr == &mons[PM_GREMLIN] && rn2(3)) {
				(void)split_mon(mtmp, (struct monst *)0);
			}
			break;
		    }
		case FIRE_TRAP:
 mfiretrap:
			if (in_sight)
#if 0 /*JP*/
			    pline("A %s erupts from the %s under %s!",
				  tower_of_flame,
				  surface(mtmp->mx,mtmp->my), mon_nam(mtmp));
#else
			    pline("火柱が%sの足元の%sから立ちのぼった！",
				  mon_nam(mtmp), surface(mtmp->mx,mtmp->my));
#endif
			else if (see_it)  /* evidently `mtmp' is invisible */
#if 0 /*JP*/
			    You("see a %s erupt from the %s!",
				tower_of_flame, surface(mtmp->mx,mtmp->my));
#else
			    You("火柱が%sから生じるのを見た！",
				surface(mtmp->mx,mtmp->my));
#endif

			if (resists_fire(mtmp)) {
			    if (in_sight) {
				shieldeff(mtmp->mx,mtmp->my);
/*JP
				pline("%s is uninjured.", Monnam(mtmp));
*/
				pline("が、%sは傷つかない。", Monnam(mtmp));
			    }
			} else {
			    int num = d(2,4), alt;
			    boolean immolate = FALSE;

			    /* paper burns very fast, assume straw is tightly
			     * packed and burns a bit slower */
			    switch (monsndx(mtmp->data)) {
			    case PM_PAPER_GOLEM:   immolate = TRUE;
						   alt = mtmp->mhpmax; break;
			    case PM_STRAW_GOLEM:   alt = mtmp->mhpmax / 2; break;
			    case PM_WOOD_GOLEM:    alt = mtmp->mhpmax / 4; break;
			    case PM_LEATHER_GOLEM: alt = mtmp->mhpmax / 8; break;
			    default: alt = 0; break;
			    }
			    if (alt > num) num = alt;

			    if (thitm(0, mtmp, (struct obj *)0, num, immolate))
				trapkilled = TRUE;
			    else
				/* we know mhp is at least `num' below mhpmax,
				   so no (mhp > mhpmax) check is needed here */
				mtmp->mhpmax -= rn2(num + 1);
			}
			if (burnarmor(mtmp) || rn2(3)) {
			    (void) destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
			    (void) destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
			    (void) destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
			}
			if (burn_floor_paper(mtmp->mx, mtmp->my, see_it, FALSE) &&
				!see_it && distu(mtmp->mx, mtmp->my) <= 3*3)
/*JP
			    You("smell smoke.");
*/
			    pline("煙の匂いがした。");
			if (is_ice(mtmp->mx,mtmp->my))
			    melt_ice(mtmp->mx,mtmp->my);
			if (see_it) seetrap(trap);
			break;

		case PIT:
		case SPIKED_PIT:
/*JP
			fallverb = "falls";
*/
			fallverb = "落ちる";
			if (is_flyer(mptr) || is_floater(mptr) ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				is_clinger(mptr)) {
			    if (!inescapable) break;	/* avoids trap */
#if 0 /*JP*/
			    fallverb = "is dragged";	/* sokoban pit */
#else
			    fallverb = "ずり落ちる";	/* sokoban pit */
#endif
			}
			if (!passes_walls(mptr))
			    mtmp->mtrapped = 1;
			if (in_sight) {
#if 0 /*JP*/
			    pline("%s %s into %s pit!",
				  Monnam(mtmp), fallverb,
				  a_your[trap->madeby_u]);
#else
			    pline("%sは%s落し穴に%s！",
				  Monnam(mtmp), set_you[trap->madeby_u],
				  jpast(fallverb));
#endif
			    if (mptr == &mons[PM_PIT_VIPER] || mptr == &mons[PM_PIT_FIEND])
/*JP
				pline("How pitiful.  Isn't that the pits?");
*/
				pline("この落し穴はいい仕事をしている。");
			    seetrap(trap);
			}
#ifdef WEBB_DISINT
			if (can_disint && tt == SPIKED_PIT){
				trap->ttyp = PIT;
				if (trap_visible){
/*JP
					pline("Some spikes dinsintegrate.");
*/
					pline("トゲトゲは粉砕された。");
				}
			}
#endif
/*JP
			mselftouch(mtmp, "Falling, ", FALSE);
*/
			mselftouch(mtmp, "落下中、", FALSE);
			if (mtmp->mhp <= 0 ||
				thitm(0, mtmp, (struct obj *)0,
				      rnd((tt == PIT) ? 6 : 10), FALSE))
			    trapkilled = TRUE;
			break;
		case HOLE:
		case TRAPDOOR:
			if (!Can_fall_thru(&u.uz)) {
			 impossible("mintrap: %ss cannot exist on this level.",
				    defsyms[trap_to_defsym(tt)].explanation);
			    break;	/* don't activate it after all */
			}
			if (is_flyer(mptr) || is_floater(mptr) ||
				mptr == &mons[PM_WUMPUS] ||
				(mtmp->wormno && count_wsegs(mtmp) > 5) ||
				mptr->msize >= MZ_HUGE) {
			    if (inescapable) {	/* sokoban hole */
				if (in_sight) {
/*JP
				    pline("%s seems to be yanked down!",
*/
				    pline("%sは引き落されたようだ！",
					  Monnam(mtmp));
				    /* suppress message in mlevel_tele_trap() */
				    in_sight = FALSE;
				    seetrap(trap);
				}
			    } else
				break;
			}
			/* Fall through */
		case LEVEL_TELEP:
		case MAGIC_PORTAL:
			{
			    int mlev_res;
			    mlev_res = mlevel_tele_trap(mtmp, trap,
							inescapable, in_sight);
			    if (mlev_res) return(mlev_res);
			}
			break;

		case TELEP_TRAP:
			mtele_trap(mtmp, trap, in_sight);
			break;

		case WEB:
			/* Monster in a web. */
			if (webmaker(mptr)) break;
#ifdef WEBB_DISINT
			if (can_disint) {
				if (in_sight) {
#if 0 /*JP*/
					pline("%s dissolves %s spider web!", Monnam(mtmp),
							a_your[trap->madeby_u]);
#else
					pline("%sは%sくもの巣をこなごなにした！", Monnam(mtmp),
							web_you[trap->madeby_u]);
#endif
				} else if (trap_visible) {
#if 0 /*JP*/
					pline("%s spider web disintegrates in a green twinkling!", 
							A_Your[trap->madeby_u]);
#else
					pline("%sくもの巣は%s輝きとともに粉砕された！", 
							web_you[trap->madeby_u], hcolor("緑色の"));
#endif
				}
				deltrap(trap);
				newsym(mtmp->mx, mtmp->my);
				break;
			} else
#endif
			if (amorphous(mptr) || is_whirly(mptr) || unsolid(mptr)){
			    if(acidic(mptr) ||
			       mptr == &mons[PM_GELATINOUS_CUBE] ||
			       mptr == &mons[PM_FIRE_ELEMENTAL]) {
				if (in_sight)
#if 0 /*JP*/
				    pline("%s %s %s spider web!",
					  Monnam(mtmp),
					  (mptr == &mons[PM_FIRE_ELEMENTAL]) ?
					    "burns" : "dissolves",
					  a_your[trap->madeby_u]);
#else
				    pline("%s%sくもの巣を%s！",
					  Monnam(mtmp),
					  web_you[trap->madeby_u],
					  (mptr == &mons[PM_FIRE_ELEMENTAL]) ?
					    "焼いた" : "こなごなにした");
#endif
				deltrap(trap);
				newsym(mtmp->mx, mtmp->my);
				break;
			    }
			    if (in_sight) {
#if 0 /*JP*/
				pline("%s flows through %s spider web.",
				      Monnam(mtmp),
				      a_your[trap->madeby_u]);
#else
				pline("%sは%sくもの巣をするりと通り抜けた。",
				      Monnam(mtmp),
				      web_you[trap->madeby_u]);
#endif
				seetrap(trap);
			    }
			    break;
			}
			tear_web = FALSE;
			switch (monsndx(mptr)) {
			    case PM_OWLBEAR: /* Eric Backus */
			    case PM_BUGBEAR:
				if (!in_sight) {
/*JP
				    You_hear("the roaring of a confused bear!");
*/
				    You_hear("混乱の咆哮を聞いた！");
				    mtmp->mtrapped = 1;
				    break;
				}
				/* fall though */
			    default:
				if (mptr->mlet == S_GIANT ||
				    (mptr->mlet == S_DRAGON &&
					extra_nasty(mptr)) || /* excl. babies */
				    (mtmp->wormno && count_wsegs(mtmp) > 5)) {
				    tear_web = TRUE;
				} else if (in_sight) {
#if 0 /*JP*/
				    pline("%s is caught in %s spider web.",
					  Monnam(mtmp),
					  a_your[trap->madeby_u]);
#else
				    pline("%sは%sくもの巣につかまった。",
					  Monnam(mtmp),
					  web_you[trap->madeby_u]);
#endif
				    seetrap(trap);
				}
				mtmp->mtrapped = tear_web ? 0 : 1;
				break;
			    /* this list is fairly arbitrary; it deliberately
			       excludes wumpus & giant/ettin zombies/mummies */
			    case PM_TITANOTHERE:
			    case PM_BALUCHITHERIUM:
			    case PM_PURPLE_WORM:
			    case PM_JABBERWOCK:
			    case PM_IRON_GOLEM:
			    case PM_BALROG:
			    case PM_KRAKEN:
			    case PM_MASTODON:
				tear_web = TRUE;
				break;
			}
			if (tear_web) {
			    if (in_sight)
#if 0 /*JP*/
				pline("%s tears through %s spider web!",
				      Monnam(mtmp), a_your[trap->madeby_u]);
#else
				pline("%sは%sくもの巣を引き裂いた！",
				      Monnam(mtmp), web_you[trap->madeby_u]);
#endif
			    deltrap(trap);
			    newsym(mtmp->mx, mtmp->my);
			}
			break;

		case STATUE_TRAP:
			break;

		case MAGIC_TRAP:
			/* A magic trap.  Monsters usually immune. */
			if (!rn2(21)) goto mfiretrap;
			break;
		case ANTI_MAGIC:
			break;

		case LANDMINE:
			if(rn2(3))
				break; /* monsters usually don't set it off */
			if(is_flyer(mptr)) {
				boolean already_seen = trap->tseen;
				if (in_sight && !already_seen) {
/*JP
	pline("A trigger appears in a pile of soil below %s.", mon_nam(mtmp));
*/
	pline("%sの足元の土の山に起爆スイッチが現われた。", mon_nam(mtmp));
					seetrap(trap);
				}
				if (rn2(3)) break;
				if (in_sight) {
					newsym(mtmp->mx, mtmp->my);
#if 0 /*JP*/
					pline_The("air currents set %s off!",
					  already_seen ? "a land mine" : "it");
#else
					pline("空気の流れでスイッチが入った！");
#endif
				}
			} else if(in_sight) {
			    newsym(mtmp->mx, mtmp->my);
#if 0 /*JP*/
			    pline("KAABLAMM!!!  %s triggers %s land mine!",
				Monnam(mtmp), a_your[trap->madeby_u]);
#else
			    pline("ちゅどーん！！%sは%s地雷の起爆スイッチを踏んだ！",
				Monnam(mtmp), set_you[trap->madeby_u]);
#endif
			}
			if (!in_sight)
/*JP
				pline("Kaablamm!  You hear an explosion in the distance!");
*/
				pline("ちゅどーん！あなたは遠方の爆発音を聞いた！");
			blow_up_landmine(trap);
			if (thitm(0, mtmp, (struct obj *)0, rnd(16), FALSE))
				trapkilled = TRUE;
			else {
				/* monsters recursively fall into new pit */
				if (mintrap(mtmp) == 2) trapkilled=TRUE;
			}
			/* a boulder may fill the new pit, crushing monster */
			fill_pit(trap->tx, trap->ty);
			if (mtmp->mhp <= 0) trapkilled = TRUE;
			if (unconscious()) {
				multi = -1;
/*JP
				nomovemsg="The explosion awakens you!";
*/
				nomovemsg="爆発であなたは起きた！";
			}
			break;

		case POLY_TRAP:
		    if (resists_magm(mtmp)) {
			shieldeff(mtmp->mx, mtmp->my);
		    } else if (!resist(mtmp, WAND_CLASS, 0, NOTELL)) {
			(void) newcham(mtmp, (struct permonst *)0,
				       FALSE, FALSE);
			if (in_sight) seetrap(trap);
		    }
		    break;

		case ROLLING_BOULDER_TRAP:
		    if (!is_flyer(mptr)) {
			int style = ROLL | (in_sight ? 0 : LAUNCH_UNSEEN);

		        newsym(mtmp->mx,mtmp->my);
			if (in_sight)
#if 0 /*JP*/
			    pline("Click! %s triggers %s.", Monnam(mtmp),
				  trap->tseen ?
				  "a rolling boulder trap" :
				  something);
#else
			  pline("カチッ！%sは%sのスイッチを踏んだ！", Monnam(mtmp),
				  trap->tseen ?
				  jtrns_obj('^', "rolling boulder trap") : "何か");
#endif
			if (launch_obj(BOULDER, trap->launch.x, trap->launch.y,
				trap->launch2.x, trap->launch2.y, style)) {
			    if (in_sight) trap->tseen = TRUE;
			    if (mtmp->mhp <= 0) trapkilled = TRUE;
			} else {
			    deltrap(trap);
			    newsym(mtmp->mx,mtmp->my);
			}
		    }
		    break;

		default:
			warning("Some monster encountered a strange trap of type %d.", tt);
	    }
	}
	if(trapkilled) return 2;
	return mtmp->mtrapped;
}

#endif /* OVL1 */
#ifdef OVLB

/* Combine cockatrice checks into single functions to avoid repeating code. */
void
instapetrify(str)
const char *str;
{
	if (Stone_resistance) return;
	if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
	    return;
/*JP
	You("turn to stone...");
*/
	You("石になった．．．");
	killer_format = KILLED_BY;
	killer = str;
	done(STONING);
}

void
minstapetrify(mon,byplayer)
struct monst *mon;
boolean byplayer;
{
	if (resists_ston(mon)) return;
	if (poly_when_stoned(mon->data)) {
		mon_to_stone(mon);
		return;
	}

	/* give a "<mon> is slowing down" message and also remove
	   intrinsic speed (comparable to similar effect on the hero) */
	mon_adjust_speed(mon, -3, (struct obj *)0);

	if (cansee(mon->mx, mon->my))
/*JP
		pline("%s turns to stone.", Monnam(mon));
*/
		pline("%sは石になった。", Monnam(mon));
	if (byplayer) {
		stoned = TRUE;
		xkilled(mon,0);
	} else monstone(mon);
}

#ifdef WEBB_DISINT
int
instadisintegrate(str)
const char * str;
{
	int result;
	if (Disint_resistance || !rn2(10))
		return 0;
/*JP
	You("disintegrate!");
*/
	You("粉砕された！");
	result = (youmonst.data->cwt);
	weight_dmg(result);
	result = min(6, result); 
	killer_format = KILLED_BY;
	killer = str;
	u.ugrave_arise = -3;
	done(DISINTEGRATED);

	return (result);
}

int
minstadisintegrate(mon)
struct monst *mon;
{
  int result = mon->data->cwt;
  if (resists_disint(mon) || !rn2(20)) return 0;
  weight_dmg(result); 
  if (canseemon(mon))
/*JP
    pline("%s disintegrates!", Monnam(mon));
*/
    pline("%sは粉砕された！", Monnam(mon));
  if (is_rider(mon->data)){
    if (canseemon(mon)){
#if 0 /*JP*/
      pline("%s body reintegrates before your %s!",
          s_suffix(Monnam(mon)),
          (eyecount(youmonst.data) == 1)?
          body_part(EYE) : makeplural(body_part(EYE)));
#else
      pline("%sの身体はあなたの%sの前で再構成された！",
          Monnam(mon), body_part(EYE));
#endif
      mon->mhp = mon->mhpmax;
    }
    return result;
  } else {
    mondead_helper(mon, AD_DISN);
    return result;
  }
}
#endif

void
selftouch(arg)
const char *arg;
{
	char kbuf[BUFSZ];

	if(uwep && uwep->otyp == CORPSE && touch_petrifies(&mons[uwep->corpsenm])
			&& !Stone_resistance) {
#if 0 /*JP*/
		pline("%s touch the %s corpse.", arg,
		        mons[uwep->corpsenm].mname);
		Sprintf(kbuf, "%s corpse", an(mons[uwep->corpsenm].mname));
#else
		pline("%s%sの死体に触った。", arg,
		        jtrns_mon(mons[uwep->corpsenm].mname));
		Sprintf(kbuf, "%sの死体で", jtrns_mon(mons[uwep->corpsenm].mname));
#endif
		instapetrify(kbuf);
	}
	/* Or your secondary weapon, if wielded */
	if(u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
			touch_petrifies(&mons[uswapwep->corpsenm]) && !Stone_resistance){
#if 0 /*JP:T*/
		pline("%s touch the %s corpse.", arg,
		        mons[uswapwep->corpsenm].mname);
#else
		pline("%s%sの死体に触った。", arg,
		        jtrns_mon(mons[uswapwep->corpsenm].mname));
#endif
/*JP
		Sprintf(kbuf, "%s corpse", an(mons[uswapwep->corpsenm].mname));
*/
		Sprintf(kbuf, "%sの死体で", jtrns_mon(mons[uswapwep->corpsenm].mname));
		instapetrify(kbuf);
	}
}

void
mselftouch(mon,arg,byplayer)
struct monst *mon;
const char *arg;
boolean byplayer;
{
	struct obj *mwep = MON_WEP(mon);

	if (mwep && mwep->otyp == CORPSE && touch_petrifies(&mons[mwep->corpsenm])) {
		if (cansee(mon->mx, mon->my)) {
#if 0 /*JP*/
			pline("%s%s touches the %s corpse.",
			    arg ? arg : "", arg ? mon_nam(mon) : Monnam(mon),
			    mons[mwep->corpsenm].mname);
#else
			pline("%s%sは%sの死体に触った。", 
			    arg ? arg : "", arg ? mon_nam(mon) : Monnam(mon),
			    jtrns_mon(mons[mwep->corpsenm].mname));
#endif
		}
		minstapetrify(mon, byplayer);
	}
}

void
float_up()
{
	if(u.utrap) {
		if(u.utraptype == TT_PIT) {
			u.utrap = 0;
/*JP
			You("float up, out of the pit!");
*/
			You("浮き上がって、落し穴から出た！");
			vision_full_recalc = 1;	/* vision limits change */
			fill_pit(u.ux, u.uy);
		} else if (u.utraptype == TT_INFLOOR) {
/*JP
			Your("body pulls upward, but your %s are still stuck.",
*/
			Your("体は引き上げられた。しかし%sはまだはまっている。",
			     makeplural(body_part(LEG)));
		} else {
/*JP
			You("float up, only your %s is still stuck.",
*/
			You("浮き出た。%sだけがはまっている。",
				body_part(LEG));
		}
	}
	else if(Is_waterlevel(&u.uz))
/*JP
		pline("It feels as though you've lost some weight.");
*/
		You("まるで体重が減ったように感じた。");
	else if(u.uinwater)
		spoteffects(TRUE);
	else if(u.uswallow)
		You(is_animal(u.ustuck->data) ?
#if 0 /*JP*/
			"float away from the %s."  :
			"spiral up into %s.",
#else
			"%sの中で浮いた。" :
			"%sの中でぐるぐる回転した。",
#endif
		    is_animal(u.ustuck->data) ?
			surface(u.ux, u.uy) :
			mon_nam(u.ustuck));
	else if (Hallucination)
/*JP
		pline("Up, up, and awaaaay!  You're walking on air!");
*/
		pline("上れ、上れ、上れぇぇぇぇ！あなたは空中を歩いている！");
	else if(Is_airlevel(&u.uz))
/*JP
		You("gain control over your movements.");
*/
		You("うまく歩けるようになった。");
	else
/*JP
		You("start to float in the air!");
*/
		You("空中に浮きはじめた！");
#ifdef STEED
	if (u.usteed && !is_floater(u.usteed->data) &&
						!is_flyer(u.usteed->data)) {
	    if (Lev_at_will)
/*JP
	    	pline("%s magically floats up!", Monnam(u.usteed));
*/
	    	pline("%sは魔法の力で浮いた！", Monnam(u.usteed));
	    else {
/*JP
	    	You("cannot stay on %s.", mon_nam(u.usteed));
*/
	    	You("%sの上に乗っていられない。", mon_nam(u.usteed));
	    	dismount_steed(DISMOUNT_GENERIC);
	    }
	}
#endif
	return;
}

void
fill_pit(x, y)
int x, y;
{
	struct obj *otmp;
	struct trap *t;

	if ((t = t_at(x, y)) &&
	    ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT)) &&
	    (otmp = sobj_at(BOULDER, x, y))) {
		obj_extract_self(otmp);
/*JP
		(void) flooreffects(otmp, x, y, "settle");
*/
		(void) flooreffects(otmp, x, y, "はまる");
	}
}

int
float_down(hmask, emask)
long hmask, emask;     /* might cancel timeout */
{
	register struct trap *trap = (struct trap *)0;
	d_level current_dungeon_level;
	boolean no_msg = FALSE;

	HLevitation &= ~hmask;
	ELevitation &= ~emask;
	if(Levitation) return(0); /* maybe another ring/potion/boots */
	if(u.uswallow) {
#if 0 /*JP*/
	    You("float down, but you are still %s.",
		is_animal(u.ustuck->data) ? "swallowed" : "engulfed");
#else
	    You("着地したが、まだ飲み込まれたままだ。");
#endif
	    return(1);
	}

	if (Punished && !carried(uball) &&
	    (is_pool(uball->ox, uball->oy) ||
	     ((trap = t_at(uball->ox, uball->oy)) &&
	      ((trap->ttyp == PIT) || (trap->ttyp == SPIKED_PIT) ||
	       (trap->ttyp == TRAPDOOR) || (trap->ttyp == HOLE))))) {
			u.ux0 = u.ux;
			u.uy0 = u.uy;
			u.ux = uball->ox;
			u.uy = uball->oy;
			movobj(uchain, uball->ox, uball->oy);
			newsym(u.ux0, u.uy0);
			vision_full_recalc = 1;	/* in case the hero moved. */
	}
	/* check for falling into pool - added by GAN 10/20/86 */
	if(!Flying) {
		if (!u.uswallow && u.ustuck) {
			if (sticks(youmonst.data))
#if 0 /*JP:T*/
				You("aren't able to maintain your hold on %s.",
					mon_nam(u.ustuck));
#else
				You("%sをつかまえ続けていられなくなった。",
					mon_nam(u.ustuck));
#endif
			else
#if 0 /*JP:T*/
				pline("Startled, %s can no longer hold you!",
					mon_nam(u.ustuck));
#else
				pline("%sは驚いてあなたを放してしまった！",
					mon_nam(u.ustuck));
#endif
			u.ustuck = 0;
		}
		/* kludge alert:
		 * drown() and lava_effects() print various messages almost
		 * every time they're called which conflict with the "fall
		 * into" message below.  Thus, we want to avoid printing
		 * confusing, duplicate or out-of-order messages.
		 * Use knowledge of the two routines as a hack -- this
		 * should really be handled differently -dlc
		 */
		if(is_pool(u.ux,u.uy) && !Wwalking && !Swimming && !u.uinwater)
			no_msg = drown();

		if(is_lava(u.ux,u.uy)) {
			(void) lava_effects();
			no_msg = TRUE;
		}
	}
	if (!trap) {
	    trap = t_at(u.ux,u.uy);
	    if(Is_airlevel(&u.uz))
/*JP
		You("begin to tumble in place.");
*/
		You("その場でひっくり返りはじめた。");
	    else if (Is_waterlevel(&u.uz) && !no_msg)
/*JP
		You_feel("heavier.");
*/
		You("重くなったような気がした。");
	    /* u.uinwater msgs already in spoteffects()/drown() */
	    else if (!u.uinwater && !no_msg) {
#ifdef STEED
		if (!(emask & W_SADDLE))
#endif
		{
		    boolean sokoban_trap = (In_sokoban(&u.uz) && trap);
		    if (Hallucination)
#if 0 /*JP*/
			pline("Bummer!  You've %s.",
			      is_pool(u.ux,u.uy) ?
			      "splashed down" : sokoban_trap ? "crashed" :
			      "hit the ground");
#else
				pline("やめてぇ！あなたは%s。",
				      is_pool(u.ux,u.uy) ?
					"ザブンと落ちた" : "地面にたたきつけられた");
#endif
		    else {
			if (!sokoban_trap)
/*JP
			    You("float gently to the %s.",
*/
			    You("静かに%sまで辿りついた。",
				surface(u.ux, u.uy));
			else {
			    /* Justification elsewhere for Sokoban traps
			     * is based on air currents. This is
			     * consistent with that.
			     * The unexpected additional force of the
			     * air currents once leviation
			     * ceases knocks you off your feet.
			     */
/*JP
			    You("fall over.");
*/
			    You("つまづいた。");
/*JP
			    losehp(rnd(2), "dangerous winds", KILLED_BY);
*/
			    losehp(rnd(2), "危険な風で", KILLED_BY);
#ifdef STEED
			    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
/*JP
			    selftouch("As you fall, you");
*/
			    selftouch("落ちながら、あなたは");
			}
		    }
		}
	    }
	}

	/* can't rely on u.uz0 for detecting trap door-induced level change;
	   it gets changed to reflect the new level before we can check it */
	assign_level(&current_dungeon_level, &u.uz);

	if(trap)
		switch(trap->ttyp) {
		case STATUE_TRAP:
			break;
		case HOLE:
		case TRAPDOOR:
			if(!Can_fall_thru(&u.uz) || u.ustuck)
				break;
			/* fall into next case */
		default:
			if (!u.utrap) /* not already in the trap */
				dotrap(trap, 0);
	}

	if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !u.uswallow &&
		/* falling through trap door calls goto_level,
		   and goto_level does its own pickup() call */
		on_level(&u.uz, &current_dungeon_level))
	    (void) pickup(1);
	return 1;
}

STATIC_OVL void
dofiretrap(box)
struct obj *box;	/* null for floor trap */
{
	boolean see_it = !Blind;
	int num, alt;

/* Bug: for box case, the equivalent of burn_floor_paper() ought
 * to be done upon its contents.
 */

	if ((box && !carried(box)) ? is_pool(box->ox, box->oy) : Underwater) {
#if 0 /*JP*/
	    pline("A cascade of steamy bubbles erupts from %s!",
		    the(box ? xname(box) : surface(u.ux,u.uy)));
#else
	    pline("蒸気の泡が%sからしゅーっと発生した！",
		    box ? xname(box) : surface(u.ux,u.uy));
#endif
/*JP
	    if (Fire_resistance) You("are uninjured.");
*/
	    if (Fire_resistance) You("傷つかない。");
/*JP
	    else losehp(rnd(3), "boiling water", KILLED_BY);
*/
	    else losehp(rnd(3), "沸騰した水で", KILLED_BY);
	    return;
	}
#if 0 /*JP*/
	pline("A %s %s from %s!", tower_of_flame,
	      box ? "bursts" : "erupts",
	      the(box ? xname(box) : surface(u.ux,u.uy)));
#else
	pline("火柱が%sから%s！",
	      box ? xname(box) : surface(u.ux,u.uy),
	      box ? "吹き出した" : "立ちのぼった");
#endif
	if (Fire_resistance) {
	    shieldeff(u.ux, u.uy);
	    num = rn2(2);
	} else if (Upolyd) {
	    num = d(2,4);
	    switch (u.umonnum) {
	    case PM_PAPER_GOLEM:   alt = u.mhmax; break;
	    case PM_STRAW_GOLEM:   alt = u.mhmax / 2; break;
	    case PM_WOOD_GOLEM:    alt = u.mhmax / 4; break;
	    case PM_LEATHER_GOLEM: alt = u.mhmax / 8; break;
	    default: alt = 0; break;
	    }
	    if (alt > num) num = alt;
	    if (u.mhmax > mons[u.umonnum].mlevel)
		u.mhmax -= rn2(min(u.mhmax,num + 1)), flags.botl = 1;
	} else {
	    num = d(2,4);
	    if (u.uhpmax > u.ulevel)
		u.uhpmax -= rn2(min(u.uhpmax,num + 1)), flags.botl = 1;
	}
	if (!num)
/*JP
	    You("are uninjured.");
*/
	    You("傷つかない。");
	else
/*JP
	    losehp(num, tower_of_flame, KILLED_BY_AN);
*/
	    losehp(num, "火柱で", KILLED_BY_AN);
	burn_away_slime();

	if (burnarmor(&youmonst) || rn2(3)) {
	    destroy_item(SCROLL_CLASS, AD_FIRE);
	    destroy_item(SPBOOK_CLASS, AD_FIRE);
	    destroy_item(POTION_CLASS, AD_FIRE);
	}
	if (!box && burn_floor_paper(u.ux, u.uy, see_it, TRUE) && !see_it)
/*JP
	    You("smell paper burning.");
*/
	    You("紙のこげる匂いがした。");
	if (is_ice(u.ux, u.uy))
	    melt_ice(u.ux, u.uy);
}

STATIC_OVL void
domagictrap()
{
	register int fate = rnd(20);

	/* What happened to the poor sucker? */

	if (fate < 10) {
	  /* Most of the time, it creates some monsters. */
	  register int cnt = rnd(4);

	  if (!resists_blnd(&youmonst)) {
/*JP
		You("are momentarily blinded by a flash of light!");
*/
		You("まばゆい光で一瞬目がくらんだ！");
		make_blinded((long)rn1(5,10),FALSE);
		if (!Blind) Your(vision_clears);
	  } else if (!Blind) {
/*JP
		You("see a flash of light!");
*/
		You("まばゆい光を浴びた！");
	  }  else
/*JP
		You_hear("a deafening roar!");
*/
		You_hear("耳をつんざくような咆哮を聞いた！");
	  while(cnt--)
		(void) makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);
	}
	else
	  switch (fate) {

	     case 10:
	     case 11:
		      /* sometimes nothing happens */
			break;
	     case 12: /* a flash of fire */
			dofiretrap((struct obj *)0);
			break;

	     /* odd feelings */
/*JP
	     case 13:	pline("A shiver runs up and down your %s!",
*/
	     case 13:	pline("震えがあなたの%sを走った！",
			      body_part(SPINE));
			break;
#if 0 /*JP*/
	     case 14:	You_hear(Hallucination ?
				"the moon howling at you." :
				"distant howling.");
#else
	     case 14:	if (Hallucination)
				pline("ふにゃ？月が吠えている。") ;
			else
				You_hear("遠方の遠吠を聞いた。");
#endif
			break;
	     case 15:	if (on_level(&u.uz, &qstart_level))
#if 0 /*JP*/
			    You_feel("%slike the prodigal son.",
			      (flags.female || (Upolyd && is_neuter(youmonst.data))) ?
				     "oddly " : "");
#else
/*JP 訳注 prodigal sonは聖書のルカ伝かららしい。よーわからん */
			    You("%s放蕩息子を好きになったような気がした。",
			      (flags.female || (Upolyd && is_neuter(youmonst.data))) ?
				     "異常に" : "");
#endif
			else
#if 0 /*JP*/
			    You("suddenly yearn for %s.",
				Hallucination ? "Cleveland" :
			    (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ?
						"your nearby homeland" :
						"your distant homeland");
#else
			    You("突然%sが恋しくなった。",
				Hallucination ? "青森" :
			    (In_quest(&u.uz) || at_dgn_entrance("The Quest")) ?
						"すぐそこにある故郷" :
						"はるかかなたの故郷");
#endif
			break;
/*JP
	     case 16:   Your("pack shakes violently!");
*/
	     case 16:   Your("袋は激しく揺れた！");
			break;
	     case 17:	You(Hallucination ?
/*JP
				"smell hamburgers." :
*/
				"ハンバーガーの匂いがした。" :
/*JP
				"smell charred flesh.");
*/
				"黒焦げの肉の匂いがした。");
			break;
/*JP
	     case 18:	You_feel("tired.");
*/
	     case 18:	You("疲れを感じた。");
			break;

	     /* very occasionally something nice happens. */

	     case 19:
		    /* tame nearby monsters */
		   {   register int i,j;
		       register struct monst *mtmp;

		       (void) adjattrib(A_CHA,1,FALSE);
		       for(i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			   if(!isok(u.ux+i, u.uy+j)) continue;
			   mtmp = m_at(u.ux+i, u.uy+j);
			   if(mtmp)
			       (void) tamedog(mtmp, (struct obj *)0);
		       }
		       break;
		   }

	     case 20:
		    /* uncurse stuff */
		   {	struct obj pseudo;
			long save_conf = HConfusion;

			pseudo = zeroobj;   /* neither cursed nor blessed */
			pseudo.otyp = SCR_REMOVE_CURSE;
			HConfusion = 0L;
			(void) seffects(&pseudo);
			HConfusion = save_conf;
			break;
		   }
	     default: break;
	  }
}

/*
 * Scrolls, spellbooks, potions, and flammable items
 * may get affected by the fire.
 *
 * Return number of objects destroyed. --ALI
 */
int
fire_damage(chain, force, here, x, y)
struct obj *chain;
boolean force, here;
xchar x, y;
{
    int chance;
    struct obj *obj, *otmp, *nobj, *ncobj;
    int retval = 0;
    int in_sight = !Blind && couldsee(x, y);	/* Don't care if it's lit */
    int dindx;

    for (obj = chain; obj; obj = nobj) {
	nobj = here ? obj->nexthere : obj->nobj;

	/* object might light in a controlled manner */
	if (catch_lit(obj))
	    continue;

	if (Is_container(obj)) {
	    switch (obj->otyp) {
	    case ICE_BOX:
	    case IRON_SAFE:
		continue;		/* Immune */
		/*NOTREACHED*/
		break;
	    case CHEST:
		chance = 40;
		break;
	    case LARGE_BOX:
		chance = 30;
		break;
	    default:
		chance = 20;
		break;
	    }
	    if (!force && (Luck + 5) > rn2(chance))
		continue;
	    /* Container is burnt up - dump contents out */
/*JP
	    if (in_sight) pline("%s catches fire and burns.", Yname2(obj));
*/
	    if (in_sight) pline("%sは火がついて燃えた。", Yname2(obj));
	    if (Has_contents(obj)) {
/*JP
		if (in_sight) pline("Its contents fall out.");
*/
		if (in_sight) pline("中身が出てきた。");
		for (otmp = obj->cobj; otmp; otmp = ncobj) {
		    ncobj = otmp->nobj;
		    obj_extract_self(otmp);
		    if (!flooreffects(otmp, x, y, ""))
			place_object(otmp, x, y);
		}
	    }
	    delobj(obj);
	    retval++;
	} else if (!force && (Luck + 5) > rn2(20)) {
	    /*  chance per item of sustaining damage:
	     *	max luck (full moon):	 5%
	     *	max luck (elsewhen):	10%
	     *	avg luck (Luck==0):	75%
	     *	awful luck (Luck<-4):  100%
	     */
	    continue;
	} else if (obj->oclass == SCROLL_CLASS || obj->oclass == SPBOOK_CLASS) {
	    if (obj->otyp == SCR_FIRE || obj->otyp == SPE_FIREBALL)
		continue;
	    if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
/*JP
		if (in_sight) pline("Smoke rises from %s.", the(xname(obj)));
*/
		if (in_sight) pline("%sから煙があがった。", the(xname(obj)));
		continue;
	    }
	    dindx = (obj->oclass == SCROLL_CLASS) ? 2 : 3;
	    if (in_sight)
#if 0 /*JP*/
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
#else
		pline("%sは%s。", Yname2(obj), destroy_strings[dindx*3]);
#endif
	    delobj(obj);
	    retval++;
	} else if (obj->oclass == POTION_CLASS) {
	    dindx = 1;
	    if (in_sight)
#if 0 /*JP*/
		pline("%s %s.", Yname2(obj), (obj->quan > 1) ?
		      destroy_strings[dindx*3 + 1] : destroy_strings[dindx*3]);
#else
		pline("%sは%s。", Yname2(obj), destroy_strings[dindx*3]);
#endif
	    delobj(obj);
	    retval++;
	} else if (is_flammable(obj) && obj->oeroded < MAX_ERODE &&
		   !(obj->oerodeproof || (obj->blessed && !rnl(4)))) {
	    if (in_sight) {
#if 0 /*JP*/
		pline("%s %s%s.", Yname2(obj), otense(obj, "burn"),
		      obj->oeroded+1 == MAX_ERODE ? " completely" :
		      obj->oeroded ? " further" : "");
#else
		pline("%sは%s燃えた。", Yname2(obj),
		      obj->oeroded+1 == MAX_ERODE ? "完全に" :
		      obj->oeroded ? "さらに" : "");
#endif
	    }
	    obj->oeroded++;
	}
    }

    if (retval && !in_sight)
/*JP
	You("smell smoke.");
*/
	You("煙のにおいがした。");
    return retval;
}

/* returns TRUE if obj is destroyed */
boolean
water_damage(obj, force, here)
register struct obj *obj;
register boolean force, here;
{
	struct obj *otmp;
	struct obj *obj_original = obj;
	boolean obj_destroyed = FALSE;

	/* Scrolls, spellbooks, potions, weapons and
	   pieces of armor may get affected by the water */
	for (; obj; obj = otmp) {
		otmp = here ? obj->nexthere : obj->nobj;

		(void) snuff_lit(obj);

		if(obj->otyp == CAN_OF_GREASE && obj->spe > 0) {
			continue;
		} else if(obj->greased) {
			if (force || !rn2(2)) obj->greased = 0;
		} else if(Is_container(obj) && !Is_box(obj) &&
			(obj->otyp != OILSKIN_SACK || (obj->cursed && !rn2(3)))) {
			water_damage(obj->cobj, force, FALSE);
		} else if (!force && (Luck + 5) > rn2(20)) {
			/*  chance per item of sustaining damage:
			 *	max luck (full moon):	 5%
			 *	max luck (elsewhen):	10%
			 *	avg luck (Luck==0):	75%
			 *	awful luck (Luck<-4):  100%
			 */
			continue;
		} else if (obj->oclass == SCROLL_CLASS) {
#ifdef MAIL
		    if (obj->otyp != SCR_MAIL)
#endif
		    {
			obj->otyp = SCR_BLANK_PAPER;
			obj->spe = 0;
		    }
		} else if (obj->oclass == SPBOOK_CLASS) {
			if (obj->otyp == SPE_BOOK_OF_THE_DEAD)
/*JP
				pline("Steam rises from %s.", the(xname(obj)));
*/
				pline("蒸気が%sから立ちのぼった。", xname(obj));
			else obj->otyp = SPE_BLANK_PAPER;
		} else if (obj->oclass == POTION_CLASS) {
			if (obj->otyp == POT_ACID) {
				/* damage player/monster? */
/*JP
				pline("A potion explodes!");
*/
				pline("薬瓶が爆発した！");
				delobj(obj);
				obj_destroyed = (obj == obj_original);
				continue;
			} else if (obj->odiluted) {
				obj->otyp = POT_WATER;
				obj->blessed = obj->cursed = 0;
				obj->odiluted = 0;
			} else if (obj->otyp != POT_WATER)
				obj->odiluted++;
		} else if (is_rustprone(obj) && obj->oeroded < MAX_ERODE &&
			  !(obj->oerodeproof || (obj->blessed && !rnl(4)))) {
			/* all metal stuff and armor except (body armor
			   protected by oilskin cloak) */
			if(obj->oclass != ARMOR_CLASS || obj != uarm ||
			   !uarmc || uarmc->otyp != OILSKIN_CLOAK ||
			   (uarmc->cursed && !rn2(3)))
				obj->oeroded++;
		}
		obj_destroyed = FALSE;
	}
	return obj_destroyed;
}

/*
 * This function is potentially expensive - rolling
 * inventory list multiple times.  Luckily it's seldom needed.
 * Returns TRUE if disrobing made player unencumbered enough to
 * crawl out of the current predicament.
 */
STATIC_OVL boolean
emergency_disrobe(lostsome)
boolean *lostsome;
{
	int invc = inv_cnt();

	while (near_capacity() > (Punished ? UNENCUMBERED : SLT_ENCUMBER)) {
	    register struct obj *obj, *otmp = (struct obj *)0;
	    register int i;

	    /* Pick a random object */
	    if (invc > 0) {
		i = rn2(invc);
		for (obj = invent; obj; obj = obj->nobj) {
		    /*
		     * Undroppables are: body armor, boots, gloves,
		     * amulets, and rings because of the time and effort
		     * in removing them + loadstone and other cursed stuff
		     * for obvious reasons.
		     */
		    if (!((obj->otyp == LOADSTONE && obj->cursed) ||
			  obj == uamul || obj == uleft || obj == uright ||
			  obj == ublindf || obj == uarm || obj == uarmc ||
			  obj == uarmg || obj == uarmf ||
#ifdef TOURIST
			  obj == uarmu ||
#endif
			  (obj->cursed && (obj == uarmh || obj == uarms)) ||
			  welded(obj)))
			otmp = obj;
		    /* reached the mark and found some stuff to drop? */
		    if (--i < 0 && otmp) break;

		    /* else continue */
		}
	    }
#ifndef GOLDOBJ
	    if (!otmp) {
		/* Nothing available left to drop; try gold */
		if (u.ugold) {
/*JP
		    pline("In desperation, you drop your purse.");
*/
		    You("やけくそで財布を落した。");
		    /* Hack: gold is not in the inventory, so make a gold object
		     * and put it at the head of the inventory list.
		     */
		    obj = mkgoldobj(u.ugold);    /* removes from u.ugold */
		    obj->in_use = TRUE;
		    u.ugold = obj->quan;         /* put the gold back */
		    assigninvlet(obj);           /* might end up as NOINVSYM */
		    obj->nobj = invent;
		    invent = obj;
		    *lostsome = TRUE;
		    dropx(obj);
		    continue;                    /* Try again */
		}
		/* We can't even drop gold! */
		return (FALSE);
	    }
#else
	    if (!otmp) return (FALSE); /* nothing to drop! */	
#endif
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    *lostsome = TRUE;
	    dropx(otmp);
	    invc--;
	}
	return(TRUE);
}

/*
 *  return(TRUE) == player relocated
 */
boolean
drown()
{
	boolean inpool_ok = FALSE, crawl_ok;
	int i, x, y;

	/* happily wading in the same contiguous pool */
	if (u.uinwater && is_pool(u.ux-u.dx,u.uy-u.dy) &&
	    (Swimming || Amphibious)) {
		/* water effects on objects every now and then */
		if (!rn2(5)) inpool_ok = TRUE;
		else return(FALSE);
	}

	if (!u.uinwater) {
#if 0 /*JP*/
	    You("%s into the water%c",
		Is_waterlevel(&u.uz) ? "plunge" : "fall",
		Amphibious || Swimming ? '.' : '!');
	    if (!Swimming && !Is_waterlevel(&u.uz))
		    You("sink like %s.",
			Hallucination ? "the Titanic" : "a rock");
#else
	    You("水の中に%s%s",
		Is_waterlevel(&u.uz) ? "飛びこんだ" : "落ちた",
		Amphibious ? "。" : "！");
	    if (!Swimming && !Is_waterlevel(&u.uz))
		You("%sのように沈んだ。",
		    Hallucination ? "タイタニック号" : "岩");
#endif
	}

	water_damage(invent, FALSE, FALSE);

	if (u.umonnum == PM_GREMLIN && rn2(3))
	    (void)split_mon(&youmonst, (struct monst *)0);
	else if (u.umonnum == PM_IRON_GOLEM) {
/*JP
	    You("rust!");
*/
	    You("錆びた！");
	    i = d(2,6);
	    if (u.mhmax > i) u.mhmax -= i;
/*JP
	    losehp(i, "rusting away", KILLED_BY);
*/
	    losehp(i, "完全に錆びて", KILLED_BY);
	}
	if (inpool_ok) return(FALSE);

	if ((i = number_leashed()) > 0) {
#if 0 /*JP*/
		pline_The("leash%s slip%s loose.",
			(i > 1) ? "es" : "",
			(i > 1) ? "" : "s");
#else
		pline("%sがゆるんだ。", jtrns_obj(TOOL_SYM,"leash"));
#endif
		unleash_all();
	}

	if (Amphibious || Swimming) {
		if (Amphibious) {
			if (flags.verbose)
/*JP
				pline("But you aren't drowning.");
*/
				pline("しかし、あなたは溺れなかった。");
			if (!Is_waterlevel(&u.uz)) {
				if (Hallucination)
/*JP
					Your("keel hits the bottom.");
*/
					You("底にニードロップを決めた。");
				else
/*JP
					You("touch bottom.");
*/
					You("底についた。");
			}
		}
		if (Punished) {
			unplacebc();
			placebc();
		}
		vision_recalc(2);	/* unsee old position */
		u.uinwater = 1;
		under_water(1);
		vision_full_recalc = 1;
		return(FALSE);
	}
	if ((Teleportation || can_teleport(youmonst.data)) &&
		    !u.usleep && (Teleport_control || rn2(3) < Luck+2)) {
#if 0 /*JP:T*/
		You("attempt a teleport spell.");	/* utcsri!carroll */
#else
		You("瞬間移動の呪文を唱えてみた。");
#endif
		if (!level.flags.noteleport) {
			(void) dotele();
			if(!is_pool(u.ux,u.uy))
				return(TRUE);
/*JP
		} else pline_The("attempted teleport spell fails.");
*/
		} else pline("瞬間移動の呪文は失敗した。");
	}
#ifdef STEED
	if (u.usteed) {
		dismount_steed(DISMOUNT_GENERIC);
		if(!is_pool(u.ux,u.uy))
			return(TRUE);
	}
#endif
	crawl_ok = FALSE;
	x = y = 0;		/* lint suppression */
	/* if sleeping, wake up now so that we don't crawl out of water
	   while still asleep; we can't do that the same way that waking
	   due to combat is handled; note unmul() clears u.usleep */
/*JP
	if (u.usleep) unmul("Suddenly you wake up!");
*/
	if (u.usleep) unmul("突然あなたは目が覚めた！");
	/* can't crawl if unable to move (crawl_ok flag stays false) */
	if (multi < 0 || (Upolyd && !youmonst.data->mmove)) goto crawl;
	/* look around for a place to crawl to */
	for (i = 0; i < 100; i++) {
		x = rn1(3,u.ux - 1);
		y = rn1(3,u.uy - 1);
		if (goodpos(x, y, &youmonst, 0)) {
			crawl_ok = TRUE;
			goto crawl;
		}
	}
	/* one more scan */
	for (x = u.ux - 1; x <= u.ux + 1; x++)
		for (y = u.uy - 1; y <= u.uy + 1; y++)
			if (goodpos(x, y, &youmonst, 0)) {
				crawl_ok = TRUE;
				goto crawl;
			}
 crawl:
	if (crawl_ok) {
		boolean lost = FALSE;
		/* time to do some strip-tease... */
		boolean succ = Is_waterlevel(&u.uz) ? TRUE :
				emergency_disrobe(&lost);

/*JP
		You("try to crawl out of the water.");
*/
		You("水からはいあがろうとした。");
		if (lost)
/*JP
			You("dump some of your gear to lose weight...");
*/
			You("体を軽くするためいくつか物を投げすてた．．．");
		if (succ) {
/*JP
			pline("Pheew!  That was close.");
*/
			pline("ハァハァ！よかった。");
			teleds(x,y,TRUE);
			return(TRUE);
		}
		/* still too much weight */
/*JP
		pline("But in vain.");
*/
		pline("が、無駄だった。");
	}
	u.uinwater = 1;
/*JP
	You("drown.");
*/
	You("溺れた。");
	/* [ALI] Vampires return to vampiric form on drowning.
	 */
	if (Upolyd && !Unchanging && Race_if(PM_VAMPIRE)) {
		rehumanize();
		u.uinwater = 0;
		/* should be unnecessary as spoteffects() should get called */
		/* You("fly up out of the water!"); */
		return (TRUE);
	}
	killer_format = KILLED_BY_AN;
	killer = (levl[u.ux][u.uy].typ == POOL || Is_medusa_level(&u.uz)) ?
/*JP
	    "pool of water" : "moat";
*/
	    "水たまりで" : "堀で";
	done(DROWNING);
	/* oops, we're still alive.  better get out of the water. */
	while (!safe_teleds(TRUE)) {
/*JP
		pline("You're still drowning.");
*/
		You("溺れている。");
		done(DROWNING);
	}
	if (u.uinwater) {
		u.uinwater = 0;
#if 0 /*JP*/
		You("find yourself back %s.", Is_waterlevel(&u.uz) ?
				"in an air bubble" : "on land");
#else
		You("いつのまにか%sにいるのに気がついた。", Is_waterlevel(&u.uz) ?
				"空気の泡の中" : "地面");
#endif
	}
	return(TRUE);
}

void
drain_en(n)
register int n;
{
	if (!u.uenmax) return;
/*JP
	You_feel("your magical energy drain away!");
*/
	You("魔法のエネルギーが吸いとられたような気がした！");
	u.uen -= n;
	if(u.uen < 0)  {
		u.uenmax += u.uen;
		if(u.uenmax < 0) u.uenmax = 0;
		u.uen = 0;
	}
	flags.botl = 1;
}

int
dountrap()	/* disarm a trap */
{
	if (near_capacity() >= HVY_ENCUMBER) {
/*JP
	    pline("You're too strained to do that.");
*/
	    pline("罠を解除しようにも物を持ちすぎている。");
	    return 0;
	}
	if ((nohands(youmonst.data) && !webmaker(youmonst.data)) || !youmonst.data->mmove) {
/*JP
	    pline("And just how do you expect to do that?");
*/
	    pline("いったい何を期待しているんだい？");
	    return 0;
	} else if (u.ustuck && sticks(youmonst.data)) {
/*JP
	    pline("You'll have to let go of %s first.", mon_nam(u.ustuck));
*/
	    pline("%sを手離さないことにはできない。", mon_nam(u.ustuck));
	    return 0;
	}
	if (u.ustuck || (welded(uwep) && bimanual(uwep))) {
#if 0 /*JP*/
	    Your("%s seem to be too busy for that.",
		 makeplural(body_part(HAND)));
#else
	    Your("そんなことをする余裕なんてない。");
#endif
	    return 0;
	}
	return untrap(FALSE);
}
#endif /* OVLB */
#ifdef OVL2

/* Probability of disabling a trap.  Helge Hafting */
STATIC_OVL int
untrap_prob(ttmp)
struct trap *ttmp;
{
	int chance = 3;

	/* Only spiders know how to deal with webs reliably */
	if (ttmp->ttyp == WEB && !webmaker(youmonst.data))
	 	chance = 30;
	if (Confusion || Hallucination) chance++;
	if (Blind) chance++;
	if (Stunned) chance += 2;
	if (Fumbling) chance *= 2;
	/* Your own traps are better known than others. */
	if (ttmp && ttmp->madeby_u) chance--;
	if (Role_if(PM_ROGUE)) {
	    if (rn2(2 * MAXULEV) < u.ulevel) chance--;
	    if (u.uhave.questart && chance > 1) chance--;
	} else if (Role_if(PM_RANGER) && chance > 1) chance--;
	return rn2(chance);
}

/* Replace trap with object(s).  Helge Hafting */
STATIC_OVL void
cnv_trap_obj(otyp, cnt, ttmp)
int otyp;
int cnt;
struct trap *ttmp;
{
	struct obj *otmp = mksobj(otyp, TRUE, FALSE);
	otmp->quan=cnt;
	otmp->owt = weight(otmp);
	/* Only dart traps are capable of being poisonous */
	if (otyp != DART)
	    otmp->opoisoned = 0;
	place_object(otmp, ttmp->tx, ttmp->ty);
	/* Sell your own traps only... */
	if (ttmp->madeby_u) sellobj(otmp, ttmp->tx, ttmp->ty);
	stackobj(otmp);
	newsym(ttmp->tx, ttmp->ty);
	deltrap(ttmp);
}

/* while attempting to disarm an adjacent trap, we've fallen into it */
STATIC_OVL void
move_into_trap(ttmp)
struct trap *ttmp;
{
	int bc;
	xchar x = ttmp->tx, y = ttmp->ty, bx, by, cx, cy;
	boolean unused;

	/* we know there's no monster in the way, and we're not trapped */
	if (!Punished || drag_ball(x, y, &bc, &bx, &by, &cx, &cy, &unused,
		TRUE)) {
	    u.ux0 = u.ux,  u.uy0 = u.uy;
	    u.ux = x,  u.uy = y;
	    u.umoved = TRUE;
	    newsym(u.ux0, u.uy0);
	    vision_recalc(1);
	    check_leash(u.ux0, u.uy0);
	    if (Punished) move_bc(0, bc, bx, by, cx, cy);
	    spoteffects(FALSE);	/* dotrap() */
	    exercise(A_WIS, FALSE);
	}
}

/* 0: doesn't even try
 * 1: tries and fails
 * 2: succeeds
 */
STATIC_OVL int
try_disarm(ttmp, force_failure)
struct trap *ttmp;
boolean force_failure;
{
	struct monst *mtmp = m_at(ttmp->tx,ttmp->ty);
	int ttype = ttmp->ttyp;
	boolean under_u = (!u.dx && !u.dy);
	boolean holdingtrap = (ttype == BEAR_TRAP || ttype == WEB);
	
	/* Test for monster first, monsters are displayed instead of trap. */
	if (mtmp && (!mtmp->mtrapped || !holdingtrap)) {
/*JP
		pline("%s is in the way.", Monnam(mtmp));
*/
		pline("そこには%sがいる。", Monnam(mtmp));
		return 0;
	}
	/* We might be forced to move onto the trap's location. */
	if (sobj_at(BOULDER, ttmp->tx, ttmp->ty)
				&& !Passes_walls && !under_u) {
/*JP
		There("is a boulder in your way.");
*/
		pline("そこには岩がある。");
		return 0;
	}
	/* duplicate tight-space checks from test_move */
	if (u.dx && u.dy &&
	    bad_rock(youmonst.data,u.ux,ttmp->ty) &&
	    bad_rock(youmonst.data,ttmp->tx,u.uy)) {
	    if ((invent && (inv_weight() + weight_cap() > 600)) ||
		bigmonst(youmonst.data)) {
		/* don't allow untrap if they can't get thru to it */
#if 0 /*JP*/
		You("are unable to reach the %s!",
		    defsyms[trap_to_defsym(ttype)].explanation);
#else
		You("%sに届かない！",
		    jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation));
#endif
		return 0;
	    }
	}
	/* untrappable traps are located on the ground. */
	if (!can_reach_floor()) {
#ifdef STEED
		if (u.usteed && P_SKILL(P_RIDING) < P_BASIC)
/*JP
			You("aren't skilled enough to reach from %s.",
*/
			You("まだ十分に技量を積んでいないので、%sから届かない。",
				mon_nam(u.usteed));
		else
#endif
#if 0 /*JP*/
		You("are unable to reach the %s!",
			defsyms[trap_to_defsym(ttype)].explanation);
#else
		You("%sに届かない！",
			jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation));
#endif
		return 0;
	}

	/* Will our hero succeed? */
	if (force_failure || untrap_prob(ttmp)) {
		if (rnl(5)) {
/*JP
		    pline("Whoops...");
*/
		    pline("うわっ．．．");
		    if (mtmp) {		/* must be a trap that holds monsters */
			if (ttype == BEAR_TRAP) {
			    if (mtmp->mtame) abuse_dog(mtmp);
			    if ((mtmp->mhp -= rnd(4)) <= 0) killed(mtmp);
			} else if (ttype == WEB) {
			    if (!webmaker(youmonst.data)) {
				struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB);
				if (ttmp2) {
/*JP
				    pline_The("webbing sticks to you. You're caught too!");
*/
				    pline_The("くもの巣があなたにからんできた。ますます捕まってしまった！");
				    dotrap(ttmp2, NOWEBMSG);
#ifdef STEED
				    if (u.usteed && u.utrap) {
					/* you, not steed, are trapped */
					dismount_steed(DISMOUNT_FELL);
				    }
#endif
				}
			    } else
/*JP
				pline("%s remains entangled.", Monnam(mtmp));
*/
				pline("%sはからまったままだ。", Monnam(mtmp));
			}
		    } else if (under_u) {
			dotrap(ttmp, 0);
		    } else {
			move_into_trap(ttmp);
		    }
		} else {
#if 0 /*JP*/
		    pline("%s %s is difficult to %s.",
			  ttmp->madeby_u ? "Your" : under_u ? "This" : "That",
			  defsyms[trap_to_defsym(ttype)].explanation,
			  (ttype == WEB) ? "remove" : "disarm");
#else
		    pline("%s%sを解除するのは困難だ。",
			  ttmp->madeby_u ? "あなたの仕掛けた" : under_u ? "この" : "その",
			  jtrns_obj('^', defsyms[trap_to_defsym(ttype)].explanation));
#endif
		}
		return 1;
	}
	return 2;
}

STATIC_OVL void
reward_untrap(ttmp, mtmp)
struct trap *ttmp;
struct monst *mtmp;
{
	if (!ttmp->madeby_u) {
	    if (rnl(10) < 8 && !mtmp->mpeaceful &&
		    !mtmp->msleeping && !mtmp->mfrozen &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN) {
		mtmp->mpeaceful = 1;
		set_malign(mtmp);	/* reset alignment */
/*JP
		pline("%s is grateful.", Monnam(mtmp));
*/
		pline("%sは喜んでいる。", Monnam(mtmp));
	    }
	    /* Helping someone out of a trap is a nice thing to do,
	     * A lawful may be rewarded, but not too often.  */
	    if (!rn2(3) && !rnl(8) && u.ualign.type == A_LAWFUL) {
		adjalign(1);
/*JP
		You_feel("that you did the right thing.");
*/
		You("正しいことをしたような気がした。");
	    }
	}
}

STATIC_OVL int
disarm_holdingtrap(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	struct monst *mtmp;
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;

	/* ok, disarm it. */

	/* untrap the monster, if any.
	   There's no need for a cockatrice test, only the trap is touched */
	if ((mtmp = m_at(ttmp->tx,ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
#if 0 /*JP*/
		You("remove %s %s from %s.", the_your[ttmp->madeby_u],
			(ttmp->ttyp == BEAR_TRAP) ? "bear trap" : "webbing",
			mon_nam(mtmp));
#else
		You("%s%sを%sからはずした。", set_you[ttmp->madeby_u],
			(ttmp->ttyp == BEAR_TRAP) ? "熊の罠" : "くもの巣",
			mon_nam(mtmp));
#endif
		reward_untrap(ttmp, mtmp);
	} else {
		if (ttmp->ttyp == BEAR_TRAP) {
/*JP
			You("disarm %s bear trap.", the_your[ttmp->madeby_u]);
*/
			You("%s熊の罠を解除した。", set_you[ttmp->madeby_u]);
			cnv_trap_obj(BEARTRAP, 1, ttmp);
		} else /* if (ttmp->ttyp == WEB) */ {
/*JP
			You("succeed in removing %s web.", the_your[ttmp->madeby_u]);
*/
			You("%sくもの巣を取り除いた。", set_you[ttmp->madeby_u]);
			deltrap(ttmp);
		}
	}
	newsym(u.ux + u.dx, u.uy + u.dy);
	return 1;
}

STATIC_OVL int
disarm_landmine(ttmp) /* Helge Hafting */
struct trap *ttmp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
/*JP
	You("disarm %s land mine.", the_your[ttmp->madeby_u]);
*/
	You("%s地雷を解除した。", set_you[ttmp->madeby_u]);
	cnv_trap_obj(LAND_MINE, 1, ttmp);
	return 1;
}

/* getobj will filter down to cans of grease and known potions of oil */
static NEARDATA const char oil[] = { ALL_CLASSES, TOOL_CLASS, POTION_CLASS, 0 };

/* it may not make much sense to use grease on floor boards, but so what? */
STATIC_OVL int
disarm_squeaky_board(ttmp)
struct trap *ttmp;
{
	struct obj *obj;
	boolean bad_tool;
	int fails;

	obj = getobj(oil, "untrap with");
	if (!obj) return 0;

	bad_tool = (obj->cursed ||
			((obj->otyp != POT_OIL || obj->lamplit) &&
			 (obj->otyp != CAN_OF_GREASE || !obj->spe)));

	fails = try_disarm(ttmp, bad_tool);
	if (fails < 2) return fails;

	/* successfully used oil or grease to fix squeaky board */
	if (obj->otyp == CAN_OF_GREASE) {
	    consume_obj_charge(obj, TRUE);
	} else {
	    useup(obj);	/* oil */
	    makeknown(POT_OIL);
	}
#if 0 /*JP*/
	You("repair the squeaky board.");	/* no madeby_u */
#else
	You("きしむ板を修理した。");	/* no madeby_u */
#endif
	deltrap(ttmp);
	newsym(u.ux + u.dx, u.uy + u.dy);
	more_experienced(1, 1, 5);
	newexplevel();
	return 1;
}

/* removes traps that shoot arrows, darts, etc. */
STATIC_OVL int
disarm_shooting_trap(ttmp, otyp)
struct trap *ttmp;
int otyp;
{
	int fails = try_disarm(ttmp, FALSE);

	if (fails < 2) return fails;
/*JP
	You("disarm %s trap.", the_your[ttmp->madeby_u]);
*/
	pline("%s罠を解除した。", set_you[ttmp->madeby_u]);
	cnv_trap_obj(otyp, 50-rnl(50), ttmp);
	return 1;
}

/* Is the weight too heavy?
 * Formula as in near_capacity() & check_capacity() */
STATIC_OVL int
try_lift(mtmp, ttmp, wt, stuff)
struct monst *mtmp;
struct trap *ttmp;
int wt;
boolean stuff;
{
	int wc = weight_cap();

	if (((wt * 2) / wc) >= HVY_ENCUMBER) {
#if 0 /*JP*/
	    pline("%s is %s for you to lift.", Monnam(mtmp),
		  stuff ? "carrying too much" : "too heavy");
#else
		pline("%sは%s持ちあげることができない。", Monnam(mtmp),
		  stuff ? "物を持ちすぎており" : "重すぎて");
#endif
	    if (!ttmp->madeby_u && !mtmp->mpeaceful && mtmp->mcanmove &&
		    !mindless(mtmp->data) &&
		    mtmp->data->mlet != S_HUMAN && rnl(10) < 3) {
		mtmp->mpeaceful = 1;
		set_malign(mtmp);		/* reset alignment */
/*JP
		pline("%s thinks it was nice of you to try.", Monnam(mtmp));
*/
		pline("%sはあなたの努力に感謝しているようだ。", Monnam(mtmp));
	    }
	    return 0;
	}
	return 1;
}

/* Help trapped monster (out of a (spiked) pit) */
STATIC_OVL int
help_monster_out(mtmp, ttmp)
struct monst *mtmp;
struct trap *ttmp;
{
	int wt;
	struct obj *otmp;
	boolean uprob;
#ifdef WEBB_DISINT
	boolean udied;
	boolean can_disint = (touch_disintegrates(mtmp->data) &&
	                      !mtmp->mcan &&
	                       mtmp->mhp>6);
#endif

	/*
	 * This works when levitating too -- consistent with the ability
	 * to hit monsters while levitating.
	 *
	 * Should perhaps check that our hero has arms/hands at the
	 * moment.  Helping can also be done by engulfing...
	 *
	 * Test the monster first - monsters are displayed before traps.
	 */
	if (!mtmp->mtrapped) {
/*JP
		pline("%s isn't trapped.", Monnam(mtmp));
*/
		pline("%sは罠にかかっていない。", Monnam(mtmp));
		return 0;
	}
	/* Do you have the necessary capacity to lift anything? */
	if (check_capacity((char *)0)) return 1;

	/* Will our hero succeed? */
	if ((uprob = untrap_prob(ttmp)) && !mtmp->msleeping && mtmp->mcanmove) {
/*JP
		You("try to reach out your %s, but %s backs away skeptically.",
*/
		You("%sを差し延べようとしたが%sは警戒している。",
			makeplural(body_part(ARM)),
			mon_nam(mtmp));
		return 1;
	}


	/* is it a cockatrice?... */
	if (touch_petrifies(mtmp->data) && !uarmg && !Stone_resistance) {
#if 0 /*JP*/
		You("grab the trapped %s using your bare %s.",
				mtmp->data->mname, makeplural(body_part(HAND)));
#else
		You("罠にかかっている%sを素%sで掴んだ。",
				jtrns_mon_gen(mtmp->data->mname, mtmp->female), body_part(HAND));
#endif

		if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			display_nhwindow(WIN_MESSAGE, FALSE);
		else {
			char kbuf[BUFSZ];

#if 0 /*JP*/
			Sprintf(kbuf, "trying to help %s out of a pit",
					an(mtmp->data->mname));
#else
			Sprintf(kbuf, "穴の外から%sを助けようとして",
				a_monnam(mtmp));
#endif
			instapetrify(kbuf);
			return 1;
		}
	}
	/* need to do cockatrice check first if sleeping or paralyzed */
	if (uprob) {
#ifdef WEBB_DISINT
		if (can_disint && (!(uarmg) || !oresist_disintegration(uarmg))) {
			char kbuf[BUFSZ];
#if 0 /*JP*/
			Sprintf(kbuf, "trying to help %s out of a pit",
					an(mtmp->data->mname));
#else
			Sprintf(kbuf, "穴の外から%sを助けようとして",
				a_monnam(mtmp));
#endif
/*JP
			You("try to grab %s, but...", mon_nam(mtmp));
*/
			You("%sをつかもうとしたが、しかし．．．", mon_nam(mtmp));
			if (uarmg) {
				destroy_arm(uarmg);
			} else {
				if (!instadisintegrate(kbuf))
/*JP
					You("cannot get a firm grasp.");
*/
					You("しっかりと握れなかった。");
			}
		} else
#endif
#if 0 /*JP*/
	    You("try to grab %s, but cannot get a firm grasp.",
		mon_nam(mtmp));
#else
	    You("%sをつかもうとしたが、しっかりと握れなかった。",
		mon_nam(mtmp));
#endif
	    if (mtmp->msleeping) {
		mtmp->msleeping = 0;
/*JP
		pline("%s awakens.", Monnam(mtmp));
*/
		pline("%sは目を覚ました。", Monnam(mtmp));
	    }
	    return 1;
	}

#if 0 /*JP*/
	You("reach out your %s and grab %s.",
	    makeplural(body_part(ARM)), mon_nam(mtmp));
#else
	You("%sを伸ばして%sをつかんだ。",
	    body_part(ARM), mon_nam(mtmp));
#endif

#ifdef WEBB_DISINT
	if (can_disint) {
		char kbuf[BUFSZ];
#if 0 /*JP*/
		Sprintf(kbuf, "trying to help %s out of a pit",
				an(mtmp->data->mname));
#else
		Sprintf(kbuf, "穴の外から%sを助けようとして",
			a_monnam(mtmp));
#endif
		if (uarmg) {
			if (!oresist_disintegration(uarmg)) {
				destroy_arm(uarmg);
				udied = (instadisintegrate(kbuf)) ? 1 : 0;
			}
		} else
			udied = (instadisintegrate(kbuf)) ? 1 : 0;
	}
#endif

	if (mtmp->msleeping) {
	    mtmp->msleeping = 0;
/*JP
	    pline("%s awakens.", Monnam(mtmp));
*/
	    pline("%sは目を覚ました。", Monnam(mtmp));
	} else if (mtmp->mfrozen && !rn2(mtmp->mfrozen)) {
	    /* After such manhandling, perhaps the effect wears off */
	    mtmp->mcanmove = 1;
	    mtmp->mfrozen = 0;
/*JP
	    pline("%s stirs.", Monnam(mtmp));
*/
	    pline("%sは動き始めた。", Monnam(mtmp));
	}
#ifdef WEBB_DISINT
	if (udied) return 1;
#endif
	/* is the monster too heavy? */
	wt = inv_weight() + mtmp->data->cwt;
	if (!try_lift(mtmp, ttmp, wt, FALSE)) return 1;

	/* is the monster with inventory too heavy? */
	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		wt += otmp->owt;
	if (!try_lift(mtmp, ttmp, wt, TRUE)) return 1;

/*JP
	You("pull %s out of the pit.", mon_nam(mtmp));
*/
	You("%sを落し穴からひっぱった。", mon_nam(mtmp));
	mtmp->mtrapped = 0;
	fill_pit(mtmp->mx, mtmp->my);
	reward_untrap(ttmp, mtmp);
	return 1;
}

int
untrap(force)
boolean force;
{
	register struct obj *otmp;
	register boolean confused = (Confusion > 0 || Hallucination > 0);
	register int x,y;
	int ch;
	struct trap *ttmp;
	struct monst *mtmp;
	boolean trap_skipped = FALSE;
	boolean box_here = FALSE;
	boolean deal_with_floor_trap = FALSE;
	char the_trap[BUFSZ], qbuf[QBUFSZ];
	int containercnt = 0;

	if(!getdir((char *)0)) return(0);
	x = u.ux + u.dx;
	y = u.uy + u.dy;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
		if(Is_box(otmp) && !u.dx && !u.dy) {
			box_here = TRUE;
			containercnt++;
			if (containercnt > 1) break;
		}
	}

	if ((ttmp = t_at(x,y)) && ttmp->tseen) {
		deal_with_floor_trap = TRUE;
/*JP
		Strcpy(the_trap, the(defsyms[trap_to_defsym(ttmp->ttyp)].explanation));
*/
		Strcpy(the_trap, jtrns_obj('^', defsyms[trap_to_defsym(ttmp->ttyp)].explanation));
		if (box_here) {
			if (ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT) {
#if 0 /*JP*/
			    You_cant("do much about %s%s.",
					the_trap, u.utrap ?
					" that you're stuck in" :
					" while standing on the edge of it");
#else
			    pline("%s%sに対してはたいしたことはできない。",
					u.utrap ?
					"自分がはまっている" :
					"すぐそばの", the_trap);
#endif
			    trap_skipped = TRUE;
			    deal_with_floor_trap = FALSE;
			} else {
#if 0 /*JP*/
			    Sprintf(qbuf, "There %s and %s here. %s %s?",
				(containercnt == 1) ? "is a container" : "are containers",
				an(defsyms[trap_to_defsym(ttmp->ttyp)].explanation),
				ttmp->ttyp == WEB ? "Remove" : "Disarm", the_trap);
#else
			    Sprintf(qbuf, "箱と%sがある。%s？",
				jtrns_obj('^', an(defsyms[trap_to_defsym(ttmp->ttyp)].explanation)),
				ttmp->ttyp == WEB ? "取り除く" : "解除する");
#endif
			    switch (ynq(qbuf)) {
				case 'q': return(0);
				case 'n': trap_skipped = TRUE;
					  deal_with_floor_trap = FALSE;
					  break;
			    }
			}
		}
		if (deal_with_floor_trap) {
		    if (u.utrap) {
#if 0 /*JP*/
			You("cannot deal with %s while trapped%s!", the_trap,
				(x == u.ux && y == u.uy) ? " in it" : "");
#else
			pline("罠にかかっている間は罠を解除できない！");
#endif
			return 1;
		    }
		    switch(ttmp->ttyp) {
			case BEAR_TRAP:
			case WEB:
				return disarm_holdingtrap(ttmp);
			case LANDMINE:
				return disarm_landmine(ttmp);
			case SQKY_BOARD:
				return disarm_squeaky_board(ttmp);
			case DART_TRAP:
				return disarm_shooting_trap(ttmp, DART);
			case ARROW_TRAP:
				return disarm_shooting_trap(ttmp, ARROW);
			case PIT:
			case SPIKED_PIT:
				if (!u.dx && !u.dy) {
/*JP
				    You("are already on the edge of the pit.");
*/
				    You("もう落し穴の端にいる。");
				    return 0;
				}
				if (!(mtmp = m_at(x,y))) {
/*JP
				    pline("Try filling the pit instead.");
*/
				    pline("なんとか埋めることを考えてみたら？");
				    return 0;
				}
				return help_monster_out(mtmp, ttmp);
			default:
/*JP
				You("cannot disable %s trap.", (u.dx || u.dy) ? "that" : "this");
*/
				pline("%s罠は解除できない。", (u.dx || u.dy) ? "その" : "この");
				return 0;
		    }
		}
	} /* end if */

	if(!u.dx && !u.dy) {
	    for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if(Is_box(otmp)) {
#if 0 /*JP*/
		    Sprintf(qbuf, "There is %s here. Check it for traps?",
			safe_qbuf("", sizeof("There is  here. Check it for traps?"),
				doname(otmp), an(simple_typename(otmp->otyp)), "a box"));
#else
		    Sprintf(qbuf, "%sがある。わなを調べますか？",
			safe_qbuf("", sizeof("がある。わなを調べますか？"),
				doname(otmp), an(simple_typename(otmp->otyp)), "箱"));
#endif
		    switch (ynq(qbuf)) {
			case 'q': return(0);
			case 'n': continue;
		    }
#ifdef STEED
		    if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
/*JP
			You("aren't skilled enough to reach from %s.",
*/
			You("まだ十分に技量を積んでいないので、%sから届かない。",
				mon_nam(u.usteed));
			return(0);
		    }
#endif
		    if((otmp->otrapped && (force || (!confused
				&& rn2(MAXULEV + 1 - u.ulevel) < 10)))
		       || (!force && confused && !rn2(3))) {
/*JP
			You("find a trap on %s!", the(xname(otmp)));
*/
			pline("%sに罠を発見した！", the(xname(otmp)));
			if (!confused) exercise(A_WIS, TRUE);

/*JP
			switch (ynq("Disarm it?")) {
*/
			switch (ynq("解除しますか？")) {
			    case 'q': return(1);
			    case 'n': trap_skipped = TRUE;  continue;
			}

			if(otmp->otrapped) {
			    exercise(A_DEX, TRUE);
			    ch = ACURR(A_DEX) + u.ulevel;
			    if (Role_if(PM_ROGUE)) ch *= 2;
			    if(!force && (confused || Fumbling ||
				rnd(75+level_difficulty()/2) > ch)) {
				(void) chest_trap(otmp, FINGER, TRUE);
			    } else {
/*JP
				You("disarm it!");
*/
				You("解除した！");
				otmp->otrapped = 0;
			    }
/*JP
			} else pline("That %s was not trapped.", xname(otmp));
*/
			} else pline("その%sに罠はない。", xname(otmp));
			return(1);
		    } else {
/*JP
			You("find no traps on %s.", the(xname(otmp)));
*/
			pline("罠を発見できなかった。");
			return(1);
		    }
		}

#if 0 /*JP*/
	    You(trap_skipped ? "find no other traps here."
			     : "know of no traps here.");
#else
	    You(trap_skipped ? "他の罠を見つけられなかった。"
			     : "ここに罠がないことを知っている。");
#endif
	    return(0);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	if (!IS_DOOR(levl[x][y].typ)) {
	    if ((ttmp = t_at(x,y)) && ttmp->tseen)
/*JP
		You("cannot disable that trap.");
*/
		You("罠を解除できなかった。");
	    else
/*JP
		You("know of no traps there.");
*/
		You("そこに罠がないことを知っている。");
	    return(0);
	}

	switch (levl[x][y].doormask) {
	    case D_NODOOR:
/*JP
		You("%s no door there.", Blind ? "feel" : "see");
*/
		pline("そこには扉がない%s。", Blind ? "ようだ" : "ように見える");
		return(0);
	    case D_ISOPEN:
/*JP
		pline("This door is safely open.");
*/
		pline("その扉は安全に開いている。");
		return(0);
	    case D_BROKEN:
/*JP
		pline("This door is broken.");
*/
		pline("その扉は壊れている。");
		return(0);
	}

	if ((levl[x][y].doormask & D_TRAPPED
	     && (force ||
		 (!confused && rn2(MAXULEV - u.ulevel + 11) < 10)))
	    || (!force && confused && !rn2(3))) {
/*JP
		You("find a trap on the door!");
*/
		pline("扉に罠を発見した！");
		exercise(A_WIS, TRUE);
/*JP
		if (ynq("Disarm it?") != 'y') return(1);
*/
		if (ynq("解除しますか？") != 'y') return(1);
		if (levl[x][y].doormask & D_TRAPPED) {
		    ch = 15 + (Role_if(PM_ROGUE) ? u.ulevel*3 : u.ulevel);
		    exercise(A_DEX, TRUE);
		    if(!force && (confused || Fumbling ||
				     rnd(75+level_difficulty()/2) > ch)) {
/*JP
			You("set it off!");
*/
		        You("スイッチを入れてしまった！");
/*JP
			b_trapped("door", FINGER);
*/
			b_trapped("扉", FINGER);
			levl[x][y].doormask = D_NODOOR;
			unblock_point(x, y);
			newsym(x, y);
			/* (probably ought to charge for this damage...) */
			if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
		    } else {
/*JP
			You("disarm it!");
*/
		        You("解除した！");
			levl[x][y].doormask &= ~D_TRAPPED;
		    }
/*JP
		} else pline("This door was not trapped.");
*/
		} else pline("扉に罠はなかった。");
		return(1);
	} else {
/*JP
		You("find no traps on the door.");
*/
		pline("扉に罠を発見できなかった。");
		return(1);
	}
}
#endif /* OVL2 */
#ifdef OVLB

/* only called when the player is doing something to the chest directly */
boolean
chest_trap(obj, bodypart, disarm)
register struct obj *obj;
register int bodypart;
boolean disarm;
{
	register struct obj *otmp = obj, *otmp2;
	char	buf[80];
	const char *msg;
	coord cc;

	if (get_obj_location(obj, &cc.x, &cc.y, 0))	/* might be carried */
	    obj->ox = cc.x,  obj->oy = cc.y;

	otmp->otrapped = 0;	/* trap is one-shot; clear flag first in case
				   chest kills you and ends up in bones file */
/*JP
	You(disarm ? "set it off!" : "trigger a trap!");
*/
	You(disarm ? "スイッチを入れてしまった！" : "罠にひっかかった！");
	display_nhwindow(WIN_MESSAGE, FALSE);
	if (Luck > -13 && rn2(13+Luck) > 7) {	/* saved by luck */
	    /* trap went off, but good luck prevents damage */
	    switch (rn2(13)) {
		case 12:
/*JP
		case 11:  msg = "explosive charge is a dud";  break;
*/
		case 11:  msg = "爆発は不発だった";  break;
		case 10:
/*JP
		case  9:  msg = "electric charge is grounded";  break;
*/
		case  9:  msg = "電撃が放出されたがアースされていた";  break;
		case  8:
/*JP
		case  7:  msg = "flame fizzles out";  break;
*/
		case  7:  msg = "炎はシューっと消えた";  break;
		case  6:
		case  5:
/*JP
		case  4:  msg = "poisoned needle misses";  break;
*/
		case  4:  msg = "毒針は刺さらなかった";  break;
		case  3:
		case  2:
		case  1:
/*JP
		case  0:  msg = "gas cloud blows away";  break;
*/
		case  0:  msg = "ガス雲は吹き飛んだ";  break;
		default:  warning("chest disarm bug");  msg = (char *)0;
			  break;
	    }
/*JP
	    if (msg) pline("But luckily the %s!", msg);
*/
	    if (msg) pline("が運のよいことに%s！", msg);
	} else {
	    switch(rn2(20) ? ((Luck >= 13) ? 0 : rn2(13-Luck)) : rn2(26)) {
		case 25:
		case 24:
		case 23:
		case 22:
		case 21: {
			  struct monst *shkp = 0;
			  long loss = 0L;
			  boolean costly, insider;
			  register xchar ox = obj->ox, oy = obj->oy;

			  /* the obj location need not be that of player */
			  costly = (costly_spot(ox, oy) &&
				   (shkp = shop_keeper(*in_rooms(ox, oy,
				    SHOPBASE))) != (struct monst *)0);
			  insider = (*u.ushops && inside_shop(u.ux, u.uy) &&
				    *in_rooms(ox, oy, SHOPBASE) == *u.ushops);

/*JP
			  pline("%s!", Tobjnam(obj, "explode"));
*/
			  pline("%sは爆発した！", xname(obj));
/*JP
			  Sprintf(buf, "exploding %s", xname(obj));
*/
			  Sprintf(buf, "%sの爆発で", xname(obj));

			  if(costly)
			      loss += stolen_value(obj, ox, oy,
						(boolean)shkp->mpeaceful, TRUE);
			  delete_contents(obj);
			  /* we're about to delete all things at this location,
			   * which could include the ball & chain.
			   * If we attempt to call unpunish() in the
			   * for-loop below we can end up with otmp2
			   * being invalid once the chain is gone.
			   * Deal with ball & chain right now instead.
			   */
			  if (Punished && !carried(uball) &&
				((uchain->ox == u.ux && uchain->oy == u.uy) ||
				 (uball->ox == u.ux && uball->oy == u.uy)))
				unpunish();

			  for(otmp = level.objects[u.ux][u.uy];
							otmp; otmp = otmp2) {
			      otmp2 = otmp->nexthere;
			      if(costly)
				  loss += stolen_value(otmp, otmp->ox,
					  otmp->oy, (boolean)shkp->mpeaceful,
					  TRUE);
			      delobj(otmp);
			  }
			  wake_nearby();
			  losehp(d(6,6), buf, KILLED_BY_AN);
			  exercise(A_STR, FALSE);
			  if(costly && loss) {
			      if(insider)
#if 0 /*JP*/
			      You("owe %ld %s for objects destroyed.",
#else
			      You("器物破損で%ld%sの借りをつくった。",
#endif
							loss, currency(loss));
			      else {
#if 0 /*JP*/
				  You("caused %ld %s worth of damage!",
#else
				  You("%ld%s分の損害を引きおこした！",
#endif
							loss, currency(loss));
				  make_angry_shk(shkp, ox, oy);
			      }
			  }
			  return TRUE;
			}
		case 20:
		case 19:
		case 18:
		case 17:
/*JP
			pline("A cloud of noxious gas billows from %s.",
*/
			pline("有毒ガスが%sから渦まいた。",
							the(xname(obj)));
/*JP
			poisoned("gas cloud", A_STR, "cloud of poison gas",15);
*/
			poisoned("ガス雲", A_STR, "毒ガスの雲",15);
			exercise(A_CON, FALSE);
			break;
		case 16:
		case 15:
		case 14:
		case 13:
/*JP
			You_feel("a needle prick your %s.",body_part(bodypart));
*/
			You("%sチクッという痛みを感じた。", body_part(bodypart));
/*JP
			poisoned("needle", A_CON, "poisoned needle",10);
*/
			poisoned("針", A_CON, "毒針",10);
			exercise(A_CON, FALSE);
			break;
		case 12:
		case 11:
		case 10:
		case 9:
			dofiretrap(obj);
			break;
		case 8:
		case 7:
		case 6: {
			int dmg;

/*JP
			You("are jolted by a surge of electricity!");
*/
			You("電気ショックをくらった！");
			if(Shock_resistance)  {
			    shieldeff(u.ux, u.uy);
/*JP
			    You("don't seem to be affected.");
*/
			    pline("しかしあなたは影響を受けない。");
			    dmg = 0;
			} else
			    dmg = d(4, 4);
			destroy_item(RING_CLASS, AD_ELEC);
			destroy_item(WAND_CLASS, AD_ELEC);
/*JP
			if (dmg) losehp(dmg, "electric shock", KILLED_BY_AN);
*/
			if (dmg) losehp(dmg, "電気ショックで", KILLED_BY_AN);
			break;
		      }
		case 5:
		case 4:
		case 3:
			if (!Free_action) {                        
/*JP
			pline("Suddenly you are frozen in place!");
*/
			pline("突然その場で動けなくなった！");
/*JP
			nomul(-d(5, 6), "frozen by a trap");
*/
			nomul(-d(5, 6), "罠で動けなくなった時に");
			exercise(A_DEX, FALSE);
			nomovemsg = You_can_move_again;
/*JP
			} else You("momentarily stiffen.");
*/
			} else You("一瞬硬直した。");
			break;
		case 2:
		case 1:
		case 0:
#if 0 /*JP:T*/
			pline("A cloud of %s gas billows from %s.",
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				rndcolor(), the(xname(obj)));
#else
			pline("%sガス雲が%sの底で渦まいた。",
				Blind ? blindgas[rn2(SIZE(blindgas))] :
				jconj_adj(rndcolor()), the(xname(obj)));
#endif
			if(!Stunned) {
			    if (Hallucination)
/*JP
				pline("What a groovy feeling!");
*/
				pline("なんて素敵なんだ！");
			    else if (Blind)
#if 0 /*JP:T*/
				You("%s and get dizzy...",
				    stagger(youmonst.data, "stagger"));
#else
				You("くらくらし、めまいがした．．．");
#endif
			    else
#if 0 /*JP:T*/
				You("%s and your vision blurs...",
				    stagger(youmonst.data, "stagger"));
#else
				You("くらくらし、景色がぼやけてきた．．．");
#endif
			}
			make_stunned(HStun + rn1(7, 16),FALSE);
			(void) make_hallucinated(HHallucination + rn1(5, 16),FALSE,0L);
			break;
		default: warning("bad chest trap");
			break;
	    }
	    bot();			/* to get immediate botl re-display */
	}

	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

struct trap *
t_at(x,y)
register int x, y;
{
	register struct trap *trap = ftrap;
	while(trap) {
		if(trap->tx == x && trap->ty == y) return(trap);
		trap = trap->ntrap;
	}
	return((struct trap *)0);
}

#endif /* OVL0 */
#ifdef OVLB

void
deltrap(trap)
register struct trap *trap;
{
	register struct trap *ttmp;

	if(trap == ftrap)
		ftrap = ftrap->ntrap;
	else {
		for(ttmp = ftrap; ttmp->ntrap != trap; ttmp = ttmp->ntrap) ;
		ttmp->ntrap = trap->ntrap;
	}
	dealloc_trap(trap);
}

boolean
delfloortrap(ttmp)
register struct trap *ttmp;
{
	/* Destroy a trap that emanates from the floor. */
	/* some of these are arbitrary -dlc */
	if (ttmp && ((ttmp->ttyp == SQKY_BOARD) ||
		     (ttmp->ttyp == BEAR_TRAP) ||
		     (ttmp->ttyp == LANDMINE) ||
		     (ttmp->ttyp == FIRE_TRAP) ||
		     (ttmp->ttyp == PIT) ||
		     (ttmp->ttyp == SPIKED_PIT) ||
		     (ttmp->ttyp == HOLE) ||
		     (ttmp->ttyp == TRAPDOOR) ||
		     (ttmp->ttyp == TELEP_TRAP) ||
		     (ttmp->ttyp == LEVEL_TELEP) ||
		     (ttmp->ttyp == WEB) ||
		     (ttmp->ttyp == MAGIC_TRAP) ||
		     (ttmp->ttyp == ANTI_MAGIC))) {
	    register struct monst *mtmp;

	    if (ttmp->tx == u.ux && ttmp->ty == u.uy) {
		u.utrap = 0;
		u.utraptype = 0;
	    } else if ((mtmp = m_at(ttmp->tx, ttmp->ty)) != 0) {
		mtmp->mtrapped = 0;
	    }
	    deltrap(ttmp);
	    return TRUE;
	} else
	    return FALSE;
}

/* used for doors (also tins).  can be used for anything else that opens. */
void
b_trapped(item, bodypart)
register const char *item;
register int bodypart;
{
	register int lvl = level_difficulty();
	int dmg = rnd(5 + (lvl < 5 ? lvl : 2+lvl/2));

/*JP
	pline("KABOOM!!  %s was booby-trapped!", The(item));
*/
	pline("ちゅどーん！！%sにブービートラップが仕掛けられていた！", The(item));
	wake_nearby();
/*JP
	losehp(dmg, "explosion", KILLED_BY_AN);
*/
	losehp(dmg, "ブービートラップの爆発で", KILLED_BY_AN);
	exercise(A_STR, FALSE);
	if (bodypart) exercise(A_CON, FALSE);
	make_stunned(HStun + dmg, TRUE);
}

/* Monster is hit by trap. */
/* Note: doesn't work if both obj and d_override are null */
STATIC_OVL boolean
thitm(tlev, mon, obj, d_override, nocorpse)
int tlev;
struct monst *mon;
struct obj *obj;
int d_override;
boolean nocorpse;
{
	int strike;
	boolean trapkilled = FALSE;

	if (d_override) strike = 1;
	else if (obj) strike = (find_mac(mon) + tlev + obj->spe <= rnd(20));
	else strike = (find_mac(mon) + tlev <= rnd(20));

	/* Actually more accurate than thitu, which doesn't take
	 * obj->spe into account.
	 */
	if(!strike) {
		if (obj && cansee(mon->mx, mon->my))
#if 0 /*JP*/
		    pline("%s is almost hit by %s!", Monnam(mon), doname(obj));
#else
		    pline("もう少しで%sが%sに命中するところだった！", doname(obj),
			      Monnam(mon));
#endif
	} else {
		int dam = 1;

		if (obj && cansee(mon->mx, mon->my))
/*JP
			pline("%s is hit by %s!", Monnam(mon), doname(obj));
*/
			pline("%sが%sに命中した！", doname(obj), Monnam(mon));
		if (d_override) dam = d_override;
		else if (obj) {
			dam = dmgval(obj, mon);
			if (dam < 1) dam = 1;
		}

#ifdef WEBB_DISINT
		if (obj && touch_disintegrates(mon->data) &&
				!mon->mcan && (mon->mhp > 6) && !oresist_disintegration(obj)) {
			dam = obj->owt;
			weight_dmg(dam);
			if (cansee(mon->mx, mon->my))
/*JP
				pline("It disintegrates!");
*/
				pline("それは粉砕された！");
			dealloc_obj(obj);
			obj = 0;
		}
#endif

		if ((mon->mhp -= dam) <= 0) {
			int xx = mon->mx;
			int yy = mon->my;

			monkilled(mon, "", nocorpse ? -AD_RBRE : AD_PHYS);
			if (mon->mhp <= 0) {
				newsym(xx, yy);
				trapkilled = TRUE;
			}
		}
	}
	if (obj && (!strike || d_override)) {
		place_object(obj, mon->mx, mon->my);
		stackobj(obj);
	} else if (obj) dealloc_obj(obj);

	return trapkilled;
}

boolean
unconscious()
{
/*
**	もう、かんべんしてよー。こんなコーディング．．．sigh．．．
**
**	You regain con  --  eat.c(2), potion.c(1)
**	You are consci  --  eat.c(2)
*/
	return((boolean)(multi < 0 && (!nomovemsg ||
		u.usleep ||
#if 1 /*JP*/
		!strncmp(nomovemsg,"あなたは正気づいた", 18) ||
		!strncmp(nomovemsg,"あなたはまた正気づ", 18) ||
#endif
		!strncmp(nomovemsg,"You regain con", 14) ||
		!strncmp(nomovemsg,"You are consci", 14))));
}

/*JP
static const char lava_killer[] = "molten lava";
*/
static const char lava_killer[] = "どろどろの溶岩で";

boolean
lava_effects()
{
    register struct obj *obj, *obj2;
    int dmg;
    boolean usurvive;

    burn_away_slime();
    if (likes_lava(youmonst.data)) return FALSE;

    if (!Fire_resistance) {
	if(Wwalking) {
	    dmg = d(6,6);
/*JP
	    pline_The("lava here burns you!");
*/
	    pline("溶岩があなたを焼きつくした！");
	    if(dmg < u.uhp) {
		losehp(dmg, lava_killer, KILLED_BY);
		goto burn_stuff;
	    }
	} else
/*JP
	    You("fall into the lava!");
*/
	    You("溶岩に落ちた！");

	usurvive = Lifesaved || discover;
#ifdef WIZARD
	if (wizard) usurvive = TRUE;
#endif
	for(obj = invent; obj; obj = obj2) {
	    obj2 = obj->nobj;
	    if(is_organic(obj) && !obj->oerodeproof) {
		if(obj->owornmask) {
		    if (usurvive)
/*JP
			Your("%s into flame!", aobjnam(obj, "burst"));
*/
			Your("%sは燃えた！", xname(obj));

		    if(obj == uarm) (void) Armor_gone();
		    else if(obj == uarmc) (void) Cloak_off();
		    else if(obj == uarmh) (void) Helmet_off();
		    else if(obj == uarms) (void) Shield_off();
		    else if(obj == uarmg) (void) Gloves_off();
		    else if(obj == uarmf) (void) Boots_off();
#ifdef TOURIST
		    else if(obj == uarmu) setnotworn(obj);
#endif
		    else if(obj == uleft) Ring_gone(obj);
		    else if(obj == uright) Ring_gone(obj);
		    else if(obj == ublindf) Blindf_off(obj);
		    else if(obj == uamul) Amulet_off();
		    else if(obj == uwep) uwepgone();
		    else if (obj == uquiver) uqwepgone();
		    else if (obj == uswapwep) uswapwepgone();
		}
		useupall(obj);
	    }
	}

	/* s/he died... */
	u.uhp = -1;
	killer_format = KILLED_BY;
	killer = lava_killer;
/*JP
	You("burn to a crisp...");
*/
	You("燃えてパリパリになった．．．");
	done(BURNING);
	while (!safe_teleds(TRUE)) {
/*JP
		pline("You're still burning.");
*/
		You("まだ燃えている。");
		done(BURNING);
	}
/*JP
	You("find yourself back on solid %s.", surface(u.ux, u.uy));
*/
	You("いつのまにか固い%sに戻っていた。", surface(u.ux, u.uy));
	return(TRUE);
    }

    if (!Wwalking) {
	u.utrap = rn1(4, 4) + (rn1(4, 12) << 8);
	u.utraptype = TT_LAVA;
/*JP
	You("sink into the lava, but it only burns slightly!");
*/
	You("溶岩に沈んだが、ちょっと焦げただけだ！");
	if (u.uhp > 1)
	    losehp(1, lava_killer, KILLED_BY);
    }
    /* just want to burn boots, not all armor; destroy_item doesn't work on
       armor anyway */
burn_stuff:
    if(uarmf && !uarmf->oerodeproof && is_organic(uarmf)) {
	/* save uarmf value because Boots_off() sets uarmf to null */
	obj = uarmf;
/*JP
	Your("%s bursts into flame!", xname(obj));
*/
	Your("%sは燃えた！", xname(obj));
	(void) Boots_off();
	useup(obj);
    }
    destroy_item(SCROLL_CLASS, AD_FIRE);
    destroy_item(SPBOOK_CLASS, AD_FIRE);
    destroy_item(POTION_CLASS, AD_FIRE);
    return(FALSE);
}

#endif /* OVLB */

/*trap.c*/
