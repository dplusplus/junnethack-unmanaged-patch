/*	SCCS Id: @(#)potion.c	3.4	2002/10/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2009
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009-2010
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"

#ifdef OVLB
boolean notonhead = FALSE;

static NEARDATA int nothing, unkn;
static NEARDATA const char beverages[] = { POTION_CLASS, 0 };

STATIC_DCL long FDECL(itimeout, (long));
STATIC_DCL long FDECL(itimeout_incr, (long,int));
STATIC_DCL void NDECL(ghost_from_bottle);
STATIC_OVL void NDECL(alchemy_init);
STATIC_DCL short FDECL(mixtype, (struct obj *,struct obj *));

#ifndef TESTING
STATIC_DCL int FDECL(dip, (struct obj *,struct obj *));
#endif

/* force `val' to be within valid range for intrinsic timeout value */
STATIC_OVL long
itimeout(val)
long val;
{
    if (val >= TIMEOUT) val = TIMEOUT;
    else if (val < 1) val = 0;

    return val;
}

/* increment `old' by `incr' and force result to be valid intrinsic timeout */
STATIC_OVL long
itimeout_incr(old, incr)
long old;
int incr;
{
    return itimeout((old & TIMEOUT) + (long)incr);
}

/* set the timeout field of intrinsic `which' */
void
set_itimeout(which, val)
long *which, val;
{
    *which &= ~TIMEOUT;
    *which |= itimeout(val);
}

/* increment the timeout field of intrinsic `which' */
void
incr_itimeout(which, incr)
long *which;
int incr;
{
    set_itimeout(which, itimeout_incr(*which, incr));
}

void
make_confused(xtime,talk)
long xtime;
boolean talk;
{
	long old = HConfusion;

	if (!xtime && old) {
		if (talk)
#if 0 /*JP*/
		    You_feel("less %s now.",
			Hallucination ? "trippy" : "confused");
#else
		    You("%sがおさまった。",
			Hallucination ? "ヘロヘロ" : "混乱");
#endif
	}
	if ((xtime && !old) || (!xtime && old)) flags.botl = TRUE;

	set_itimeout(&HConfusion, xtime);
}

void
make_stunned(xtime,talk)
long xtime;
boolean talk;
{
	long old = HStun;

	if (!xtime && old) {
		if (talk)
#if 0 /*JP*/
		    You_feel("%s now.",
			Hallucination ? "less wobbly" : "a bit steadier");
#else
		    You_feel("%s。",
			Hallucination ? "へろりらがおさまった" : "だんだんしっかりしてきた");
#endif
	}
	if (xtime && !old) {
		if (talk) {
#ifdef STEED
			if (u.usteed)
/*JP
				You("wobble in the saddle.");
*/
				You("鞍の上でぐらぐらした。");
			else
#endif
/*JP
			You("%s...", stagger(youmonst.data, "stagger"));
*/
			You("%s．．．", stagger(youmonst.data, "くらくらした"));
		}
	}
	if ((!xtime && old) || (xtime && !old)) flags.botl = TRUE;

	set_itimeout(&HStun, xtime);
}

void
make_sick(xtime, cause, talk, type)
long xtime;
const char *cause;	/* sickness cause */
boolean talk;
int type;
{
	long old = Sick;

	if (xtime > 0L) {
	    if (Sick_resistance) return;
	    if (!old) {
		/* newly sick */
/*JP
		You_feel("deathly sick.");
*/
		You("病気で死にそうだ。");
	    } else {
		/* already sick */
#if 0 /*JP*/
		if (talk) You_feel("%s worse.",
			      xtime <= Sick/2L ? "much" : "even");
#else
		if (talk) You("%s悪化したような気がする。",
			      xtime <= Sick/2L ? "さらに" : "もっと");
#endif
	    }
	    set_itimeout(&Sick, xtime);
	    u.usick_type |= type;
	    flags.botl = TRUE;
	} else if (old && (type & u.usick_type)) {
	    /* was sick, now not */
	    u.usick_type &= ~type;
	    if (u.usick_type) { /* only partly cured */
/*JP
		if (talk) You_feel("somewhat better.");
*/
		if (talk) You("ちょっとよくなった。");
		set_itimeout(&Sick, Sick * 2); /* approximation */
	    } else {
/*JP
		if (talk) pline("What a relief!");
*/
		if (talk) pline("ああ助かった！");
		Sick = 0L;		/* set_itimeout(&Sick, 0L) */
	    }
	    flags.botl = TRUE;
	}

	if (Sick) {
	    exercise(A_CON, FALSE);
	    if (cause) {
		(void) strncpy(u.usick_cause, cause, sizeof(u.usick_cause));
		u.usick_cause[sizeof(u.usick_cause)-1] = 0;
		}
	    else
		u.usick_cause[0] = 0;
	} else
	    u.usick_cause[0] = 0;
}

void
make_vomiting(xtime, talk)
long xtime;
boolean talk;
{
	long old = Vomiting;

	if(!xtime && old)
/*JP
	    if(talk) You_feel("much less nauseated now.");
*/
	    if(talk) You("吐き気がおさまった。");

	set_itimeout(&Vomiting, xtime);
}

/*JP
static const char vismsg[] = "vision seems to %s for a moment but is %s now.";
*/
static const char vismsg[] = "視界は一瞬%sなったがまた%sなった。";
/*JP
static const char eyemsg[] = "%s momentarily %s.";
*/
static const char eyemsg[] = "%sは一瞬%s。";

void
make_blinded(xtime, talk)
long xtime;
boolean talk;
{
	long old = Blinded;
	boolean u_could_see, can_see_now;
#if 0 /*JP*/
	int eyecnt;
	char buf[BUFSZ];
#endif

	/* we need to probe ahead in case the Eyes of the Overworld
	   are or will be overriding blindness */
	u_could_see = !Blind;
	Blinded = xtime ? 1L : 0L;
	can_see_now = !Blind;
	Blinded = old;		/* restore */

	if (u.usleep) talk = FALSE;

	if (can_see_now && !u_could_see) {	/* regaining sight */
	    if (talk) {
		if (Hallucination)
/*JP
		    pline("Far out!  Everything is all cosmic again!");
*/
		    pline("げ！なにもかもがまた虹色に見える！");
		else
/*JP
		    You("can see again.");
*/
		    You("また見えるようになった。");
	    }
	} else if (old && !xtime) {
	    /* clearing temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
#if 0 /*JP*/
		    Strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
#else
		    Your(eyemsg, body_part(EYE), "かゆくなった");
#endif
		} else {	/* Eyes of the Overworld */
#if 0 /*JP*/
		    Your(vismsg, "brighten",
			 Hallucination ? "sadder" : "normal");
#else
		    Your(vismsg, "明るく",
			 Hallucination ? "根暗に" : "普通に");
#endif
		}
	    }
	}

	if (u_could_see && !can_see_now) {	/* losing sight */
	    if (talk) {
		if (Hallucination)
/*JP
		    pline("Oh, bummer!  Everything is dark!  Help!");
*/
		    pline("暗いよー、狭いよー、恐いよー！");
		else
/*JP
		    pline("A cloud of darkness falls upon you.");
*/
		    pline("暗黒の雲があなたを覆った。");
	    }
	    /* Before the hero goes blind, set the ball&chain variables. */
	    if (Punished) set_bc(0);
	} else if (!old && xtime) {
	    /* setting temporary blindness without toggling blindness */
	    if (talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blindfolded) {
#if 0 /*JP*/
		    Strcpy(buf, body_part(EYE));
		    eyecnt = eyecount(youmonst.data);
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "twitches" : "twitch");
#else
		    Your(eyemsg, body_part(EYE), "ピクピクした");
#endif
		} else {	/* Eyes of the Overworld */
#if 0 /*JP*/
		    Your(vismsg, "dim",
			 Hallucination ? "happier" : "normal");
#else
		    Your(vismsg, "薄暗く",
			 Hallucination ? "ハッピーに" : "普通に");
#endif
		}
	    }
	}

	set_itimeout(&Blinded, xtime);

	if (u_could_see ^ can_see_now) {  /* one or the other but not both */
	    flags.botl = 1;
	    vision_full_recalc = 1;	/* blindness just got toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	}
}

boolean
make_hallucinated(xtime, talk, mask)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
long mask;	/* nonzero if resistance status should change by mask */
{
	long old = HHallucination;
	boolean changed = 0;
	const char *message, *verb;

#if 0 /*JP*/
	message = (!xtime) ? "Everything %s SO boring now." :
			     "Oh wow!  Everything %s so cosmic!";
#else
	message = (!xtime) ? "何もかもが＊退屈＊に%sる。" :
			     "ワーオ！何もかも虹色に%sる！";
#endif
/*JP
	verb = (!Blind) ? "looks" : "feels";
*/
	verb = (!Blind) ? "見え" : "感じ";

	if (mask) {
	    if (HHallucination) changed = TRUE;

	    if (!xtime) EHalluc_resistance |= mask;
	    else EHalluc_resistance &= ~mask;
	} else {
	    if (!EHalluc_resistance && (!!HHallucination != !!xtime))
		changed = TRUE;
	    set_itimeout(&HHallucination, xtime);

	    /* clearing temporary hallucination without toggling vision */
	    if (!changed && !HHallucination && old && talk) {
		if (!haseyes(youmonst.data)) {
		    strange_feeling((struct obj *)0, (char *)0);
		} else if (Blind) {
#if 0 /*JP*/
		    char buf[BUFSZ];
		    int eyecnt = eyecount(youmonst.data);

		    Strcpy(buf, body_part(EYE));
		    Your(eyemsg, (eyecnt == 1) ? buf : makeplural(buf),
			 (eyecnt == 1) ? "itches" : "itch");
#else
		    Your(eyemsg, body_part(EYE), "かゆくなった");
#endif
		} else {	/* Grayswandir */
/*JP
		    Your(vismsg, "flatten", "normal");
*/
		    Your(vismsg, "おかしく", "普通に");
		}
	    }
	}

	if (changed) {
	    if (u.uswallow) {
		swallowed(0);	/* redraw swallow display */
	    } else {
		/* The see_* routines should be called *before* the pline. */
		see_monsters();
		see_objects();
		see_traps();
	    }

	    /* for perm_inv and anything similar
	    (eg. Qt windowport's equipped items display) */
	    update_inventory();

	    flags.botl = 1;
	    if (talk) pline(message, verb);
	}
	return changed;
}

STATIC_OVL void
ghost_from_bottle()
{
	struct monst *mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, NO_MM_FLAGS);

	if (!mtmp) {
/*JP
		pline("This bottle turns out to be empty.");
*/
		pline("瓶は空っぽだった。");
		return;
	}
	if (Blind) {
/*JP
		pline("As you open the bottle, %s emerges.", something);
*/
		pline("瓶を開けると、何かが出てきた。");
		return;
	}
/*JP
	pline("As you open the bottle, an enormous %s emerges!",
*/
	pline("瓶を開けると、巨大な%sが出てきた！",
/*JP
		Hallucination ? rndmonnam() : (const char *)"ghost");
*/
		Hallucination ? rndmonnam() : (const char *)"幽霊");
	if(flags.verbose)
/*JP
	    You("are frightened to death, and unable to move.");
*/
	    You("まっさおになって驚き、動けなくなった。");
/*JP
	nomul(-3, "being frightened to death");
*/
	nomul(-3, "死ぬほど驚いた隙に");
/*JP
	nomovemsg = "You regain your composure.";
*/
	nomovemsg = "あなたは平静を取り戻した。";
}

/* "Quaffing is like drinking, except you spill more."  -- Terry Pratchett
 */
