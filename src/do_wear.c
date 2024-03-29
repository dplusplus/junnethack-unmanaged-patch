/*	SCCS Id: @(#)do_wear.c	3.4	2003/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	changing point is marked `JP' (94/6/7)
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2004
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009-2010
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"

#ifndef OVLB

STATIC_DCL long takeoff_mask, taking_off;

#else /* OVLB */

STATIC_OVL NEARDATA long takeoff_mask = 0L;
static NEARDATA long taking_off = 0L;

static NEARDATA int todelay;
static boolean cancelled_don = FALSE;

#if 0 /*JP*/
static NEARDATA const char see_yourself[] = "see yourself";
#endif
static NEARDATA const char unknown_type[] = "Unknown type of %s (%d)";
#if 0 /*JP*/
static NEARDATA const char c_armor[]  = "armor",
			   c_suit[]   = "suit",
#ifdef TOURIST
			   c_shirt[]  = "shirt",
#endif
			   c_cloak[]  = "cloak",
			   c_gloves[] = "gloves",
			   c_boots[]  = "boots",
			   c_helmet[] = "helmet",
			   c_shield[] = "shield",
			   c_weapon[] = "weapon",
			   c_sword[]  = "sword",
			   c_axe[]    = "axe",
			   c_that_[]  = "that";
#else /*JP*/
static NEARDATA const char c_armor[]  = "鎧",
			   c_suit[]   = "服",
#ifdef TOURIST
			   c_shirt[]  = "シャツ",
#endif
			   c_cloak[]  = "クローク",
			   c_gloves[] = "小手",
			   c_boots[]  = "靴",
			   c_helmet[] = "兜",
			   c_shield[] = "盾",
			   c_weapon[] = "武器",
			   c_sword[]  = "剣",
			   c_axe[]    = "斧",
			   c_that_[]  = "それ";
#endif /*JP*/

static NEARDATA const long takeoff_order[] = { WORN_BLINDF, W_WEP,
	WORN_SHIELD, WORN_GLOVES, LEFT_RING, RIGHT_RING, WORN_CLOAK,
	WORN_HELMET, WORN_AMUL, WORN_ARMOR,
#ifdef TOURIST
	WORN_SHIRT,
#endif
	WORN_BOOTS, W_SWAPWEP, W_QUIVER, 0L };

STATIC_DCL void FDECL(on_msg, (struct obj *));
STATIC_PTR int NDECL(Armor_on);
STATIC_PTR int NDECL(Boots_on);
STATIC_DCL int NDECL(Cloak_on);
STATIC_PTR int NDECL(Helmet_on);
STATIC_PTR int NDECL(Gloves_on);
STATIC_PTR int NDECL(Shield_on);
#ifdef TOURIST
STATIC_PTR int NDECL(Shirt_on);
#endif
STATIC_DCL void NDECL(Amulet_on);
STATIC_DCL void FDECL(Ring_off_or_gone, (struct obj *, BOOLEAN_P));
STATIC_PTR int FDECL(select_off, (struct obj *));
STATIC_DCL struct obj *NDECL(do_takeoff);
STATIC_PTR int NDECL(take_off);
STATIC_DCL int FDECL(menu_remarm, (int));
#if 0 /*JP*/
STATIC_DCL void FDECL(already_wearing, (const char*));
#else
STATIC_DCL void FDECL(already_wearing, (const char*, struct obj *));
#endif
STATIC_DCL void FDECL(already_wearing2, (const char*, const char*));

void
off_msg(otmp)
register struct obj *otmp;
{
#if 0 /*JP*/
	if(flags.verbose)
	    You("were wearing %s.", doname(otmp));
#else
	const char *j;
	const char *m;
	m = joffmsg(otmp, &j);
	if(flags.verbose)
	    You("%s%s%s。", doname(otmp), j, jpast(m));
#endif
}

/* for items that involve no delay */
STATIC_OVL void
on_msg(otmp)
register struct obj *otmp;
{
#if 0 /*JP*/
	if (flags.verbose) {
	    char how[BUFSZ];

	    how[0] = '\0';
	    if (otmp->otyp == TOWEL)
		Sprintf(how, " around your %s", body_part(HEAD));
	    You("are now wearing %s%s.",
		obj_is_pname(otmp) ? the(xname(otmp)) : an(xname(otmp)),
		how);
	}
#else
  const char *j;
  const char *m;
  m = jonmsg(otmp, &j);

	if(flags.verbose)
	  You("%s%s%s。", xname(otmp), j, jpast(m));
#endif
}

/*
 * The Type_on() functions should be called *after* setworn().
 * The Type_off() functions call setworn() themselves.
 */

STATIC_PTR
int
Boots_on()
{
    long oldprop =
	u.uprops[objects[uarmf->otyp].oc_oprop].extrinsic & ~WORN_BOOTS;

    switch(uarmf->otyp) {
	case LOW_BOOTS:
	case IRON_SHOES:
	case HIGH_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
		break;
	case WATER_WALKING_BOOTS:
		if (u.uinwater) spoteffects(TRUE);
		break;
	case SPEED_BOOTS:
		/* Speed boots are still better than intrinsic speed, */
		/* though not better than potion speed */
		if (!oldprop && !(HFast & TIMEOUT)) {
			makeknown(uarmf->otyp);
#if 0 /*JP*/
			You_feel("yourself speed up%s.",
				(oldprop || HFast) ? " a bit more" : "");
#else
			You("%s素早くなったような気がした。",
			        (oldprop  || HFast) ? "さらに" : "");
#endif
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth) {
			makeknown(uarmf->otyp);
/*JP
			You("walk very quietly.");
*/
			Your("足音は小さくなった。");
		}
		break;
	case FUMBLE_BOOTS:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case LEVITATION_BOOTS:
		if (!oldprop && !HLevitation) {
			makeknown(uarmf->otyp);
			float_up();
			spoteffects(FALSE);
		}
		break;
	default: warning(unknown_type, c_boots, uarmf->otyp);
    }
    return 0;
}

int
Boots_off()
{
    int otyp = uarmf->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_BOOTS;

    takeoff_mask &= ~W_ARMF;
	/* For levitation, float_down() returns if Levitation, so we
	 * must do a setworn() _before_ the levitation case.
	 */
    setworn((struct obj *)0, W_ARMF);
    switch (otyp) {
	case SPEED_BOOTS:
		if (!Very_fast && !cancelled_don) {
			makeknown(otyp);
#if 0 /*JP*/
			You_feel("yourself slow down%s.",
				Fast ? " a bit" : "");
#else
			You("%sのろくなったような気がした。",
				oldprop ? "ちょっと" : "");
#endif
		}
		break;
	case WATER_WALKING_BOOTS:
		if (is_pool(u.ux,u.uy) && !Levitation && !Flying &&
		    !is_clinger(youmonst.data) && !cancelled_don) {
			makeknown(otyp);
			/* make boots known in case you survive the drowning */
			spoteffects(TRUE);
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth && !cancelled_don) {
			makeknown(otyp);
/*JP
			You("sure are noisy.");
*/
			Your("足音は大きくなった。");
		}
		break;
	case FUMBLE_BOOTS:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			HFumbling = EFumbling = 0;
		break;
	case LEVITATION_BOOTS:
		if (!oldprop && !HLevitation && !cancelled_don) {
			(void) float_down(0L, 0L);
			makeknown(otyp);
		}
		break;
	case LOW_BOOTS:
	case IRON_SHOES:
	case HIGH_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
		break;
	default: warning(unknown_type, c_boots, otyp);
    }
    cancelled_don = FALSE;
    return 0;
}

STATIC_OVL int
Cloak_on()
{
    long oldprop =
	u.uprops[objects[uarmc->otyp].oc_oprop].extrinsic & ~WORN_CLOAK;

    switch(uarmc->otyp) {
	case ELVEN_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_DISPLACEMENT:
		makeknown(uarmc->otyp);
		break;
	case ORCISH_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case ROBE:
	case LEATHER_CLOAK:
		break;
	case MUMMY_WRAPPING:
		/* Note: it's already being worn, so we have to cheat here. */
		if ((HInvis || EInvis || pm_invisible(youmonst.data)) && !Blind) {
		    newsym(u.ux,u.uy);
#if 0 /*JP*/
		    You("can %s!",
			See_invisible ? "no longer see through yourself"
			: see_yourself);
#else
		    pline("%s！",
			  See_invisible ? "あなたは透明でなくなった" :
			  "自分自身が見えるようになった");
#endif
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		/* since cloak of invisibility was worn, we know mummy wrapping
		   wasn't, so no need to check `oldprop' against blocked */
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(uarmc->otyp);
		    newsym(u.ux,u.uy);
#if 0 /*JP*/
		    pline("Suddenly you can%s yourself.",
			See_invisible ? " see through" : "not see");
#else
		    pline("突然、%s。",
			See_invisible ? "あなたは透明になった" : "自分自身が見えなくなった");
#endif
		}
		break;
	case OILSKIN_CLOAK:
/*JP
		pline("%s very tightly.", Tobjnam(uarmc, "fit"));
*/
		pline("%sはとてもぴっちり合う。",xname(uarmc));
		break;
	/* Alchemy smock gives poison _and_ acid resistance */
	case ALCHEMY_SMOCK:
		EAcid_resistance |= WORN_CLOAK;
  		break;
	default: warning(unknown_type, c_cloak, uarmc->otyp);
    }
    return 0;
}

