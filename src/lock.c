/*	SCCS Id: @(#)lock.c	3.4	2000/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2003
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"

STATIC_PTR int NDECL(picklock);
STATIC_PTR int NDECL(forcelock);

/* at most one of `door' and `box' should be non-null at any given time */
STATIC_VAR NEARDATA struct xlock_s {
	struct rm  *door;
	struct obj *box;
	int picktyp, chance, usedtime;
} xlock;

#ifdef OVLB

STATIC_DCL const char *NDECL(lock_action);
STATIC_DCL boolean FDECL(obstructed,(int,int));
STATIC_DCL void FDECL(chest_shatter_msg, (struct obj *));

boolean
picking_lock(x, y)
	int *x, *y;
{
	if (occupation == picklock) {
	    *x = u.ux + u.dx;
	    *y = u.uy + u.dy;
	    return TRUE;
	} else {
	    *x = *y = 0;
	    return FALSE;
	}
}

boolean
picking_at(x, y)
int x, y;
{
	return (boolean)(occupation == picklock && xlock.door == &levl[x][y]);
}

/* produce an occupation string appropriate for the current activity */
STATIC_OVL const char *
lock_action()
{
	/* "unlocking"+2 == "locking" */
	static const char *actions[] = {
/*
** �p��� un �����邾���ŋt�̈Ӗ��ɂȂ邪�C���{��͂����͂����Ȃ��D
** �N���H����Ȑ��o�C�g������R�[�h��������́H
*/
#if 0 /*JP*/
		/* [0] */	"unlocking the door",
		/* [1] */	"unlocking the chest",
		/* [2] */	"unlocking the box",
		/* [3] */	"cracking the safe",
		/* [4] */	"picking the lock"
#else
		/* [0] */	"���̌����͂���", 
		/* [1] */	"�󔠂̌����͂���",
		/* [2] */	"���̌����͂���",  
		/* [3] */	"���ɂ�j��"    
		/* [4] */	"���O�������J����"    
#endif /*JP*/
	};

	/* if the target is currently unlocked, we're trying to lock it now */
	if (xlock.door && !(xlock.door->doormask & D_LOCKED))
#if 0 /*JP*/
		return actions[0]+2;	/* "locking the door" */
#else
		return "���Ɍ���������";	/* "locking the door" */
#endif
	else if (xlock.box && !xlock.box->olocked)
#if 0 /*JP*/
		return xlock.box->otyp == CHEST ? actions[1]+2 :
		        xlock.box->otyp == IRON_SAFE ? actions[3]+2 : actions[2]+2;
#else
		return xlock.box->otyp == CHEST ? "�󔠂Ɍ���������" :
		        xlock.box->otyp == IRON_SAFE ? "���ɂɌ���������" : "���Ɍ���������";
#endif
	/* otherwise we're trying to unlock it */
	else if (xlock.picktyp == LOCK_PICK)
		return actions[4];	/* "picking the lock" */
#ifdef TOURIST
	else if (xlock.picktyp == CREDIT_CARD)
		return actions[4];	/* same as lock_pick */
#endif
	else if (xlock.door)
		return actions[0];	/* "unlocking the door" */
	else
		return xlock.box->otyp == CHEST ? actions[1] : 
		        xlock.box->otyp == IRON_SAFE ? actions[3] : actions[2];
}

STATIC_PTR
int
picklock()	/* try to open/close a lock */
{

	if (xlock.box) {
	    if(((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy)) &&
				 (xlock.box->otyp != IRON_SAFE || abs(xlock.box->oy - u.uy) > 1 ||
				  abs(xlock.box->ox - u.ux) > 1)) {
		return((xlock.usedtime = 0));		/* you or it moved */
	    }
	} else {		/* door */
	    if(xlock.door != &(levl[u.ux+u.dx][u.uy+u.dy])) {
		return((xlock.usedtime = 0));		/* you moved */
	    }
	    switch (xlock.door->doormask) {
		case D_NODOOR:
/*JP
		    pline("This doorway has no door.");
*/
		    pline("�o�����ɂ͔����Ȃ��D");
		    return((xlock.usedtime = 0));
		case D_ISOPEN:
/*JP
		    You("cannot lock an open door.");
*/
		    pline("�J���Ă���Ɍ����������Ȃ��D");
		    return((xlock.usedtime = 0));
		case D_BROKEN:
/*JP
		    pline("This door is broken.");
*/
		    pline("���͉��Ă���D");
		    return((xlock.usedtime = 0));
	    }
	}

	if (xlock.usedtime++ >= 50 || nohands(youmonst.data)) {
/*JP
	    You("give up your attempt at %s.", lock_action());
*/
	    pline("%s�̂�������߂��D", lock_action());
	    exercise(A_DEX, TRUE);	/* even if you don't succeed */
	    return((xlock.usedtime = 0));
	}

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */

/*JP
	You("succeed in %s.", lock_action());
*/
	You("%s�̂ɐ��������D", lock_action());
	if (xlock.door) {
	    if(xlock.door->doormask & D_TRAPPED) {
/*JP
		    b_trapped("door", FINGER);
*/
		    b_trapped("��", FINGER);
		    xlock.door->doormask = D_NODOOR;
		    unblock_point(u.ux+u.dx, u.uy+u.dy);
		    if (*in_rooms(u.ux+u.dx, u.uy+u.dy, SHOPBASE))
			add_damage(u.ux+u.dx, u.uy+u.dy, 0L);
		    newsym(u.ux+u.dx, u.uy+u.dy);
	    } else if (xlock.door->doormask & D_LOCKED)
		xlock.door->doormask = D_CLOSED;
	    else xlock.door->doormask = D_LOCKED;
	} else {
	    xlock.box->olocked = !xlock.box->olocked;
	    if(xlock.box->otrapped)	
		(void) chest_trap(xlock.box, FINGER, FALSE);
	}
	exercise(A_DEX, TRUE);
	return((xlock.usedtime = 0));
}