int
dodrink()
{
	register struct obj *otmp;
	const char *potion_descr;

	if (Strangled) {
/*JP
		pline("If you can't breathe air, how can you drink liquid?");
*/
		pline("息もできないのに、どうやって液体を飲むんだい？");
		return 0;
	}
	/* Is there a fountain to drink from here? */
	if (IS_FOUNTAIN(levl[u.ux][u.uy].typ) && !Levitation) {
/*JP
		if(yn("Drink from the fountain?") == 'y') {
*/
		if(yn("泉の水を飲みますか？") == 'y') {
			drinkfountain();
			return 1;
		}
	}
#ifdef SINKS
	/* Or a kitchen sink? */
	if (IS_SINK(levl[u.ux][u.uy].typ)) {
/*JP
		if (yn("Drink from the sink?") == 'y') {
*/
		if (yn("流し台の水を飲みますか？") == 'y') {
			drinksink();
			return 1;
		}
	}
#endif

	/* Or are you surrounded by water? */
	if (Underwater) {
/*JP
		if (yn("Drink the water around you?") == 'y') {
*/
		if (yn("まわりの水を飲みますか？") == 'y') {

			if (Role_if(PM_ARCHEOLOGIST)) {
#if 0 /*JP*/
				pline("No thank you, fish make love in it!"); /* Indiana Jones 3 */
#else
				pline("水はいらん。魚にやれ。"); /* インディ・ジョーンズ 最後の聖戦 */
#endif
			} else {
/*JP
		    pline("Do you know what lives in this water!");
*/
		    pline("この水中で何が生きているのか知ってるかい！");
			}
			return 1;
		}
	}

	otmp = getobj(beverages, "drink");
	if(!otmp) return(0);
	otmp->in_use = TRUE;		/* you've opened the stopper */

#define POTION_OCCUPANT_CHANCE(n) (13 + 2*(n))	/* also in muse.c */

	potion_descr = OBJ_DESCR(objects[otmp->otyp]);
	if (potion_descr) {
	    if (!strcmp(potion_descr, "milky") &&
		    flags.ghost_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.ghost_count))) {
		ghost_from_bottle();
		useup(otmp);
		return(1);
	    } else if (!strcmp(potion_descr, "smoky") &&
		    flags.djinni_count < MAXMONNO &&
		    !rn2(POTION_OCCUPANT_CHANCE(flags.djinni_count))) {
		djinni_from_bottle(otmp);
		useup(otmp);
		return(1);
	    }
	}
	return dopotion(otmp);
}

int
dopotion(otmp)
register struct obj *otmp;
{
	int retval;

	otmp->in_use = TRUE;
	nothing = unkn = 0;
	if((retval = peffects(otmp)) >= 0) return(retval);

	if(nothing) {
	    unkn++;
#if 0 /*JP*/
	    You("have a %s feeling for a moment, then it passes.",
		  Hallucination ? "normal" : "peculiar");
#else
	    You("%s気分におそわれたが、すぐに消えさった。",
		  Hallucination ? "普通の" : "独特な");
#endif
	}
	if(otmp->dknown && !objects[otmp->otyp].oc_name_known) {
		if(!unkn) {
			makeknown(otmp->otyp);
			more_experienced(0,0,10);
		} else if(!objects[otmp->otyp].oc_uname)
			docall(otmp);
	}
	useup(otmp);
	return(1);
}

