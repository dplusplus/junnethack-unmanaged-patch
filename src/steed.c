/*	SCCS Id: @(#)steed.c	3.4	2003/01/10	*/
/* Copyright (c) Kevin Hugo, 1998-1999. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2003
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"


#ifdef STEED

/* Monsters that might be ridden */
static NEARDATA const char steeds[] = {
	S_QUADRUPED, S_UNICORN, S_ANGEL, S_CENTAUR, S_DRAGON, S_JABBERWOCK, '\0'
};

STATIC_DCL boolean FDECL(landing_spot, (coord *, int, int));

/* caller has decided that hero can't reach something while mounted */
void
rider_cant_reach()
{
/*JP
     You("aren't skilled enough to reach from %s.", y_monnam(u.usteed));
*/
     You("まだ十分に技量を積んでいないので、%sから届かない。", y_monnam(u.usteed));
}

/*** Putting the saddle on ***/

/* Can this monster wear a saddle? */
boolean
can_saddle(mtmp)
	struct monst *mtmp;
{
	struct permonst *ptr = mtmp->data;

	return (index(steeds, ptr->mlet) && (ptr->msize >= MZ_MEDIUM) &&
			(!humanoid(ptr) || ptr->mlet == S_CENTAUR) &&
			!amorphous(ptr) && !noncorporeal(ptr) &&
			!is_whirly(ptr) && !unsolid(ptr));
}


int
use_saddle(otmp)
	struct obj *otmp;
{
	struct monst *mtmp;
	struct permonst *ptr;
	int chance;
	const char *s;


	/* Can you use it? */
	if (nohands(youmonst.data)) {
#if 0 /*JP*/
		You("have no hands!");	/* not `body_part(HAND)' */
#else
		pline("あなたには手がない！");	/* not `body_part(HAND)' */
#endif
		return 0;
	} else if (!freehand()) {
/*JP
		You("have no free %s.", body_part(HAND));
*/
		Your("%sは塞がっている。", body_part(HAND));
		return 0;
	}

	/* Select an animal */
	if (u.uswallow || Underwater || !getdir((char *)0)) {
	    pline(Never_mind);
	    return 0;
	}
	if (!u.dx && !u.dy) {
#if 0 /*JP*/
	    pline("Saddle yourself?  Very funny...");
#else
	    pline("自分自身に鞍？おもしろい．．．");
#endif
	    return 0;
	}
	if (!isok(u.ux+u.dx, u.uy+u.dy) ||
			!(mtmp = m_at(u.ux+u.dx, u.uy+u.dy)) ||
			!canspotmon(mtmp)) {
/*JP
	    pline("I see nobody there.");
*/
	    pline("そこには誰もいないように見える。");
	    return 1;
	}

	/* Is this a valid monster? */
	if (mtmp->misc_worn_check & W_SADDLE ||
			which_armor(mtmp, W_SADDLE)) {
/*JP
	    pline("%s doesn't need another one.", Monnam(mtmp));
*/
	    pline("%sはもう鞍が取りつけられている。", Monnam(mtmp));
	    return 1;
	}
	ptr = mtmp->data;
	if (touch_petrifies(ptr) && !uarmg && !Stone_resistance) {
	    char kbuf[BUFSZ];

/*JP
	    You("touch %s.", mon_nam(mtmp));
*/
	    You("%sに触れた。", mon_nam(mtmp));
 	    if (!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
/*JP
		Sprintf(kbuf, "attempting to saddle %s", an(mtmp->data->mname));
*/
		Sprintf(kbuf, "%sに鞍を取りつけようとして", a_monnam(mtmp));
		instapetrify(kbuf);
 	    }
	}
#ifdef WEBB_DISINT
	if (touch_disintegrates(ptr)){
		char kbuf[BUFSZ];
		if(!oresist_disintegration(otmp)){
/*JP
			pline("%s disintegrates!", Yname2(otmp));
*/
			pline("%sは粉々になった！", Yname2(otmp));
			useup(otmp);
		}
/*JP
		Sprintf(kbuf,"attempting to saddle %s", a_monnam(mtmp));
*/
		Sprintf(kbuf,"%sに鞍を取りつけようとして", a_monnam(mtmp));
		instadisintegrate(kbuf);
	}
#endif