STATIC_PTR
int
forcelock()	/* try to force a locked chest */
{

	register struct obj *otmp;

	if((xlock.box->ox != u.ux) || (xlock.box->oy != u.uy))
		return((xlock.usedtime = 0));		/* you or it moved */

	if (xlock.usedtime++ >= 50 || !uwep || nohands(youmonst.data)) {
/*JP
	    You("give up your attempt to force the lock.");
*/
	    pline("���������J����̂�������߂��D");
	    if(xlock.usedtime >= 50)		/* you made the effort */
	      exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	    return((xlock.usedtime = 0));
	}

	if(xlock.picktyp) {	/* blade */

	    if(rn2(1000-(int)uwep->spe) > (992-greatest_erosion(uwep)*10) &&
	       !uwep->cursed && !obj_resists(uwep, 0, 99)) {
		/* for a +0 weapon, probability that it survives an unsuccessful
		 * attempt to force the lock is (.992)^50 = .67
		 */
#if 0 /*JP*/
		pline("%sour %s broke!",
		      (uwep->quan > 1L) ? "One of y" : "Y", xname(uwep));
#else
		pline("%s�͉��Ă��܂����I",xname(uwep));
#endif
		useup(uwep);
/*JP
		You("give up your attempt to force the lock.");
*/
		pline("���������J����̂�������߂��D");
		exercise(A_DEX, TRUE);
		return((xlock.usedtime = 0));
	    }
	} else			/* blunt */
	    wake_nearby();	/* due to hammering on the container */

	if(rn2(100) >= xlock.chance) return(1);		/* still busy */

/*JP
	You("succeed in forcing the lock.");
*/
	pline("���������J�����D");
	xlock.box->olocked = 0;
	xlock.box->obroken = 1;
	if(!xlock.picktyp && !rn2(3)) {
	    struct monst *shkp;
	    boolean costly;
	    long loss = 0L;

	    costly = (*u.ushops && costly_spot(u.ux, u.uy));
	    shkp = costly ? shop_keeper(*u.ushops) : 0;

#if 0 /*JP*/
	    pline("In fact, you've totally destroyed %s.",
		  the(xname(xlock.box)));
#else
	    pline("���ۂ̂Ƃ���C%s�����S�ɉ󂵂Ă��܂����D",
		  xname(xlock.box));
#endif

	    /* Put the contents on ground at the hero's feet. */
	    while ((otmp = xlock.box->cobj) != 0) {
		obj_extract_self(otmp);
		if(!rn2(3) || otmp->oclass == POTION_CLASS) {
		    chest_shatter_msg(otmp);
		    if (costly)
		        loss += stolen_value(otmp, u.ux, u.uy,
					     (boolean)shkp->mpeaceful, TRUE);
		    if (otmp->quan == 1L) {
			obfree(otmp, (struct obj *) 0);
			continue;
		    }
		    useup(otmp);
		}
		if (xlock.box->otyp == ICE_BOX && otmp->otyp == CORPSE) {
		    otmp->age = monstermoves - otmp->age; /* actual age */
		    start_corpse_timeout(otmp);
		}
		place_object(otmp, u.ux, u.uy);
		stackobj(otmp);
	    }

	    if (costly)
		loss += stolen_value(xlock.box, u.ux, u.uy,
					     (boolean)shkp->mpeaceful, TRUE);
/*JP
	    if(loss) You("owe %ld %s for objects destroyed.", loss, currency(loss));
*/
	    if(loss) You("�함�j����%ld%s�̎؂���������D", loss, currency(loss));
	    delobj(xlock.box);
	}
	exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
	return((xlock.usedtime = 0));
}

#endif /* OVLB */
#ifdef OVL0

void
reset_pick()
{
	xlock.usedtime = xlock.chance = xlock.picktyp = 0;
	xlock.door = 0;
	xlock.box = 0;
}

#endif /* OVL0 */
#ifdef OVLB