int
Cloak_off()
{
    int otyp = uarmc->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_CLOAK;

    takeoff_mask &= ~W_ARMC;
	/* For mummy wrapping, taking it off first resets `Invisible'. */
    setworn((struct obj *)0, W_ARMC);
    switch (otyp) {
	case ELVEN_CLOAK:
	case ORCISH_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case CLOAK_OF_DISPLACEMENT:
	case OILSKIN_CLOAK:
	case ROBE:
	case LEATHER_CLOAK:
		break;
	case MUMMY_WRAPPING:
		if (Invis && !Blind) {
		    newsym(u.ux,u.uy);
#if 0 /*JP*/
		    You("can %s.",
			See_invisible ? "see through yourself"
			: "no longer see yourself");
#else
		    pline("%s。",
			See_invisible ? "あなたは透明になった" : "自分自身が見えなくなった");
#endif
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(CLOAK_OF_INVISIBILITY);
		    newsym(u.ux,u.uy);
#if 0 /*JP*/
		    pline("Suddenly you can %s.",
			See_invisible ? "no longer see through yourself"
			: see_yourself);
#else
		    pline("突然、%s。",
			See_invisible ? "あなたは透明でなくなった" :
			"自分自身が見えるようになった");
#endif
		}
		break;
	/* Alchemy smock gives poison _and_ acid resistance */
	case ALCHEMY_SMOCK:
		EAcid_resistance &= ~WORN_CLOAK;
  		break;
	default: warning(unknown_type, c_cloak, otyp);
    }
    return 0;
}

STATIC_PTR
int
Helmet_on()
{
    switch(uarmh->otyp) {
	case FEDORA:
	case HELMET:
	case DENTED_POT:
	case ELVEN_LEATHER_HELM:
	case DWARVISH_IRON_HELM:
	case ORCISH_HELM:
	case HELM_OF_TELEPATHY:
	case TINFOIL_HAT:
		break;
	case HELM_OF_BRILLIANCE:
		adj_abon(uarmh, uarmh->spe);
		break;
	case CORNUTHAUM:
		/* people think marked wizards know what they're talking
		 * about, but it takes trained arrogance to pull it off,
		 * and the actual enchantment of the hat is irrelevant.
		 */
		ABON(A_CHA) += (Role_if(PM_WIZARD) ? 1 : -1);
		flags.botl = 1;
		makeknown(uarmh->otyp);
		break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
		if (u.ualign.type == A_NEUTRAL)
		    u.ualign.type = rn2(2) ? A_CHAOTIC : A_LAWFUL;
		else u.ualign.type = -(u.ualign.type);
		u.ublessed = 0; /* lose your god's protection */
	     /* makeknown(uarmh->otyp);   -- moved below, after xname() */
		/*FALLTHRU*/
	case DUNCE_CAP:
		if (!uarmh->cursed) {
		    if (Blind)
/*JP
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
*/
			pline("%sは一瞬震えた。", xname(uarmh));
		    else
#if 0 /*JP*/
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
#else
			pline("%sは一瞬%s輝いた。",
			      xname(uarmh), jconj_adj(hcolor(NH_BLACK)));
#endif
		    curse(uarmh);
		}
		flags.botl = 1;		/* reveal new alignment or INT & WIS */
		if (Hallucination) {
#if 0 /*JP*/
		    pline("My brain hurts!"); /* Monty Python's Flying Circus */
#else
		    pline("のーみそバーン！"); /*モンティパイソンとはちょっと違うけど*/
#endif
		} else if (uarmh->otyp == DUNCE_CAP) {
#if 0 /*JP*/
		    You_feel("%s.",	/* track INT change; ignore WIS */
		  ACURR(A_INT) <= (ABASE(A_INT) + ABON(A_INT) + ATEMP(A_INT)) ?
			     "like sitting in a corner" : "giddy");
#else
		    You("%sような気がした。",
		  ACURR(A_INT) <= (ABASE(A_INT) + ABON(A_INT) + ATEMP(A_INT)) ?
			     "街角に座っている" : "目がまわった");
#endif
		} else {
/*JP
		    Your("mind oscillates briefly.");
*/
		    You("少しも迷わず寝返った。");
		    makeknown(HELM_OF_OPPOSITE_ALIGNMENT);
		}
		break;
	default: warning(unknown_type, c_helmet, uarmh->otyp);
    }
    return 0;
}