	if (ptr == &mons[PM_INCUBUS] || ptr == &mons[PM_SUCCUBUS]) {
/*JP
	    pline("Shame on you!");
*/
	    pline("みっともないぞ！");
	    exercise(A_WIS, FALSE);
	    return 1;
	}
	if (mtmp->isminion || mtmp->isshk || mtmp->ispriest ||
			mtmp->isgd || mtmp->iswiz) {
/*JP
	    pline("I think %s would mind.", mon_nam(mtmp));
*/
	    pline("%sは嫌がっているようだ。", mon_nam(mtmp));
	    return 1;
	}
	if (!can_saddle(mtmp)) {
/*JP
		You_cant("saddle such a creature.");
*/
		You("その生き物に鞍はとりつけられない。");
		return 1;
	}

	/* Calculate your chance */
	chance = ACURR(A_DEX) + ACURR(A_CHA)/2 + 2*mtmp->mtame;
	chance += u.ulevel * (mtmp->mtame ? 20 : 5);
	if (!mtmp->mtame) chance -= 10*mtmp->m_lev;
	if (Role_if(PM_KNIGHT))
	    chance += 20;
	switch (P_SKILL(P_RIDING)) {
	case P_ISRESTRICTED:
	case P_UNSKILLED:
	default:
	    chance -= 20;	break;
	case P_BASIC:
	    break;
	case P_SKILLED:
	    chance += 15;	break;
	case P_EXPERT:
	    chance += 30;	break;
	}
	if (Confusion || Fumbling || Glib)
	    chance -= 20;
	else if (uarmg &&
		(s = OBJ_DESCR(objects[uarmg->otyp])) != (char *)0 &&
		!strncmp(s, "riding ", 7))
	    /* Bonus for wearing "riding" (but not fumbling) gloves */
	    chance += 10;
	else if (uarmf &&
		(s = OBJ_DESCR(objects[uarmf->otyp])) != (char *)0 &&
		!strncmp(s, "riding ", 7))
	    /* ... or for "riding boots" */
	    chance += 10;
	if (otmp->cursed)
	    chance -= 50;

	/* Make the attempt */
	if (rn2(100) < chance) {
/*JP
	    You("put the saddle on %s.", mon_nam(mtmp));
*/
	    You("鞍を%sに取りつけた。", mon_nam(mtmp));
	    if (otmp->owornmask) remove_worn_item(otmp, FALSE);
	    freeinv(otmp);
	    /* mpickobj may free otmp it if merges, but we have already
	       checked for a saddle above, so no merger should happen */
	    (void) mpickobj(mtmp, otmp);
	    mtmp->misc_worn_check |= W_SADDLE;
	    otmp->owornmask = W_SADDLE;
	    otmp->leashmon = mtmp->m_id;
	    update_mon_intrinsics(mtmp, otmp, TRUE, FALSE);
	} else
/*JP
	    pline("%s resists!", Monnam(mtmp));
*/
	    pline("%sは拒否した！", Monnam(mtmp));
	return 1;
}


/*** Riding the monster ***/

/* Can we ride this monster?  Caller should also check can_saddle() */
boolean
can_ride(mtmp)
	struct monst *mtmp;
{
	return (mtmp->mtame && humanoid(youmonst.data) &&
			!verysmall(youmonst.data) && !bigmonst(youmonst.data) &&
			(!Underwater || is_swimmer(mtmp->data)));
}


int
doride()
{
	boolean forcemount = FALSE;

	if (u.usteed)
	    dismount_steed(DISMOUNT_BYCHOICE);
	else if (getdir((char *)0) && isok(u.ux+u.dx, u.uy+u.dy)) {
#ifdef WIZARD
/*JP
	if (wizard && yn("Force the mount to succeed?") == 'y')
*/
	if (wizard && yn("無理矢理成功させますか？") == 'y')
		forcemount = TRUE;
#endif
	    return (mount_steed(m_at(u.ux+u.dx, u.uy+u.dy), forcemount));
	} else
	    return 0;
	return 1;
}