int
pick_lock(pick,rx,ry) /* pick a lock with a given object */
register struct	obj	*pick;
int rx,ry;
{
	/* rx and ry are passed only from the use-stethoscope stuff */
	int picktyp, c, ch;
	coord cc;
	struct rm	*door;
	struct obj	*otmp;
	char qbuf[QBUFSZ];

	picktyp = pick->otyp;

	/* check whether we're resuming an interrupted previous attempt */
	if (xlock.usedtime && picktyp == xlock.picktyp) {
/*JP
	    static char no_longer[] = "Unfortunately, you can no longer %s %s.";
*/
	    static char no_longer[] = "����˂�Ȃ���C���Ȃ���%s%s";
	    if (nohands(youmonst.data)) {
/*JP
		const char *what = (picktyp == LOCK_PICK) ? "pick" : "key";
*/
		const char *what = (picktyp == LOCK_PICK) ? "���J�����" : "��";
#ifdef TOURIST
/*JP
		if (picktyp == CREDIT_CARD) what = "card";
*/
		if (picktyp == CREDIT_CARD) what = "�J�[�h";
#endif
/*JP
		if (picktyp == STETHOSCOPE) what = "stethoscope";
*/
		if (picktyp == STETHOSCOPE) what = "���f��";

/*JP
		pline(no_longer, "hold the", what);
*/
		pline(no_longer, what, "�����߂Ȃ�");
		reset_pick();
		return 0;
	    } else if (xlock.box && !can_reach_floor()) {
/*JP
		pline(no_longer, "reach the", "lock");
*/
		pline(no_longer, "����", "�͂��Ȃ�");
		reset_pick();
		return 0;
	    } else {
		const char *action = lock_action();
/*JP
		You("resume your attempt at %s.", action);
*/
		pline("%s�̂��ĊJ�����D", action);
		set_occupation(picklock, action, 0);
		return(1);
	    }
	}

	if(nohands(youmonst.data)) {
/*JP
		You_cant("hold %s -- you have no hands!", doname(pick));
*/
		You("%s�����ނ��Ƃ��ł��Ȃ��I�肪�Ȃ��񂾂��́I", xname(pick));
		return(0);
	}

	if ((picktyp != LOCK_PICK && picktyp != STETHOSCOPE &&
#ifdef TOURIST
	    picktyp != CREDIT_CARD &&
#endif
	    picktyp != SKELETON_KEY)) {
		impossible("picking lock with object %d?", picktyp);
		return(0);
	}
	ch = 0;		/* lint suppression */

	/* If this is a stethoscope, we know where we came from */
	if (picktyp == STETHOSCOPE) {
		cc.x = rx; cc.y = ry;
	} else {
/*JP
		if(!get_adjacent_loc((char *)0, "Invalid location!", u.ux, u.uy, &cc)) return 0;
*/
		if(!get_adjacent_loc((char *)0, "�ʒu�����������I", u.ux, u.uy, &cc)) return 0;
	}

	/* Very clumsy special case for this, but forcing the player to
	 * a)pply > just to open a safe, when a)pply . works in all other cases? */
	if ((cc.x == u.ux && cc.y == u.uy) || picktyp == STETHOSCOPE) {	/* pick lock on a container */
	    const char *verb;
	    boolean it;
	    int count;

	    if (u.dz < 0) {
#if 0 /*JP*/
		There("isn't any sort of lock up %s.",
		      Levitation ? "here" : "there");
#else
		pline("%s�ɂ͌���������悤�ȕ��͂Ȃ��D",
		      Levitation ? "����" : "����");
#endif
		return 0;
	    } else if (is_lava(u.ux, u.uy)) {
/*JP
		pline("Doing that would probably melt your %s.",
*/
		pline("����Ȃ��Ƃ�������%s���n���Ă��܂��D",
		      xname(pick));
		return 0;
	    } else if (is_pool(u.ux, u.uy) && !Underwater) {
/*JP
		pline_The("water has no lock.");
*/
		pline("���ɏ��O�͂Ȃ��D");
		return 0;
	    }

	    count = 0;
	    c = 'n';			/* in case there are no boxes here */
	    for(otmp = level.objects[cc.x][cc.y]; otmp; otmp = otmp->nexthere)
		if (Is_box(otmp)) {
		    ++count;
		    if (!can_reach_floor()) {
/*JP
			You_cant("reach %s from up here.", the(xname(otmp)));
*/
			You("�����ɂ���%s�ɓ͂��Ȃ��D", the(xname(otmp)));
			return 0;
		    }
		    it = 0;
#if 0 /*JP*/
		    if (otmp->obroken) verb = "fix";
		    else if (otmp->otyp == IRON_SAFE) verb = "crack", it = 1;
		    else if (!otmp->olocked) verb = "lock", it = 1;
		    else if (picktyp != LOCK_PICK) verb = "unlock", it = 1;
		    else verb = "pick";
#else
		    if (otmp->obroken) verb = "�C������";
		    else if (otmp->otyp == IRON_SAFE) verb = "�j��", it = 1;
		    else if (!otmp->olocked) "����������", it = 1;
		    else if (picktyp != LOCK_PICK) verb = "�����͂���", it = 1;
		    else verb = "�����J����";
#endif
#if 0 /*JP*/
		    Sprintf(qbuf, "There is %s here, %s %s?",
		            safe_qbuf("", sizeof("There is  here, unlock its lock?"),
		            doname(otmp), an(simple_typename(otmp->otyp)), "a box"),
		            verb, it ? "it" : "its lock");
#else
		    Sprintf(qbuf, "�����ɂ�%s������D%s�H",
		            safe_qbuf("", sizeof("�����ɂ͂�����D�����͂����܂����H"),
		            doname(otmp), an(simple_typename(otmp->otyp)), "��"),
		            verb);
#endif

		    c = ynq(qbuf);
		    if(c == 'q') return(0);
		    if(c == 'n') continue;

			if (otmp->otyp == IRON_SAFE && picktyp != STETHOSCOPE) {
/*JP
				You("aren't sure how to go about opening the safe that way.");
*/
				You("���ɂ̊J����m���ȕ��@�͂킩��Ȃ������D");
				return 0;
			}
			 if (!otmp->olocked && otmp->otyp == IRON_SAFE) {
/*JP
				 You_cant("change the combination.");
*/
				 You_cant("���̑g�ݍ��킹��ύX�ł��Ȃ��D");
				 return 0;
			 }
		    if (otmp->obroken) {
/*JP
				You_cant("fix its broken lock with %s.", doname(pick));
*/
				You("��ꂽ����%s�ŏC���ł��Ȃ��D", doname(pick));
				return 0;
		    }
#ifdef TOURIST
		    else if (picktyp == CREDIT_CARD && !otmp->olocked) {
			/* credit cards are only good for unlocking */
/*JP
			You_cant("do that with %s.", doname(pick));
*/
			pline("%s���Ⴛ��Ȃ��Ƃ͂ł��Ȃ��D", doname(pick));
			return 0;
		    }
#endif
		    switch(picktyp) {
#ifdef TOURIST
			case CREDIT_CARD:
			    ch = ACURR(A_DEX) + 20*Role_if(PM_ROGUE);
			    break;
#endif
			case LOCK_PICK:
			    ch = 4*ACURR(A_DEX) + 25*Role_if(PM_ROGUE);
			    break;
			case SKELETON_KEY:
			    ch = 75 + ACURR(A_DEX);
			    break;
			case STETHOSCOPE:
			   ch = 5 + 2*ACURR(A_DEX)*Role_if(PM_ROGUE);
			   break;
			default:	ch = 0;
		    }
		    if(otmp->cursed) ch /= 2;

		    xlock.picktyp = picktyp;
		    xlock.box = otmp;
		    xlock.door = 0;
		    break;
		}
	    if (c != 'y') {
		if (!count)
/*JP
			There("doesn't seem to be any sort of pickable lock here.");
*/
			pline("�����ɂ͌���������悤�ȕ��͂Ȃ��悤���D");
		return(0);		/* decided against all boxes */
	    }
	} else {			/* pick the lock in a door */
	    struct monst *mtmp;

	    if (u.utrap && u.utraptype == TT_PIT) {
/*JP
		You_cant("reach over the edge of the pit.");
*/
		pline("�������̒�����͂��Ȃ��D");
		return(0);
	    }

	    door = &levl[cc.x][cc.y];
	    if ((mtmp = m_at(cc.x, cc.y)) && canseemon(mtmp)
			&& mtmp->m_ap_type != M_AP_FURNITURE
			&& mtmp->m_ap_type != M_AP_OBJECT) {
#ifdef TOURIST
		if (picktyp == CREDIT_CARD &&
		    (mtmp->isshk || mtmp->data == &mons[PM_ORACLE]))
/*JP
		    verbalize("No checks, no credit, no problem.");
*/
		    verbalize("�����j�R�j�R���������D");
		else
#endif
/*JP
		    pline("I don't think %s would appreciate that.", mon_nam(mtmp));
*/
		    pline("%s�����̉��l��F�߂�Ƃ͎v���Ȃ��D", mon_nam(mtmp));
		return(0);
	    }
	    if(!IS_DOOR(door->typ)) {
		if (is_drawbridge_wall(cc.x,cc.y) >= 0)
#if 0 /*JP*/
		    You("%s no lock on the drawbridge.",
				Blind ? "feel" : "see");
#else
		    pline("���ˋ��ɂ͌����Ȃ�%s�D",
				Blind ? "�悤��" : "�悤�Ɍ�����");
#endif
		else
#if 0 /*JP*/
		    You("%s no door there.",
				Blind ? "feel" : "see");
#else
		    pline("�����ɂ͔����Ȃ�%s�D",
				Blind ? "�悤��" : "�悤�Ɍ�����");
#endif
		return(0);
	    }
	    switch (door->doormask) {
		case D_NODOOR:
/*JP
		    pline("This doorway has no door.");
*/
	            pline("�o�����ɂ͔����Ȃ��D");
		    return(0);
		case D_ISOPEN:
/*JP
		    You("cannot lock an open door.");
*/
		    pline("�J���Ă���Ɍ���������Ȃ��D");
		    return(0);
		case D_BROKEN:
/*JP
		    pline("This door is broken.");
*/
		    pline("���͉��Ă���D");
		    return(0);
		default:
#ifdef TOURIST
		    /* credit cards are only good for unlocking */
		    if(picktyp == CREDIT_CARD && !(door->doormask & D_LOCKED)) {
/*JP
			You_cant("lock a door with a credit card.");
*/
		        You("�N���W�b�g�J�[�h�Ō��������邱�Ƃ͂ł��Ȃ��D");
			return(0);
		    }
#endif

#if 0 /*JP*/
		    Sprintf(qbuf,"%sock it?",
			(door->doormask & D_LOCKED) ? "Unl" : "L" );
#else
		    Sprintf(qbuf,"%s�܂����H",
			(door->doormask & D_LOCKED) ? "�͂���" : "����" );
#endif

		    c = yn(qbuf);
		    if(c == 'n') return(0);

		    switch(picktyp) {
#ifdef TOURIST
			case CREDIT_CARD:
			    ch = 2*ACURR(A_DEX) + 20*Role_if(PM_ROGUE);
			    break;
#endif
			case LOCK_PICK:
			    ch = 3*ACURR(A_DEX) + 30*Role_if(PM_ROGUE);
			    break;
			case SKELETON_KEY:
			    ch = 70 + ACURR(A_DEX);
			    break;
			default:    ch = 0;
		    }
		    xlock.door = door;
		    xlock.box = 0;
	    }
	}
	flags.move = 0;
	xlock.chance = ch;
	xlock.picktyp = picktyp;
	xlock.usedtime = 0;
	set_occupation(picklock, lock_action(), 0);
	return(1);
}