int
Helmet_off()
{
    takeoff_mask &= ~W_ARMH;

    switch(uarmh->otyp) {
	case FEDORA:
	case HELMET:
	case DENTED_POT:
	case ELVEN_LEATHER_HELM:
	case DWARVISH_IRON_HELM:
	case ORCISH_HELM:
	case TINFOIL_HAT:
	    break;
	case DUNCE_CAP:
	    flags.botl = 1;
	    break;
	case CORNUTHAUM:
	    if (!cancelled_don) {
		ABON(A_CHA) += (Role_if(PM_WIZARD) ? -1 : 1);
		flags.botl = 1;
	    }
	    break;
	case HELM_OF_TELEPATHY:
	    /* need to update ability before calling see_monsters() */
	    setworn((struct obj *)0, W_ARMH);
	    see_monsters();
	    return 0;
	case HELM_OF_BRILLIANCE:
	    if (!cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
	    u.ualign.type = u.ualignbase[A_CURRENT];
	    u.ublessed = 0; /* lose the other god's protection */
	    flags.botl = 1;
	    break;
	default: warning(unknown_type, c_helmet, uarmh->otyp);
    }
    setworn((struct obj *)0, W_ARMH);
    cancelled_don = FALSE;
    return 0;
}

STATIC_PTR
int
Gloves_on()
{
    long oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    switch(uarmg->otyp) {
	case LEATHER_GLOVES:
		break;
	case GAUNTLETS_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case GAUNTLETS_OF_POWER:
		makeknown(uarmg->otyp);
		flags.botl = 1; /* taken care of in attrib.c */
		break;
	case GAUNTLETS_OF_DEXTERITY:
		adj_abon(uarmg, uarmg->spe);
		break;
	default: warning(unknown_type, c_gloves, uarmg->otyp);
    }
    return 0;
}

int
Gloves_off()
{
    long oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    takeoff_mask &= ~W_ARMG;

    switch(uarmg->otyp) {
	case LEATHER_GLOVES:
	    break;
	case GAUNTLETS_OF_FUMBLING:
	    if (!oldprop && !(HFumbling & ~TIMEOUT))
		HFumbling = EFumbling = 0;
	    break;
	case GAUNTLETS_OF_POWER:
	    makeknown(uarmg->otyp);
	    flags.botl = 1; /* taken care of in attrib.c */
	    break;
	case GAUNTLETS_OF_DEXTERITY:
	    if (!cancelled_don) adj_abon(uarmg, -uarmg->spe);
	    break;
	default: warning(unknown_type, c_gloves, uarmg->otyp);
    }
    setworn((struct obj *)0, W_ARMG);
    cancelled_don = FALSE;
    (void) encumber_msg();		/* immediate feedback for GoP */

    /* Prevent wielding cockatrice when not wearing gloves */
    if (uwep && uwep->otyp == CORPSE &&
		touch_petrifies(&mons[uwep->corpsenm])) {
	char kbuf[BUFSZ];

#if 0 /*JP*/
	You("wield the %s in your bare %s.",
	    corpse_xname(uwep, TRUE), makeplural(body_part(HAND)));
#else
	You("%sを素%sで持った。",
	   corpse_xname(uwep, TRUE),  body_part(HAND));
#endif
/*JP
	Strcpy(kbuf, an(corpse_xname(uwep, TRUE)));
*/
	Sprintf(kbuf, "%sに触れて", corpse_xname(uwep, TRUE));
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */
    }

    /* KMH -- ...or your secondary weapon when you're wielding it */
    if (u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
	touch_petrifies(&mons[uswapwep->corpsenm])) {
	char kbuf[BUFSZ];

#if 0 /*JP*/
	You("wield the %s in your bare %s.",
#else
	You("%sを素%sで持った。",
#endif
	    corpse_xname(uswapwep, TRUE), body_part(HAND));

/*JP
	Strcpy(kbuf, an(corpse_xname(uswapwep, TRUE)));
*/
	Sprintf(kbuf, "%sに触れて", corpse_xname(uswapwep, TRUE));
	instapetrify(kbuf);
	uswapwepgone();	/* lifesaved still doesn't allow touching cockatrice */
    }

    return 0;
}

STATIC_OVL int
Shield_on()
{
/*
    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case SHIELD_OF_REFLECTION:
		break;
	default: warning(unknown_type, c_shield, uarms->otyp);
    }
*/
    return 0;
}

int
Shield_off()
{
    takeoff_mask &= ~W_ARMS;
/*
    switch (uarms->otyp) {
	case SMALL_SHIELD:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case LARGE_SHIELD:
	case SHIELD_OF_REFLECTION:
		break;
	default: warning(unknown_type, c_shield, uarms->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMS);
    return 0;
}

#ifdef TOURIST
STATIC_OVL int
Shirt_on()
{
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: warning(unknown_type, c_shirt, uarmu->otyp);
    }
*/
    return 0;
}

int
Shirt_off()
{
    takeoff_mask &= ~W_ARMU;
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: warning(unknown_type, c_shirt, uarmu->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMU);
    return 0;
}
#endif	/*TOURIST*/

/* This must be done in worn.c, because one of the possible intrinsics conferred
 * is fire resistance, and we have to immediately set HFire_resistance in worn.c
 * since worn.c will check it before returning.
 */
STATIC_PTR
int
Armor_on()
{
	if (uarm && Is_gold_dragon_armor(uarm->otyp)) {
		begin_burn(uarm,FALSE);
		if (!Blind)
/*JP
			pline("%s to glow.",Tobjnam(uarm,"begin"));
*/
			pline("%sは輝きはじめた。",xname(uarm));
	}
    return 0;
}

int
Armor_off()
{
	if (uarm && Is_gold_dragon_armor(uarm->otyp)) {
		end_burn(uarm,FALSE);
		if (!Blind)
/*JP
			pline("%s glowing.",Tobjnam(uarm,"stop"));
*/
			pline("%sの輝きはなくなった。",xname(uarm));
	}
    takeoff_mask &= ~W_ARM;
    setworn((struct obj *)0, W_ARM);
    cancelled_don = FALSE;
    return 0;
}

/* The gone functions differ from the off functions in that if you die from
 * taking it off and have life saving, you still die.
 */
int
Armor_gone()
{
	if (uarm && Is_gold_dragon_armor(uarm->otyp))
		end_burn(uarm,FALSE);
	takeoff_mask &= ~W_ARM;
    setnotworn(uarm);
    cancelled_don = FALSE;
    return 0;
}

STATIC_OVL void
Amulet_on()
{
    switch(uamul->otyp) {
	case AMULET_OF_ESP:
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_MAGICAL_BREATHING:
	case FAKE_AMULET_OF_YENDOR:
		break;
	case AMULET_OF_UNCHANGING:
		if (Slimed) {
		    Slimed = 0;
		    flags.botl = 1;
		}
		break;
	case AMULET_OF_CHANGE:
	    {
		int orig_sex = poly_gender();

		if (Unchanging) break;
		change_sex();
		/* Don't use same message as polymorph */
		if (orig_sex != poly_gender()) {
		    makeknown(AMULET_OF_CHANGE);
#if 0 /*JP*/
		    You("are suddenly very %s!", flags.female ? "feminine"
			: "masculine");
#else
		    You("突然%s！", flags.female ? "女っぽくなった"
			: "男っぽくなった");
#endif
		    flags.botl = 1;
		} else
		    /* already polymorphed into single-gender monster; only
		       changed the character's base sex */
/*JP
		    You("don't feel like yourself.");
*/
		    You("自分自身じゃなくなった気がした。");
/*JP
		pline_The("amulet disintegrates!");
*/
		pline("魔除けはこなごなになった！");
		if (orig_sex == poly_gender() && uamul->dknown &&
			!objects[AMULET_OF_CHANGE].oc_name_known &&
			!objects[AMULET_OF_CHANGE].oc_uname)
		    docall(uamul);
		useup(uamul);
		break;
	    }
	case AMULET_OF_STRANGULATION:
		makeknown(AMULET_OF_STRANGULATION);
/*JP
		pline("It constricts your throat!");
*/
		pline("魔除けはあなたの喉を絞めつけた！");
		Strangled = 6;
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		HSleeping = rnd(100);
		break;
	case AMULET_OF_YENDOR:
		break;
    }
}

void
Amulet_off()
{
    takeoff_mask &= ~W_AMUL;

    switch(uamul->otyp) {
	case AMULET_OF_ESP:
		/* need to update ability before calling see_monsters() */
		setworn((struct obj *)0, W_AMUL);
		see_monsters();
		return;
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_CHANGE:
	case AMULET_OF_UNCHANGING:
	case FAKE_AMULET_OF_YENDOR:
		break;
	case AMULET_OF_MAGICAL_BREATHING:
		if (Underwater) {
		    /* HMagical_breathing must be set off
			before calling drown() */
		    setworn((struct obj *)0, W_AMUL);
		    if (!breathless(youmonst.data) && !amphibious(youmonst.data)
						&& !Swimming) {
/*JP
			You("suddenly inhale an unhealthy amount of water!");
*/
			You("突然、大量の水を飲み込んだ！");
		    	(void) drown();
		    }
		    return;
		}
		break;
	case AMULET_OF_STRANGULATION:
		if (Strangled) {
/*JP
			You("can breathe more easily!");
*/
			You("楽に呼吸できるようになった！");
			Strangled = 0;
		}
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		setworn((struct obj *)0, W_AMUL);
		if (!ESleeping)
			HSleeping = 0;
		return;
	case AMULET_OF_YENDOR:
		break;
    }
    setworn((struct obj *)0, W_AMUL);
    return;
}

void
Ring_on(obj)
register struct obj *obj;
{
    long oldprop = u.uprops[objects[obj->otyp].oc_oprop].extrinsic;
    int old_attrib, which;

    if (obj == uwep) setuwep((struct obj *) 0);
    if (obj == uswapwep) setuswapwep((struct obj *) 0);
    if (obj == uquiver) setuqwep((struct obj *) 0);

    /* only mask out W_RING when we don't have both
       left and right rings of the same type */
    if ((oldprop & W_RING) != W_RING) oldprop &= ~W_RING;

    switch(obj->otyp){
	case RIN_TELEPORTATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_FIRE_RESISTANCE:
	case RIN_COLD_RESISTANCE:
	case RIN_SHOCK_RESISTANCE:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	case RIN_FREE_ACTION:
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_REGENERATION:
		if (!oldprop && !HRegeneration && !regenerates(youmonst.data)) {
			if ((uhp() < uhpmax()) &&
			    !objects[obj->otyp].oc_name_known) {
/*JP
				Your("wounds are rapidly healing!");
*/
				Your("傷は勢いよく治りはじめた！");
				makeknown(RIN_REGENERATION);
			}
		}
		break;
	case RIN_CONFLICT:
#ifdef BLACKMARKET
		if (Is_blackmarket(&u.uz)) {
			set_black_marketeer_angry();
		}
#endif /* BLACKMARKET */
		break;
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_SEE_INVISIBLE:
		/* can now see invisible monsters */
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();
#ifdef INVISIBLE_OBJECTS
		see_objects();
#endif

		if (Invis && !oldprop && !HSee_invisible &&
				!perceives(youmonst.data) && !Blind) {
		    newsym(u.ux,u.uy);
/*JP
		    pline("Suddenly you are transparent, but there!");
*/
		    pline("突然透明になった。しかし存在はしている！");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INVISIBILITY:
		if (!oldprop && !HInvis && !BInvis && !Blind) {
		    makeknown(RIN_INVISIBILITY);
		    newsym(u.ux,u.uy);
		    self_invis_message();
		}
		break;
	case RIN_LEVITATION:
		if (!oldprop && !HLevitation) {
		    float_up();
		    makeknown(RIN_LEVITATION);
		    spoteffects(FALSE);	/* for sinks */
		}
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_GAIN_INTELLIGENCE:
		which = A_INT;
		goto adjust_attrib;
	case RIN_GAIN_WISDOM:
		which = A_WIS;
		goto adjust_attrib;
	case RIN_GAIN_DEXTERITY:
		which = A_DEX;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) += obj->spe;
		if (ACURR(which) != old_attrib ||
			(objects[obj->otyp].oc_name_known &&
			    old_attrib != 25 && old_attrib != 3)) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc += obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc += obj->spe;
		break;
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		rescham();
		break;
	case RIN_PROTECTION:
		if (obj->spe || objects[RIN_PROTECTION].oc_name_known) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
		break;
    }
}

STATIC_OVL void
Ring_off_or_gone(obj,gone)
register struct obj *obj;
boolean gone;
{
    long mask = (obj->owornmask & W_RING);
    int old_attrib, which;

    takeoff_mask &= ~mask;
    if(!(u.uprops[objects[obj->otyp].oc_oprop].extrinsic & mask))
	warning("Strange... I didn't know you had that ring.");
    if(gone) setnotworn(obj);
    else setworn((struct obj *)0, obj->owornmask);

    switch(obj->otyp) {
	case RIN_TELEPORTATION:
	case RIN_REGENERATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_FIRE_RESISTANCE:
	case RIN_COLD_RESISTANCE:
	case RIN_SHOCK_RESISTANCE:
	case RIN_CONFLICT:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	case RIN_FREE_ACTION:                
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_SEE_INVISIBLE:
		/* Make invisible monsters go away */
		if (!See_invisible) {
		    set_mimic_blocking(); /* do special mimic handling */
		    see_monsters();
#ifdef INVISIBLE_OBJECTS                
		    see_objects();
#endif
		}

		if (Invisible && !Blind) {
		    newsym(u.ux,u.uy);
/*JP
		    pline("Suddenly you cannot see yourself.");
*/
		    pline("突然、自分自身が見えなくなった。");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INVISIBILITY:
		if (!Invis && !BInvis && !Blind) {
		    newsym(u.ux,u.uy);
#if 0 /*JP*/
		    Your("body seems to unfade%s.",
			 See_invisible ? " completely" : "..");
#else
			Your("体が%s現われ%s。",
			     See_invisible ? "完全に" : "次第に",
			     See_invisible ? "た" : "てきた");
#endif
		    makeknown(RIN_INVISIBILITY);
		}
		break;
	case RIN_LEVITATION:
		(void) float_down(0L, 0L);
		if (!Levitation) makeknown(RIN_LEVITATION);
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_GAIN_INTELLIGENCE:
		which = A_INT;
		goto adjust_attrib;
	case RIN_GAIN_WISDOM:
		which = A_WIS;
		goto adjust_attrib;
	case RIN_GAIN_DEXTERITY:
		which = A_DEX;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) -= obj->spe;
		if (ACURR(which) != old_attrib) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc -= obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc -= obj->spe;
		break;
	case RIN_PROTECTION:
		/* might have forgotten it due to amnesia */
		if (obj->spe) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		/* If you're no longer protected, let the chameleons
		 * change shape again -dgk
		 */
		restartcham();
		break;
    }
}

void
Ring_off(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,FALSE);
}

void
Ring_gone(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,TRUE);
}