int
peffects(otmp)
	register struct obj	*otmp;
{
	register int i, ii, lim;

	switch(otmp->otyp){
	case POT_RESTORE_ABILITY:
	case SPE_RESTORE_ABILITY:
		unkn++;
		if(otmp->cursed) {
/*JP
		    pline("Ulch!  This makes you feel mediocre!");
*/
		    pline("うーん、どうもさえないなあ。");
		    break;
		} else {
#if 0 /*JP*/
		    pline("Wow!  This makes you feel %s!",
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "better" : "great")
			  : "good");
#else
		    pline("ワーオ！気分が%sなった！",
			  (otmp->blessed) ?
				(unfixable_trouble_count(FALSE) ? "だいぶよく" : "とてもよく")
			  : "よく");
#endif
		    i = rn2(A_MAX);		/* start at a random point */
		    for (ii = 0; ii < A_MAX; ii++) {
			lim = AMAX(i);
			if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
			if (ABASE(i) < lim) {
			    ABASE(i) = lim;
			    flags.botl = 1;
			    /* only first found if not blessed */
			    if (!otmp->blessed) break;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_HALLUCINATION:
		if (Hallucination || Halluc_resistance) nothing++;
		(void) make_hallucinated(itimeout_incr(HHallucination,
					   rn1(200, 600 - 300 * bcsign(otmp))),
				  TRUE, 0L);
		break;
	case POT_WATER:
		if(!otmp->blessed && !otmp->cursed) {
/*JP
		    pline("This tastes like water.");
*/
		    pline("水のような味がする。");
		    u.uhunger += rnd(10);
		    newuhs(FALSE);
		    break;
		}
		unkn++;
		if(is_undead(youmonst.data) || is_demon(youmonst.data) ||
				u.ualign.type == A_CHAOTIC) {
		    if(otmp->blessed) {
/*JP
			pline("This burns like acid!");
*/
			pline("酸のように舌がひりひりする！");
			exercise(A_CON, FALSE);
			if (u.ulycn >= LOW_PM) {
#if 0 /*JP*/
			    Your("affinity to %s disappears!",
				 makeplural(mons[u.ulycn].mname));
#else
			    Your("%s達への親近感はなくなった！",
				 jtrns_mon(mons[u.ulycn].mname));
#endif
			    if (youmonst.data == &mons[u.ulycn])
				you_unwere(FALSE);
			    u.ulycn = NON_PM;	/* cure lycanthropy */
			}
/*JP
			losehp(d(2,6), "potion of holy water", KILLED_BY_AN);
*/
			losehp(d(2,6), "聖水で", KILLED_BY_AN);
		    } else if(otmp->cursed) {
/*JP
			You_feel("quite proud of yourself.");
*/
			You("自尊心を感じた。");
			healup(d(2,6),0,0,0);
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, TRUE);
		    }
		} else {
		    if(otmp->blessed) {
/*JP
			You_feel("full of awe.");
*/
			You("畏怖の念にかられた。");
			make_sick(0L, (char *) 0, TRUE, SICK_ALL);
			exercise(A_WIS, TRUE);
			exercise(A_CON, TRUE);
			if (u.ulycn >= LOW_PM)
			    you_unwere(TRUE);	/* "Purified" */
			/* make_confused(0L,TRUE); */
		    } else {
			if(u.ualign.type == A_LAWFUL) {
/*JP
			    pline("This burns like acid!");
*/
			    pline("酸のように舌がひりひりする！");
#if 0 /*JP*/
			    losehp(d(2,6), "potion of unholy water",
				KILLED_BY_AN);
#else
			    losehp(d(2,6), "不浄な水で",
				KILLED_BY_AN);
#endif
			} else
/*JP
			    You_feel("full of dread.");
*/
			    You("恐怖の念にかられた。");
			if (u.ulycn >= LOW_PM && !Upolyd) you_were();
			exercise(A_CON, FALSE);
		    }
		}
		break;
	case POT_BOOZE:
		unkn++;
#if 0 /*JP*/
		pline("Ooph!  This tastes like %s%s!",
		      otmp->odiluted ? "watered down " : "",
		      Hallucination ? "dandelion wine" : "liquid fire");
#else
		pline("うぇっぷ！これは%s%sのような味がする！",
		      otmp->odiluted ? "水で薄めた" : "",
		      Hallucination ? "タンポポワイン" : "燃料オイル");
#endif
		if (!otmp->blessed)
		    make_confused(itimeout_incr(HConfusion, d(3,8)), FALSE);
		/* the whiskey makes us feel better */
		if (!otmp->odiluted) healup(1, 0, FALSE, FALSE);
		u.uhunger += 10 * (2 + bcsign(otmp));
		newuhs(FALSE);
		exercise(A_WIS, FALSE);
		if(otmp->cursed) {
/*JP
			You("pass out.");
*/
			You("気絶した。");
			multi = -rnd(15);
/*JP
			nomovemsg = "You awake with a headache.";
*/
			nomovemsg = "目がさめたが頭痛がする。";
		}
		break;
	case POT_ENLIGHTENMENT:
		if(otmp->cursed) {
			unkn++;
/*JP
			You("have an uneasy feeling...");
*/
			You("不安な気持になった．．．");
			exercise(A_WIS, FALSE);
		} else {
			if (otmp->blessed) {
				(void) adjattrib(A_INT, 1, FALSE);
				(void) adjattrib(A_WIS, 1, FALSE);
			}
/*JP
			You_feel("self-knowledgeable...");
*/
			You("自分自身が判るような気がした．．．");
			display_nhwindow(WIN_MESSAGE, FALSE);
			enlightenment(0, TRUE);
/*JP
			pline_The("feeling subsides.");
*/
			pline("その感じはなくなった。");
			exercise(A_WIS, TRUE);
		}
		break;
	case SPE_INVISIBILITY:
		/* spell cannot penetrate mummy wrapping */
		if (BInvis && uarmc->otyp == MUMMY_WRAPPING) {
/*JP
			You_feel("rather itchy under your %s.", xname(uarmc));
*/
			You("%sの下がムズムズした。", xname(uarmc));
			break;
		}
		/* FALLTHRU */
	case POT_INVISIBILITY:
		if (Invis || Blind || BInvis) {
		    nothing++;
		} else {
		    self_invis_message();
		}
		if (otmp->blessed) HInvis |= FROMOUTSIDE;
		else incr_itimeout(&HInvis, rn1(15,31));
		newsym(u.ux,u.uy);	/* update position */
		if(otmp->cursed) {
/*JP
		    pline("For some reason, you feel your presence is known.");
*/
		    pline("なぜか、存在が知られているような気がした。");
		    aggravate();
		}
		break;
	case POT_SEE_INVISIBLE:
		/* tastes like fruit juice in Rogue */
	case POT_FRUIT_JUICE:
	    {
		int msg = Invisible && !Blind;

		unkn++;
		if (otmp->cursed)
#if 0 /*JP*/
		    pline("Yecch!  This tastes %s.",
			  Hallucination ? "overripe" : "rotten");
#else
		    pline("オェー！これは%sジュースの味がする。",
			  Hallucination ? "熟しすぎた" : "腐った");
#endif
		else
#if 0 /*JP*/
		    pline(Hallucination ?
		      "This tastes like 10%% real %s%s all-natural beverage." :
				"This tastes like %s%s.",
			  otmp->odiluted ? "reconstituted " : "",
			  fruitname(TRUE));
#else
		    pline(Hallucination ?
			   "10%%%sの純自然飲料のような味がする。" :
			   "%s%sジュースのような味がする。",
			  otmp->odiluted ? "成分調整された" : "", pl_fruit);
#endif
		if (otmp->otyp == POT_FRUIT_JUICE) {
		    u.uhunger += (otmp->odiluted ? 5 : 10) * (2 + bcsign(otmp));
		    newuhs(FALSE);
		    break;
		}
		if (!otmp->cursed) {
			/* Tell them they can see again immediately, which
			 * will help them identify the potion...
			 */
			make_blinded(0L,TRUE);
		}
		if (otmp->blessed)
			HSee_invisible |= FROMOUTSIDE;
		else
			incr_itimeout(&HSee_invisible, rn1(100,750));
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();	/* see invisible monsters */
		newsym(u.ux,u.uy); /* see yourself! */
		if (msg && !Blind) { /* Blind possible if polymorphed */
/*JP
		    You("can see through yourself, but you are visible!");
*/
		    You("透明である。しかし見えるようになった！");
		    unkn--;
		}
		break;
	    }
	case POT_PARALYSIS:
		if (Free_action)
/*JP
		    You("stiffen momentarily.");
*/
		    You("一瞬動けなくなった。");             
		else {
		    if (Levitation || Is_airlevel(&u.uz)||Is_waterlevel(&u.uz))
/*JP
			You("are motionlessly suspended.");
*/
			You("空中で動けなくなった。");
#ifdef STEED
		    else if (u.usteed)
/*JP
			You("are frozen in place!");
*/
			You("その場で動けなくなった！");
#endif
		    else
#if 0 /*JP*/
			Your("%s are frozen to the %s!",
			     makeplural(body_part(FOOT)), surface(u.ux, u.uy));
#else
			You("動けなくなった！");
#endif
#if 0 /*JP*/
		    nomul(-(rn1(10, 25 - 12*bcsign(otmp))), "frozen by a potion");
#else
		    nomul(-(rn1(10, 25 - 12*bcsign(otmp)))
			, "薬で動けなくなっている");
#endif
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
		}
		break;
	case POT_SLEEPING:
		if(Sleep_resistance || Free_action)
/*JP
		    You("yawn.");
*/
		    You("あくびをした。");
		else {
/*JP
		    You("suddenly fall asleep!");
*/
		    pline("突然眠ってしまった！");
		    fall_asleep(-rn1(10, 25 - 12*bcsign(otmp)), TRUE);
		}
		break;
	case POT_MONSTER_DETECTION:
	case SPE_DETECT_MONSTERS:
		if (otmp->blessed) {
		    int x, y;

		    if (Detect_monsters) nothing++;
		    unkn++;
		    /* after a while, repeated uses become less effective */
		    if (HDetect_monsters >= 300L)
			i = 1;
		    else
			i = rn1(40,21);
		    incr_itimeout(&HDetect_monsters, i);
		    for (x = 1; x < COLNO; x++) {
			for (y = 0; y < ROWNO; y++) {
			    if (levl[x][y].glyph == GLYPH_INVISIBLE) {
				unmap_object(x, y);
				newsym(x,y);
			    }
			    if (MON_AT(x,y)) unkn = 0;
			}
		    }
		    see_monsters();
/*JP
		    if (unkn) You_feel("lonely.");
*/
		    if (unkn) You("心細くなった。");
		    break;
		}
		if (monster_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_OBJECT_DETECTION:
	case SPE_DETECT_TREASURE:
		if (object_detect(otmp, 0))
			return(1);		/* nothing detected */
		exercise(A_WIS, TRUE);
		break;
	case POT_SICKNESS:
/*JP
		pline("Yecch!  This stuff tastes like poison.");
*/
		pline("ウェー！毒のような味がする。");
		if (otmp->blessed) {
#if 0 /*JP*/
		    pline("(But in fact it was mildly stale %s.)",
			  fruitname(TRUE));
#else
		    pline("(しかし実際それは少し古くなった%s。)",
			  fruitname(TRUE));
#endif
		    if (!Role_if(PM_HEALER)) {
			/* NB: blessed otmp->fromsink is not possible */
/*JP
			losehp(1, "mildly contaminated potion", KILLED_BY_AN);
*/
			losehp(1, "病気に汚染された薬で", KILLED_BY_AN);
		    }
		} else {
		    if(Poison_resistance)
#if 0 /*JP*/
			pline(
			  "(But in fact it was biologically contaminated %s.)",
			      fruitname(TRUE));
#else
			pline(
			  "(しかし実際それは生物学的に汚染された%sだ。)",
			      fruitname(TRUE));
#endif
		    if (Role_if(PM_HEALER))
/*JP
			pline("Fortunately, you have been immunized.");
*/
			pline("幸運なことに、あなたは免疫がある。");
		    else {
			int typ = rn2(A_MAX);

			if (!Fixed_abil) {
			    poisontell(typ);
			    (void) adjattrib(typ,
			    		Poison_resistance ? -1 : -rn1(4,3),
			    		TRUE);
			}
			if(!Poison_resistance) {
			    if (otmp->fromsink)
				losehp(rnd(10)+5*!!(otmp->cursed),
/*JP
				       "contaminated tap water", KILLED_BY);
*/
				       "疫病に汚染された水で", KILLED_BY_AN);
			    else
				losehp(rnd(10)+5*!!(otmp->cursed),
/*JP
				       "contaminated potion", KILLED_BY_AN);
*/
				       "疫病に汚染された薬で", KILLED_BY_AN);
			}
			exercise(A_CON, FALSE);
		    }
		}
		if(Hallucination) {
/*JP
			You("are shocked back to your senses!");
*/
			You("五感に衝撃を受けた！");
			(void) make_hallucinated(0L,FALSE,0L);
		}
		break;
	case POT_CONFUSION:
		if(!Confusion)
		    if (Hallucination) {
/*JP
			pline("What a trippy feeling!");
*/
			pline("なんかヘロヘロする！");
			unkn++;
		    } else
/*JP
			pline("Huh, What?  Where am I?");
*/
			pline("ほえ？私は誰？");
		else	nothing++;
		make_confused(itimeout_incr(HConfusion,
					    rn1(7, 16 - 8 * bcsign(otmp))),
			      FALSE);
		break;
	case POT_GAIN_ABILITY:
		if(otmp->cursed) {
/*JP
		    pline("Ulch!  That potion tasted foul!");
*/
		    pline("ウェ！悪臭がする！");
		    unkn++;
		} else if (Fixed_abil) {
		    nothing++;
		} else {      /* If blessed, try very hard to find an ability */
		              /* that can be increased; if not, try up to     */
		    int itmp; /* 3 times to find one which can be increased.  */
		    i = -1;		/* increment to 0 */
		    for (ii = (otmp->blessed ? 1000 : A_MAX/2); ii > 0; ii--) {
			i = rn2(A_MAX);
			/* only give "your X is already as high as it can get"
			   message on last attempt */
			itmp = (ii == 1) ? 0 : -1;
			if (adjattrib(i, 1, itmp))
			    break;
		    }
		}
		break;
	case POT_SPEED:
		if(Wounded_legs && !otmp->cursed
#ifdef STEED
		   && !u.usteed	/* heal_legs() would heal steeds legs */
#endif
						) {
			heal_legs();
			unkn++;
			break;
		} /* and fall through */
	case SPE_HASTE_SELF:
		if(!Very_fast) /* wwf@doe.carleton.ca */
#if 0 /*JP*/
			You("are suddenly moving %sfaster.",
				Fast ? "" : "much ");
#else
			You("突然%s速く移動できるようになった。",
				Fast ? "" : "とても");
#endif
		else {
/*JP
			Your("%s get new energy.",
*/
			pline("%sにエネルギーが注ぎこまれるような感じがした。",
				makeplural(body_part(LEG)));
			unkn++;
		}
		exercise(A_DEX, TRUE);
		incr_itimeout(&HFast, rn1(10, 100 + 60 * bcsign(otmp)));
		break;
	case POT_BLINDNESS:
		if(Blind) nothing++;
		make_blinded(itimeout_incr(Blinded,
					   rn1(200, 250 - 125 * bcsign(otmp))),
			     (boolean)!Blind);
		break;
	case POT_GAIN_LEVEL:
		if (otmp->cursed) {
			unkn++;
			/* they went up a level */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet) ||
				Can_rise_up(u.ux, u.uy, &u.uz)) {
/*JP
			    const char *riseup ="rise up, through the %s!";
*/
			    const char *riseup ="%sを突き抜けた！";
			    if(ledger_no(&u.uz) == 1) {
			        You(riseup, ceiling(u.ux,u.uy));
#ifdef RANDOMIZED_PLANES
				goto_level(get_first_elemental_plane(), FALSE, FALSE, FALSE);
#else
				goto_level(&earth_level, FALSE, FALSE, FALSE);
#endif
			    } else {
			        register int newlev = depth(&u.uz)-1;
				d_level newlevel;

				get_level(&newlevel, newlev);
				if(on_level(&newlevel, &u.uz)) {
/*JP
				    pline("It tasted bad.");
*/
				    pline("とてもまずい。");
				    break;
				} else You(riseup, ceiling(u.ux,u.uy));
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			    }
			}
/*JP
			else You("have an uneasy feeling.");
*/
			else You("不安な気持になった。");
			break;
		}
		pluslvl(FALSE);
		if (otmp->blessed)
			/* blessed potions place you at a random spot in the
			 * middle of the new level instead of the low point
			 */
			u.uexp = rndexp(TRUE);
		break;
	case POT_HEALING:
/*JP
		You_feel("better.");
*/
		You("気分がよくなった。");
		healup(d(6 + 2 * bcsign(otmp), 4),
		       !otmp->cursed ? 1 : 0, !!otmp->blessed, !otmp->cursed);
		exercise(A_CON, TRUE);
		break;
	case POT_EXTRA_HEALING:
/*JP
		You_feel("much better.");
*/
		You("気分がとてもよくなった。");
		healup(d(6 + 2 * bcsign(otmp), 8),
		       otmp->blessed ? 5 : !otmp->cursed ? 2 : 0,
		       !otmp->cursed, TRUE);
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_CON, TRUE);
		exercise(A_STR, TRUE);
		break;
	case POT_FULL_HEALING:
/*JP
		You_feel("completely healed.");
*/
		You("完全に回復した。");
		healup(400, 4+4*bcsign(otmp), !otmp->cursed, TRUE);
		/* Restore one lost level if blessed */
		if (otmp->blessed && u.ulevel < u.ulevelmax) {
		    /* when multiple levels have been lost, drinking
		       multiple potions will only get half of them back */
		    u.ulevelmax -= 1;
		    pluslvl(FALSE);
		}
		(void) make_hallucinated(0L,TRUE,0L);
		exercise(A_STR, TRUE);
		exercise(A_CON, TRUE);
		break;
	case POT_LEVITATION:
	case SPE_LEVITATION:
		if (otmp->cursed) HLevitation &= ~I_SPECIAL;
		if(!Levitation) {
			/* kludge to ensure proper operation of float_up() */
			HLevitation = 1;
			float_up();
			/* reverse kludge */
			HLevitation = 0;
			if (otmp->cursed && !Is_waterlevel(&u.uz)) {
	if((u.ux != xupstair || u.uy != yupstair)
	   && (u.ux != sstairs.sx || u.uy != sstairs.sy || !sstairs.up)
	   && (!xupladder || u.ux != xupladder || u.uy != yupladder)
	) {
#if 0 /*JP*/
					You("hit your %s on the %s.",
						body_part(HEAD),
						ceiling(u.ux,u.uy));
#else
					You("%sを%sにぶつけた。",
						body_part(HEAD),
						ceiling(u.ux,u.uy));
#endif
#if 0 /*JP*/
					losehp(uarmh ? 1 : rnd(10),
						"colliding with the ceiling",
						KILLED_BY);
#else
					losehp(uarmh ? 1 : rnd(10),
						"天井に頭をぶつけて",
						KILLED_BY);
#endif
				} else (void) doup();
			}
		} else
			nothing++;
		if (otmp->blessed) {
		    incr_itimeout(&HLevitation, rn1(50,250));
		    HLevitation |= I_SPECIAL;
		} else incr_itimeout(&HLevitation, rn1(140,10));
		spoteffects(FALSE);	/* for sinks */
		break;
	case POT_GAIN_ENERGY:			/* M. Stephenson */
		{	register int num;
			if(otmp->cursed)
/*JP
			    You_feel("lackluster.");
*/
			    You("意気消沈した。");
			else
/*JP
			    pline("Magical energies course through your body.");
*/
			    pline("魔法のエネルギーがあなたの体に満ちた。");
			num = rnd(5) + 5 * otmp->blessed + 1;
			u.uenmax += (otmp->cursed) ? -num : num;
			u.uen += (otmp->cursed) ? -num : num;
			if(u.uenmax <= 0) u.uenmax = 0;
			if(u.uen <= 0) u.uen = 0;
			flags.botl = 1;
			exercise(A_WIS, TRUE);
		}
		break;
	case POT_OIL:				/* P. Winner */
		{
			boolean good_for_you = FALSE;

			if (otmp->lamplit) {
			    if (likes_fire(youmonst.data)) {
/*JP
				pline("Ahh, a refreshing drink.");
*/
				pline("わーお、生き返る。");
				good_for_you = TRUE;
			    } else {
/*JP
				You("burn your %s.", body_part(FACE));
*/
				Your("%sは黒焦げになった。", body_part(FACE));
				losehp(d(Fire_resistance ? 1 : 3, 4),
/*JP
				       "burning potion of oil", KILLED_BY_AN);
*/
				       "燃えている油を飲んで", KILLED_BY_AN);
			    }
			} else if(otmp->cursed)
/*JP
			    pline("This tastes like castor oil.");
*/
			    pline("ひまし油のような味がする。");
			else
/*JP
			    pline("That was smooth!");
*/
			    pline("口あたりがよい！");
			exercise(A_WIS, good_for_you);
		}
		break;
	case POT_ACID:
		if (Acid_resistance)
			/* Not necessarily a creature who _likes_ acid */
/*JP
			pline("This tastes %s.", Hallucination ? "tangy" : "sour");
*/
			pline("%s味がする。", Hallucination ? "ぴりっとした" : "酸の");
		else {
#if 0 /*JP*/
			pline("This burns%s!", otmp->blessed ? " a little" :
					otmp->cursed ? " a lot" : " like acid");
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"potion of acid", KILLED_BY_AN);
#else
			pline("%s焦げた！", otmp->blessed ? "少し" :
					otmp->cursed ? "すごく" : "");
			losehp(d(otmp->cursed ? 2 : 1, otmp->blessed ? 4 : 8),
					"酸の薬を飲んで", KILLED_BY_AN);
#endif
			exercise(A_CON, FALSE);
		}
		if (Stoned) fix_petrification();
		unkn++; /* holy/unholy water can burn like acid too */
		break;
	case POT_POLYMORPH:
/*JP
		You_feel("a little %s.", Hallucination ? "normal" : "strange");
*/
		You("少し%sな感じがした。", Hallucination ? "普通" : "変");
		if (!Unchanging) polyself(FALSE);
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		unkn++;
		u.uconduct.unvegan++;
		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
		    violated_vegetarian();
		    if (otmp->cursed)
#if 0 /*JP*/
			pline("Yecch!  This %s.", Hallucination ?
			"liquid could do with a good stir" : "blood has congealed");
#else
			pline("ゲェー！こりゃ%s。", Hallucination ?
			"よくかき混ぜないとダメだ" : "血が固まっちまってる");
#endif
		    else pline(Hallucination ?
#if 0 /*JP*/
		      "The %s liquid stirs memories of home." :
		      "The %s blood tastes delicious.",
			  otmp->odiluted ? "watery" : "thick");
#else
		      "%s液体が故郷の記憶を呼び起こす。" :
		      "%s血液は美味しい。",
			  otmp->odiluted ? "水っぽい" : "濃い");
#endif
		    if (!otmp->cursed)
			lesshungry((otmp->odiluted ? 1 : 2) *
			  (otmp->otyp == POT_VAMPIRE_BLOOD ? 400 :
			  otmp->blessed ? 15 : 10));
		    if (otmp->otyp == POT_VAMPIRE_BLOOD && otmp->blessed) {
			int num = newhp();
			if (Upolyd) {
			    u.mhmax += num;
			    u.mh += num;
			} else {
			    u.uhpmax += num;
			    u.uhp += num;
			}
		    }
		} else if (otmp->otyp == POT_VAMPIRE_BLOOD) {
		    /* [CWC] fix conducts for potions of (vampire) blood -
		       doesn't use violated_vegetarian() to prevent
		       duplicated "you feel guilty" messages */
		    u.uconduct.unvegetarian++;
		    if (u.ualign.type == A_LAWFUL || Role_if(PM_MONK)) {
#if 0 /*JP*/
			You_feel("%sguilty about drinking such a vile liquid.",
				Role_if(PM_MONK) ? "especially " : "");
#else
			You("不道徳なものを飲んでしまったことに罪を%s感じた。",
				Role_if(PM_MONK) ? "激しく" : "");
#endif
			u.ugangr++;
			adjalign(-15);
		    } else if (u.ualign.type == A_NEUTRAL)
			adjalign(-3);
		    exercise(A_CON, FALSE);
		    if (!Unchanging) {
			int successful_polymorph = FALSE;
			if (otmp->blessed)
				successful_polymorph = polymon(PM_VAMPIRE_LORD);
			else if (otmp->cursed)
				successful_polymorph = polymon(PM_VAMPIRE_BAT);
			else
				successful_polymorph = polymon(PM_VAMPIRE);
			if (successful_polymorph)
				u.mtimedone = 0;	/* "Permament" change */
		    }
		} else {
		    violated_vegetarian();
/*JP
		    pline("Ugh.  That was vile.");
*/
		    pline("ウゲェー、こりゃ酷い。");
		    make_vomiting(Vomiting+d(10,8), TRUE);
		}
		break;
	default:
		impossible("What a funny potion! (%u)", otmp->otyp);
		return(0);
	}
	return(-1);
}

#ifdef	JPEXTENSION
void
make_totter(xtime, talk)
long xtime;	/* nonzero if this is an attempt to turn on hallucination */
boolean talk;
{
	const char *message = 0;

	if (!xtime)
	    message = "方向感覚が正常になった。";
	else
	    message = "方向感覚が麻痺した。";

	set_itimeout(&Totter, xtime);
	pline(message);
}
#endif

void
healup(nhp, nxtra, curesick, cureblind)
	int nhp, nxtra;
	register boolean curesick, cureblind;
{
	if (nhp) {
		if (Upolyd) {
			u.mh += nhp;
			if (u.mh > u.mhmax) u.mh = (u.mhmax += nxtra);
		} else {
			u.uhp += nhp;
			if(u.uhp > u.uhpmax) u.uhp = (u.uhpmax += nxtra);
		}
	}
	if(cureblind)	make_blinded(0L,TRUE);
	if(curesick)	make_sick(0L, (char *) 0, TRUE, SICK_ALL);
	flags.botl = 1;
	return;
}

void
strange_feeling(obj,txt)
register struct obj *obj;
register const char *txt;
{
	if (flags.beginner || !txt)
#if 0 /*JP*/
		You("have a %s feeling for a moment, then it passes.",
		Hallucination ? "normal" : "strange");
#else
		You("%s気分におそわれたが、すぐに消えさった。",
		Hallucination ? "普通の" : "奇妙な");
#endif
	else
		pline(txt);

	if(!obj)	/* e.g., crystal ball finds no traps */
		return;

	if(obj->dknown && !objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	useup(obj);
}

const char *bottlenames[] = {
#if 0 /*JP*/
	"bottle", "phial", "flagon", "carafe", "flask", "jar", "vial"
#else
	"瓶","瑠璃瓶","一升瓶","水差し","フラスコ","壷","ガラス瓶"
#endif
};


const char *
bottlename()
{
	return bottlenames[rn2(SIZE(bottlenames))];
}

void
potionhit(mon, obj, your_fault)
register struct monst *mon;
register struct obj *obj;
boolean your_fault;
{
	register const char *botlnam = bottlename();
	boolean isyou = (mon == &youmonst);
	int distance;
#ifdef WEBB_DISINT
	boolean disint = (touch_disintegrates(mon->data) && 
	                  !oresist_disintegration(obj) &&
	                  !mon->mcan &&
	                   mon->mhp>6);
#endif

	if (isyou) {
		distance = 0;
/*JP
		pline_The("%s crashes on your %s and breaks into shards.",
*/
		pline("%sがあなたの%sの上で壊れ破片となった。",
			botlnam, body_part(HEAD));
/*JP
		losehp(rnd(2), "thrown potion", KILLED_BY_AN);
*/
		losehp(rnd(2), "投げられた薬で", KILLED_BY_AN);
	} else {
		distance = distu(mon->mx,mon->my);
#if 0 /*JP*/
#ifdef WEBB_DISINT
		if (!cansee(mon->mx,mon->my)) pline(disint?"Vip!":"Crash!");
#else
		if (!cansee(mon->mx,mon->my)) pline("Crash!");
#endif
#else /*JP*/
#ifdef WEBB_DISINT
		if (!cansee(mon->mx,mon->my)) pline(disint?"バシュッ！":"ガシャン！");
#else
		if (!cansee(mon->mx,mon->my)) pline("ガシャン！");
#endif
#endif /*JP*/
		else {
		    char *mnam = mon_nam(mon);
		    char buf[BUFSZ];

		    if(has_head(mon->data)) {
#if 0 /*JP*/
			Sprintf(buf, "%s %s",
				s_suffix(mnam),
				(notonhead ? "body" : "head"));
#else
			Sprintf(buf, "%sの%s", s_suffix(mnam),
				(notonhead ? "体" : "頭"));
#endif
		    } else {
			Strcpy(buf, mnam);
		    }
#ifdef WEBB_DISINT
#  if 0 /*JP*/
		    pline_The("%s crashes on %s and %s.",
			   botlnam, buf, disint?"disintegrates":"breaks into shards");
#  else
		    pline_The("%sが%sの上で%s。",
			   botlnam, buf, disint?"粉砕された":"壊れ破片となった");
#  endif
#else
#  if 0 /*JP*/
          pline_The("%s crashes on %s breaks into shards.",
			   botlnam, buf);
#  else
          pline("%sが%sの上で壊れ破片となった。", botlnam, buf);
#  endif
#endif
		}
		if(rn2(5) && mon->mhp > 1)
			mon->mhp--;
	}

	/* oil doesn't instantly evaporate */
	if (obj->otyp != POT_OIL && cansee(mon->mx,mon->my) 
#ifdef WEBB_DISINT
       && !disint
#endif
       )
/*JP
		pline("%s.", Tobjnam(obj, "evaporate"));
*/
		pline("%sは蒸発した。", xname(obj));

    if (isyou) {
	switch (obj->otyp) {
	case POT_OIL:
		if (obj->lamplit)
		    splatter_burning_oil(u.ux, u.uy);
		break;
	case POT_POLYMORPH:
/*JP
		You_feel("a little %s.", Hallucination ? "normal" : "strange");
*/
		You("%sな感じがした。", Hallucination ? "普通" : "変");
		if (!Unchanging && !Antimagic) polyself(FALSE);
		break;
	case POT_ACID:
		if (!Acid_resistance) {
#if 0 /*JP*/
		    pline("This burns%s!", obj->blessed ? " a little" :
				    obj->cursed ? " a lot" : "");
#else
		    pline("%s燃えた！", obj->blessed ? "少し" :
				    obj->cursed ? "はげしく" : "");
#endif
#if 0 /*JP*/
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "potion of acid", KILLED_BY_AN);
#else
		    losehp(d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8),
				    "酸の薬を浴びて", KILLED_BY_AN);
#endif
		}
		break;
	}
    } else {
	boolean angermon = TRUE;

	if (!your_fault) angermon = FALSE;
#ifdef WEBB_DISINT
	if (!disint)
#endif
     {
	switch (obj->otyp) {
	case POT_HEALING:
	case POT_EXTRA_HEALING:
	case POT_FULL_HEALING:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_illness;
		/*FALLTHRU*/
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
 do_healing:
		angermon = FALSE;
		if(mon->mhp < mon->mhpmax) {
		    mon->mhp = mon->mhpmax;
		    if (canseemon(mon))
/*JP
			pline("%s looks sound and hale again.", Monnam(mon));
*/
			pline("%sは元気になったように見える。", Monnam(mon));
		}
		break;
	case POT_SICKNESS:
		if (mon->data == &mons[PM_PESTILENCE]) goto do_healing;
		if (dmgtype(mon->data, AD_DISE) ||
			   dmgtype(mon->data, AD_PEST) || /* won't happen, see prior goto */
			   resists_poison(mon)) {
		    if (canseemon(mon))
#if 0 /*JP*/
			pline("%s looks unharmed.", Monnam(mon));
#else
			pline("%sはなんともないようだ。", Monnam(mon));
#endif
		    break;
		}
 do_illness:
		if((mon->mhpmax > 3) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhpmax /= 2;
		if((mon->mhp > 2) && !resist(mon, POTION_CLASS, 0, NOTELL))
			mon->mhp /= 2;
		if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
		if (canseemon(mon))
/*JP
		    pline("%s looks rather ill.", Monnam(mon));
*/
		    pline("%sは病気っぽく見える。", Monnam(mon));
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
		if(!resist(mon, POTION_CLASS, 0, NOTELL))  mon->mconf = TRUE;
		break;
	case POT_INVISIBILITY:
		angermon = FALSE;
		mon_set_minvis(mon);
		break;
	case POT_SLEEPING:
		/* wakeup() doesn't rouse victims of temporary sleep */
		if (sleep_monst(mon, rnd(12), POTION_CLASS)) {
/*JP
		    pline("%s falls asleep.", Monnam(mon));
*/
		    pline("%sは眠ってしまった。", Monnam(mon));
		    slept_monst(mon);
		}
		break;
	case POT_PARALYSIS:
		if (mon->mcanmove) {
			mon->mcanmove = 0;
			/* really should be rnd(5) for consistency with players
			 * breathing potions, but...
			 */
			mon->mfrozen = rnd(25);
		}
		break;
	case POT_SPEED:
		angermon = FALSE;
		mon_adjust_speed(mon, 1, obj);
		break;
	case POT_BLINDNESS:
		if(haseyes(mon->data)) {
		    register int btmp = 64 + rn2(32) +
			rn2(32) * !resist(mon, POTION_CLASS, 0, NOTELL);
		    btmp += mon->mblinded;
		    mon->mblinded = min(btmp,127);
		    mon->mcansee = 0;
		}
		break;
	case POT_WATER:
		if (is_undead(mon->data) || is_demon(mon->data) ||
			is_were(mon->data)) {
		    if (obj->blessed) {
#if 0 /*JP*/
			pline("%s %s in pain!", Monnam(mon),
			      is_silent(mon->data) ? "writhes" : "shrieks");
#else
			pline("%sは苦痛%s！", Monnam(mon),
			      is_silent(mon->data) ? "に身もだえした" : "の叫び声をあげた");
#endif
			mon->mhp -= d(2,6);
			/* should only be by you */
			if (mon->mhp < 1) killed(mon);
			else if (is_were(mon->data) && !is_human(mon->data))
			    new_were(mon);	/* revert to human */
		    } else if (obj->cursed) {
			angermon = FALSE;
			if (canseemon(mon))
/*JP
			    pline("%s looks healthier.", Monnam(mon));
*/
			    pline("%sはより元気になったように見える。", Monnam(mon));
			mon->mhp += d(2,6);
			if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
			if (is_were(mon->data) && is_human(mon->data) &&
				!Protection_from_shape_changers)
			    new_were(mon);	/* transform into beast */
		    }
		} else if(mon->data == &mons[PM_GREMLIN]) {
		    angermon = FALSE;
		    (void)split_mon(mon, (struct monst *)0);
		} else if(mon->data == &mons[PM_IRON_GOLEM]) {
		    if (canseemon(mon))
#if 0 /*JP*/
			pline("%s rusts.", Monnam(mon));
#else
			pline("%sは錆びた。", Monnam(mon));
#endif
		    mon->mhp -= d(1,6);
		    /* should only be by you */
		    if (mon->mhp < 1) killed(mon);
		}
		break;
	case POT_OIL:
		if (obj->lamplit)
			splatter_burning_oil(mon->mx, mon->my);
		break;
	case POT_ACID:
		if (!resists_acid(mon) && !resist(mon, POTION_CLASS, 0, NOTELL)) {
#if 0 /*JP*/
		    pline("%s %s in pain!", Monnam(mon),
			  is_silent(mon->data) ? "writhes" : "shrieks");
#else
		    pline("%sは苦痛%s！", Monnam(mon),
			  is_silent(mon->data) ? "に身もだえした" : "の叫び声をあげた");
#endif
		    mon->mhp -= d(obj->cursed ? 2 : 1, obj->blessed ? 4 : 8);
		    if (mon->mhp < 1) {
			if (your_fault)
			    killed(mon);
			else
			    monkilled(mon, "", AD_ACID);
		    }
		}
		break;
	case POT_POLYMORPH:
		(void) bhitm(mon, obj);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
		break;
*/
	}
     }

	if (angermon)
	    wakeup(mon);
	else
	    mon->msleeping = 0;
    }
#ifdef WEBB_DISINT
    if (!disint)
#endif 
   {

	/* Note: potionbreathe() does its own docall() */
	if ((distance==0 || ((distance < 3) && rn2(5))) &&
	    (!breathless(youmonst.data) || haseyes(youmonst.data)))
		potionbreathe(obj);
	else if (obj->dknown && !objects[obj->otyp].oc_name_known &&
		   !objects[obj->otyp].oc_uname && cansee(mon->mx,mon->my))
		docall(obj);
   }
	if(*u.ushops && obj->unpaid) {
	        register struct monst *shkp =
			shop_keeper(*in_rooms(u.ux, u.uy, SHOPBASE));

		if(!shkp)
		    obj->unpaid = 0;
		else {
		    (void)stolen_value(obj, u.ux, u.uy,
				 (boolean)shkp->mpeaceful, FALSE);
		    subfrombill(obj, shkp);
		}
	}
	obfree(obj, (struct obj *)0);
}