int
doforce()		/* try to force a chest with your weapon */
{
	register struct obj *otmp;
	register int c, picktyp;
	char qbuf[QBUFSZ];

	if(!uwep ||	/* proper type test */
	   (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) &&
	    uwep->oclass != ROCK_CLASS) ||
	   (objects[uwep->otyp].oc_skill < P_DAGGER) ||
	   (objects[uwep->otyp].oc_skill > P_LANCE) ||
	   uwep->otyp == FLAIL || uwep->otyp == AKLYS
#ifdef KOPS
	   || uwep->otyp == RUBBER_HOSE
#endif
	  ) {
#if 0 /*JP*/
	    You_cant("force anything without a %sweapon.",
		  (uwep) ? "proper " : "");
#else
	    pline("%s����Ȃ��Ō������������邱�Ƃ͂ł��Ȃ��D",
		  (uwep) ? "�K�؂�" : "");
#endif
	    return(0);
	}

	picktyp = is_blade(uwep);
	if(xlock.usedtime && xlock.box && picktyp == xlock.picktyp) {
/*JP
	    You("resume your attempt to force the lock.");
*/
	    pline("��������������̂��ĊJ�����D");
/*JP
	    set_occupation(forcelock, "forcing the lock", 0);
*/
	    set_occupation(forcelock, "��������������", 0);
	    return(1);
	}

	/* A lock is made only for the honest man, the thief will break it. */
	xlock.box = (struct obj *)0;
	for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere)
	    if(Is_box(otmp)) {
			 if (otmp->otyp == IRON_SAFE) {
/*JP
				 You("would need dynamite to force %s.", the(xname(otmp)));
*/
				 pline("���Ȃ���%s������������ɂ̓_�C�i�}�C�g���K�v���낤�D", the(xname(otmp)));
				 continue;
			 }
		if (otmp->obroken || !otmp->olocked) {
#if 0 /*JP*/
		    There("is %s here, but its lock is already %s.",
			  doname(otmp), otmp->obroken ? "broken" : "unlocked");
#else
		    pline("�����ɂ�%s������C���������̌��͂���%s�D",
			  doname(otmp), otmp->obroken ? "���Ă���" : "�͂�����Ă���");
#endif
		    continue;
		}
#if 0 /*JP*/
		Sprintf(qbuf,"There is %s here, force its lock?",
			safe_qbuf("", sizeof("There is  here, force its lock?"),
				doname(otmp), an(simple_typename(otmp->otyp)),
				"a box"));
#else
		Sprintf(qbuf,"�����ɂ�%s������C�������������܂����H",
			safe_qbuf("", sizeof("�����ɂ͂�����C�������������܂����H"),
				doname(otmp), an(simple_typename(otmp->otyp)),
				"��"));
#endif

		c = ynq(qbuf);
		if(c == 'q') return(0);
		if(c == 'n') continue;

		if(picktyp)
/*JP
		    You("force your %s into a crack and pry.", xname(uwep));
*/
		    You("%s�������ɓ���ăK�`���K�`�������D",xname(uwep));
		else
/*JP
		    You("start bashing it with your %s.", xname(uwep));
*/
		    pline("%s�ŉ�������D", xname(uwep));
		xlock.box = otmp;
		xlock.chance = objects[uwep->otyp].oc_wldam * 2;
		xlock.picktyp = picktyp;
		xlock.usedtime = 0;
		break;
	    }