void
Blindf_on(otmp)
register struct obj *otmp;
{
	boolean already_blind = Blind, changed = FALSE;

	if (otmp == uwep)
	    setuwep((struct obj *) 0);
	if (otmp == uswapwep)
	    setuswapwep((struct obj *) 0);
	if (otmp == uquiver)
	    setuqwep((struct obj *) 0);
	setworn(otmp, W_TOOL);
	on_msg(otmp);

	if (Blind && !already_blind) {
	    changed = TRUE;
/*JP
	    if (flags.verbose) You_cant("see any more.");
*/
	    if (flags.verbose) You("何も見えなくなった。");
	    /* set ball&chain variables before the hero goes blind */
	    if (Punished) set_bc(0);
	} else if (already_blind && !Blind) {
	    changed = TRUE;
	    /* "You are now wearing the Eyes of the Overworld." */
/*JP
	    You("can see!");
*/
	    You("目が見えるようになった！");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}
}

void
Blindf_off(otmp)
register struct obj *otmp;
{
	boolean was_blind = Blind, changed = FALSE;

	violated(CONDUCT_BLINDFOLDED);
	takeoff_mask &= ~W_TOOL;
	setworn((struct obj *)0, otmp->owornmask);
	off_msg(otmp);

	if (Blind) {
	    if (was_blind) {
		/* "still cannot see" makes no sense when removing lenses
		   since they can't have been the cause of your blindness */
		if (otmp->otyp != LENSES)
/*JP
		    You("still cannot see.");
*/
		    You("まだ目が見えない。");
	    } else {
		changed = TRUE;	/* !was_blind */
		/* "You were wearing the Eyes of the Overworld." */
/*JP
		You_cant("see anything now!");
*/
		You("今は何も見ることができない！");
		/* set ball&chain variables before the hero goes blind */
		if (Punished) set_bc(0);
	    }
	} else if (was_blind) {
	    changed = TRUE;	/* !Blind */
/*JP
	    You("can see again.");
*/
	    You("ふたたび目が見えるようになった。");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}
}

/* called in main to set intrinsics of worn start-up items */
void
set_wear()
{
#ifdef TOURIST
	if (uarmu) (void) Shirt_on();
#endif
	if (uarm)  (void) Armor_on();
	if (uarmc) (void) Cloak_on();
	if (uarmf) (void) Boots_on();
	if (uarmg) (void) Gloves_on();
	if (uarmh) (void) Helmet_on();
	if (uarms) (void) Shield_on();
}

/* check whether the target object is currently being put on (or taken off) */
boolean
donning(otmp)		/* also checks for doffing */
register struct obj *otmp;
{
 /* long what = (occupation == take_off) ? taking_off : 0L; */
    long what = taking_off;	/* if nonzero, occupation is implied */
    boolean result = FALSE;

    if (otmp == uarm)
	result = (afternmv == Armor_on || afternmv == Armor_off ||
		  what == WORN_ARMOR);
#ifdef TOURIST
    else if (otmp == uarmu)
	result = (afternmv == Shirt_on || afternmv == Shirt_off ||
		  what == WORN_SHIRT);
#endif
    else if (otmp == uarmc)
	result = (afternmv == Cloak_on || afternmv == Cloak_off ||
		  what == WORN_CLOAK);
    else if (otmp == uarmf)
	result = (afternmv == Boots_on || afternmv == Boots_off ||
		  what == WORN_BOOTS);
    else if (otmp == uarmh)
	result = (afternmv == Helmet_on || afternmv == Helmet_off ||
		  what == WORN_HELMET);
    else if (otmp == uarmg)
	result = (afternmv == Gloves_on || afternmv == Gloves_off ||
		  what == WORN_GLOVES);
    else if (otmp == uarms)
	result = (afternmv == Shield_on || afternmv == Shield_off ||
		  what == WORN_SHIELD);

    return result;
}

void
cancel_don()
{
	/* the piece of armor we were donning/doffing has vanished, so stop
	 * wasting time on it (and don't dereference it when donning would
	 * otherwise finish)
	 */
	cancelled_don = (afternmv == Boots_on || afternmv == Helmet_on ||
			 afternmv == Gloves_on || afternmv == Armor_on);
	afternmv = 0;
	nomovemsg = (char *)0;
	multi = 0;
	todelay = 0;
	taking_off = 0L;
}

static NEARDATA const char clothes[] = {ARMOR_CLASS, 0};
static NEARDATA const char accessories[] = {RING_CLASS, AMULET_CLASS, TOOL_CLASS, FOOD_CLASS, 0};