/* Start riding, with the given monster */
boolean
mount_steed(mtmp, force)
	struct monst *mtmp;	/* The animal */
	boolean force;		/* Quietly force this animal */
{
	struct obj *otmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	/* Sanity checks */
	if (u.usteed) {
/*JP
	    You("are already riding %s.", mon_nam(u.usteed));
*/
	    You("もう%sに乗っている。", mon_nam(u.usteed));
	    return (FALSE);
	}

	/* Is the player in the right form? */
	if (Hallucination && !force) {
#if 0 /*JP*/
	    pline("Maybe you should find a designated driver.");
#else
	    pline("おそらくあなたは指定ドライバーを探すべきだろう。");
#endif
	    return (FALSE);
	}
	/* While riding Wounded_legs refers to the steed's,
	 * not the hero's legs.
	 * That opens up a potential abuse where the player
	 * can mount a steed, then dismount immediately to
	 * heal leg damage, because leg damage is always
	 * healed upon dismount (Wounded_legs context switch).
	 * By preventing a hero with Wounded_legs from
	 * mounting a steed, the potential for abuse is
	 * minimized, if not eliminated altogether.
	 */
	if (Wounded_legs) {
#if 0 /*JP*/
	    Your("%s are in no shape for riding.", makeplural(body_part(LEG)));
#else
	    pline("%sを怪我しているので乗れない。", makeplural(body_part(LEG)));
#endif
#ifdef WIZARD
	    if (force && wizard && yn("Heal your legs?") == 'y')
		HWounded_legs = EWounded_legs = 0;
	    else
#endif
	    return (FALSE);
	}

	if (Upolyd && (!humanoid(youmonst.data) || verysmall(youmonst.data) ||
			bigmonst(youmonst.data) || slithy(youmonst.data))) {
/*JP
	    You("won't fit on a saddle.");
*/
	    You("鞍に合わない。");
	    return (FALSE);
	}
	if(!force && (near_capacity() > SLT_ENCUMBER)) {
/*JP
	    You_cant("do that while carrying so much stuff.");
*/
	    You("沢山物を持ちすぎており出来ない。");
	    return (FALSE);
	}

	/* Can the player reach and see the monster? */
	if (!mtmp || (!force && ((Blind && !Blind_telepat) ||
		mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT))) {
/*JP
	    pline("I see nobody there.");
*/
	    pline("そこには何も見えない。");
	    return (FALSE);
	}
	if (u.uswallow || u.ustuck || u.utrap || Punished ||
	    !test_move(u.ux, u.uy, mtmp->mx-u.ux, mtmp->my-u.uy, TEST_MOVE)) {
	    if (Punished || !(u.uswallow || u.ustuck || u.utrap))
/*JP
		You("are unable to swing your %s over.", body_part(LEG)); 
*/
		You("鞍をまたぐことができない。");
	    else
/*JP
		You("are stuck here for now.");
*/
		You("はまっているので出来ない。");
	    return (FALSE);
	}

	/* Is this a valid monster? */
	otmp = which_armor(mtmp, W_SADDLE);
	if (!otmp) {
/*JP
	    pline("%s is not saddled.", Monnam(mtmp));
*/
	    pline("%sには鞍が取りつけられていない。", Monnam(mtmp));
	    return (FALSE);
	}
	ptr = mtmp->data;
	if (touch_petrifies(ptr) && !Stone_resistance) {
	    char kbuf[BUFSZ];

/*JP
	    You("touch %s.", mon_nam(mtmp));
*/
	    You("%sに触れた。", mon_nam(mtmp));
/*JP
	    Sprintf(kbuf, "attempting to ride %s", an(mtmp->data->mname));
*/
	    Sprintf(kbuf, "%sに乗ろうとして", a_monnam(mtmp));
	    instapetrify(kbuf);
	}
	if (!mtmp->mtame || mtmp->isminion) {
/*JP
	    pline("I think %s would mind.", mon_nam(mtmp));
*/
	    pline("%sは嫌がっているようだ。", mon_nam(mtmp));
	    return (FALSE);
	}
	if (mtmp->mtrapped) {
	    struct trap *t = t_at(mtmp->mx, mtmp->my);

#if 0 /*JP*/
	    You_cant("mount %s while %s's trapped in %s.",
		     mon_nam(mtmp), mhe(mtmp),
		     an(defsyms[trap_to_defsym(t->ttyp)].explanation));
#else
	    You("%sに捕まっている%sには乗れない。",
		     jtrns_obj('^', defsyms[trap_to_defsym(t->ttyp)].explanation),
		     mon_nam(mtmp));
#endif
	    return (FALSE);
	}

	if (!force && !Role_if(PM_KNIGHT) && !(--mtmp->mtame)) {
	    /* no longer tame */
	    newsym(mtmp->mx, mtmp->my);
#if 0 /*JP*/
	    pline("%s resists%s!", Monnam(mtmp),
		  mtmp->mleashed ? " and its leash comes off" : "");
#else
	    if (mtmp->mleashed)
		pline("%sは拒否した！", Monnam(mtmp));
	    else
		pline("%sは拒否して、%sをはずした！", Monnam(mtmp),
			jtrns_obj(TOOL_SYM,"leash"));
#endif
	    if (mtmp->mleashed) m_unleash(mtmp, FALSE);
	    return (FALSE);
	}
	if (!force && Underwater && !is_swimmer(ptr)) {
/*JP
	    You_cant("ride that creature while under water.");
*/
	    You("水中で乗ることはできない。");
	    return (FALSE);
	}
	if (!can_saddle(mtmp) || !can_ride(mtmp)) {
/*JP
	    You_cant("ride such a creature.");
*/
	    You("その生き物に乗ることはできない。");
	    return (0);
	}

	/* Is the player impaired? */
	if (!force && !is_floater(ptr) && !is_flyer(ptr) &&
			Levitation && !Lev_at_will) {
/*JP
	    You("cannot reach %s.", mon_nam(mtmp));
*/
	    You("%sに届かない。", mon_nam(mtmp));
	    return (FALSE);
	}
	if (!force && uarm && is_metallic(uarm) &&
			greatest_erosion(uarm)) {
#if 0 /*JP*/
	    Your("%s armor is too stiff to be able to mount %s.",
			uarm->oeroded ? "rusty" : "corroded",
			mon_nam(mtmp));
#else
	    Your("%s鎧はギシギシいっており%sに乗れない。",
			uarm->oeroded ? "錆びた" : "腐食した",
			mon_nam(mtmp));
#endif
	    return (FALSE);
	}
	if (!force && (Confusion || Fumbling || Glib || Wounded_legs ||
		otmp->cursed || (u.ulevel+mtmp->mtame < rnd(MAXULEV/2+5)))) {
	    if (Levitation) {
/*JP
		pline("%s slips away from you.", Monnam(mtmp));
*/
		pline("%sはあなたからはなれていった。", Monnam(mtmp));
		return FALSE;
	    }
/*JP
	    You("slip while trying to get on %s.", mon_nam(mtmp));
*/
	    You("%sに乗ろうとしてすべった。", mon_nam(mtmp));

#if 0 /*JP*/
	    Sprintf(buf, "slipped while mounting %s",
		    /* "a saddled mumak" or "a saddled pony called Dobbin" */
		    x_monnam(mtmp, ARTICLE_A, (char *)0,
			SUPPRESS_IT|SUPPRESS_INVISIBLE|SUPPRESS_HALLUCINATION,
			     TRUE));
	    losehp(rn1(5,10), buf, NO_KILLER_PREFIX);
#else
	    Sprintf(buf, "%sに乗ろうとしてすべり落ちて",
		    m_monnam(mtmp));
	    losehp(rn1(5,10), buf, KILLED_BY);
#endif
	    return (FALSE);
	}

	/* Success */
	if (!force) {
	    if (Levitation && !is_floater(ptr) && !is_flyer(ptr))
	    	/* Must have Lev_at_will at this point */
/*JP
	    	pline("%s magically floats up!", Monnam(mtmp));
*/
	    	pline("%sは魔法の力で浮いた！", Monnam(mtmp));
/*JP
	    You("mount %s.", mon_nam(mtmp));
*/
	    You("%sに乗った。", mon_nam(mtmp));
	}
	/* setuwep handles polearms differently when you're mounted */
	if (uwep && is_pole(uwep)) unweapon = FALSE;
	u.usteed = mtmp;
	remove_monster(mtmp->mx, mtmp->my);
	teleds(mtmp->mx, mtmp->my, TRUE);
	return (TRUE);
}