/*JP
	if(xlock.box)	set_occupation(forcelock, "forcing the lock", 0);
*/
	if(xlock.box)	set_occupation(forcelock, "��������������", 0);
/*JP
	else		You("decide not to force the issue.");
*/
	else		pline("����͖��Ӗ��ȍs�ׂ��D");
	return(1);
}

int
doopen()		/* try to open a door */
{
#ifdef AUTO_OPEN
	return doopen_indir(0, 0);
}

int
doopen_indir(x, y)		/* try to open a door in direction u.dx/u.dy */
	int x, y;		/* if true, prompt for direction */
{
#endif /* AUTO_OPEN */
	coord cc;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data)) {
/*JP
	    You_cant("open anything -- you have no hands!");
*/
	    You("�����J���邱�Ƃ��ł��Ȃ��I�肪�Ȃ��񂾂��́I");
	    return 0;
	}

	if (u.utrap && u.utraptype == TT_PIT) {
/*JP
	    You_cant("reach over the edge of the pit.");
*/
	    pline("�������̒�����͂��Ȃ��D");
	    return 0;
	}

#ifdef AUTO_OPEN
	if (x > 0 && y > 0) {
	    cc.x = x;
	    cc.y = y;
	}
	else
#endif
	if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return(0);

	if((cc.x == u.ux) && (cc.y == u.uy)) return(0);

	if ((mtmp = m_at(cc.x,cc.y))			&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[cc.x][cc.y];

	if(!IS_DOOR(door->typ)) {
		if (is_db_wall(cc.x,cc.y)) {
/*JP
		    There("is no obvious way to open the drawbridge.");
*/
		    pline("���ˋ����~�낷�����ȕ��@�͂Ȃ��D");
		    return(0);
		}
#if 0 /*JP*/
		You("%s no door there.",
				Blind ? "feel" : "see");
#else
		pline("�����ɂ͔��͂Ȃ�%s�D",
				Blind ? "�悤��" : "�悤�Ɍ�����");
#endif
		return(0);
	}

	if (!(door->doormask & D_CLOSED)) {
#if 0 /*JP*/
	    const char *mesg;

	    switch (door->doormask) {
	    case D_BROKEN: mesg = " is broken"; break;
	    case D_NODOOR: mesg = "way has no door"; break;
	    case D_ISOPEN: mesg = " is already open"; break;
	    default:	   mesg = " is locked"; break;
	    }
	    pline("This door%s.", mesg);
#else
	    switch(door->doormask) {
		case D_BROKEN:
		  pline("���͉��Ă���D"); 
		  break;
		case D_NODOOR:
		  pline("�o�����ɂ͔����Ȃ��D");
		  break;
		case D_ISOPEN:
		  pline("���͂����J���Ă���D");
		  break;
		default:
		  pline("���ɂ͌����|�����Ă���D"); 
		  break;
	    }
#endif
	    if (Blind) feel_location(cc.x,cc.y);
	    return(0);
	}

	if(verysmall(youmonst.data)) {
/*JP
	    pline("You're too small to pull the door open.");
*/
	    You("���������Ĕ����J�����Ȃ��D");
	    return(0);
	}

	/* door is known to be CLOSED */
	if (rnl(20) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
/*JP
	    pline_The("door opens.");
*/
	    pline("���͊J�����D");
	    if(door->doormask & D_TRAPPED) {
/*JP
		b_trapped("door", FINGER);
*/
		b_trapped("��", FINGER);
		door->doormask = D_NODOOR;
		if (*in_rooms(cc.x, cc.y, SHOPBASE)) add_damage(cc.x, cc.y, 0L);
	    } else
		door->doormask = D_ISOPEN;
	    if (Blind)
		feel_location(cc.x,cc.y);	/* the hero knows she opened it  */
	    else
		newsym(cc.x,cc.y);
	    unblock_point(cc.x,cc.y);		/* vision: new see through there */
	} else {
	    exercise(A_STR, TRUE);
/*JP
	    pline_The("door resists!");
*/
	    pline("�Ȃ��Ȃ��J���Ȃ��I");
	}

	return(1);
}