/* the 'T' command */
int
dotakeoff()
{
	register struct obj *otmp = (struct obj *)0;
	int armorpieces = 0;
	const char *j;
	const char *m;

#define MOREARM(x) if (x) { armorpieces++; otmp = x; }
	MOREARM(uarmh);
	MOREARM(uarms);
	MOREARM(uarmg);
	MOREARM(uarmf);
	if (uarmc) {
		armorpieces++;
		otmp = uarmc;
	} else if (uarm) {
		armorpieces++;
		otmp = uarm;
#ifdef TOURIST
	} else if (uarmu) {
		armorpieces++;
		otmp = uarmu;
#endif
	}
	if (!armorpieces) {
		if (uskin)
#if 0 /*JP*/
		    pline_The("%s merged with your skin!",
			      Is_dragon_scales(uskin->otyp) ?
				"dragon scales are" : "dragon scale mail is");
#else
		    pline("ドラゴンの鱗%sはあなたの肌と融合してしまっている！",
			      Is_dragon_scales(uskin->otyp) ?
				"" : "鎧");
#endif
		else
#if 0 /*JP*/
		    pline("Not wearing any armor.%s", (iflags.cmdassist && 
				(uleft || uright || uamul || ublindf)) ?
			  "  Use 'R' command to remove accessories." : "");
#else
		    pline("鎧の類を装備していない。%s", (iflags.cmdassist && 
				(uleft || uright || uamul || ublindf)) ?
			  "'R'コマンドで装飾品を外すことができる。" : "");
#endif
		return 0;
	}
	if (armorpieces > 1 || iflags.paranoid_remove)
		otmp = getobj(clothes, "take off");
	if (otmp == 0) return(0);
	if (!(otmp->owornmask & W_ARMOR)) {
/*JP
		You("are not wearing that.");
*/
		You("それは装備していない。");
		return(0);
	}
	/* note: the `uskin' case shouldn't be able to happen here; dragons
	   can't wear any armor so will end up with `armorpieces == 0' above */
	if (otmp == uskin || ((otmp == uarm) && uarmc)
#ifdef TOURIST
			  || ((otmp == uarmu) && (uarmc || uarm))
#endif
		) {
#if 0 /*JP*/
	    You_cant("take that off.");
#else
	    m = joffmsg(otmp, &j);
	    You("それ%s%sことはできない。", j, m);
#endif
	    return 0;
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* armoroff() doesn't use takeoff_mask */

	(void) armoroff(otmp);
	return(1);
}

/* the 'R' command */
int
doremring()
{
	register struct obj *otmp = 0;
	int Accessories = 0;

#define MOREACC(x) if (x) { Accessories++; otmp = x; }
	MOREACC(uleft);
	MOREACC(uright);
	MOREACC(uamul);
	MOREACC(ublindf);

	if(!Accessories) {
#if 0 /*JP*/
		pline("Not wearing any accessories.%s", (iflags.cmdassist &&
#else
		pline("装飾品は身につけていない。%s", (iflags.cmdassist &&
#endif
			    (uarm || uarmc ||
#ifdef TOURIST
			     uarmu ||
#endif
			     uarms || uarmh || uarmg || uarmf)) ?
#if 0 /*JP*/
		      "  Use 'T' command to take off armor." : "");
#else
		      "'T'コマンドで鎧を外すことができる。" : "");
#endif
		return(0);
	}
	if (Accessories != 1 || iflags.paranoid_remove)
		otmp = getobj(accessories, "remove");
	if(!otmp) return(0);
	if(!(otmp->owornmask & (W_RING | W_AMUL | W_TOOL))) {
/*JP
		You("are not wearing that.");
*/
		You("それは身につけていない。");
		return(0);
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* not used by Ring_/Amulet_/Blindf_off() */

	if (otmp == uright || otmp == uleft) {
		/* Sometimes we want to give the off_msg before removing and
		 * sometimes after; for instance, "you were wearing a moonstone
		 * ring (on right hand)" is desired but "you were wearing a
		 * square amulet (being worn)" is not because of the redundant
		 * "being worn".
		 */
		off_msg(otmp);
		Ring_off(otmp);
	} else if (otmp == uamul) {
		Amulet_off();
		off_msg(otmp);
	} else if (otmp == ublindf) {
		Blindf_off(otmp);	/* does its own off_msg */
	} else {
		warning("removing strange accessory?");
	}
	return(1);
}

/* Check if something worn is cursed _and_ unremovable. */
int
cursed(otmp)
register struct obj *otmp;
{
	/* Curses, like chickens, come home to roost. */
	if((otmp == uwep) ? welded(otmp) : (int)otmp->cursed) {
#if 0 /*JP*/
		You("can't.  %s cursed.",
			(is_boots(otmp) || is_gloves(otmp) || otmp->quan > 1L)
			? "They are" : "It is");
#else
		pline("無理だ。それは呪われている。");
#endif
		otmp->bknown = TRUE;
		return(1);
	}
	return(0);
}

int
armoroff(otmp)
register struct obj *otmp;
{
	register int delay = -objects[otmp->otyp].oc_delay;

	if(cursed(otmp)) return(0);
	if(delay) {
/*JP
		nomul(delay, "disrobing");
*/
		nomul(delay, "脱いでる間に");
		if (is_helmet(otmp)) {
/*JP
			nomovemsg = "You finish taking off your helmet.";
*/
			nomovemsg = "あなたは兜を脱ぎおえた。";
			afternmv = Helmet_off;
		     }
		else if (is_gloves(otmp)) {
/*JP
			nomovemsg = "You finish taking off your gloves.";
*/
			nomovemsg = "あなたは手袋を脱ぎおえた。";
			afternmv = Gloves_off;
		     }
		else if (is_boots(otmp)) {
/*JP
			nomovemsg = "You finish taking off your boots.";
*/
			nomovemsg = "あなたは靴を脱ぎおえた。";
			afternmv = Boots_off;
		     }
		else {
/*JP
			nomovemsg = "You finish taking off your suit.";
*/
			nomovemsg = "あなたは着ている物を脱ぎおえた。";
			afternmv = Armor_off;
		}
	} else {
		/* Be warned!  We want off_msg after removing the item to
		 * avoid "You were wearing ____ (being worn)."  However, an
		 * item which grants fire resistance might cause some trouble
		 * if removed in Hell and lifesaving puts it back on; in this
		 * case the message will be printed at the wrong time (after
		 * the messages saying you died and were lifesaved).  Luckily,
		 * no cloak, shield, or fast-removable armor grants fire
		 * resistance, so we can safely do the off_msg afterwards.
		 * Rings do grant fire resistance, but for rings we want the
		 * off_msg before removal anyway so there's no problem.  Take
		 * care in adding armors granting fire resistance; this code
		 * might need modification.
		 * 3.2 (actually 3.1 even): this comment is obsolete since
		 * fire resistance is not needed for Gehennom.
		 */
		if(is_cloak(otmp))
			(void) Cloak_off();
		else if(is_shield(otmp))
			(void) Shield_off();
		else setworn((struct obj *)0, otmp->owornmask & W_ARMOR);
		off_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

/*JP 
** 物によって動詞が変化するので otmpを追加
*/
STATIC_OVL void
#if 0 /*JP*/
already_wearing(cc)
const char *cc;
#else
already_wearing(cc, otmp)
const char *cc;
struct obj *otmp;
#endif
{
#if 1 /*JP*/
	const char *j;
	const char *m;
	m = jconj(jonmsg(otmp, &j), "ている");
#endif
/*JP
	You("are already wearing %s%c", cc, (cc == c_that_) ? '!' : '.');
*/
	You("もう%s%s%s%s", cc, j,  m, (cc == c_that_) ? "！" : "。");
}

STATIC_OVL void
already_wearing2(cc1, cc2)
const char *cc1, *cc2;
{
/*JP
	You_cant("wear %s because you're wearing %s there already.", cc1, cc2);
*/
	You_cant("%sを身に付けているので%sを身につけられない。", cc2, cc1);
}

/*
 * canwearobj checks to see whether the player can wear a piece of armor
 *
 * inputs: otmp (the piece of armor)
 *         noisy (if TRUE give error messages, otherwise be quiet about it)
 * output: mask (otmp's armor type)
 */
int
canwearobj(otmp,mask,noisy)
struct obj *otmp;
long *mask;
boolean noisy;
{
    int err = 0;
    const char *which;
#if 1 /*JP*/
    const char *m;
    const char *j;
#endif

    which = is_cloak(otmp) ? c_cloak :
#ifdef TOURIST
	    is_shirt(otmp) ? c_shirt :
#endif
	    is_suit(otmp) ? c_suit : 0;
    if (which && cantweararm(youmonst.data) &&
	    /* same exception for cloaks as used in m_dowear() */
	    (which != c_cloak || youmonst.data->msize != MZ_SMALL) &&
	    (racial_exception(&youmonst, otmp) < 1)) {
/*JP
	if (noisy) pline_The("%s will not fit on your body.", which);
*/
	if (noisy) pline("%sはあなたの体に合わない。", which);
	return 0;
    } else if (otmp->owornmask & W_ARMOR) {
/*JP
	if (noisy) already_wearing(c_that_);
*/
	if (noisy) already_wearing(c_that_, otmp);
	return 0;
    }

    if (welded(uwep) && bimanual(uwep) &&
	    (is_suit(otmp)
#ifdef TOURIST
			|| is_shirt(otmp)
#endif
	    )) {
	if (noisy)
/*JP
	    You("cannot do that while holding your %s.",
*/
	    pline("%sを持ったままではそれは出来ない。",
		is_sword(uwep) ? c_sword : c_weapon);
	return 0;
    }

    if (is_helmet(otmp)) {
	if (uarmh) {
/*JP
	    if (noisy) already_wearing(an(c_helmet));
*/
	    if (noisy) already_wearing(an(c_helmet), uarmh);
	    err++;
	} else if (Upolyd && has_horns(youmonst.data) && !is_flimsy(otmp)) {
	    /* (flimsy exception matches polyself handling) */
	    if (noisy)
#if 0 /*JP:T*/
		pline_The("%s won't fit over your horn%s.",
			  c_helmet, plur(num_horns(youmonst.data)));
#else
		pline("角がじゃまで%sを身につけられない。", c_helmet);
#endif
	    err++;
	} else
	    *mask = W_ARMH;
    } else if (is_shield(otmp)) {
	if (uarms) {
/*JP
	    if (noisy) already_wearing(an(c_shield));
*/
	    if (noisy) already_wearing(an(c_shield), uarms);
	    err++;
	} else if (uwep && bimanual(uwep)) {
	    if (noisy) 
#if 0 /*JP*/
		You("cannot wear a shield while wielding a two-handed %s.",
		    is_sword(uwep) ? c_sword :
		    (uwep->otyp == BATTLE_AXE) ? c_axe : c_weapon);
#else
		{
		    m = jconj(jonmsg(uwep, &j), "ている");
		    You("両手持ちの%s%s%sので盾で身を守れない。",
		        is_sword(uwep) ? c_sword :
		        uwep->otyp == BATTLE_AXE ? c_axe : c_weapon,
		        j, m);
		}
#endif
	    err++;
	} else if (u.twoweap) {
	    if (noisy)
/*JP
		You("cannot wear a shield while wielding two weapons.");
*/
		You("二刀流をしているので盾で身を守れない。");
	    err++;
	} else
	    *mask = W_ARMS;
    } else if (is_boots(otmp)) {
	if (uarmf) {
/*JP
	    if (noisy) already_wearing(c_boots);
*/
	    if (noisy) already_wearing(c_boots, uarmf);
	    err++;
	} else if (Upolyd && slithy(youmonst.data)) {
#if 0 /*JP*/
	    if (noisy) You("have no feet...");	/* not body_part(FOOT) */
#else
	    if (noisy) You("足がない．．．");	/* not body_part(FOOT) */
#endif
	    err++;
	} else if (Upolyd && youmonst.data->mlet == S_CENTAUR) {
	    /* break_armor() pushes boots off for centaurs,
	       so don't let dowear() put them back on... */
#if 0 /*JP*/
	    if (noisy) pline("You have too many hooves to wear %s.",
			     c_boots);	/* makeplural(body_part(FOOT)) yields
					   "rear hooves" which sounds odd */
#else
	    if (noisy) pline("%sを履くには足が多すぎる。",
			     c_boots);	/* makeplural(body_part(FOOT)) yields
					   "rear hooves" which sounds odd */
#endif
	    err++;
	} else if (u.utrap && (u.utraptype == TT_BEARTRAP ||
				u.utraptype == TT_INFLOOR)) {
	    if (u.utraptype == TT_BEARTRAP) {
/*JP
		if (noisy) Your("%s is trapped!", body_part(FOOT));
*/
		if (noisy) Your("%sは罠にかかっている！", body_part(FOOT));
	    } else {
/*JP
		if (noisy) Your("%s are stuck in the %s!",
*/
		if (noisy) Your("%sは%sにはまっている！",
				makeplural(body_part(FOOT)),
				surface(u.ux, u.uy));
	    }
	    err++;
	} else
	    *mask = W_ARMF;
    } else if (is_gloves(otmp)) {
	if (uarmg) {
#if 0 /*JP*/
	    if (noisy) already_wearing(c_gloves);
#else
	    if (noisy) already_wearing(c_gloves, uarmg);
#endif
	    err++;
	} else if (welded(uwep)) {
/*JP
	    if (noisy) You("cannot wear gloves over your %s.",
*/
	    if (noisy) You("%sの上から手袋を装備できない。",
			   is_sword(uwep) ? c_sword : c_weapon);
	    err++;
	} else
	    *mask = W_ARMG;
#ifdef TOURIST
    } else if (is_shirt(otmp)) {
	if (uarm || uarmc || uarmu) {
	    if (uarmu) {
#if 0 /*JP*/
		if (noisy) already_wearing(an(c_shirt));
#else
		if (noisy) already_wearing(an(c_shirt), uarmu);
#endif
	    } else {
/*JP
		if (noisy) You_cant("wear that over your %s.",
*/
		if (noisy) You("%sの上から着ることはできない。",
			           (uarm && !uarmc) ? c_armor : cloak_simple_name(uarmc));
	    }
	    err++;
	} else
	    *mask = W_ARMU;
#endif
    } else if (is_cloak(otmp)) {
	if (uarmc) {
/*JP
	    if (noisy) already_wearing(an(cloak_simple_name(uarmc)));
*/
	    if (noisy) already_wearing(an(cloak_simple_name(uarmc)), uarmc);
	    err++;
	} else
	    *mask = W_ARMC;
    } else if (is_suit(otmp)) {
	if (uarmc) {
/*JP
	    if (noisy) You("cannot wear armor over a %s.", cloak_simple_name(uarmc));
*/
	    if (noisy) You("%sの上から着ることはできない。", cloak_simple_name(uarmc));
	    err++;
	} else if (uarm) {
/*JP
	    if (noisy) already_wearing("some armor");
*/
	    if (noisy) already_wearing("鎧", uarm);
	    err++;
	} else
	    *mask = W_ARM;
    } else {
	/* getobj can't do this after setting its allow_all flag; that
	   happens if you have armor for slots that are covered up or
	   extra armor for slots that are filled */
/*JP
	if (noisy) silly_thing("wear", otmp);
*/
	if (noisy) silly_thing("身につける", otmp);
	err++;
    }
/* Unnecessary since now only weapons and special items like pick-axes get
 * welded to your hand, not armor
    if (welded(otmp)) {
	if (!err++) {
	    if (noisy) weldmsg(otmp);
	}
    }
 */
    return !err;
}

/* the 'W' command */
int
dowear()
{
	struct obj *otmp;
	int delay;
	long mask = 0;
#if 0 /*JP*/
	const char *j;
	const char *m;
#endif

	/* cantweararm checks for suits of armor */
	/* verysmall or nohands checks for shields, gloves, etc... */
	if ((verysmall(youmonst.data) || nohands(youmonst.data))) {
/*JP
		pline("Don't even bother.");
*/
		pline("そんなつまらないことにこだわるな。");
		return(0);
	}

	otmp = getobj(clothes, "wear");
	if(!otmp) return(0);

	if (!canwearobj(otmp,&mask,TRUE)) return(0);

	if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
	    return 1;	/* costs a turn even though it didn't get worn */

	if (otmp->otyp == HELM_OF_OPPOSITE_ALIGNMENT &&
			qstart_level.dnum == u.uz.dnum) {	/* in quest */
		if (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL])
/*JP
			You("narrowly avoid losing all chance at your goal.");
*/
			You("目的を達成する可能性を失うことをなんとか回避した。");
		else	/* converted */
/*JP
			You("are suddenly overcome with shame and change your mind.");
*/
			You("突然恥ずかしくなって考えを変えた。");
		u.ublessed = 0; /* lose your god's protection */
		makeknown(otmp->otyp);
		flags.botl = 1;
		return 1;
	}

	otmp->known = TRUE;
	if(otmp == uwep)
		setuwep((struct obj *)0);
	if (otmp == uswapwep)
		setuswapwep((struct obj *) 0);
	if (otmp == uquiver)
		setuqwep((struct obj *) 0);
	setworn(otmp, mask);
	delay = -objects[otmp->otyp].oc_delay;
	if(delay){
/*JP
		nomul(delay, "dressing up");
*/
		nomul(delay, "装備している間に");
		if(is_boots(otmp)) afternmv = Boots_on;
		if(is_helmet(otmp)) afternmv = Helmet_on;
		if(is_gloves(otmp)) afternmv = Gloves_on;
		if(otmp == uarm) afternmv = Armor_on;
/*JP
		nomovemsg = "You finish your dressing maneuver.";
*/
		nomovemsg = "あなたは装備し終った。";
	} else {
		if(is_cloak(otmp)) (void) Cloak_on();
		if (is_shield(otmp)) (void) Shield_on();
#ifdef TOURIST
		if (is_shirt(otmp)) (void) Shirt_on();
#endif
		on_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

int
doputon()
{
	register struct obj *otmp;
	long mask = 0L;

	if(uleft && uright && uamul && ublindf) {
#if 0 /*JP*/
		Your("%s%s are full, and you're already wearing an amulet and %s.",
			humanoid(youmonst.data) ? "ring-" : "",
			makeplural(body_part(FINGER)),
			ublindf->otyp==LENSES ? "some lenses" :
			ublindf->otyp==TOWEL ? "a towel" : "a blindfold");
#else
		Your("%s%sはふさがってるし、すでに魔除けと%sも身につけている。",
			humanoid(youmonst.data) ? "薬" : "",
			makeplural(body_part(FINGER)),
			ublindf->otyp==LENSES ? "眼鏡" :
			ublindf->otyp==TOWEL ? "タオル" : "目隠し");
#endif
		return(0);
	}
	otmp = getobj(accessories, "put on");
	if(!otmp) return(0);
	if(otmp->owornmask & (W_RING | W_AMUL | W_TOOL)) {
/*JP
		already_wearing(c_that_);
*/
		already_wearing(c_that_, otmp);
		return(0);
	}
	if(welded(otmp)) {
		weldmsg(otmp);
		return(0);
	}
	if(otmp == uwep)
		setuwep((struct obj *)0);
	if(otmp == uswapwep)
		setuswapwep((struct obj *) 0);
	if(otmp == uquiver)
		setuqwep((struct obj *) 0);
	if(otmp->oclass == RING_CLASS || otmp->otyp == MEAT_RING) {
		if(nolimbs(youmonst.data)) {
/*JP
			You("cannot make the ring stick to your body.");
*/
			You("指輪をはめれない体だ。");
			return(0);
		}
		if(uleft && uright){
#if 0 /*JP*/
			There("are no more %s%s to fill.",
				humanoid(youmonst.data) ? "ring-" : "",
				makeplural(body_part(FINGER)));
#else
			pline("はめることのできる%s%sがない。",
				humanoid(youmonst.data) ? "薬" : "",
				makeplural(body_part(FINGER)));
#endif
			return(0);
		}
		if(uleft) mask = RIGHT_RING;
		else if(uright) mask = LEFT_RING;
		else do {
			char qbuf[QBUFSZ];
			char answer;

#if 0 /*JP*/
			Sprintf(qbuf, "Which %s%s, Right or Left?",
				humanoid(youmonst.data) ? "ring-" : "",
				body_part(FINGER));
#else
			Sprintf(qbuf, "どちらの%s%s、右(r)それとも左(l)？",
				humanoid(youmonst.data) ? "薬" : "",
				body_part(FINGER));
#endif
			if(!(answer = yn_function(qbuf, "rl", '\0')))
				return(0);
			switch(answer){
			case 'l':
			case 'L':
				mask = LEFT_RING;
				break;
			case 'r':
			case 'R':
				mask = RIGHT_RING;
				break;
			}
		} while(!mask);
		if (uarmg && uarmg->cursed) {
			uarmg->bknown = TRUE;
/*JP
		    You("cannot remove your gloves to put on the ring.");
*/
		    You("指輪をはめようとしたが手袋が脱げない。");
			return(0);
		}
		if (welded(uwep) && bimanual(uwep)) {
			/* welded will set bknown */
/*JP
	    You("cannot free your weapon hands to put on the ring.");
*/
	    You("指輪をはめようとしたが利腕の自由がきかない。");
			return(0);
		}
		if (welded(uwep) && mask==RIGHT_RING) {
			/* welded will set bknown */
/*JP
	    You("cannot free your weapon hand to put on the ring.");
*/
	    You("指輪をはめようとしたが利腕の自由がきかない。");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1; /* costs a turn even though it didn't get worn */
		setworn(otmp, mask);
		Ring_on(otmp);
	} else if (otmp->oclass == AMULET_CLASS) {
		if(uamul) {
/*JP
			already_wearing("an amulet");
*/
			already_wearing("魔除け", uamul);
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
		setworn(otmp, W_AMUL);
		if (otmp->otyp == AMULET_OF_CHANGE) {
			Amulet_on();
			/* Don't do a prinv() since the amulet is now gone */
			return(1);
		}
		Amulet_on();
	} else {	/* it's a blindfold, towel, or lenses */
		if (ublindf) {
			if (ublindf->otyp == TOWEL)
#if 0 /*JP*/
				Your("%s is already covered by a towel.",
					body_part(FACE));
#else
				You("既にタオルを身につけている。");
#endif
			else if (ublindf->otyp == BLINDFOLD) {
				if (otmp->otyp == LENSES)
/*JP
					already_wearing2("lenses", "a blindfold");
*/
					already_wearing2("眼鏡", "目隠し");
				else
/*JP
					already_wearing("a blindfold");
*/
					already_wearing("目隠し", ublindf);
			} else if (ublindf->otyp == LENSES) {
				if (otmp->otyp == BLINDFOLD)
/*JP
					already_wearing2("a blindfold", "some lenses");
*/
					already_wearing2("目隠し", "眼鏡");
				else
/*JP
					already_wearing("some lenses");
*/
					already_wearing("眼鏡", ublindf);
			} else
#if 0 /*JP*/
				already_wearing(something); /* ??? */
#else
				already_wearing("何か", ublindf); /* ??? */
#endif
			return(0);
		}
		if (otmp->otyp != BLINDFOLD && otmp->otyp != TOWEL && otmp->otyp != LENSES) {
/*JP
			You_cant("wear that!");
*/
			You_cant("それを身につけることはできない！");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst))
		    return 1;
		Blindf_on(otmp);
		return(1);
	}
	if (is_worn(otmp))
	    prinv((char *)0, otmp, 0L);
	return(1);
}

#endif /* OVLB */

#ifdef OVL0

void
find_ac()
{
	int uac = mons[u.umonnum].ac;

	if(uarm) uac -= ARM_BONUS(uarm);
	if(uarmc) uac -= ARM_BONUS(uarmc);
	if(uarmh) uac -= ARM_BONUS(uarmh);
	if(uarmf) uac -= ARM_BONUS(uarmf);
	if(uarms) uac -= ARM_BONUS(uarms);
	if(uarmg) uac -= ARM_BONUS(uarmg);
#ifdef TOURIST
	if(uarmu) uac -= ARM_BONUS(uarmu);
#endif
	if(uleft && uleft->otyp == RIN_PROTECTION) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_PROTECTION) uac -= uright->spe;
	if (HProtection & INTRINSIC) uac -= u.ublessed;
	uac -= u.uspellprot;
	if (uac < -128) uac = -128;	/* u.uac is an schar */
	if(uac != u.uac){
		u.uac = uac;
		flags.botl = 1;
	}
}

#endif /* OVL0 */
#ifdef OVLB

void
glibr()
{
	register struct obj *otmp;
	int xfl = 0;
	boolean leftfall, rightfall;
	const char *otherwep = 0;

	leftfall = (uleft && !uleft->cursed &&
		    (!uwep || !welded(uwep) || !bimanual(uwep)));
	rightfall = (uright && !uright->cursed && (!welded(uwep)));
	if (!uarmg && (leftfall || rightfall) && !nolimbs(youmonst.data)) {
		/* changed so cursed rings don't fall off, GAN 10/30/86 */
#if 0 /*JP*/
		Your("%s off your %s.",
			(leftfall && rightfall) ? "rings slip" : "ring slips",
			(leftfall && rightfall) ? makeplural(body_part(FINGER)) :
			body_part(FINGER));
#else
		Your("指輪は%sから滑り落ちた。", body_part(FINGER));
#endif
		xfl++;
		if (leftfall) {
			otmp = uleft;
			Ring_off(uleft);
			dropx(otmp);
		}
		if (rightfall) {
			otmp = uright;
			Ring_off(uright);
			dropx(otmp);
		}
	}

	otmp = uswapwep;
	if (u.twoweap && otmp) {
#if 0 /*JP*/
		otherwep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		Your("%s %sslips from your %s.",
			otherwep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
#else
		otherwep = is_sword(otmp) ? c_sword :
		    jtrns_obj('S',oclass_names[(int)otmp->oclass]);
		You("%s%s%sから滑り落とした。",
			otherwep,
			xfl ? "もまた" : "を",
			body_part(HAND));
#endif
		setuswapwep((struct obj *)0);
		xfl++;
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
	otmp = uwep;
	if (otmp && !welded(otmp)) {
		const char *thiswep;

		/* nice wording if both weapons are the same type */
#if 0 /*JP*/
		thiswep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
#else
		thiswep = is_sword(otmp) ? c_sword :
		    jtrns_obj('S',oclass_names[(int)otmp->oclass]);
#endif
		if (otherwep && strcmp(thiswep, otherwep)) otherwep = 0;

		/* changed so cursed weapons don't fall, GAN 10/30/86 */
#if 0 /*JP*/
		Your("%s%s %sslips from your %s.",
			otherwep ? "other " : "", thiswep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
#else
		You("%s%s%s%sから滑り落とした。",
			otherwep ? "もうひとつの" : "", thiswep,
			xfl ? "もまた" : "を",
			body_part(HAND));
#endif
		setuwep((struct obj *)0);
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
}

struct obj *
some_armor(victim)
struct monst *victim;
{
	register struct obj *otmph, *otmp;

	otmph = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
#ifdef TOURIST
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
#endif
	
	otmp = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	return(otmph);
}

/* erode some arbitrary armor worn by the victim */
void
erode_armor(victim, acid_dmg)
struct monst *victim;
boolean acid_dmg;
{
	struct obj *otmph = some_armor(victim);

	if (otmph && (otmph != uarmf)) {
	    erode_obj(otmph, acid_dmg, FALSE);
	    if (carried(otmph)) update_inventory();
	}
}

/* used for praying to check and fix levitation trouble */
struct obj *
stuck_ring(ring, otyp)
struct obj *ring;
int otyp;
{
    if (ring != uleft && ring != uright) {
	warning("stuck_ring: neither left nor right?");
	return (struct obj *)0;
    }

    if (ring && ring->otyp == otyp) {
	/* reasons ring can't be removed match those checked by select_off();
	   limbless case has extra checks because ordinarily it's temporary */
	if (nolimbs(youmonst.data) &&
		uamul && uamul->otyp == AMULET_OF_UNCHANGING && uamul->cursed)
	    return uamul;
	if (welded(uwep) && (ring == uright || bimanual(uwep))) return uwep;
	if (uarmg && uarmg->cursed) return uarmg;
	if (ring->cursed) return ring;
    }
    /* either no ring or not right type or nothing prevents its removal */
    return (struct obj *)0;
}

/* also for praying; find worn item that confers "Unchanging" attribute */
struct obj *
unchanger()
{
    if (uamul && uamul->otyp == AMULET_OF_UNCHANGING) return uamul;
    return 0;
}

/* occupation callback for 'A' */
STATIC_PTR
int
select_off(otmp)
register struct obj *otmp;
{
	struct obj *why;
	char buf[BUFSZ];

	if (!otmp) return 0;
	*buf = '\0';			/* lint suppresion */

	/* special ring checks */
	if (otmp == uright || otmp == uleft) {
	    if (nolimbs(youmonst.data)) {
#if 0 /*JP*/
		pline_The("ring is stuck.");
#else
		pline("指輪は体に埋まってしまっている。");
#endif
		return 0;
	    }
	    why = 0;	/* the item which prevents ring removal */
	    if (welded(uwep) && (otmp == uright || bimanual(uwep))) {
/*JP
		Sprintf(buf, "free a weapon %s", body_part(HAND));
*/
		Sprintf(buf, "利腕の自由がきかない");
		why = uwep;
	    } else if (uarmg && uarmg->cursed) {
/*JP
		Sprintf(buf, "take off your %s", c_gloves);
*/
		Sprintf(buf, "%sが脱げない", c_gloves);
		why = uarmg;
	    }
	    if (why) {
/*JP
		You("cannot %s to remove the ring.", buf);
*/
		You("指輪をはずそうとしたが%s。", buf);
		why->bknown = TRUE;
		return 0;
	    }
	}
	/* special glove checks */
	if (otmp == uarmg) {
	    if (welded(uwep)) {
#if 0 /*JP*/
		You("are unable to take off your %s while wielding that %s.",
		    c_gloves, is_sword(uwep) ? c_sword : c_weapon);
#else
		You("%sを持ったまま%sをはずすことはできない。",
		    is_sword(uwep) ? c_sword : c_weapon, c_gloves);
#endif
		uwep->bknown = TRUE;
		return 0;
	    } else if (Glib) {
#if 0 /*JP*/
		You_cant("take off the slippery %s with your slippery %s.",
			 c_gloves, makeplural(body_part(FINGER)));
#else
		You_cant("滑りやすい%sを滑りやすい%sではずせない。",
			 c_gloves, body_part(FINGER));
#endif
		return 0;
	    }
	}
	/* special boot checks */
	if (otmp == uarmf) {
	    if (u.utrap && u.utraptype == TT_BEARTRAP) {
/*JP
		pline_The("bear trap prevents you from pulling your %s out.",
*/
		pline("%sが熊の罠につかまっているので脱ぐことができない。",
			  body_part(FOOT));
		return 0;
	    } else if (u.utrap && u.utraptype == TT_INFLOOR) {
/*JP
		You("are stuck in the %s, and cannot pull your %s out.",
*/
		You("%sが%sにはまっているので脱ぐことができない。",
		    surface(u.ux, u.uy), makeplural(body_part(FOOT)));
		return 0;
	    }
	}
	/* special suit and shirt checks */
	if (otmp == uarm
#ifdef TOURIST
			|| otmp == uarmu
#endif
		) {
	    why = 0;	/* the item which prevents disrobing */
	    if (uarmc && uarmc->cursed) {
/*JP
		Sprintf(buf, "remove your %s", cloak_simple_name(uarmc));
*/
		Sprintf(buf, "%sが脱げない", cloak_simple_name(uarmc));
		why = uarmc;
#ifdef TOURIST
	    } else if (otmp == uarmu && uarm && uarm->cursed) {
/*JP
		Sprintf(buf, "remove your %s", c_suit);
*/
		Sprintf(buf, "%sが脱げない", c_suit);
		why = uarm;
#endif
	    } else if (welded(uwep) && bimanual(uwep)) {
#if 0 /*JP*/
		Sprintf(buf, "release your %s",
			is_sword(uwep) ? c_sword :
			(uwep->otyp == BATTLE_AXE) ? c_axe : c_weapon);
#else
		Sprintf(buf, "%sが手放せない",
			is_sword(uwep) ? c_sword :
			(uwep->otyp == BATTLE_AXE) ? c_axe : c_weapon);
#endif
		why = uwep;
	    }
	    if (why) {
/*JP
		You("cannot %s to take off %s.", buf, the(xname(otmp)));
*/
		You("%sをはずそうとしたが%s。", xname(otmp), buf);
		why->bknown = TRUE;
		return 0;
	    }
	}
	/* basic curse check */
	if (otmp == uquiver || (otmp == uswapwep && !u.twoweap)) {
	    ;	/* some items can be removed even when cursed */
	} else {
	    /* otherwise, this is fundamental */
	    if (cursed(otmp)) return 0;
	}

	if(otmp == uarm) takeoff_mask |= WORN_ARMOR;
	else if(otmp == uarmc) takeoff_mask |= WORN_CLOAK;
	else if(otmp == uarmf) takeoff_mask |= WORN_BOOTS;
	else if(otmp == uarmg) takeoff_mask |= WORN_GLOVES;
	else if(otmp == uarmh) takeoff_mask |= WORN_HELMET;
	else if(otmp == uarms) takeoff_mask |= WORN_SHIELD;
#ifdef TOURIST
	else if(otmp == uarmu) takeoff_mask |= WORN_SHIRT;
#endif
	else if(otmp == uleft) takeoff_mask |= LEFT_RING;
	else if(otmp == uright) takeoff_mask |= RIGHT_RING;
	else if(otmp == uamul) takeoff_mask |= WORN_AMUL;
	else if(otmp == ublindf) takeoff_mask |= WORN_BLINDF;
	else if(otmp == uwep) takeoff_mask |= W_WEP;
	else if(otmp == uswapwep) takeoff_mask |= W_SWAPWEP;
	else if(otmp == uquiver) takeoff_mask |= W_QUIVER;

	else warning("select_off: %s???", doname(otmp));

	return(0);
}

STATIC_OVL struct obj *
do_takeoff()
{
	register struct obj *otmp = (struct obj *)0;

	if (taking_off == W_WEP) {
	  if(!cursed(uwep)) {
	    setuwep((struct obj *) 0);
/*JP
	    You("are empty %s.", body_part(HANDED));
*/
	    You("何も%sにしていない。", body_part(HAND));
	    u.twoweap = FALSE;
	  }
	} else if (taking_off == W_SWAPWEP) {
	  setuswapwep((struct obj *) 0);
/*JP
	  You("no longer have a second weapon readied.");
*/
	  You("予備の武器をおさめた。");
	  u.twoweap = FALSE;
	} else if (taking_off == W_QUIVER) {
	  setuqwep((struct obj *) 0);
/*JP
	  You("no longer have ammunition readied.");
*/
	  You("装填した矢(など)をおさめた。");
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  if(!cursed(otmp)) (void) Armor_off();
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	  if(!cursed(otmp)) (void) Cloak_off();
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	  if(!cursed(otmp)) (void) Boots_off();
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	  if(!cursed(otmp)) (void) Gloves_off();
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	  if(!cursed(otmp)) (void) Helmet_off();
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
	  if(!cursed(otmp)) (void) Shield_off();
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  if (!cursed(otmp)) (void) Shirt_off();
#endif
	} else if (taking_off == WORN_AMUL) {
	  otmp = uamul;
	  if(!cursed(otmp)) Amulet_off();
	} else if (taking_off == LEFT_RING) {
	  otmp = uleft;
	  if(!cursed(otmp)) Ring_off(uleft);
	} else if (taking_off == RIGHT_RING) {
	  otmp = uright;
	  if(!cursed(otmp)) Ring_off(uright);
	} else if (taking_off == WORN_BLINDF) {
	  if (!cursed(ublindf)) Blindf_off(ublindf);
	} else warning("do_takeoff: taking off %lx", taking_off);

	return(otmp);
}

#if 0 /*JP*/
static const char *disrobing = "";
#endif

STATIC_PTR
int
take_off()
{
	register int i;
	register struct obj *otmp;

	if (taking_off) {
	    if (todelay > 0) {
		todelay--;
		return(1);	/* still busy */
	    } else {
		if ((otmp = do_takeoff())) off_msg(otmp);
	    }
	    takeoff_mask &= ~taking_off;
	    taking_off = 0L;
	}

	for(i = 0; takeoff_order[i]; i++)
	    if(takeoff_mask & takeoff_order[i]) {
		taking_off = takeoff_order[i];
		break;
	    }

	otmp = (struct obj *) 0;
	todelay = 0;

	if (taking_off == 0L) {
/*JP
	  You("finish %s.", disrobing);
*/
	  You("装備を解きおえた。");
	  return 0;
	} else if (taking_off == W_WEP) {
	  todelay = 1;
	} else if (taking_off == W_SWAPWEP) {
	  todelay = 1;
	} else if (taking_off == W_QUIVER) {
	  todelay = 1;
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  /* If a cloak is being worn, add the time to take it off and put
	   * it back on again.  Kludge alert! since that time is 0 for all
	   * known cloaks, add 1 so that it actually matters...
	   */
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  /* add the time to take off and put back on armor and/or cloak */
	  if (uarm)  todelay += 2 * objects[uarm->otyp].oc_delay;
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
#endif
	} else if (taking_off == WORN_AMUL) {
	  todelay = 1;
	} else if (taking_off == LEFT_RING) {
	  todelay = 1;
	} else if (taking_off == RIGHT_RING) {
	  todelay = 1;
	} else if (taking_off == WORN_BLINDF) {
	  todelay = 2;
	} else {
	  warning("take_off: taking off %lx", taking_off);
	  return 0;	/* force done */
	}

	if (otmp) todelay += objects[otmp->otyp].oc_delay;

	/* Since setting the occupation now starts the counter next move, that
	 * would always produce a delay 1 too big per item unless we subtract
	 * 1 here to account for it.
	 */
	if (todelay > 0) todelay--;

#if 0 /*JP*/
	set_occupation(take_off, disrobing, 0);
#else
	set_occupation(take_off, "装備を解く", 0);
#endif
	return(1);		/* get busy */
}

/* clear saved context to avoid inappropriate resumption of interrupted 'A' */
void
reset_remarm()
{
	taking_off = takeoff_mask = 0L;
#if 0 /*JP*/
	disrobing = nul;
#endif
}

/* the 'A' command -- remove multiple worn items */
int
doddoremarm()
{
    int result = 0;

    if (taking_off || takeoff_mask) {
/*JP
	You("continue %s.", disrobing);
*/
	You("装備を解くのを再開した。");
/*JP
	set_occupation(take_off, disrobing, 0);
*/
	set_occupation(take_off, "装備を解く", 0);
	return 0;
    } else if (!uwep && !uswapwep && !uquiver && !uamul && !ublindf &&
		!uleft && !uright && !wearing_armor()) {
/*JP
	You("are not wearing anything.");
*/
	You("何も装備していない。");
	return 0;
    }

    add_valid_menu_class(0); /* reset */
    if (flags.menu_style != MENU_TRADITIONAL ||
	    (result = ggetobj("take off", select_off, 0, FALSE, (unsigned *)0)) < -1)
	result = menu_remarm(result);

    if (takeoff_mask) {
	/* default activity for armor and/or accessories,
	   possibly combined with weapons */
#if 0 /*JP*/
	disrobing = "disrobing";
	/* specific activity when handling weapons only */
	if (!(takeoff_mask & ~(W_WEP|W_SWAPWEP|W_QUIVER)))
	    disrobing = "disarming";
#endif
	(void) take_off();
    }
    /* The time to perform the command is already completely accounted for
     * in take_off(); if we return 1, that would add an extra turn to each
     * disrobe.
     */
    return 0;
}

STATIC_OVL int
menu_remarm(retry)
int retry;
{
    int n, i = 0;
    menu_item *pick_list;
    boolean all_worn_categories = TRUE;

    if (retry) {
	all_worn_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL) {
	all_worn_categories = FALSE;
/*JP
	n = query_category("What type of things do you want to take off?",
*/
	n = query_category("どのタイプの物の装備を解きますか？",
			   invent, WORN_TYPES|ALL_TYPES, &pick_list, PICK_ANY);
	if (!n) return 0;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_worn_categories = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((genericptr_t) pick_list);
    } else if (flags.menu_style == MENU_COMBINATION) {
	all_worn_categories = FALSE;
	if (ggetobj("take off", select_off, 0, TRUE, (unsigned *)0) == -2)
	    all_worn_categories = TRUE;
    }

/*JP
    n = query_objlist("What do you want to take off?", invent,
*/
    n = query_objlist("どの装備を解きますか？", invent,
			SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
			&pick_list, PICK_ANY,
			all_worn_categories ? is_worn : is_worn_by_type);
    if (n > 0) {
	for (i = 0; i < n; i++)
	    (void) select_off(pick_list[i].item.a_obj);
	free((genericptr_t) pick_list);
    } else if (n < 0 && flags.menu_style != MENU_COMBINATION) {
/*JP
	There("is nothing else you can remove or unwield.");
*/
	pline("装備を解けるものは何もない。");
    }
    return 0;
}

/* hit by destroy armor scroll/black dragon breath/monster spell */
int
destroy_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;
#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && \
			(!obj_resists(otmp, 0, 90)))

	if (DESTROY_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
#if 0 /*JP:T*/
		Your("%s crumbles and turns to dust!",
		     cloak_simple_name(uarmc));
#else
		Your("%sは粉々になった！",
		     cloak_simple_name(uarmc));
#endif
		(void) Cloak_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
/*JP
		pline("%s turns to dust and falls to the %s!",
*/
		pline("%sは塵となり%sに落ちた！",
			Ysimple_name2(otmp),
			surface(u.ux,u.uy));
		(void) Armor_gone();
		useup(otmp);
#ifdef TOURIST
	} else if (DESTROY_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
/*JP
		Your("shirt crumbles into tiny threads and falls apart!");
*/
		Your("シャツはズタズタに裂け、小さな糸屑となり落ちた！");
		(void) Shirt_off();
		useup(otmp);
#endif
	} else if (DESTROY_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
/*JP
		Your("helmet turns to dust and is blown away!");
*/
		Your("兜は塵となり吹きとんだ！");
		(void) Helmet_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
/*JP
		Your("gloves vanish!");
*/
		Your("手袋は消えた！");
		(void) Gloves_off();
		useup(otmp);
/*JP
		selftouch("You");
*/
		selftouch("そのときあなたは");
	} else if (DESTROY_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
/*JP
		Your("boots disintegrate!");
*/
		Your("靴は粉々に砕けた！");
		(void) Boots_off();
		useup(otmp);
	} else if (DESTROY_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
/*JP
		Your("shield crumbles away!");
*/
		Your("盾は砕け散った！");
		(void) Shield_off();
		useup(otmp);
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

void
adj_abon(otmp, delta)
register struct obj *otmp;
register schar delta;
{
	if (uarmg && uarmg == otmp && otmp->otyp == GAUNTLETS_OF_DEXTERITY) {
		if (delta) {
			makeknown(uarmg->otyp);
			ABON(A_DEX) += (delta);
		}
		flags.botl = 1;
	}
	if (uarmh && uarmh == otmp && otmp->otyp == HELM_OF_BRILLIANCE) {
		if (delta) {
			makeknown(uarmh->otyp);
			ABON(A_INT) += (delta);
			ABON(A_WIS) += (delta);
		}
		flags.botl = 1;
	}
}

#endif /* OVLB */

/*do_wear.c*/