/* You and your steed have moved */
void
exercise_steed()
{
	if (!u.usteed)
		return;

	/* It takes many turns of riding to exercise skill */
	if (u.urideturns++ >= 100) {
	    u.urideturns = 0;
	    use_skill(P_RIDING, 1);
	}
	return;
}


/* The player kicks or whips the steed */
void
kick_steed()
{
	char He[4];
	if (!u.usteed)
	    return;

	/* [ALI] Various effects of kicking sleeping/paralyzed steeds */
	if (u.usteed->msleeping || !u.usteed->mcanmove) {
	    /* We assume a message has just been output of the form
	     * "You kick <steed>."
	     */
	    Strcpy(He, mhe(u.usteed));
	    *He = highc(*He);
	    if ((u.usteed->mcanmove || u.usteed->mfrozen) && !rn2(2)) {
		if (u.usteed->mcanmove)
		    u.usteed->msleeping = 0;
		else if (u.usteed->mfrozen > 2)
		    u.usteed->mfrozen -= 2;
		else {
		    u.usteed->mfrozen = 0;
		    u.usteed->mcanmove = 1;
		}
		if (u.usteed->msleeping || !u.usteed->mcanmove)
#if 0 /*JP*/
		    pline("%s stirs.", He);
#else
		    pline("%sは動き出した。", He);
#endif
		else
#if 0 /*JP*/
		    pline("%s rouses %sself!", He, mhim(u.usteed));
#else
		    pline("%sは奮起した！", He);
#endif
	    } else
#if 0 /*JP*/
		pline("%s does not respond.", He);
#else
		pline("%sは反応しない。", He);
#endif
	    return;
	}

	/* Make the steed less tame and check if it resists */
	if (u.usteed->mtame) u.usteed->mtame--;
	if (!u.usteed->mtame && u.usteed->mleashed) m_unleash(u.usteed, TRUE);
	if (!u.usteed->mtame || (u.ulevel+u.usteed->mtame < rnd(MAXULEV/2+5))) {
	    newsym(u.usteed->mx, u.usteed->my);
	    dismount_steed(DISMOUNT_THROWN);
	    return;
	}

/*JP
	pline("%s gallops!", Monnam(u.usteed));
*/
	pline("%sは速足になった！", Monnam(u.usteed));
	u.ugallop += rn1(20, 30);
	return;
}

