/*	SCCS Id: @(#)detect.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Detection routines, including crystal ball, magic mapping, and search
 * command.
 */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1996-2000
**	changing point is marked `JP' (96/7/21)
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2004
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"
#include "artifact.h"

extern boolean known;	/* from read.c */

STATIC_DCL void FDECL(do_dknown_of, (struct obj *));
STATIC_DCL boolean FDECL(check_map_spot, (int,int,CHAR_P,unsigned));
STATIC_DCL boolean FDECL(clear_stale_map, (CHAR_P,unsigned));
STATIC_DCL void FDECL(sense_trap, (struct trap *,XCHAR_P,XCHAR_P,int));
STATIC_DCL void FDECL(show_map_spot, (int,int));
STATIC_PTR void FDECL(findone,(int,int,genericptr_t));
STATIC_PTR void FDECL(openone,(int,int,genericptr_t));

/* Recursively search obj for an object in class oclass and return 1st found */
struct obj *
o_in(obj, oclass)
struct obj* obj;
char oclass;
{
    register struct obj* otmp;
    struct obj *temp;

    if (obj->oclass == oclass) return obj;

    if (Has_contents(obj)) {
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (otmp->oclass == oclass) return otmp;
	    else if (Has_contents(otmp) && (temp = o_in(otmp, oclass)))
		return temp;
    }
    return (struct obj *) 0;
}

/* Recursively search obj for an object made of specified material and return 1st found */
struct obj *
o_material(obj, material)
struct obj* obj;
unsigned material;
{
    register struct obj* otmp;
    struct obj *temp;

    if (objects[obj->otyp].oc_material == material) return obj;

    if (Has_contents(obj)) {
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    if (objects[otmp->otyp].oc_material == material) return otmp;
	    else if (Has_contents(otmp) && (temp = o_material(otmp, material)))
		return temp;
    }
    return (struct obj *) 0;
}

STATIC_OVL void
do_dknown_of(obj)
struct obj *obj;
{
    struct obj *otmp;

    obj->dknown = 1;
    if (Has_contents(obj)) {
	for(otmp = obj->cobj; otmp; otmp = otmp->nobj)
	    do_dknown_of(otmp);
    }
}

/* Check whether the location has an outdated object displayed on it. */
STATIC_OVL boolean
check_map_spot(x, y, oclass, material)
int x, y;
register char oclass;
unsigned material;
{
	register int glyph;
	register struct obj *otmp;
	register struct monst *mtmp;

	glyph = glyph_at(x,y);
	if (glyph_is_object(glyph)) {
	    /* there's some object shown here */
	    if (oclass == ALL_CLASSES) {
		return((boolean)( !(level.objects[x][y] ||     /* stale if nothing here */
			    ((mtmp = m_at(x,y)) != 0 &&
				(
#ifndef GOLDOBJ
				 mtmp->mgold ||
#endif
						 mtmp->minvent)))));
	    } else {
		if (material && objects[glyph_to_obj(glyph)].oc_material == material) {
			/* the object shown here is of interest because material matches */
			for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
				if (o_material(otmp, GOLD)) return FALSE;
			/* didn't find it; perhaps a monster is carrying it */
			if ((mtmp = m_at(x,y)) != 0) {
				for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_material(otmp, GOLD)) return FALSE;
		        }
			/* detection indicates removal of this object from the map */
			return TRUE;
		}
	        if (oclass && objects[glyph_to_obj(glyph)].oc_class == oclass) {
			/* the object shown here is of interest because its class matches */
			for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
				if (o_in(otmp, oclass)) return FALSE;
			/* didn't find it; perhaps a monster is carrying it */
#ifndef GOLDOBJ
			if ((mtmp = m_at(x,y)) != 0) {
				if (oclass == COIN_CLASS && mtmp->mgold)
					return FALSE;
				else for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_in(otmp, oclass)) return FALSE;
		        }
#else
			if ((mtmp = m_at(x,y)) != 0) {
				for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
					if (o_in(otmp, oclass)) return FALSE;
		        }
#endif
			/* detection indicates removal of this object from the map */
			return TRUE;
	        }
	    }
	}
	return FALSE;
}

/*
   When doing detection, remove stale data from the map display (corpses
   rotted away, objects carried away by monsters, etc) so that it won't
   reappear after the detection has completed.  Return true if noticeable
   change occurs.
 */
STATIC_OVL boolean
clear_stale_map(oclass, material)
register char oclass;
unsigned material;
{
	register int zx, zy;
	register boolean change_made = FALSE;

	for (zx = 1; zx < COLNO; zx++)
	    for (zy = 0; zy < ROWNO; zy++)
		if (check_map_spot(zx, zy, oclass,material)) {
		    unmap_object(zx, zy);
		    change_made = TRUE;
		}

	return change_made;
}

/* look for gold, on the floor or in monsters' possession */
int
gold_detect(sobj)
register struct obj *sobj;
{
    register struct obj *obj;
    register struct monst *mtmp;
    int uw = u.uinwater;
    struct obj *temp;
    boolean stale;

    known = stale = clear_stale_map(COIN_CLASS,
				(unsigned)(sobj->blessed ? GOLD : 0));

    /* look for gold carried by monsters (might be in a container) */
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    	if (DEADMONSTER(mtmp)) continue;	/* probably not needed in this case but... */
#ifndef GOLDOBJ
	if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
	if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
	    known = TRUE;
	    goto outgoldmap;	/* skip further searching */
	} else for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (sobj->blessed && o_material(obj, GOLD)) {
	    	known = TRUE;
	    	goto outgoldmap;
	    } else if (o_in(obj, COIN_CLASS)) {
		known = TRUE;
		goto outgoldmap;	/* skip further searching */
	    }
    }
    
    /* look for gold objects */
    for (obj = fobj; obj; obj = obj->nobj) {
	if (sobj->blessed && o_material(obj, GOLD)) {
	    known = TRUE;
	    if (obj->ox != u.ux || obj->oy != u.uy) goto outgoldmap;
	} else if (o_in(obj, COIN_CLASS)) {
	    known = TRUE;
	    if (obj->ox != u.ux || obj->oy != u.uy) goto outgoldmap;
	}
    }

    if (!known) {
	/* no gold found on floor or monster's inventory.
	   adjust message if you have gold in your inventory */
	if (sobj) {
		char buf[BUFSZ];
		if (youmonst.data == &mons[PM_GOLD_GOLEM]) {
#if 0 /*JP*/
			Sprintf(buf, "You feel like a million %s!",
				currency(2L));
#else
			Strcpy(buf, "あなたは金持ちになったように感じた！");
#endif
		} else if (hidden_gold() ||
#ifndef GOLDOBJ
				u.ugold)
#else
			        money_cnt(invent))
#endif
			Strcpy(buf,
/*JP
				"You feel worried about your future financial situation.");
*/
				"あなたは将来の経済状況が心配になった。");
		else
/*JP
			Strcpy(buf, "You feel materially poor.");
*/
			Strcpy(buf, "あなたはひもじさを感じた。");
		strange_feeling(sobj, buf);
        }
	return(1);
    }
    /* only under me - no separate display required */
    if (stale) docrt();
/*JP
    You("notice some gold between your %s.", makeplural(body_part(FOOT)));
*/
    You("%sの間に金貨が落ちていることに気がついた。", body_part(FOOT));
    return(0);

outgoldmap:
    cls();

    u.uinwater = 0;
    /* Discover gold locations. */
    for (obj = fobj; obj; obj = obj->nobj) {
    	if (sobj->blessed && (temp = o_material(obj, GOLD))) {
	    if (temp != obj) {
		temp->ox = obj->ox;
		temp->oy = obj->oy;
	    }
	    map_object(temp,1);
	} else if ((temp = o_in(obj, COIN_CLASS))) {
	    if (temp != obj) {
		temp->ox = obj->ox;
		temp->oy = obj->oy;
	    }
	    map_object(temp,1);
	}
    }
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    	if (DEADMONSTER(mtmp)) continue;	/* probably overkill here */
#ifndef GOLDOBJ
	if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
	if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
	    struct obj gold;

	    gold.otyp = GOLD_PIECE;
	    gold.ox = mtmp->mx;
	    gold.oy = mtmp->my;
	    map_object(&gold,1);
	} else for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (sobj->blessed && (temp = o_material(obj, GOLD))) {
		temp->ox = mtmp->mx;
		temp->oy = mtmp->my;
		map_object(temp,1);
		break;
	    } else if ((temp = o_in(obj, COIN_CLASS))) {
		temp->ox = mtmp->mx;
		temp->oy = mtmp->my;
		map_object(temp,1);
		break;
	    }
    }
    
    newsym(u.ux,u.uy);
/*JP
    You_feel("very greedy, and sense gold!");
*/
    You("どん欲になったような気がした、そして金貨の位置を感知した！");
    exercise(A_WIS, TRUE);
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    u.uinwater = uw;
    if (Underwater) under_water(2);
    if (u.uburied) under_ground(2);
    return(0);
}

/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
int
food_detect(sobj)
register struct obj	*sobj;
{
    register struct obj *obj;
    register struct monst *mtmp;
    register int ct = 0, ctu = 0;
    boolean confused = (Confusion || (sobj && sobj->cursed)), stale;
    char oclass = confused ? POTION_CLASS : FOOD_CLASS;
/*JP
    const char *what = confused ? something : "food";
*/
    const char *what = confused ? "ハラヘリ" : "食べ物";
    int uw = u.uinwater;

    stale = clear_stale_map(oclass, 0);

    for (obj = fobj; obj; obj = obj->nobj)
	if (o_in(obj, oclass)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
    for (mtmp = fmon; mtmp && !ct; mtmp = mtmp->nmon) {
	/* no DEADMONSTER(mtmp) check needed since dmons never have inventory */
	for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if (o_in(obj, oclass)) {
		ct++;
		break;
	    }
    }
    
    if (!ct && !ctu) {
	known = stale && !confused;
	if (stale) {
	    docrt();
/*JP
	    You("sense a lack of %s nearby.", what);
*/
	    You("%sが減っているのに気がついた。",what);
	    if (sobj && sobj->blessed) {
/*JP
		if (!u.uedibility) Your("%s starts to tingle.", body_part(NOSE));
*/
		if (!u.uedibility) Your("%sがぴくっと動いた。", body_part(NOSE));
		u.uedibility = 1;
	    }
	} else if (sobj) {
	    char buf[BUFSZ];
#if 0 /*JP*/
	    Sprintf(buf, "Your %s twitches%s.", body_part(NOSE),
			(sobj->blessed && !u.uedibility) ? " then starts to tingle" : "");
#else
	    Sprintf(buf, "あなたの%sがひくひくと動い%s。", body_part(NOSE),
			(sobj->blessed && !u.uedibility) ? "て、うずうずしだした" : "た");
#endif
	    if (sobj->blessed && !u.uedibility) {
		boolean savebeginner = flags.beginner;	/* prevent non-delivery of */
		flags.beginner = FALSE;			/* 	message            */
		strange_feeling(sobj, buf);
		flags.beginner = savebeginner;
		u.uedibility = 1;
	    } else
		strange_feeling(sobj, buf);
	}
	return !stale;
    } else if (!ct) {
	known = TRUE;
/*JP
	You("%s %s nearby.", sobj ? "smell" : "sense", what);
*/
	You("近くの%sを%s。", what, sobj ? "嗅ぎつけた" : "感知した");
	if (sobj && sobj->blessed) {
/*JP
		if (!u.uedibility) pline("Your %s starts to tingle.", body_part(NOSE));
*/
		if (!u.uedibility) pline("あなたの%sはうずうずしだした。", body_part(NOSE));
		u.uedibility = 1;
	}
    } else {
	struct obj *temp;
	known = TRUE;
	cls();
	u.uinwater = 0;
	for (obj = fobj; obj; obj = obj->nobj)
	    if ((temp = o_in(obj, oclass)) != 0) {
		if (temp != obj) {
		    temp->ox = obj->ox;
		    temp->oy = obj->oy;
		}
		map_object(temp,1);
	    }
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    /* no DEADMONSTER(mtmp) check needed since dmons never have inventory */
	    for (obj = mtmp->minvent; obj; obj = obj->nobj)
		if ((temp = o_in(obj, oclass)) != 0) {
		    temp->ox = mtmp->mx;
		    temp->oy = mtmp->my;
		    map_object(temp,1);
		    break;	/* skip rest of this monster's inventory */
		}
	newsym(u.ux,u.uy);
	if (sobj) {
	    if (sobj->blessed) {
#if 0 /*JP*/
	    	Your("%s %s to tingle and you smell %s.", body_part(NOSE),
	    		u.uedibility ? "continues" : "starts", what);
#else
	    	Your("%sはうずうずし%s、%sの匂いを嗅ぎとった。", body_part(NOSE),
	    		u.uedibility ? "続け" : "始め", what);
#endif
		u.uedibility = 1;
	    } else
/*JP
		Your("%s tingles and you smell %s.", body_part(NOSE), what);
*/
		Your("%sはうずうずし、%sの匂いを嗅ぎとった。", body_part(NOSE), what);
	}
/*JP
	else You("sense %s.", what);
*/
	else You("%sを感知した。", what);
	display_nhwindow(WIN_MAP, TRUE);
	exercise(A_WIS, TRUE);
	docrt();
	u.uinwater = uw;
	if (Underwater) under_water(2);
	if (u.uburied) under_ground(2);
    }
    return(0);
}

/*
 * Used for scrolls, potions, spells, and crystal balls.  Returns:
 *
 *	1 - nothing was detected
 *	0 - something was detected
 */
int
object_detect(detector, class)
struct obj	*detector;	/* object doing the detecting */
int		class;		/* an object class, 0 for all */
{
    register int x, y;
    char stuff[BUFSZ];
    int is_cursed = (detector && detector->cursed);
    int do_dknown = (detector && (detector->oclass == POTION_CLASS ||
				    detector->oclass == SPBOOK_CLASS) &&
			detector->blessed);
    int ct = 0, ctu = 0;
    register struct obj *obj, *otmp = (struct obj *)0;
    register struct monst *mtmp;
    int uw = u.uinwater;
    int sym, boulder = 0;

    if (class < 0 || class >= MAXOCLASSES) {
	warning("object_detect:  illegal class %d", class);
	class = 0;
    }

    /* Special boulder symbol check - does the class symbol happen
     * to match iflags.bouldersym which is a user-defined?
     * If so, that means we aren't sure what they really wanted to
     * detect. Rather than trump anything, show both possibilities.
     * We can exclude checking the buried obj chain for boulders below.
     */
    sym = class ? def_oc_syms[class] : 0;
    if (sym && iflags.bouldersym && sym == iflags.bouldersym)
    	boulder = ROCK_CLASS;

    if (Hallucination || (Confusion && class == SCROLL_CLASS))
	Strcpy(stuff, something);
    else
/*JP
    	Strcpy(stuff, class ? oclass_names[class] : "objects");
*/
    	Strcpy(stuff, class ? oclass_names[class] : "物体");
/*JP
    if (boulder && class != ROCK_CLASS) Strcat(stuff, " and/or large stones");
*/
    if (boulder && class != ROCK_CLASS) Strcat(stuff, "と巨岩");

    if (do_dknown) for(obj = invent; obj; obj = obj->nobj) do_dknown_of(obj);

    for (obj = fobj; obj; obj = obj->nobj) {
	if ((!class && !boulder) || o_in(obj, class) || o_in(obj, boulder)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
	if (do_dknown) do_dknown_of(obj);
    }

    for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
	if (!class || o_in(obj, class)) {
	    if (obj->ox == u.ux && obj->oy == u.uy) ctu++;
	    else ct++;
	}
	if (do_dknown) do_dknown_of(obj);
    }

    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj) {
	    if ((!class && !boulder) || o_in(obj, class) || o_in(obj, boulder)) ct++;
	    if (do_dknown) do_dknown_of(obj);
	}
	if ((is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
	    (!class || class == objects[mtmp->mappearance].oc_class)) ||
#ifndef GOLDOBJ
	    (mtmp->mgold && (!class || class == COIN_CLASS))) {
#else
	    (findgold(mtmp->minvent) && (!class || class == COIN_CLASS))) {
#endif
	    ct++;
	    break;
	}
    }

    if (!clear_stale_map(!class ? ALL_CLASSES : class, 0) && !ct) {
	if (!ctu) {
	    if (detector)
/*JP
		strange_feeling(detector, "You feel a lack of something.");
*/
		strange_feeling(detector, "あなたは何かが欠乏しているような気がした。");
	    return 1;
	}

/*JP
	You("sense %s nearby.", stuff);
*/
	You("近くの%sを感知した。", stuff);
	return 0;
    }

    cls();

    u.uinwater = 0;
/*
 *	Map all buried objects first.
 */
    for (obj = level.buriedobjlist; obj; obj = obj->nobj)
	if (!class || (otmp = o_in(obj, class))) {
	    if (class) {
		if (otmp != obj) {
		    otmp->ox = obj->ox;
		    otmp->oy = obj->oy;
		}
		map_object(otmp, 1);
	    } else
		map_object(obj, 1);
	}
    /*
     * If we are mapping all objects, map only the top object of a pile or
     * the first object in a monster's inventory.  Otherwise, go looking
     * for a matching object class and display the first one encountered
     * at each location.
     *
     * Objects on the floor override buried objects.
     */
    for (x = 1; x < COLNO; x++)
	for (y = 0; y < ROWNO; y++)
	    for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
		if ((!class && !boulder) ||
		    (otmp = o_in(obj, class)) || (otmp = o_in(obj, boulder))) {
		    if (class || boulder) {
			if (otmp != obj) {
			    otmp->ox = obj->ox;
			    otmp->oy = obj->oy;
			}
			map_object(otmp, 1);
		    } else
			map_object(obj, 1);
		    break;
		}

    /* Objects in the monster's inventory override floor objects. */
    for (mtmp = fmon ; mtmp ; mtmp = mtmp->nmon) {
	if (DEADMONSTER(mtmp)) continue;
	for (obj = mtmp->minvent; obj; obj = obj->nobj)
	    if ((!class && !boulder) ||
		 (otmp = o_in(obj, class)) || (otmp = o_in(obj, boulder))) {
		if (!class && !boulder) otmp = obj;
		otmp->ox = mtmp->mx;		/* at monster location */
		otmp->oy = mtmp->my;
		map_object(otmp, 1);
		break;
	    }
	/* Allow a mimic to override the detected objects it is carrying. */
	if (is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
		(!class || class == objects[mtmp->mappearance].oc_class)) {
	    struct obj temp;

	    temp.otyp = mtmp->mappearance;	/* needed for obj_to_glyph() */
	    temp.ox = mtmp->mx;
	    temp.oy = mtmp->my;
	    temp.corpsenm = PM_TENGU;		/* if mimicing a corpse */
	    map_object(&temp, 1);
#ifndef GOLDOBJ
	} else if (mtmp->mgold && (!class || class == COIN_CLASS)) {
#else
	} else if (findgold(mtmp->minvent) && (!class || class == COIN_CLASS)) {
#endif
	    struct obj gold;

	    gold.otyp = GOLD_PIECE;
	    gold.ox = mtmp->mx;
	    gold.oy = mtmp->my;
	    map_object(&gold, 1);
	}
    }

    newsym(u.ux,u.uy);
/*JP
    You("detect the %s of %s.", ct ? "presence" : "absence", stuff);
*/
    You("%s%s。", stuff, ct ? "を発見した" : "は何もないことがわかった" );
    display_nhwindow(WIN_MAP, TRUE);
    /*
     * What are we going to do when the hero does an object detect while blind
     * and the detected object covers a known pool?
     */
    docrt();	/* this will correctly reset vision */

    u.uinwater = uw;
    if (Underwater) under_water(2);
    if (u.uburied) under_ground(2);
    return 0;
}

/*
 * Used by: crystal balls, potions, fountains
 *
 * Returns 1 if nothing was detected.
 * Returns 0 if something was detected.
 */
int
monster_detect(otmp, mclass)
register struct obj *otmp;	/* detecting object (if any) */
int mclass;			/* monster class, 0 for all */
{
    register struct monst *mtmp;
    int mcnt = 0;


    /* Note: This used to just check fmon for a non-zero value
     * but in versions since 3.3.0 fmon can test TRUE due to the
     * presence of dmons, so we have to find at least one
     * with positive hit-points to know for sure.
     */
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    	if (!DEADMONSTER(mtmp)) {
		mcnt++;
		break;
	}

    if (!mcnt) {
	if (otmp)
	    strange_feeling(otmp, Hallucination ?
#if 0 /*JP*/
			    "You get the heebie jeebies." :
			    "You feel threatened.");
#else
			    "あなたは金鳥の夏でキンチョーした。" :
			    "あなたは恐怖でぞくぞくした。");
#endif
	return 1;
    } else {
	boolean woken = FALSE;

	cls();
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (!mclass || mtmp->data->mlet == mclass ||
		(mtmp->data == &mons[PM_LONG_WORM] && mclass == S_WORM_TAIL))
		    if (mtmp->mx > 0) {
		    	if (mclass && def_monsyms[mclass] == ' ')
				show_glyph(mtmp->mx,mtmp->my,
					detected_mon_to_glyph(mtmp));
			else
				show_glyph(mtmp->mx,mtmp->my,mon_to_glyph(mtmp));
			/* don't be stingy - display entire worm */
			if (mtmp->data == &mons[PM_LONG_WORM]) detect_wsegs(mtmp,0);
		    }
	    if (otmp && otmp->cursed &&
		(mtmp->msleeping || !mtmp->mcanmove)) {
		mtmp->msleeping = mtmp->mfrozen = 0;
		mtmp->mcanmove = 1;
		woken = TRUE;
	    }
	}
	display_self();
/*JP
	You("sense the presence of monsters.");
*/
	You("怪物の存在を嗅ぎつけた。");
	if (woken)
/*JP
	    pline("Monsters sense the presence of you.");
*/
	    pline("怪物はあなたの存在を嗅ぎつけた。");
	display_nhwindow(WIN_MAP, TRUE);
	docrt();
	if (Underwater) under_water(2);
	if (u.uburied) under_ground(2);
    }
    return 0;
}

STATIC_OVL void
sense_trap(trap, x, y, src_cursed)
struct trap *trap;
xchar x, y;
int src_cursed;
{
    if (Hallucination || src_cursed) {
	struct obj obj;			/* fake object */
	if (trap) {
	    obj.ox = trap->tx;
	    obj.oy = trap->ty;
	} else {
	    obj.ox = x;
	    obj.oy = y;
	}
	obj.otyp = (src_cursed) ? GOLD_PIECE : random_object();
	obj.corpsenm = random_monster();	/* if otyp == CORPSE */
	map_object(&obj,1);
    } else if (trap) {
	map_trap(trap,1);
	trap->tseen = 1;
    } else {
	struct trap temp_trap;		/* fake trap */
	temp_trap.tx = x;
	temp_trap.ty = y;
	temp_trap.ttyp = BEAR_TRAP;	/* some kind of trap */
	map_trap(&temp_trap,1);
    }

}

/* the detections are pulled out so they can	*/
/* also be used in the crystal ball routine	*/
/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
int
trap_detect(sobj)
register struct obj *sobj;
/* sobj is null if crystal ball, *scroll if gold detection scroll */
{
    register struct trap *ttmp;
    register struct obj *obj;
    register int door;
    int uw = u.uinwater;
    boolean found = FALSE;
    coord cc;

    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
	if (ttmp->tx != u.ux || ttmp->ty != u.uy)
	    goto outtrapmap;
	else found = TRUE;
    }
    for (obj = fobj; obj; obj = obj->nobj) {
	if ((obj->otyp == LARGE_BOX || obj->otyp == CHEST || obj->otyp == IRON_SAFE) && 
	    obj->otrapped) {
	    if (obj->ox != u.ux || obj->oy != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    for (door = 0; door < doorindex; door++) {
	cc = doors[door];
	if (levl[cc.x][cc.y].doormask & D_TRAPPED) {
	    if (cc.x != u.ux || cc.y != u.uy)
		goto outtrapmap;
	    else found = TRUE;
	}
    }
    if (!found) {
	char buf[42];
/*JP
	Sprintf(buf, "Your %s stop itching.", makeplural(body_part(TOE)));
*/
	Sprintf(buf, "あなたの%sのむずむずはおさまった。", makeplural(body_part(TOE)));
	strange_feeling(sobj,buf);
	return(1);
    }
    /* traps exist, but only under me - no separate display required */
/*JP
    Your("%s itch.", makeplural(body_part(TOE)));
*/
    Your("%sはむずむずした。", makeplural(body_part(TOE)));
    return(0);
outtrapmap:
    cls();

    u.uinwater = 0;
    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
	sense_trap(ttmp, 0, 0, sobj && sobj->cursed);

    for (obj = fobj; obj; obj = obj->nobj)
	if ((obj->otyp == LARGE_BOX || obj->otyp == CHEST || obj->otyp == IRON_SAFE) && 
	    obj->otrapped)
	sense_trap((struct trap *)0, obj->ox, obj->oy, sobj && sobj->cursed);

    for (door = 0; door < doorindex; door++) {
	cc = doors[door];
	if (levl[cc.x][cc.y].doormask & D_TRAPPED)
	sense_trap((struct trap *)0, cc.x, cc.y, sobj && sobj->cursed);
    }

    newsym(u.ux,u.uy);
/*JP
    You_feel("%s.", sobj && sobj->cursed ? "very greedy" : "entrapped");
*/
    You("%s気持になった。", sobj && sobj->cursed ? "とてもどん欲な" : "だまされているような");
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    u.uinwater = uw;
    if (Underwater) under_water(2);
    if (u.uburied) under_ground(2);
    return(0);
}

const char *
level_distance(where)
d_level *where;
{
    register schar ll = depth(&u.uz) - depth(where);
    register boolean indun = (u.uz.dnum == where->dnum);

    if (ll < 0) {
	if (ll < (-8 - rn2(3)))
/*JP
	    if (!indun)	return "far away";
*/
	    if (!indun)	return "はるか彼方に";
/*JP
	    else	return "far below";
*/
	    else	return "はるか下方に";
	else if (ll < -1)
/*JP
	    if (!indun)	return "away below you";
*/
	    if (!indun)	return "ずっと下方に";
/*JP
	    else	return "below you";
*/
	    else	return "下方に";
	else
/*JP
	    if (!indun)	return "in the distance";
*/
	    if (!indun)	return "遠くに";
/*JP
	    else	return "just below";
*/
	    else	return "真下に";
    } else if (ll > 0) {
	if (ll > (8 + rn2(3)))
/*JP
	    if (!indun)	return "far away";
*/
	    if (!indun)	return "はるか彼方に";
/*JP
	    else	return "far above";
*/
	    else	return "はるか上方に";
	else if (ll > 1)
/*JP
	    if (!indun)	return "away above you";
*/
	    if (!indun)	return "ずっと上方に";
/*JP
	    else	return "above you";
*/
	    else	return "上方に";
	else
/*JP
	    if (!indun)	return "in the distance";
*/
	    if (!indun)	return "遠くに";
/*JP
	    else	return "just above";
*/
	    else	return "真上に";
    } else
/*JP
	    if (!indun)	return "in the distance";
*/
	    if (!indun)	return "遠くに";
/*JP
	    else	return "near you";
*/
	    else	return "近くに";
}

static const struct {
    const char *what;
    d_level *where;
} level_detects[] = {
/*JP
  { "Delphi", &oracle_level },
*/
  { "デルファイ", &oracle_level },
/*JP
  { "Medusa's lair", &medusa_level },
*/
  { "メデューサの住みか", &medusa_level },
/*JP
  { "a castle", &stronghold_level },
*/
  { "城", &stronghold_level },
/*JP
  { "the Wizard of Yendor's tower", &wiz1_level },
*/
  { "イェンダーの魔法使いの塔", &wiz1_level },
};

void
use_crystal_ball(obj)
struct obj *obj;
{
    char ch;
    int oops;

    if (Blind) {
/*JP
	pline("Too bad you can't see %s.", the(xname(obj)));
*/
	pline("なんてことだ。%sを見ることができない。", the(xname(obj)));
	return;
    }
    oops = (rnd(obj->blessed ? 13 : 20) > ACURR(A_INT) || obj->cursed);
    if (oops && (obj->spe > 0)) {
	switch (rnd(obj->oartifact ? 4 : 5)) {
/*JP
	case 1 : pline("%s too much to comprehend!", Tobjnam(obj, "are"));
*/
	case 1 : pline("%sを覗いたが何のことだかさっぱりわからなかった！", xname(obj));
	    break;
/*JP
	case 2 : pline("%s you!", Tobjnam(obj, "confuse"));
*/
	case 2 : pline("%sを覗いてるとふらついてきた！", xname(obj));
	    make_confused(HConfusion + rnd(100),FALSE);
	    break;
	case 3 : if (!resists_blnd(&youmonst)) {
/*JP
		pline("%s your vision!", Tobjnam(obj, "damage"));
*/
		pline("%sがあなたの視界に迫ってきた。", xname(obj));
		make_blinded(Blinded + rnd(100),FALSE);
		if (!Blind) Your(vision_clears);
	    } else {
/*JP
		pline("%s your vision.", Tobjnam(obj, "assault"));
*/
		pline("%sがあなたの視界に迫ってきた。", xname(obj));
/*JP
		You("are unaffected!");
*/
		pline("しかし、あなたは影響を受けなかった！");
	    }
	    break;
/*JP
	case 4 : pline("%s your mind!", Tobjnam(obj, "zap"));
*/
	case 4 : pline("%sを覗いていると五感がおかしくなってきた！", xname(obj));
	    (void) make_hallucinated(HHallucination + rnd(100),FALSE,0L);
	    break;
/*JP
	case 5 : pline("%s!", Tobjnam(obj, "explode"));
*/
	case 5 : pline("%sは爆発した！", xname(obj));
	    useup(obj);
	    obj = 0;	/* it's gone */
/*JP
	    losehp(rnd(30), "exploding crystal ball", KILLED_BY_AN);
*/
	    losehp(rnd(30), "水晶玉の爆発で", KILLED_BY_AN);
	    break;
	}
	if (obj) consume_obj_charge(obj, TRUE);
	return;
    }

    if (Hallucination) {
	if (!obj->spe) {
/*JP
	    pline("All you see is funky %s haze.", hcolor((char *)0));
*/
	    pline("おお！ファンキーモンキーな%sもやが見える。", hcolor((char *)0));
	} else {
	    switch(rnd(6)) {
/*JP
	    case 1 : You("grok some groovy globs of incandescent lava.");
*/
	    case 1 : You("水門の鍵をもった水戸黄門が壁の影に隠れているのを見た。");
		break;
/*JP
	    case 2 : pline("Whoa!  Psychedelic colors, %s!",
*/
	    case 2 : pline("ワーオ！ラリってるかい？%s！",
/*JP
			   poly_gender() == 1 ? "babe" : "dude");
*/
			   poly_gender() == 1 ? "ベイビー" : "ユー");
		break;
/*JP
	    case 3 : pline_The("crystal pulses with sinister %s light!",
*/
	    case 3 : pline("水晶は不吉な%sパルスを発した！", 
				hcolor((char *)0));
		break;
/*JP
	    case 4 : You("see goldfish swimming above fluorescent rocks.");
*/
	    case 4 : You("蛍光岩の上を金魚が泳いでいるのを見た。");
		break;
/*JP
	    case 5 : You("see tiny snowflakes spinning around a miniature farmhouse.");
*/
	    case 5 : You("小さい雪片がミニチュアの農家の家のまわりを舞っているのを見た。");
		break;
/*JP
	    default: pline("Oh wow... like a kaleidoscope!");
*/
	    default: pline("ワーオ。万華鏡のようだ！");
		break;
	    }
	    consume_obj_charge(obj, TRUE);
	}
	return;
    }

    /* read a single character */
/*JP
    if (flags.verbose) You("may look for an object or monster symbol.");
*/
    if (flags.verbose) You("物体や怪物の記号を探せる。");
/*JP
    ch = yn_function("What do you look for?", (char *)0, '\0');
*/
    ch = yn_function("何を探しますか？", (char *)0, '\0');
    /* Don't filter out ' ' here; it has a use */
    if ((ch != def_monsyms[S_GHOST]) && index(quitchars,ch)) { 
	if (flags.verbose) pline(Never_mind);
	return;
    }
/*JP
    You("peer into %s...", the(xname(obj)));
*/
    You("%sを覗きこんだ．．．", the(xname(obj)));
/*JP
    nomul(-rnd(10), "gazing into a crystal ball");
*/
    nomul(-rnd(10), "水晶球に睨まれた隙に");
    nomovemsg = "";
    if (obj->spe <= 0)
/*JP
	pline_The("vision is unclear.");
*/
	pline("映像は不鮮明だった。");
    else {
	int class;
	int ret = 0;

	makeknown(CRYSTAL_BALL);
	consume_obj_charge(obj, TRUE);

	/* special case: accept ']' as synonym for mimic
	 * we have to do this before the def_char_to_objclass check
	 */
	if (ch == DEF_MIMIC_DEF) ch = DEF_MIMIC;

	if ((class = def_char_to_objclass(ch)) != MAXOCLASSES)
		ret = object_detect((struct obj *)0, class);
	else if ((class = def_char_to_monclass(ch)) != MAXMCLASSES)
		ret = monster_detect((struct obj *)0, class);
	else if (iflags.bouldersym && (ch == iflags.bouldersym))
		ret = object_detect((struct obj *)0, ROCK_CLASS);
	else switch(ch) {
		case '^':
		    ret = trap_detect((struct obj *)0);
		    break;
		default:
		    {
		    int i = rn2(SIZE(level_detects));
/*JP
		    You("see %s, %s.",
*/
		    You("%sを%s見た。",
			level_detects[i].what,
			level_distance(level_detects[i].where));
		    }
		    ret = 0;
		    break;
	}

	if (ret) {
	    if (!rn2(100))  /* make them nervous */
/*JP
		You("see the Wizard of Yendor gazing out at you.");
*/
		You("イェンダーの魔法使いがあなたをにらんでいるのが見えた。");
/*JP
	    else pline_The("vision is unclear.");
*/
	    else pline("映像は不鮮明になった。");
	}
    }
    return;
}

STATIC_OVL void
show_map_spot(x, y)
register int x, y;
{
    register struct rm *lev;

    if (Confusion && rn2(7)) return;
    lev = &levl[x][y];

    lev->seenv = SVALL;

    /* Secret corridors are found, but not secret doors. */
    if (lev->typ == SCORR) {
	lev->typ = CORR;
	unblock_point(x,y);
    }

    /* if we don't remember an object or trap there, map it */
    if (lev->typ == ROOM ?
	    (glyph_is_cmap(lev->glyph) && !glyph_is_trap(lev->glyph) &&
		glyph_to_cmap(lev->glyph) != ROOM) :
	    (!glyph_is_object(lev->glyph) && !glyph_is_trap(lev->glyph))) {
	if (level.flags.hero_memory) {
	    magic_map_background(x,y,0);
	    newsym(x,y);			/* show it, if not blocked */
	} else {
	    magic_map_background(x,y,1);	/* display it */
	}
    }
}

void
do_mapping()
{
    register int zx, zy;
    int uw = u.uinwater;

    u.uinwater = 0;
    for (zx = 1; zx < COLNO; zx++)
	for (zy = 0; zy < ROWNO; zy++)
	    show_map_spot(zx, zy);
    exercise(A_WIS, TRUE);
    u.uinwater = uw;
    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

void
do_vicinity_map()
{
    register int zx, zy;
    int lo_y = (u.uy-5 < 0 ? 0 : u.uy-5),
	hi_y = (u.uy+6 > ROWNO ? ROWNO : u.uy+6),
	lo_x = (u.ux-9 < 1 ? 1 : u.ux-9),	/* avoid column 0 */
	hi_x = (u.ux+10 > COLNO ? COLNO : u.ux+10);

    for (zx = lo_x; zx < hi_x; zx++)
	for (zy = lo_y; zy < hi_y; zy++)
	    show_map_spot(zx, zy);

    if (!level.flags.hero_memory || Underwater) {
	flush_screen(1);			/* flush temp screen */
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

/* convert a secret door into a normal door */
void
cvt_sdoor_to_door(lev)
struct rm *lev;
{
	int newmask = lev->doormask & ~WM_MASK;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz))
	    /* rogue didn't have doors, only doorways */
	    newmask = D_NODOOR;
	else
#endif
	    /* newly exposed door is closed */
	    if (!(newmask & D_LOCKED)) newmask |= D_CLOSED;

	lev->typ = DOOR;
	lev->doormask = newmask;
}


STATIC_PTR void
findone(zx,zy,num)
int zx,zy;
genericptr_t num;
{
	register struct trap *ttmp;
	register struct monst *mtmp;

	if(levl[zx][zy].typ == SDOOR) {
		cvt_sdoor_to_door(&levl[zx][zy]);	/* .typ = DOOR */
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if(levl[zx][zy].typ == SCORR) {
		levl[zx][zy].typ = CORR;
		unblock_point(zx,zy);
		magic_map_background(zx, zy, 0);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if ((ttmp = t_at(zx, zy)) != 0) {
		if(!ttmp->tseen && ttmp->ttyp != STATUE_TRAP) {
			ttmp->tseen = 1;
			newsym(zx,zy);
			(*(int*)num)++;
		}
	} else if ((mtmp = m_at(zx, zy)) != 0) {
		if(mtmp->m_ap_type) {
			seemimic(mtmp);
			(*(int*)num)++;
		}
		if (mtmp->mundetected &&
		    (is_hider(mtmp->data) || mtmp->data->mlet == S_EEL)) {
			mtmp->mundetected = 0;
			newsym(zx, zy);
			(*(int*)num)++;
		}
		if (!canspotmon(mtmp) &&
				    !glyph_is_invisible(levl[zx][zy].glyph))
			map_invisible(zx, zy);
	} else if (glyph_is_invisible(levl[zx][zy].glyph)) {
		unmap_object(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	}
}

STATIC_PTR void
openone(zx,zy,num)
int zx,zy;
genericptr_t num;
{
	register struct trap *ttmp;
	register struct obj *otmp;

	if(OBJ_AT(zx, zy)) {
		for(otmp = level.objects[zx][zy];
				otmp; otmp = otmp->nexthere) {
		    if(Is_box(otmp) && otmp->olocked) {
			otmp->olocked = 0;
			(*(int*)num)++;
		    }
		}
		/* let it fall to the next cases. could be on trap. */
	}
	if(levl[zx][zy].typ == SDOOR || (levl[zx][zy].typ == DOOR &&
		      (levl[zx][zy].doormask & (D_CLOSED|D_LOCKED)))) {
		if(levl[zx][zy].typ == SDOOR)
		    cvt_sdoor_to_door(&levl[zx][zy]);	/* .typ = DOOR */
		if(levl[zx][zy].doormask & D_TRAPPED) {
/*JP
		    if(distu(zx, zy) < 3) b_trapped("door", 0);
*/
		    if(distu(zx, zy) < 3) b_trapped("扉", 0);
#if 0 /*JP*/
		    else Norep("You %s an explosion!",
				cansee(zx, zy) ? "see" :
				   (flags.soundok ? "hear" :
						"feel the shock of"));
#else
		    else Norep("あなたは爆発%s！",
				cansee(zx, zy) ? "するのを見た" :
				   (flags.soundok ? "音を聞いた" :
						"の衝撃を感じた"));
#endif
		    wake_nearto(zx, zy, 11*11);
		    levl[zx][zy].doormask = D_NODOOR;
		} else
		    levl[zx][zy].doormask = D_ISOPEN;
		unblock_point(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if(levl[zx][zy].typ == SCORR) {
		levl[zx][zy].typ = CORR;
		unblock_point(zx, zy);
		newsym(zx, zy);
		(*(int*)num)++;
	} else if ((ttmp = t_at(zx, zy)) != 0) {
		if (!ttmp->tseen && ttmp->ttyp != STATUE_TRAP) {
		    ttmp->tseen = 1;
		    newsym(zx,zy);
		    (*(int*)num)++;
		}
	} else if (find_drawbridge(&zx, &zy)) {
		/* make sure it isn't an open drawbridge */
		open_drawbridge(zx, zy);
		(*(int*)num)++;
	}
}

int
findit()	/* returns number of things found */
{
	int num = 0;

	if(u.uswallow) return(0);
	do_clear_area(u.ux, u.uy, BOLT_LIM, findone, (genericptr_t) &num);
	return(num);
}

int
openit()	/* returns number of things found and opened */
{
	int num = 0;

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
/*JP
			if (Blind) pline("Its mouth opens!");
*/
			if (Blind) pline("何者かの口が開いた！");
/*JP
			else pline("%s opens its mouth!", Monnam(u.ustuck));
*/
			else pline("%sは口を開いた！", Monnam(u.ustuck));
		}
		expels(u.ustuck, u.ustuck->data, TRUE);
		return(-1);
	}

	do_clear_area(u.ux, u.uy, BOLT_LIM, openone, (genericptr_t) &num);
	return(num);
}

void
find_trap(trap)
struct trap *trap;
{
    int tt = what_trap(trap->ttyp);
    boolean cleared = FALSE;

    trap->tseen = 1;
    exercise(A_WIS, TRUE);
    if (Blind)
	feel_location(trap->tx, trap->ty);
    else
	newsym(trap->tx, trap->ty);

    if (levl[trap->tx][trap->ty].glyph != trap_to_glyph(trap)) {
    	/* There's too much clutter to see your find otherwise */
	cls();
	map_trap(trap, 1);
	display_self();
	cleared = TRUE;
    }

/*JP
    You("find %s.", an(defsyms[trap_to_defsym(tt)].explanation));
*/
    You("%sを見つけた。", jtrns_obj('^', defsyms[trap_to_defsym(tt)].explanation));

    if (cleared) {
	display_nhwindow(WIN_MAP, TRUE);	/* wait */
	docrt();
    }
}

int
dosearch0(aflag)
register int aflag;
{
#ifdef GCC_BUG
/* some versions of gcc seriously muck up nested loops. if you get strange
   crashes while searching in a version compiled with gcc, try putting
   #define GCC_BUG in *conf.h (or adding -DGCC_BUG to CFLAGS in the
   makefile).
 */
	volatile xchar x, y;
#else
	register xchar x, y;
#endif
	register struct trap *trap;
	register struct monst *mtmp;

	if(u.uswallow) {
		if (!aflag)
/*JP
			pline("What are you looking for?  The exit?");
*/
			pline("何を探すんだい？非常口？");
	} else {
	    int fund = (uwep && uwep->oartifact &&
		    spec_ability(uwep, SPFX_SEARCH)) ?
		    uwep->spe : 0;
	    if (ublindf && ublindf->otyp == LENSES && !Blind)
		    fund += 2; /* JDS: lenses help searching */
	    if (fund > 5) fund = 5;
	    for(x = u.ux-1; x < u.ux+2; x++)
	      for(y = u.uy-1; y < u.uy+2; y++) {
		if(!isok(x,y)) continue;
		if(x != u.ux || y != u.uy) {
		    if (Blind && !aflag) feel_location(x,y);
		    if(levl[x][y].typ == SDOOR) {
			if(rnl(7-fund)) continue;
			cvt_sdoor_to_door(&levl[x][y]);	/* .typ = DOOR */
			exercise(A_WIS, TRUE);
			nomul(0, 0);
			if (Blind && !aflag)
			    feel_location(x,y);	/* make sure it shows up */
			else
			    newsym(x,y);
		    } else if(levl[x][y].typ == SCORR) {
			if(rnl(7-fund)) continue;
			levl[x][y].typ = CORR;
			unblock_point(x,y);	/* vision */
			exercise(A_WIS, TRUE);
			nomul(0, 0);
			newsym(x,y);
		    } else {
		/* Be careful not to find anything in an SCORR or SDOOR */
			if((mtmp = m_at(x, y)) && !aflag) {
			    if(mtmp->m_ap_type) {
				seemimic(mtmp);
		find:		exercise(A_WIS, TRUE);
				if (!canspotmon(mtmp)) {
				    if (glyph_is_invisible(levl[x][y].glyph)) {
					/* found invisible monster in a square
					 * which already has an 'I' in it.
					 * Logically, this should still take
					 * time and lead to a return(1), but if
					 * we did that the player would keep
					 * finding the same monster every turn.
					 */
					continue;
				    } else {
/*JP
					You_feel("an unseen monster!");
*/
					You("見えない怪物の気配を感じた！");
					map_invisible(x, y);
				    }
				} else if (!sensemon(mtmp))
/*JP
				    You("find %s.", a_monnam(mtmp));
*/
				    You("%sを見つけた。", a_monnam(mtmp));
				return(1);
			    }
			    if(!canspotmon(mtmp)) {
				if (mtmp->mundetected &&
				   (is_hider(mtmp->data) || mtmp->data->mlet == S_EEL))
					mtmp->mundetected = 0;
				newsym(x,y);
				goto find;
			    }
			}

			/* see if an invisible monster has moved--if Blind,
			 * feel_location() already did it
			 */
			if (!aflag && !mtmp && !Blind &&
				    glyph_is_invisible(levl[x][y].glyph)) {
			    unmap_object(x,y);
			    newsym(x,y);
			}

			if ((trap = t_at(x,y)) && !trap->tseen && !rnl(8)) {
			    nomul(0, 0);

			    if (trap->ttyp == STATUE_TRAP) {
				if (activate_statue_trap(trap, x, y, FALSE))
				    exercise(A_WIS, TRUE);
				return(1);
			    } else {
				find_trap(trap);
			    }
			}
		    }
		}
	    }
	}
	return(1);
}

int
dosearch()
{
	return(dosearch0(0));
}

/* Pre-map the sokoban levels */
void
sokoban_detect()
{
	register int x, y;
	register struct trap *ttmp;
	register struct obj *obj;

	/* Map the background and boulders */
	for (x = 1; x < COLNO; x++)
	    for (y = 0; y < ROWNO; y++) {
		if (IS_WALL(levl[x][y].typ))
		    levl[x][y].seenv = SVALL;
		else if (levl[x][y].typ == SDOOR)
		    levl[x][y].typ = DOOR;
		else if (levl[x][y].typ == SCORR)
		    levl[x][y].typ = CORR;

		/* all Sokoban floors only shown lit when dark_room is deactivated */
		levl[x][y].waslit = (levl[x][y].typ != CORR) ? (!iflags.dark_room) : levl[x][y].lit;
	    	map_background(x, y, 1);
	    	for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
	    	    if (obj->otyp == BOULDER)
	    	    	map_object(obj, 1);
	    }

	/* Map the traps */
	for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
	    ttmp->tseen = 1;
	    map_trap(ttmp, 1);
	}
}


/*detect.c*/