/* vapors are inhaled or get in your eyes */
void
potionbreathe(obj)
register struct obj *obj;
{
	register int i, ii, kn = 0;

	switch(obj->otyp) {
	case POT_RESTORE_ABILITY:
	case POT_GAIN_ABILITY:
		if(obj->cursed) {
		    if (!breathless(youmonst.data))
/*JP
			pline("Ulch!  That potion smells terrible!");
*/
			pline("ウゲェ！薬はものすごい匂いがする！");
		    else if (haseyes(youmonst.data)) {
#if 0 /*JP*/
			int numeyes = eyecount(youmonst.data);
			Your("%s sting%s!",
			     (numeyes == 1) ? body_part(EYE) : makeplural(body_part(EYE)),
			     (numeyes == 1) ? "s" : "");
#else
			Your("%sがずきずきする！", body_part(EYE));
#endif
		    }
		    break;
		} else {
		    i = rn2(A_MAX);		/* start at a random point */
		    for(ii = 0; ii < A_MAX; ii++) {
			if(ABASE(i) < AMAX(i)) {
			    ABASE(i)++;
			    flags.botl = 1;
			}
			if(++i >= A_MAX) i = 0;
		    }
		}
		break;
	case POT_FULL_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_EXTRA_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		/*FALL THROUGH*/
	case POT_HEALING:
		if (Upolyd && u.mh < u.mhmax) u.mh++, flags.botl = 1;
		if (u.uhp < u.uhpmax) u.uhp++, flags.botl = 1;
		exercise(A_CON, TRUE);
		break;
	case POT_SICKNESS:
		if (!Role_if(PM_HEALER)) {
			if (Upolyd) {
			    if (u.mh <= 5) u.mh = 1; else u.mh -= 5;
			} else {
			    if (u.uhp <= 5) u.uhp = 1; else u.uhp -= 5;
			}
			flags.botl = 1;
			exercise(A_CON, FALSE);
		}
		break;
	case POT_HALLUCINATION:
/*JP
		You("have a momentary vision.");
*/
		You("一瞬幻影につつまれた。");
		break;
	case POT_CONFUSION:
	case POT_BOOZE:
		if(!Confusion)
/*JP
			You_feel("somewhat dizzy.");
*/
			You("めまいを感じた。");
		make_confused(itimeout_incr(HConfusion, rnd(5)), FALSE);
		break;
	case POT_INVISIBILITY:
		if (!Blind && !Invis) {
		    kn++;
#if 0 /*JP*/
		    pline("For an instant you %s!",
			See_invisible ? "could see right through yourself"
			: "couldn't see yourself");
#else
		    pline("一瞬自分自身が%s見えなくなった！",
			See_invisible ? "正しく"
			: "");
#endif
		}
		break;
	case POT_PARALYSIS:
		kn++;
		if (!Free_action) {
/*JP
		    pline("%s seems to be holding you.", Something);
*/
		    pline("%sがあなたをつかまえているような気がした。", Something);
/*JP
		    nomul(-rnd(5), "frozen by a potion");
*/
		    nomul(-rnd(5), "薬で動けなくなった隙に");
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
/*JP
		} else You("stiffen momentarily.");
*/
		} else You("一瞬硬直した。");
		break;
	case POT_SLEEPING:
		kn++;
		if (!Free_action && !Sleep_resistance) {
/*JP
		    You_feel("rather tired.");
*/
		    You("すこし疲れた。");
/*JP
		    nomul(-rnd(5), "sleeping off a magical draught");
*/
		    nomul(-rnd(5), "魔法的に眠ってしまった隙に");
		    nomovemsg = You_can_move_again;
		    exercise(A_DEX, FALSE);
/*JP
		} else You("yawn.");
*/
		} else You("あくびをした。");
		break;
	case POT_SPEED:
/*JP
		if (!Fast) Your("knees seem more flexible now.");
*/
		if (!Fast) Your("膝はよりすばやく動くようになった。");
		incr_itimeout(&HFast, rnd(5));
		exercise(A_DEX, TRUE);
		break;
	case POT_BLINDNESS:
		if (!Blind && !u.usleep) {
		    kn++;
/*JP
		    pline("It suddenly gets dark.");
*/
		    pline("突然暗くなった。");
		}
		make_blinded(itimeout_incr(Blinded, rnd(5)), FALSE);
		if (!Blind && !u.usleep) Your(vision_clears);
		break;
	case POT_WATER:
		if(u.umonnum == PM_GREMLIN) {
		    (void)split_mon(&youmonst, (struct monst *)0);
		} else if (u.ulycn >= LOW_PM) {
		    /* vapor from [un]holy water will trigger
		       transformation but won't cure lycanthropy */
		    if (obj->blessed && youmonst.data == &mons[u.ulycn])
			you_unwere(FALSE);
		    else if (obj->cursed && !Upolyd)
			you_were();
		}
		break;
	case POT_ACID:
	case POT_POLYMORPH:
		exercise(A_CON, FALSE);
		break;
	case POT_BLOOD:
	case POT_VAMPIRE_BLOOD:
		if (maybe_polyd(is_vampire(youmonst.data), Race_if(PM_VAMPIRE))) {
		    exercise(A_WIS, FALSE);
#if 0 /*JP*/
		    You_feel("a %ssense of loss.",
		      obj->otyp == POT_VAMPIRE_BLOOD ? "terrible " : "");
#else
		    You("%s喪失感におそわれた。",
		      obj->otyp == POT_VAMPIRE_BLOOD ? "恐ろしい" : "");
#endif
		} else
		    exercise(A_CON, FALSE);
		break;
/*
	case POT_GAIN_LEVEL:
	case POT_LEVITATION:
	case POT_FRUIT_JUICE:
	case POT_MONSTER_DETECTION:
	case POT_OBJECT_DETECTION:
	case POT_OIL:
		break;
*/
	}
	/* note: no obfree() */
	if (obj->dknown) {
	    if (kn)
		makeknown(obj->otyp);
	    else if (!objects[obj->otyp].oc_name_known &&
						!objects[obj->otyp].oc_uname)
		docall(obj);
	}
}

/* new alchemy scheme based on color mixing 
 * YANI by Graham Cox <aca00gac@shef.ac.uk>
 * Implemented by Nephi Allred <zindorsky@hotmail.com> on 15 Apr 2003
 *
 *	Alchemical tables are based on 4 bits describing dark/light level, yellow, blue and red
 *
 *	DYBR
 *	0000	white
 *	0001	pink
 *	0010	sky-blue
 *	0011	puce
 *	0100	yellow
 *	0101	orange
 *	0110	emerald
 *	0111	ochre
 *	1000	black
 *	1001	ruby
 *	1010	indigo
 *	1011	magenta
 *	1100	golden
 *	1101	amber
 *	1110	dark green
 *	1111	brown
 */