STATIC_OVL
boolean
obstructed(x,y)
register int x, y;
{
	register struct monst *mtmp = m_at(x, y);

	if(mtmp && mtmp->m_ap_type != M_AP_FURNITURE) {
		if (mtmp->m_ap_type == M_AP_OBJECT) goto objhere;
#if 0 /*JP*/
		pline("%s stands in the way!", !canspotmon(mtmp) ?
			"Some creature" : Monnam(mtmp));
#else
		pline("%s�������ӂ������Ă���D", !canspotmon(mtmp) ?
			"���҂�" : Monnam(mtmp));
#endif
		if (!canspotmon(mtmp))
		    map_invisible(mtmp->mx, mtmp->my);
		return(TRUE);
	}
	if (OBJ_AT(x, y)) {
/*JP
objhere:	pline("%s's in the way.", Something);
*/
objhere:	pline("�������o�����ɂ���D");
		return(TRUE);
	}
	return(FALSE);
}

int
doclose()		/* try to close a door */
{
	register int x, y;
	register struct rm *door;
	struct monst *mtmp;

	if (nohands(youmonst.data)) {
/*JP
	    You_cant("close anything -- you have no hands!");
*/
	    You("�߂邱�Ƃ��ł��Ȃ��I�肪�Ȃ��񂾂��́I");
	    return 0;
	}

	if (u.utrap && u.utraptype == TT_PIT) {
/*JP
	    You_cant("reach over the edge of the pit.");
*/
	    pline("�������̒�����͂��Ȃ��D");
	    return 0;
	}

	if(!getdir((char *)0)) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	if((x == u.ux) && (y == u.uy)) {
/*JP
		You("are in the way!");
*/
		pline("���Ȃ����o�����ɂ���̂ŕ܂�Ȃ��D");
		return(1);
	}

	if ((mtmp = m_at(x,y))				&&
		mtmp->m_ap_type == M_AP_FURNITURE	&&
		(mtmp->mappearance == S_hcdoor ||
			mtmp->mappearance == S_vcdoor)	&&
		!Protection_from_shape_changers)	 {

	    stumble_onto_mimic(mtmp);
	    return(1);
	}

	door = &levl[x][y];

	if(!IS_DOOR(door->typ)) {
		if (door->typ == DRAWBRIDGE_DOWN)
/*JP
		    There("is no obvious way to close the drawbridge.");
*/
		    pline("���ˋ����グ�閾���ȕ��@�͂Ȃ��D");
		else
#if 0 /*JP*/
		    You("%s no door there.",
				Blind ? "feel" : "see");
#else
		    pline("�����ɔ��͂Ȃ�%s�D",
				Blind ? "�悤��" : "�悤�Ɍ�����");
#endif
		return(0);
	}

	if(door->doormask == D_NODOOR) {
/*JP
	    pline("This doorway has no door.");
*/
	    pline("���͉��Ă���D");
	    return(0);
	}

	if(obstructed(x, y)) return(0);

	if(door->doormask == D_BROKEN) {
/*JP
	    pline("This door is broken.");
*/
	    pline("���͉��Ă���D");
	    return(0);
	}

	if(door->doormask & (D_CLOSED | D_LOCKED)) {
/*JP
	    pline("This door is already closed.");
*/
	    pline("���͂������Ă���D");
	    return(0);
	}

	if(door->doormask == D_ISOPEN) {
	    if(verysmall(youmonst.data)
#ifdef STEED
		&& !u.usteed
#endif
		) {
/*JP
		 pline("You're too small to push the door closed.");
*/
		 You("���������Ĕ���߂��Ȃ��D");
		 return(0);
	    }
	    if (
#ifdef STEED
		 u.usteed ||
#endif
		rn2(25) < (ACURRSTR+ACURR(A_DEX)+ACURR(A_CON))/3) {
/*JP
		pline_The("door closes.");
*/
		pline("���͕����D");
		door->doormask = D_CLOSED;
		if (Blind)
		    feel_location(x,y);	/* the hero knows she closed it */
		else
		    newsym(x,y);
		block_point(x,y);	/* vision:  no longer see there */
	    }
	    else {
	        exercise(A_STR, TRUE);
/*JP
	        pline_The("door resists!");
*/
	        pline("�Ȃ��Ȃ��܂�Ȃ��I");
	    }
	}

	return(1);
}