/*
 * Try to find a dismount point adjacent to the steed's location.
 * If all else fails, try enexto().  Use enexto() as a last resort because
 * enexto() chooses its point randomly, possibly even outside the
 * room's walls, which is not what we want.
 * Adapted from mail daemon code.
 */
STATIC_OVL boolean
landing_spot(spot, reason, forceit)
coord *spot;	/* landing position (we fill it in) */
int reason;
int forceit;
{
    int i = 0, x, y, distance, min_distance = -1;
    boolean found = FALSE;
    struct trap *t;

    /* avoid known traps (i == 0) and boulders, but allow them as a backup */
    if (reason != DISMOUNT_BYCHOICE || Stunned || Confusion || Fumbling) i = 1;
    for (; !found && i < 2; ++i) {
	for (x = u.ux-1; x <= u.ux+1; x++)
	    for (y = u.uy-1; y <= u.uy+1; y++) {
		if (!isok(x, y) || (x == u.ux && y == u.uy)) continue;

		if (ACCESSIBLE(levl[x][y].typ) &&
			    !MON_AT(x,y) && !closed_door(x,y)) {
		    distance = distu(x,y);
		    if (min_distance < 0 || distance < min_distance ||
			    (distance == min_distance && rn2(2))) {
			if (i > 0 || (((t = t_at(x, y)) == 0 || !t->tseen) &&
				      (!sobj_at(BOULDER, x, y) ||
				       throws_rocks(youmonst.data)))) {
			    spot->x = x;
			    spot->y = y;
			    min_distance = distance;
			    found = TRUE;
			}
		    }
		}
	    }
    }

    /* If we didn't find a good spot and forceit is on, try enexto(). */
    if (forceit && min_distance < 0 &&
		!enexto(spot, u.ux, u.uy, youmonst.data))
	return FALSE;

    return found;
}