/* Assumes gain ability is first potion and vampire blood is last */
char alchemy_table1[POT_VAMPIRE_BLOOD - POT_GAIN_ABILITY];
short alchemy_table2[17];

#define ALCHEMY_WHITE 0
#define ALCHEMY_BLACK 8
#define ALCHEMY_GRAY (alchemy_table2[16])
#define IS_PRIMARY_COLOR(x)		(((x)&7)==1 || ((x)&7)==2 || ((x)&7)==4)
#define IS_SECONDARY_COLOR(x)	(((x)&7)==3 || ((x)&7)==5 || ((x)&7)==6)
#define IS_LIGHT_COLOR(x)		(((x)&8)==0)
#define IS_DARK_COLOR(x)		((x)&8)

/** Does a one-time set up of alchemical tables. */
STATIC_OVL void
alchemy_init()
{
	static boolean init = FALSE;

	if (!init) {
		short i;
		const char* potion_desc;

		for(i=POT_GAIN_ABILITY; i<=POT_WATER; i++) {
			potion_desc = OBJ_DESCR(objects[i]);
			if (0==strcmp(potion_desc,"white")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=0;
				alchemy_table2[0]=i;
			} else if (0==strcmp(potion_desc,"pink")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=1;
				alchemy_table2[1]=i;
			} else if (0==strcmp(potion_desc,"sky blue")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=2;
				alchemy_table2[2]=i;
			} else if (0==strcmp(potion_desc,"puce")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=3;
				alchemy_table2[3]=i;
			} else if (0==strcmp(potion_desc,"yellow")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=4;
				alchemy_table2[4]=i;
			} else if (0==strcmp(potion_desc,"orange")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=5;
				alchemy_table2[5]=i;
			} else if (0==strcmp(potion_desc,"emerald")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=6;
				alchemy_table2[6]=i;
			} else if (0==strcmp(potion_desc,"ochre")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=7;
				alchemy_table2[7]=i;
			} else if (0==strcmp(potion_desc,"black")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=8;
				alchemy_table2[8]=i;
			} else if (0==strcmp(potion_desc,"ruby")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=9;
				alchemy_table2[9]=i;
			} else if (0==strcmp(potion_desc,"indigo")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=10;
				alchemy_table2[10]=i;
			} else if (0==strcmp(potion_desc,"magenta")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=11;
				alchemy_table2[11]=i;
			} else if (0==strcmp(potion_desc,"golden")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=12;
				alchemy_table2[12]=i;
			} else if (0==strcmp(potion_desc,"amber")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=13;
				alchemy_table2[13]=i;
			} else if (0==strcmp(potion_desc,"dark green")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=14;
				alchemy_table2[14]=i;
			} else if (0==strcmp(potion_desc,"brown")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=15;
				alchemy_table2[15]=i;
			} else if (0==strcmp(potion_desc,"silver")) {
				alchemy_table1[i-POT_GAIN_ABILITY]=-1;
				alchemy_table2[16]=i;
			} else {
				alchemy_table1[i-POT_GAIN_ABILITY]=-1;
			}
		}
		init = TRUE;
	}
}

/** Returns the potion type when object o1 is dipped into object o2 */
STATIC_OVL short
mixtype(o1, o2)
register struct obj *o1, *o2;
{
	/* cut down on the number of cases below */
	if (o1->oclass == POTION_CLASS &&
	    (o2->otyp == POT_FRUIT_JUICE)) {
		struct obj *swp;

		swp = o1; o1 = o2; o2 = swp;
	}
	switch (o1->otyp) {
		case POT_FRUIT_JUICE:
			switch (o2->otyp) {
				case POT_BLOOD:
					return POT_BLOOD;
				case POT_VAMPIRE_BLOOD:
					return POT_VAMPIRE_BLOOD;
			}
			break;
	}

	if (o1->oclass == POTION_CLASS) {
		int i1,i2,result;

		alchemy_init();
		i1 = alchemy_table1[o1->otyp-POT_GAIN_ABILITY];
		i2 = alchemy_table1[o2->otyp-POT_GAIN_ABILITY];
		
		/* check that both potions are of mixable types */
		if (i1<0 || i2<0)
			return 0;

		/* swap for simplified checks */
		if (i2==ALCHEMY_WHITE || (i2==ALCHEMY_BLACK && i1!=ALCHEMY_WHITE)) {
			result = i1;
			i1 = i2;
			i2 = result;
		}

		if (i1==ALCHEMY_WHITE && i2==ALCHEMY_BLACK) {
			return ALCHEMY_GRAY;
		} else if (	(IS_PRIMARY_COLOR(i1) && IS_PRIMARY_COLOR(i2))
					|| (IS_SECONDARY_COLOR(i1) && IS_SECONDARY_COLOR(i2)) ) {
			/* bitwise OR simulates pigment addition */
			result = i1 | i2;
			/* adjust light/dark level if necessary */
			if ((i1^i2)&8) {
				if (o1->odiluted==o2->odiluted) {
					/* same dilution level, randomly toggle */
					result ^= (rn2(2)<<3);
				} else {
					/* use dark/light level of undiluted potion */
					result ^= (o1->odiluted ? i1:i2)&8;
				}
			}
		} else if ((i1==ALCHEMY_WHITE && IS_DARK_COLOR(i2)) ||
		           (i1==ALCHEMY_BLACK && IS_LIGHT_COLOR(i2))) {
			/* toggle light/dark bit */
			result = i2 ^ 8;
		} else {
			return 0;
		}
		if (OBJ_NAME(objects[alchemy_table2[result]]) == 0) {
			/* mixed potion doesn't exist in this game */
			return 0;
		} else {
			return alchemy_table2[result];
		}
	} else {
		switch (o1->otyp) {
			case UNICORN_HORN:
				switch (o2->otyp) {
					case POT_SICKNESS:
						return POT_FRUIT_JUICE;
					case POT_HALLUCINATION:
					case POT_BLINDNESS:
					case POT_CONFUSION:
					case POT_BLOOD:
					case POT_VAMPIRE_BLOOD:
						return POT_WATER;
				}
				break;
			case AMETHYST:		/* "a-methyst" == "not intoxicated" */
				if (o2->otyp == POT_BOOZE)
					return POT_FRUIT_JUICE;
				break;
		}
	}

	return 0;
}


boolean
get_wet(obj)
register struct obj *obj;
/* returns TRUE if something happened (potion should be used up) */
{
	char Your_buf[BUFSZ];

	if (snuff_lit(obj)) return(TRUE);

	if (obj->greased) {
		grease_protect(obj,(char *)0,&youmonst);
		return(FALSE);
	}
	(void) Shk_Your(Your_buf, obj);
	/* (Rusting shop goods ought to be charged for.) */
	switch (obj->oclass) {
	    case POTION_CLASS:
		if (obj->otyp == POT_WATER) return FALSE;
		/* KMH -- Water into acid causes an explosion */
		if (obj->otyp == POT_ACID) {
/*JP
			pline("It boils vigorously!");
*/
			pline("激しく沸騰した！");
/*JP
			You("are caught in the explosion!");
*/
			You("爆発に巻き込まれた！");
			losehp(Acid_resistance ? rnd(5) : rnd(10),
/*JP
			       "elementary chemistry", KILLED_BY);
*/
			       "初等化学実験で", KILLED_BY);
			makeknown(obj->otyp);
			update_inventory();
			return (TRUE);
		}
#if 0 /*JP*/
		pline("%s %s%s.", Your_buf, aobjnam(obj,"dilute"),
		      obj->odiluted ? " further" : "");
#else
		pline("%s%sは%s薄まった。", Your_buf, xname(obj),
		      obj->odiluted ? "さらに" : "");
#endif
		if(obj->unpaid && costly_spot(u.ux, u.uy)) {
/*JP
		    You("dilute it, you pay for it.");
*/
		    You("薄めてしまった、賠償せねばならない。");
		    bill_dummy_object(obj);
		}
		if (obj->odiluted) {
			obj->odiluted = 0;
#ifdef UNIXPC
			obj->blessed = FALSE;
			obj->cursed = FALSE;
#else
			obj->blessed = obj->cursed = FALSE;
#endif
			obj->otyp = POT_WATER;
		} else obj->odiluted++;
		update_inventory();
		return TRUE;
	    case SCROLL_CLASS:
		if (obj->otyp != SCR_BLANK_PAPER
#ifdef MAIL
		    && obj->otyp != SCR_MAIL
#endif
		    ) {
			if (!Blind) {
#if 0 /*JP*/
				boolean oq1 = obj->quan == 1L;
				pline_The("scroll%s %s.",
					  oq1 ? "" : "s", otense(obj, "fade"));
#else
				pline("巻物の文字は薄れた。");
#endif
			}
			if(obj->unpaid && costly_spot(u.ux, u.uy)) {
/*JP
			    You("erase it, you pay for it.");
*/
			    You("文字を消してしまった、賠償せねばならない。");
			    bill_dummy_object(obj);
			}
			obj->otyp = SCR_BLANK_PAPER;
			obj->spe = 0;
			update_inventory();
			return TRUE;
		} else break;
	    case SPBOOK_CLASS:
		if (obj->otyp != SPE_BLANK_PAPER) {

			if (obj->otyp == SPE_BOOK_OF_THE_DEAD) {
/*JP
	pline("%s suddenly heats up; steam rises and it remains dry.",
*/
	pline("%sは突然熱くなり、蒸気がたちこめ、乾いてしまった。",
				The(xname(obj)));
			} else {
			    if (!Blind) {
#if 0 /*JP*/
				    boolean oq1 = obj->quan == 1L;
				    pline_The("spellbook%s %s.",
					oq1 ? "" : "s", otense(obj, "fade"));
#else
				    pline("呪文書の文字は薄れた。");
#endif
			    }
			    if(obj->unpaid && costly_spot(u.ux, u.uy)) {
/*JP
			        You("erase it, you pay for it.");
*/
			        You("文字を消してしまった、賠償せねばならない。");
			        bill_dummy_object(obj);
			    }
			    obj->otyp = SPE_BLANK_PAPER;
			    update_inventory();
			}
			return TRUE;
		}
		break;
	    case WEAPON_CLASS:
	    /* Just "fall through" to generic rustprone check for now. */
	    /* fall through */
	    default:
		if (!obj->oerodeproof && is_rustprone(obj) &&
		    (obj->oeroded < MAX_ERODE) && !rn2(2)) {
#if 0 /*JP*/
			pline("%s %s some%s.",
			      Your_buf, aobjnam(obj, "rust"),
			      obj->oeroded ? " more" : "what");
#else
			pline("%s%sは%s錆びた。", Your_buf, xname(obj),
			      obj->oeroded ? "さらに" : "");
#endif
			obj->oeroded++;
			update_inventory();
			return TRUE;
		} else break;
	}
/*JP
	pline("%s %s wet.", Your_buf, aobjnam(obj,"get"));
*/
	pline("%s%sは濡れた。", Your_buf, xname(obj));
	return FALSE;
}