boolean			/* box obj was hit with spell effect otmp */
boxlock(obj, otmp)	/* returns true if something happened */
register struct obj *obj, *otmp;	/* obj *is* a box */
{
	register boolean res = 0;

	switch(otmp->otyp) {
	case WAN_LOCKING:
	case SPE_WIZARD_LOCK:
	    if (!obj->olocked) {	/* lock it; fix if broken */
/*JP
		pline("Klunk!");
*/
		pline("�J�`�I");
		obj->olocked = 1;
		obj->obroken = 0;
		res = 1;
	    } /* else already closed and locked */
	    break;
	case WAN_OPENING:
	case SPE_KNOCK:
	    if (obj->olocked) {		/* unlock; couldn't be broken */
/*JP
		pline("Klick!");
*/
		pline("�R���R���I");
		obj->olocked = 0;
		res = 1;
	    } else			/* silently fix if broken */
		obj->obroken = 0;
	    break;
	case WAN_POLYMORPH:
	case SPE_POLYMORPH:
	    /* maybe start unlocking chest, get interrupted, then zap it;
	       we must avoid any attempt to resume unlocking it */
	    if (xlock.box == obj)
		reset_pick();
	    break;
	}
	return res;
}

boolean			/* Door/secret door was hit with spell effect otmp */
doorlock(otmp,x,y)	/* returns true if something happened */
struct obj *otmp;
int x, y;
{
	register struct rm *door = &levl[x][y];
	boolean res = TRUE;
	int loudness = 0;
	const char *msg = (const char *)0;
#if 0 /*JP*/
	const char *dustcloud = "A cloud of dust";
	const char *quickly_dissipates = "quickly dissipates";
#else
	const char *dustcloud = "�ق���";
	const char *quickly_dissipates = "�����ƌ����܂ɔ�юU����";
#endif
	
	if (door->typ == SDOOR) {
	    switch (otmp->otyp) {
	    case WAN_OPENING:
	    case SPE_KNOCK:
	    case WAN_STRIKING:
	    case SPE_FORCE_BOLT:
		door->typ = DOOR;
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
		newsym(x,y);
/*JP
		if (cansee(x,y)) pline("A door appears in the wall!");
*/
		if (cansee(x,y)) pline("�ǂ���������ꂽ�I");
		if (otmp->otyp == WAN_OPENING || otmp->otyp == SPE_KNOCK)
		    return TRUE;
		break;		/* striking: continue door handling below */
	    case WAN_LOCKING:
	    case SPE_WIZARD_LOCK:
	    default:
		return FALSE;
	    }
	}

	switch(otmp->otyp) {
	case WAN_LOCKING:
	case SPE_WIZARD_LOCK:
#ifdef REINCARNATION
	    if (Is_rogue_level(&u.uz)) {
	    	boolean vis = cansee(x,y);
		/* Can't have real locking in Rogue, so just hide doorway */
/*JP
		if (vis) pline("%s springs up in the older, more primitive doorway.",
*/
		if (vis) pline("�Â������C���n�I�ȏo������%s���������߂��D",
			dustcloud);
		else
/*JP
			You_hear("a swoosh.");
*/
			You_hear("�V���[�b�Ƃ������𕷂����D");
		if (obstructed(x,y)) {
/*JP
			if (vis) pline_The("cloud %s.",quickly_dissipates);
*/
			if (vis) pline("�ق����%s�D",quickly_dissipates);
			return FALSE;
		}
		block_point(x, y);
		door->typ = SDOOR;
/*JP
		if (vis) pline_The("doorway vanishes!");
*/
		if (vis) pline("�o�����͏������I");
		newsym(x,y);
		return TRUE;
	    }
#endif
	    if (obstructed(x,y)) return FALSE;
	    /* Don't allow doors to close over traps.  This is for pits */
	    /* & trap doors, but is it ever OK for anything else? */
	    if (t_at(x,y)) {
		/* maketrap() clears doormask, so it should be NODOOR */
		pline(
/*JP
		"%s springs up in the doorway, but %s.",
*/
		"%s���o�����ɗ������߂��C������%s",
		dustcloud, quickly_dissipates);
		return FALSE;
	    }

	    switch (door->doormask & ~D_TRAPPED) {
	    case D_CLOSED:
/*JP
		msg = "The door locks!";
*/
	        msg = "���Ɍ������������I";
		break;
	    case D_ISOPEN:
/*JP
		msg = "The door swings shut, and locks!";
*/
		msg = "���͐����悭�܂�C�������������I";
		break;
	    case D_BROKEN:
/*JP
		msg = "The broken door reassembles and locks!";
*/
		msg = "��ꂽ�����W�܂��āC�������������I";
		break;
	    case D_NODOOR:
		msg =
/*JP
		"A cloud of dust springs up and assembles itself into a door!";
*/
		"�ق��肪�������߁C�W�܂��Ĕ��ɂȂ����I";
		break;
	    default:
		res = FALSE;
		break;
	    }
	    block_point(x, y);
	    door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
	    newsym(x,y);
	    break;
	case WAN_OPENING:
	case SPE_KNOCK:
	    if (door->doormask & D_LOCKED) {
/*JP
		msg = "The door unlocks!";
*/
		msg = "���̌��͂͂��ꂽ�I";
		door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
	    } else res = FALSE;
	    break;
	case WAN_STRIKING:
	case SPE_FORCE_BOLT:
	    if (door->doormask & (D_LOCKED | D_CLOSED)) {
		if (door->doormask & D_TRAPPED) {
		    if (MON_AT(x, y))
			(void) mb_trapped(m_at(x,y));
		    else if (flags.verbose) {
			if (cansee(x,y))
/*JP
			    pline("KABOOM!!  You see a door explode.");
*/
			    pline("����ǁ[��I�������������D");
			else if (flags.soundok)
/*JP
			    You_hear("a distant explosion.");
*/
			    You_hear("�����̔������𕷂����D");
		    }
		    door->doormask = D_NODOOR;
		    unblock_point(x,y);
		    newsym(x,y);
		    loudness = 40;
		    break;
		}
		door->doormask = D_BROKEN;
		if (flags.verbose) {
		    if (cansee(x,y))
/*JP
			pline_The("door crashes open!");
*/
			pline("���͉��J�����I");
		    else if (flags.soundok)
/*JP
			You_hear("a crashing sound.");
*/
			You_hear("���������鉹�𕷂����D");
		}
		unblock_point(x,y);
		newsym(x,y);
		/* force vision recalc before printing more messages */
		if (vision_full_recalc) vision_recalc(0);
		loudness = 20;
	    } else res = FALSE;
	    break;
	default: impossible("magic (%d) attempted on door.", otmp->otyp);
	    break;
	}
	if (msg && cansee(x,y)) pline(msg);
	if (loudness > 0) {
	    /* door was destroyed */
	    wake_nearto(x, y, loudness);
	    if (*in_rooms(x, y, SHOPBASE)) add_damage(x, y, 0L);
	}

	if (res && picking_at(x, y)) {
	    /* maybe unseen monster zaps door you're unlocking */
	    stop_occupation();
	    reset_pick();
	}
	return res;
}