/* Stop riding the current steed */
void
dismount_steed(reason)
	int reason;		/* Player was thrown off etc. */
{
	struct monst *mtmp;
	struct obj *otmp;
	coord cc;
/*JP
	const char *verb = "fall";
*/
	const char *verb = "落ちた";
	boolean repair_leg_damage = TRUE;
	unsigned save_utrap = u.utrap;
	boolean have_spot = landing_spot(&cc,reason,0);
	
	mtmp = u.usteed;		/* make a copy of steed pointer */
	/* Sanity check */
	if (!mtmp)		/* Just return silently */
	    return;

	/* Check the reason for dismounting */
	otmp = which_armor(mtmp, W_SADDLE);
	switch (reason) {
	    case DISMOUNT_THROWN:
/*JP
		verb = "are thrown";
*/
		verb = "ふり落された";
	    case DISMOUNT_FELL:
/*JP
		You("%s off of %s!", verb, mon_nam(mtmp));
*/
		You("%sから%s！", mon_nam(mtmp), verb);
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
/*JP
		losehp(rn1(10,10), "riding accident", KILLED_BY_AN);
*/
		losehp(rn1(10,10), "乗りものから落ちて", KILLED_BY_AN);
		set_wounded_legs(BOTH_SIDES, (int)HWounded_legs + rn1(5,5));
		repair_leg_damage = FALSE;
		break;
	    case DISMOUNT_POLY:
/*JP
		You("can no longer ride %s.", mon_nam(u.usteed));
*/
		You("%sに乗ってられない。", mon_nam(u.usteed));
		if (!have_spot) have_spot = landing_spot(&cc,reason,1);
		break;
	    case DISMOUNT_ENGULFED:
		/* caller displays message */
		break;
	    case DISMOUNT_BONES:
		/* hero has just died... */
		break;
	    case DISMOUNT_GENERIC:
		/* no messages, just make it so */
		break;
	    case DISMOUNT_BYCHOICE:
	    default:
		if (otmp && otmp->cursed) {
#if 0 /*JP*/
		    You("can't.  The saddle %s cursed.",
			otmp->bknown ? "is" : "seems to be");
#else
		    You("降りられない。鞍は呪われている%s。",
			otmp->bknown ? "" : "ようだ");
#endif
		    otmp->bknown = TRUE;
		    return;
		}
		if (!have_spot) {
/*JP
		    You("can't. There isn't anywhere for you to stand.");
*/
		    pline("あなたの立つ場所がないので降りられない。");
		    return;
		}
		if (!mtmp->mnamelth) {
#if 0 /*JP*/
			pline("You've been through the dungeon on %s with no name.",
				an(mtmp->data->mname));
#else
			pline("あなたは名無しの%sと共に迷宮内にいる。",
				jtrns_mon(mtmp->data->mname));
#endif
			if (Hallucination)
/*JP
				pline("It felt good to get out of the rain.");
*/
				pline("雨のなかから抜け出るのによいと思った。");
		} else
/*JP
			You("dismount %s.", mon_nam(mtmp));
*/
			You("%sから降りた。", mon_nam(mtmp));
	}
	/* While riding these refer to the steed's legs
	 * so after dismounting they refer to the player's
	 * legs once again.
	 */
	if (repair_leg_damage) HWounded_legs = EWounded_legs = 0;

	/* Release the steed and saddle */
	u.usteed = 0;
	u.ugallop = 0L;

	/* Set player and steed's position.  Try moving the player first
	   unless we're in the midst of creating a bones file. */
	if (reason == DISMOUNT_BONES) {
	    /* move the steed to an adjacent square */
	    if (enexto(&cc, u.ux, u.uy, mtmp->data))
		rloc_to(mtmp, cc.x, cc.y);
	    else	/* evidently no room nearby; move steed elsewhere */
		(void) rloc(mtmp, FALSE);
	    return;
	}
	if (!DEADMONSTER(mtmp)) {
	    place_monster(mtmp, u.ux, u.uy);
	    if (!u.uswallow && !u.ustuck && have_spot) {
		struct permonst *mdat = mtmp->data;

		/* The steed may drop into water/lava */
		if (!is_flyer(mdat) && !is_floater(mdat) && !is_clinger(mdat)) {
		    if (is_pool(u.ux, u.uy)) {
			if (!Underwater)
/*JP
			    pline("%s falls into the %s!", Monnam(mtmp),
*/
			    pline("%sは%sに落ちた！", Monnam(mtmp),
							surface(u.ux, u.uy));
			if (!is_swimmer(mdat) && !amphibious(mdat)) {
			    killed(mtmp);
			    adjalign(-1);
			}
		    } else if (is_lava(u.ux, u.uy)) {
/*JP
			pline("%s is pulled into the lava!", Monnam(mtmp));
*/
			pline("%sは溶岩の中にひっぱられた！", Monnam(mtmp));
			if (!likes_lava(mdat)) {
			    killed(mtmp);
			    adjalign(-1);
			}
		    }
		}
	    /* Steed dismounting consists of two steps: being moved to another
	     * square, and descending to the floor.  We have functions to do
	     * each of these activities, but they're normally called
	     * individually and include an attempt to look at or pick up the
	     * objects on the floor:
	     * teleds() --> spoteffects() --> pickup()
	     * float_down() --> pickup()
	     * We use this kludge to make sure there is only one such attempt.
	     *
	     * Clearly this is not the best way to do it.  A full fix would
	     * involve having these functions not call pickup() at all, instead
	     * calling them first and calling pickup() afterwards.  But it
	     * would take a lot of work to keep this change from having any
	     * unforseen side effects (for instance, you would no longer be
	     * able to walk onto a square with a hole, and autopickup before
	     * falling into the hole).
	     */
		/* [ALI] No need to move the player if the steed died. */
		if (!DEADMONSTER(mtmp)) {
		    /* Keep steed here, move the player to cc;
		     * teleds() clears u.utrap
		     */
		    in_steed_dismounting = TRUE;
		    teleds(cc.x, cc.y, TRUE);
		    in_steed_dismounting = FALSE;

		    /* Put your steed in your trap */
		    if (save_utrap)
			(void) mintrap(mtmp);
		}
	    /* Couldn't... try placing the steed */
	    } else if (enexto(&cc, u.ux, u.uy, mtmp->data)) {
		/* Keep player here, move the steed to cc */
		rloc_to(mtmp, cc.x, cc.y);
		/* Player stays put */
	    /* Otherwise, kill the steed */
	    } else {
		killed(mtmp);
		adjalign(-1);
	    }
	}

	/* Return the player to the floor */
	if (reason != DISMOUNT_ENGULFED) {
	    in_steed_dismounting = TRUE;
	    (void) float_down(0L, W_SADDLE);
	    in_steed_dismounting = FALSE;
	    flags.botl = 1;
	    (void)encumber_msg();
	    vision_full_recalc = 1;
	} else
	    flags.botl = 1;
	/* polearms behave differently when not mounted */
	if (uwep && is_pole(uwep)) unweapon = TRUE;
	return;
}

void
place_monster(mon, x, y)
struct monst *mon;
int x, y;
{
    if (mon == u.usteed ||
	    /* special case is for convoluted vault guard handling */
	    (DEADMONSTER(mon) && !(mon->isgd && x == 0 && y == 0))) {
	impossible("placing %s onto map?",
		   (mon == u.usteed) ? "steed" : "defunct monster");
	return;
    }
    mon->mx = x, mon->my = y;
    level.monsters[x][y] = mon;
    if (mon->data == &mons[PM_GIANT_TURTLE] && (!mon->minvis || See_invisible))
	block_point(x,y);
}

#endif /* STEED */

/*steed.c*/