/** User command for dipping objects. */
int
dodip()
{
	register struct obj *potion, *obj;
	const char *tmp;
	uchar here;
	char allowall[2];
	char qbuf[QBUFSZ];

	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	if(!(obj = getobj(allowall, "dip")))
		return(0);

	here = levl[u.ux][u.uy].typ;
	/* Is there a fountain to dip into here? */
	if (IS_FOUNTAIN(here)) {
#if 0 /*JP*/
		Sprintf(qbuf, "Dip %s into the fountain?",
				safe_qbuf("", sizeof("Dip  into the fountain?"),
					the(xname(obj)), the(simple_typename(obj->otyp)),
					"this item"));
#else
		Sprintf(qbuf, "泉に%sを浸しますか？",
				safe_qbuf("", sizeof("泉にを浸しますか？"),
					xname(obj), simple_typename(obj->otyp),
					"このアイテム"));
#endif
		if(yn(qbuf) == 'y') {
			dipfountain(obj);
			return(1);
		}
	} else if (is_pool(u.ux,u.uy)) {
		tmp = waterbody_name(u.ux,u.uy);
#if 0 /*JP*/
		Sprintf(qbuf, "Dip %s into the %s?", 
				safe_qbuf("", sizeof("Dip  into the pool of water?"),
					the(xname(obj)), the(simple_typename(obj->otyp)),
					"this item"), tmp);
#else
		Sprintf(qbuf, "%sに%sを浸しますか？", tmp,
				safe_qbuf("", sizeof("水たまりにを浸しますか？"),
					xname(obj), simple_typename(obj->otyp),
					"このアイテム"));
#endif
		if (yn(qbuf) == 'y') {
		    if (Levitation) {
			floating_above(tmp);
#ifdef STEED
		    } else if (u.usteed && !is_swimmer(u.usteed->data) &&
			    P_SKILL(P_RIDING) < P_BASIC) {
			rider_cant_reach(); /* not skilled enough to reach */
#endif
		    } else {
			(void) get_wet(obj);
			if (obj->otyp == POT_ACID) useup(obj);
		    }
		    return 1;
		}
	}

#if 0 /*JP*/
	Sprintf(qbuf, "dip %s into",
			safe_qbuf("", sizeof("dip  into"),
				the(xname(obj)), the(simple_typename(obj->otyp)),
				"this item"));
#else
	Sprintf(qbuf, "dip into:%s",
			safe_qbuf("", sizeof("dip into:"),
				xname(obj), simple_typename(obj->otyp),
				"このアイテム"));
#endif
	if(!(potion = getobj(beverages, qbuf)))
		return(0);
	if (potion == obj && potion->quan == 1L) {
/*JP
		pline("That is a potion bottle, not a Klein bottle!");
*/
		pline("これは薬瓶だ！クラインの壷じゃない！");
		return 0;
	}

	return dip(potion, obj);
}

/** Dip an arbitrary object into a potion. */
int
dip(potion, obj)
struct obj *potion, *obj;
{
	struct obj *singlepotion;
	const char *tmp;
	short mixture;
	char Your_buf[BUFSZ];

	potion->in_use = TRUE;		/* assume it will be used up */
	if(potion->otyp == POT_WATER) {
		boolean useeit = !Blind;
		if (useeit) (void) Shk_Your(Your_buf, obj);
		if (potion->blessed) {
			if (obj->cursed) {
				if (useeit)
#if 0 /*JP*/
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  hcolor(NH_AMBER));
#else
				    pline("%s%sはうっすらと%s輝いた。",
					  Your_buf,
					  xname(obj),
					  jconj_adj(hcolor(NH_AMBER)));
#endif
				uncurse(obj);
				obj->bknown=1;
	poof:
				if(!(objects[potion->otyp].oc_name_known) &&
				   !(objects[potion->otyp].oc_uname))
					docall(potion);
				useup(potion);
				return(1);
			} else if(!obj->blessed) {
				if (useeit) {
				    tmp = hcolor(NH_LIGHT_BLUE);
#if 0 /*JP*/
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "softly glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
#else
				    pline("%s%sはぼんやりとした%sオーラにつつまれた。",
					  Your_buf, xname(obj), tmp);
#endif
				}
				bless(obj);
				obj->bknown=1;
				goto poof;
			}
		} else if (potion->cursed) {
			if (obj->blessed) {
				if (useeit)
#if 0 /*JP*/
				    pline("%s %s %s.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  hcolor((const char *)"brown"));
#else
				    pline("%s%sは%s輝いた。",
					  Your_buf, xname(obj),
					  jconj_adj(hcolor((const char *)"茶色の")));
#endif
				unbless(obj);
				obj->bknown=1;
				goto poof;
			} else if(!obj->cursed) {
				if (useeit) {
				    tmp = hcolor(NH_BLACK);
#if 0 /*JP*/
				    pline("%s %s with a%s %s aura.",
					  Your_buf,
					  aobjnam(obj, "glow"),
					  index(vowels, *tmp) ? "n" : "", tmp);
#else
				    pline("%s%sは%sオーラにつつまれた。",
					  Your_buf, xname(obj),tmp);
#endif
				}
				curse(obj);
				obj->bknown=1;
				goto poof;
			}
		} else
			if (get_wet(obj))
			    goto poof;
	} else if (obj->otyp == POT_POLYMORPH ||
		potion->otyp == POT_POLYMORPH) {
	    /* some objects can't be polymorphed */
	    if (obj->otyp == potion->otyp ||	/* both POT_POLY */
		    obj->otyp == WAN_POLYMORPH ||
		    obj->otyp == SPE_POLYMORPH ||
		    obj->otyp == AMULET_OF_UNCHANGING ||
		    obj == uball || obj == uskin ||
		    obj_resists(obj->otyp == POT_POLYMORPH ?
				potion : obj, 5, 95)) {
		pline(nothing_happens);
	    } else {
	    	boolean was_wep = FALSE, was_swapwep = FALSE, was_quiver = FALSE;
		short save_otyp = obj->otyp;
		/* KMH, conduct */
		u.uconduct.polypiles++;

		if (obj == uwep) was_wep = TRUE;
		else if (obj == uswapwep) was_swapwep = TRUE;
		else if (obj == uquiver) was_quiver = TRUE;

		obj = poly_obj(obj, STRANGE_OBJECT);

		if (was_wep) setuwep(obj);
		else if (was_swapwep) setuswapwep(obj);
		else if (was_quiver) setuqwep(obj);

		if (obj->otyp != save_otyp) {
			makeknown(POT_POLYMORPH);
			useup(potion);
			prinv((char *)0, obj, 0L);
			return 1;
		} else {
/*JP
			pline("Nothing seems to happen.");
*/
			pline("何も起こらなかったようだ。");
			goto poof;
		}
	    }
	    potion->in_use = FALSE;	/* didn't go poof */
	    return(1);
	} else if(obj->oclass == POTION_CLASS && obj->otyp != potion->otyp) {
		/* Mixing potions is dangerous... */
		/* Give a clue to what's going on ... */
		if(potion->dknown && obj->dknown) {
#if 0 /*JP*/
			You("mix the %s potion with the %s one ...", 
				OBJ_DESCR(objects[potion->otyp]),
				OBJ_DESCR(objects[obj->otyp]));
#else
			You("%s薬に%s薬を調合した．．．", 
				jtrns_obj(POTION_SYM, OBJ_DESCR(objects[potion->otyp])),
				jtrns_obj(POTION_SYM, OBJ_DESCR(objects[obj->otyp])));
#endif
		} else
/*JP
			pline_The("potions mix...");
*/
			pline("薬は調合された．．．");
		/* KMH, balance patch -- acid is particularly unstable */
		if (obj->cursed || obj->otyp == POT_ACID ||
		    potion->otyp == POT_ACID ||
		    !rn2(10)) {
/*JP
			pline("BOOM!  They explode!");
*/
			pline("バーン！爆発した！");
			exercise(A_STR, FALSE);
			if (!breathless(youmonst.data) || haseyes(youmonst.data))
				potionbreathe(obj);
			useup(obj);
			useup(potion);
			losehp(Acid_resistance ? rnd(5) : rnd(10),
/*JP
			       "alchemic blast", KILLED_BY_AN);
*/
			       "調合の失敗で", KILLED_BY_AN);
			return(1);
		}

		obj->blessed = obj->cursed = obj->bknown = 0;
		if (Blind || Hallucination) obj->dknown = 0;

		if ((mixture = mixtype(obj, potion)) != 0) {
			obj->otyp = mixture;
		} else {
		    switch (obj->odiluted ? 1 : rnd(8)) {
			case 1:
				obj->otyp = POT_WATER;
				break;
			case 2:
			case 3:
				obj->otyp = POT_SICKNESS;
				break;
			case 4:
				{
				  struct obj *otmp;
				  otmp = mkobj(POTION_CLASS,FALSE);
				  obj->otyp = otmp->otyp;
				  obfree(otmp, (struct obj *)0);
				}
				break;
			default:
				if (!Blind)
/*JP
			  pline_The("mixture glows brightly and evaporates.");
*/
				  pline("混ぜると薬は明るく輝き、蒸発した。");
				useup(obj);
				useup(potion);
				return(1);
		    }
		}

		obj->odiluted = (obj->otyp != POT_WATER);

		if (OBJ_NAME(objects[obj->otyp]) == 0) {
			panic("dipping created an inexistant potion (%d)", obj->otyp);
		}

		if (obj->otyp == POT_WATER && !Hallucination) {
#if 0 /*JP*/
			pline_The("mixture bubbles%s.",
				Blind ? "" : ", then clears");
#else
			pline("薬を混ぜると%s泡だった。",
				Blind ? "" : "しばらく");
#endif
		} else if (!Blind) {
#if 0 /*JP*/
			pline_The("mixture looks %s.",
				hcolor(OBJ_DESCR(objects[obj->otyp])));
#else
			pline("混ぜた薬は%s薬に見える。",
				jtrns_obj(POTION_SYM,OBJ_DESCR(objects[obj->otyp])));
#endif
		}

		useup(potion);
		return(1);
	}

/*JP これは NH3.4では#defineされていない */
#ifdef INVISIBLE_OBJECTS
	if (potion->otyp == POT_INVISIBILITY && !obj->oinvis) {
		obj->oinvis = TRUE;
		if (!Blind) {
		    if (!See_invisible) pline("Where did %s go?",
		    		the(xname(obj)));
		    else You("notice a little haziness around %s.",
		    		the(xname(obj)));
		}
		goto poof;
	} else if (potion->otyp == POT_SEE_INVISIBLE && obj->oinvis) {
		obj->oinvis = FALSE;
		if (!Blind) {
		    if (!See_invisible) pline("So that's where %s went!",
		    		the(xname(obj)));
		    else pline_The("haziness around %s disappears.",
		    		the(xname(obj)));
		}
		goto poof;
	}
#endif
	if (potion->otyp == POT_ACID && obj->otyp == CORPSE &&
	    obj->corpsenm == PM_LICHEN && !Blind) {
#if 0 /*JP*/
		pline("%s %s %s around the edges.", The(cxname(obj)),
		      otense(obj, "turn"),
		      potion->odiluted ? hcolor(NH_ORANGE) : hcolor(NH_RED));
#else
		pline("%sの端が%s変色した。", cxname(obj),
		      jconj_adj(hcolor(potion->odiluted ? NH_ORANGE : NH_RED)));
#endif
		potion->in_use = FALSE;	/* didn't go poof */
		return(1);
	}

	if(is_poisonable(obj)) {
	    if(potion->otyp == POT_SICKNESS && !obj->opoisoned) {
		char buf[BUFSZ];
		if (potion->quan > 1L)
/*JP
		    Sprintf(buf, "One of %s", the(xname(potion)));
*/
		    Sprintf(buf, "%sの一つ", the(xname(potion)));
		else
		    Strcpy(buf, The(xname(potion)));
/*JP
		pline("%s forms a coating on %s.",
*/
		pline("%sが%sに塗られた。",
		      buf, the(xname(obj)));
		obj->opoisoned = TRUE;
		goto poof;
	    } else if(obj->opoisoned &&
		      (potion->otyp == POT_HEALING ||
		       potion->otyp == POT_EXTRA_HEALING ||
		       potion->otyp == POT_FULL_HEALING)) {
/*JP
		pline("A coating wears off %s.", the(xname(obj)));
*/
		pline("毒が%sから剥げおちた。", the(xname(obj)));
		obj->opoisoned = 0;
		goto poof;
	    }
	}