STATIC_OVL void
chest_shatter_msg(otmp)
struct obj *otmp;
{
	const char *disposition;
	const char *thing;
	long save_Blinded;

	if (otmp->oclass == POTION_CLASS) {
/*JP
		You("%s %s shatter!", Blind ? "hear" : "see", an(bottlename()));
*/
		if (Blind)
		    You_hear("%s������鉹�𕷂����I", bottlename());
		else
		    pline("%s�����ꂽ�I", bottlename());
		if (!breathless(youmonst.data) || haseyes(youmonst.data))
			potionbreathe(otmp);
		return;
	}
	/* We have functions for distant and singular names, but not one */
	/* which does _both_... */
	save_Blinded = Blinded;
	Blinded = 1;
	thing = singular(otmp, xname);
	Blinded = save_Blinded;
	switch (objects[otmp->otyp].oc_material) {
/*JP
	case PAPER:	disposition = "is torn to shreds";
*/
	case PAPER:	disposition = "�͐��f���ꂽ";
		break;
/*JP
	case WAX:	disposition = "is crushed";
*/
	case WAX:	disposition = "�����ɂԂ��܂���";
		break;
/*JP
	case VEGGY:	disposition = "is pulped";
*/
	case VEGGY:	disposition = "�͂ǂ�ǂ�ɂȂ���";
		break;
/*JP
	case FLESH:	disposition = "is mashed";
*/
	case FLESH:	disposition = "�͂ǂ�ǂ�ɂȂ���";
		break;
/*JP
	case GLASS:	disposition = "shatters";
*/
	case GLASS:	disposition = "�͊��ꂽ";
		break;
/*JP
	case WOOD:	disposition = "splinters to fragments";
*/
	case WOOD:	disposition = "�͂�����ɂȂ���";
		break;
/*JP
	default:	disposition = "is destroyed";
*/
	default:	disposition = "�͉�ꂽ";
		break;
	}
/*JP
	pline("%s %s!", An(thing), disposition);
*/
	pline("%s%s�I", thing, disposition);
}

#endif /* OVLB */

/*lock.c*/