	if (potion->otyp == POT_OIL) {
	    boolean wisx = FALSE;
	    if (potion->lamplit) {	/* burning */
		int omat = objects[obj->otyp].oc_material;
		/* the code here should be merged with fire_damage */
		if (catch_lit(obj)) {
		    /* catch_lit does all the work if true */
		} else if (obj->oerodeproof || obj_resists(obj, 5, 95) ||
			   !is_flammable(obj) || obj->oclass == FOOD_CLASS) {
#if 0 /*JP*/
		    pline("%s %s to burn for a moment.",
			  Yname2(obj), otense(obj, "seem"));
#else
		    pline("%sはしばらくの間燃えた。", Yname2(obj));
#endif
		} else {
		    if ((omat == PLASTIC || omat == PAPER) && !obj->oartifact)
			obj->oeroded = MAX_ERODE;
#if 0 /*JP*/
		    pline_The("burning oil %s %s.",
			    obj->oeroded == MAX_ERODE ? "destroys" : "damages",
			    yname(obj));
#else
		    pline("%sは燃えている油によって%s。",
			    yname(obj),
			    obj->oeroded == MAX_ERODE ? "破壊された" : "傷つけられた");
#endif
		    if (obj->oeroded == MAX_ERODE) {
			obj_extract_self(obj);
			obfree(obj, (struct obj *)0);
			obj = (struct obj *) 0;
		    } else {
			/* we know it's carried */
			if (obj->unpaid) {
			    /* create a dummy duplicate to put on bill */
/*JP
			    verbalize("You burnt it, you bought it!");
*/
			    verbalize("燃やしたのなら買ってもらうよ！");
			    bill_dummy_object(obj);
			}
			obj->oeroded++;
		    }
		}
	    } else if (potion->cursed) {
/*JP
		pline_The("potion spills and covers your %s with oil.",
*/
		pline("油は飛び散りあなたの%sにかかった。",
			  makeplural(body_part(FINGER)));
		incr_itimeout(&Glib, d(2,10));
	    } else if (obj->oclass != WEAPON_CLASS && !is_weptool(obj)) {
		/* the following cases apply only to weapons */
		goto more_dips;
	    /* Oil removes rust and corrosion, but doesn't unburn.
	     * Arrows, etc are classed as metallic due to arrowhead
	     * material, but dipping in oil shouldn't repair them.
	     */
	    } else if ((!is_rustprone(obj) && !is_corrodeable(obj)) ||
			is_ammo(obj) || (!obj->oeroded && !obj->oeroded2)) {
		/* uses up potion, doesn't set obj->greased */
#if 0 /*JP*/
		pline("%s %s with an oily sheen.",
		      Yname2(obj), otense(obj, "gleam"));
#else
		pline("%sは油の光沢できらりと光った。",
		      Yname2(obj));
#endif
	    } else {
#if 0 /*JP*/
		pline("%s %s less %s.",
		      Yname2(obj), otense(obj, "are"),
		      (obj->oeroded && obj->oeroded2) ? "corroded and rusty" :
			obj->oeroded ? "rusty" : "corroded");
#else
		pline("%sの%sが取れた。", Yname2(obj),
		      (obj->oeroded && obj->oeroded2) ? "腐食と錆" :
		      obj->oeroded ? "錆" : "腐食");
#endif
		if (obj->oeroded > 0) obj->oeroded--;
		if (obj->oeroded2 > 0) obj->oeroded2--;
		wisx = TRUE;
	    }
	    exercise(A_WIS, wisx);
	    makeknown(potion->otyp);
	    useup(potion);
	    return 1;
	}
    more_dips:

	/* Allow filling of MAGIC_LAMPs to prevent identification by player */
	if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP) &&
	   (potion->otyp == POT_OIL)) {
	    /* Turn off engine before fueling, turn off fuel too :-)  */
	    if (obj->lamplit || potion->lamplit) {
		useup(potion);
		explode(u.ux, u.uy, 11, d(6,6), 0, EXPL_FIERY);
		exercise(A_WIS, FALSE);
		return 1;
	    }
	    /* Adding oil to an empty magic lamp renders it into an oil lamp */
	    if ((obj->otyp == MAGIC_LAMP) && obj->spe == 0) {
		obj->otyp = OIL_LAMP;
		obj->age = 0;
	    }
	    if (obj->age > 1000L) {
/*JP
		pline("%s %s full.", Yname2(obj), otense(obj, "are"));
*/
		pline("%sにはたっぷり入っている。", Yname2(obj));
		potion->in_use = FALSE;	/* didn't go poof */
	    } else {
/*JP
		You("fill %s with oil.", yname(obj));
*/
		You("%sに油を入れた。", yname(obj));
		check_unpaid(potion);	/* Yendorian Fuel Tax */
		obj->age += 2*potion->age;	/* burns more efficiently */
		if (obj->age > 1500L) obj->age = 1500L;
		useup(potion);
		exercise(A_WIS, TRUE);
	    }
	    makeknown(POT_OIL);
	    obj->spe = 1;
	    update_inventory();
	    return 1;
	}

	potion->in_use = FALSE;		/* didn't go poof */
	if ((obj->otyp == UNICORN_HORN || obj->otyp == AMETHYST) &&
	    (mixture = mixtype(obj, potion)) != 0) {
#if 0 /*JP*/
		char oldbuf[BUFSZ], newbuf[BUFSZ];
#else
		char oldbuf[BUFSZ];
#endif
		short old_otyp = potion->otyp;
		boolean old_dknown = FALSE;
#if 0 /*JP*/
		boolean more_than_one = potion->quan > 1;
#endif

		oldbuf[0] = '\0';
		if (potion->dknown) {
		    old_dknown = TRUE;
#if 0 /*JP*/
		    Sprintf(oldbuf, "%s ",
			    hcolor(OBJ_DESCR(objects[potion->otyp])));
#else
		    Sprintf(oldbuf, "%s",
			    hcolor(jtrns_obj(POTION_SYM, OBJ_DESCR(objects[potion->otyp]))));
#endif
		}
		/* with multiple merged potions, split off one and
		   just clear it */
		if (potion->quan > 1L) {
		    singlepotion = splitobj(potion, 1L);
		} else singlepotion = potion;
		
		if(singlepotion->unpaid && costly_spot(u.ux, u.uy)) {
/*JP
		    You("use it, you pay for it.");
*/
		    You("使ってしまった、賠償せねばならない。");
		    bill_dummy_object(singlepotion);
		}
		singlepotion->otyp = mixture;
		singlepotion->blessed = 0;
		if (mixture == POT_WATER)
		    singlepotion->cursed = singlepotion->odiluted = 0;
		else
		    singlepotion->cursed = obj->cursed;  /* odiluted left as-is */
		singlepotion->bknown = FALSE;
		if (Blind) {
		    singlepotion->dknown = FALSE;
		} else {
		    singlepotion->dknown = !Hallucination;
#if 0 /*JP*/
		    if (mixture == POT_WATER && singlepotion->dknown)
			Sprintf(newbuf, "clears");
		    else
			Sprintf(newbuf, "turns %s",
				hcolor(OBJ_DESCR(objects[mixture])));
		    pline_The("%spotion%s %s.", oldbuf,
			      more_than_one ? " that you dipped into" : "",
			      newbuf);
#else
		    if (mixture == POT_WATER && singlepotion->dknown)
			pline("%s薬は透明になった。", oldbuf);
		    else
			pline("%s薬は%s薬になった。", oldbuf,
				hcolor(jtrns_obj(POTION_SYM,
				    OBJ_DESCR(objects[mixture]))));
#endif
		    if(!objects[old_otyp].oc_uname &&
			!objects[old_otyp].oc_name_known && old_dknown) {
			struct obj fakeobj;
			fakeobj = zeroobj;
			fakeobj.dknown = 1;
			fakeobj.otyp = old_otyp;
			fakeobj.oclass = POTION_CLASS;
			docall(&fakeobj);
		    }
		}
		obj_extract_self(singlepotion);
		singlepotion = hold_another_object(singlepotion,
/*JP
					"You juggle and drop %s!",
*/
					"お手玉して%sを落としてしまった！",
					doname(singlepotion), (const char *)0);
		update_inventory();
		return(1);
	}

/*JP
	pline("Interesting...");
*/
	pline("面白い．．．");
	return(1);
}


void
djinni_from_bottle(obj)
register struct obj *obj;
{
	struct monst *mtmp;
	int chance;

	if(!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
#if 0 /*JP:T*/
		pline("It turns out to be empty.");
#else
		if (obj->otyp == MAGIC_LAMP) {
			pline("ランプは空っぽだった。");
		} else {
			pline("薬は空っぽだった。");
		}
#endif
		return;
	}

	if (!Blind) {
#if 0 /*JP*/
		pline("In a cloud of smoke, %s emerges!", a_monnam(mtmp));
		pline("%s speaks.", Monnam(mtmp));
#else
		pline("煙の中から、%sが現われた！", a_monnam(mtmp));
		pline("%sは話しかけた。", Monnam(mtmp));
#endif
	} else {
#if 0 /*JP*/
		You("smell acrid fumes.");
		pline("%s speaks.", Something);
#else
		You("ツンとする匂いがした。");
		pline("%sが話しかけた。", Something);
#endif
	}

	chance = rn2(5);
	if (obj->blessed) chance = (chance == 4) ? rnd(4) : 0;
	else if (obj->cursed) chance = (chance == 0) ? rn2(4) : 4;
	/* 0,1,2,3,4:  b=80%,5,5,5,5; nc=20%,20,20,20,20; c=5%,5,5,5,80 */

	switch (chance) {
/*JP
	case 0 : verbalize("I am in your debt.  I will grant one wish!");
*/
	case 0 : verbalize("お前には借りができた。１つ願いをかなえてやろう！");
		makewish(FALSE);
		mongone(mtmp);
		break;
/*JP
	case 1 : verbalize("Thank you for freeing me!");
*/
	case 1 : verbalize("私を解放してくれたことを感謝する！");
		(void) tamedog(mtmp, (struct obj *)0);
		break;
/*JP
	case 2 : verbalize("You freed me!");
*/
	case 2 : verbalize("解放してくれたのはお前か！");
		mtmp->mpeaceful = TRUE;
		set_malign(mtmp);
		break;
/*JP
	case 3 : verbalize("It is about time!");
*/
	case 3 : verbalize("さらばだ！");
/*JP
		pline("%s vanishes.", Monnam(mtmp));
*/
		pline("%sは消えた。", Monnam(mtmp));
		mongone(mtmp);
		break;
/*JP
	default: verbalize("You disturbed me, fool!");
*/
	default: verbalize("おまえは私の眠りを妨げた。おろかものめ！");
		break;
	}
}

/* clone a gremlin or mold (2nd arg non-null implies heat as the trigger);
   hit points are cut in half (odd HP stays with original) */
struct monst *
split_mon(mon, mtmp)
struct monst *mon,	/* monster being split */
	     *mtmp;	/* optional attacker whose heat triggered it */
{
	struct monst *mtmp2;
	char reason[BUFSZ];

	reason[0] = '\0';
#if 0 /*JP*/
	if (mtmp) Sprintf(reason, " from %s heat",
			  (mtmp == &youmonst) ? (const char *)"your" :
			      (const char *)s_suffix(mon_nam(mtmp)));
#endif

	if (mon == &youmonst) {
	    mtmp2 = cloneu();
	    if (mtmp2) {
		mtmp2->mhpmax = u.mhmax / 2;
		u.mhmax -= mtmp2->mhpmax;
		flags.botl = 1;
/*JP
		You("multiply%s!", reason);
*/
		You("%s分裂した！", reason);
	    }
	} else {
	    mtmp2 = clone_mon(mon, 0, 0);
	    if (mtmp2) {
		mtmp2->mhpmax = mon->mhpmax / 2;
		mon->mhpmax -= mtmp2->mhpmax;
		if (canspotmon(mon))
/*JP
		    pline("%s multiplies%s!", Monnam(mon), reason);
*/
		    pline("%sは%s分裂した！", Monnam(mon), reason);
	    }
	}
	return mtmp2;
}

#endif /* OVLB */

/*potion.c*/
