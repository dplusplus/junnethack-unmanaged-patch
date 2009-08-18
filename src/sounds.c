/*	SCCS Id: @(#)sounds.c	3.4	2002/05/06	*/
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2003
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"
#include "edog.h"
#ifdef USER_SOUNDS
# ifdef USER_SOUNDS_REGEX
#include <regex.h>
# endif
#endif

#ifdef OVLB

static int FDECL(domonnoise,(struct monst *));
static int NDECL(dochat);

#endif /* OVLB */

#ifdef OVL0

static int FDECL(mon_in_room, (struct monst *,int));

/* this easily could be a macro, but it might overtax dumb compilers */
static int
mon_in_room(mon, rmtyp)
struct monst *mon;
int rmtyp;
{
    int rno = levl[mon->mx][mon->my].roomno;

    return rooms[rno - ROOMOFFSET].rtype == rmtyp;
}

void
dosounds()
{
    register struct mkroom *sroom;
    register int hallu, vx, vy;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
    int xx;
#endif
    struct monst *mtmp;

    if (!flags.soundok || u.uswallow || Underwater) return;

    hallu = Hallucination ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[4] = {
/*JP
		"bubbling water.",
*/
		"�S�{�S�{�Ƃ������𕷂����D",
/*JP
		"water falling on coins.",
*/
		"�s�`���s�`���Ƃ������𕷂����D",
/*JP
		"the splashing of a naiad.",
*/
		"�o�V���o�V���Ƃ������𕷂����D",
/*JP
		"a soda fountain!",
*/
		"�Y�_�����̃V���[�Ƃ������𕷂����I"
	};
	You_hear(fountain_msg[rn2(3)+hallu]);
    }
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[3] = {
/*JP
		"a slow drip.",
*/
	        "�����ۂ��ۂ��Ɨ����鉹�𕷂����D",
/*JP
		"a gurgling noise.",
*/
		"���炪��Ƃ������𕷂����D",
/*JP
		"dishes being washed!",
*/
		"�M��􂤉��𕷂����I",
	};
	You_hear(sink_msg[rn2(2)+hallu]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[4] = {
/*JP
		"the tones of courtly conversation.",
*/
	        "��i�Șb�����𕷂����D",
/*JP
		"a sceptre pounded in judgment.",
*/
		"�ٔ���│�˂����𕷂����D",
/*JP
		"Someone shouts \"Off with %s head!\"",
*/
		"���ꂩ���u���̂��̂̎���͂˂�I�v�Ƌ��Ԑ��𕷂����D",
/*JP
		"Queen Beruthiel's cats!",
*/
		"�x���V�G�����܂̔L�̐��𕷂����I",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(3)+hallu;

		if (which != 2) You_hear(throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_garden && !rn2(200)) {
	static const char *garden_msg[4] = {
/*JP
		"crickets chirping!",
*/
		"�R�I���M�B�̖����𕷂����I",
/*JP
		"birds singing!",
*/
		"���B�̉̂𕷂����I",
/*JP
		"grass growing!",
*/
		"���̐������鉹�𕷂����I",
/*JP
		"wind in the willows!",
*/
		"���̖؁X��ʂ蔲���镗�̉��𕷂����I",
	};
	You_hear(garden_msg[rn2(2) + 2 * hallu]);
	return;
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[3] = {
#if 0 /*JP*/
		"hear mosquitoes!",
		"smell marsh gas!",	/* so it's a smell...*/
		"hear Donald Duck!",
#else
		"��̉H���𕷂����D",
		"�����������������I",	/* so it's a smell...*/
		"�h�i���h�_�b�N�̐��𕷂����I",
#endif
	};
#if 0 /*JP*/
	You(swamp_msg[rn2(2)+hallu]);
#else
	{
		int c;
		c = rn2(2)+hallu;
		if(c == 1){
			pline(swamp_msg[c]);
		} else {
			You_hear(swamp_msg[c]);
		}
	}
#endif
	return;
    }
    if (level.flags.has_vault && !rn2(200)) {
	if (!(sroom = search_special(VAULT))) {
	    /* strange ... */
	    level.flags.has_vault = 0;
	    return;
	}
	if(gd_sound())
	    switch (rn2(2)+hallu) {
		case 1: {
		    boolean gold_in_vault = FALSE;

		    for (vx = sroom->lx;vx <= sroom->hx; vx++)
			for (vy = sroom->ly; vy <= sroom->hy; vy++)
			    if (g_at(vx, vy))
				gold_in_vault = TRUE;
#if defined(AMIGA) && defined(AZTEC_C_WORKAROUND)
		    /* Bug in aztec assembler here. Workaround below */
		    xx = ROOM_INDEX(sroom) + ROOMOFFSET;
		    xx = (xx != vault_occupied(u.urooms));
		    if(xx)
#else
		    if (vault_occupied(u.urooms) !=
			 (ROOM_INDEX(sroom) + ROOMOFFSET))
#endif /* AZTEC_C_WORKAROUND */
		    {
			if (gold_in_vault)
#if 0 /*JP*/
			    You_hear(!hallu ? "someone counting money." :
				"the quarterback calling the play.");
#else
			    You_hear(!hallu ? "�N���������𐔂��Ă��鉹�𕷂����D" :
				"�N�H�[�^�o�b�N���w�������鐺�𕷂����D");
#endif
			else
/*JP
			    You_hear("someone searching.");
*/
			    You_hear("�N�����{�����Ă��鉹�𕷂����D");
			break;
		    }
		    /* fall into... (yes, even for hallucination) */
		}
		case 0:
/*JP
		    You_hear("the footsteps of a guard on patrol.");
*/
		    You_hear("�x�����̃p�g���[�����鉹�𕷂����D");
		    break;
		case 2:
/*JP
		    You_hear("Ebenezer Scrooge!");
*/
		    You_hear("�����T�̗�����̐��𕷂����I");
		    break;
	    }
	return;
    }
    if (level.flags.has_beehive && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->data->mlet == S_ANT && is_flyer(mtmp->data)) &&
		mon_in_room(mtmp, BEEHIVE)) {
		switch (rn2(2)+hallu) {
		    case 0:
/*JP
			You_hear("a low buzzing.");
*/
			You_hear("�ԁ[��Ƃ������𕷂����D");
			break;
		    case 1:
/*JP
			You_hear("an angry drone.");
*/
			You_hear("���������Y�o�`�̉��𕷂����D");
			break;
		    case 2:
#if 0 /*JP*/
			You_hear("bees in your %sbonnet!",
			    uarmh ? "" : "(nonexistent) ");
#else
			You_hear("�n�`�����Ȃ��̖X�q%s�̒��ɂ��鉹�𕷂����I",
			    uarmh ? "" : "(����ĂȂ�����)");
#endif
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_lemurepit && !rn2(20)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ( mtmp->data == &mons[PM_LEMURE]
	      && mon_in_room(mtmp, LEMUREPIT) ) {
		if (hallu) {
		    switch (rn2(3)) {
		        case 0:
/*JP
			    You_hear("screams of lust!");
*/
			    You_hear("�D�F�țg���𕷂����I");
			    break;
		        case 1:
/*JP
			    You_hear("the crack of your mistress's whip!");
*/
			    You_hear("�����̈��l�̐U�邤�ڂ̉��𕷂����I");
			    break;
		        case 2:
/*JP
			    You_hear("a weeping willow!");
*/
			    You_hear("��������̉��𕷂����I");
			    break;
		    }	
		} else {
		    switch (rn2(6)) {
		        case 0:
/*JP
			    You_hear("the crack of a barbed whip!");
*/
			    You_hear("�g�Q�t���̕ڂ�U�邤���𕷂����I");
			    break;
		        case 1:
/*JP
			    You_hear("the screams of tortured souls!");
*/
			    You_hear("���₳��鍰�̐⋩�𕷂����I");
			    break;
		        case 2:
/*JP
			    You_hear("a wail of eternal anguish!");
*/
			    You_hear("�i���̌��ɂ��������Ԑ��𕷂����I");
			    break;
		        case 3:
/*JP
			    You_hear("diabolical laughter!");
*/
			    You_hear("�����̂��Ƃ��΂����𕷂����I");
			    break;
		        case 4:
/*JP
			    You_hear("cries of repentance!");
*/
			    You_hear("����̋��т𕷂����I");
			    break;
		        case 5:
/*JP
			    You_hear("futile pleas for mercy!");
*/
			    You_hear("���������߂�����𕷂����I");
			    break;
		    }
		}
		return;
	    }
	}
    }
    if (level.flags.has_morgue && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_undead(mtmp->data) &&
		mon_in_room(mtmp, MORGUE)) {
		switch (rn2(2)+hallu) {
		    case 0:
/*JP
			You("suddenly realize it is unnaturally quiet.");
*/
			You("�s���R�Ȃ��炢�Â��Ȃ̂ɋC�Â����D");
			break;
		    case 1:
#if 0 /*JP*/
			pline_The("%s on the back of your %s stands up.",
				body_part(HAIR), body_part(NECK));
#else
			pline("���Ȃ���%s�̂������%s���t�������D",
				body_part(NECK), body_part(HAIR));
#endif
			break;
		    case 2:
#if 0 /*JP*/
			pline_The("%s on your %s seems to stand up.",
				body_part(HAIR), body_part(HEAD));
#else
			pline("���Ȃ���%s��%s�͋t�������D",
				body_part(HEAD), body_part(HAIR));
#endif
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[4] = {
#if 0 /*JP*/
		"blades being honed.",
		"loud snoring.",
		"dice being thrown.",
		"General MacArthur!",
#else
		"�n�����������𕷂����D",
		"�傫�Ȃ��т��𕷂����D",
		"�_�C�X���U���鉹�𕷂����D",
		"�}�b�J�[�T�[���R�̐��𕷂����I",
#endif
	};
	int count = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_mercenary(mtmp->data) &&
#if 0		/* don't bother excluding these */
		!strstri(mtmp->data->mname, "watch") &&
		!strstri(mtmp->data->mname, "guard") &&
#endif
		mon_in_room(mtmp, BARRACKS) &&
		/* sleeping implies not-yet-disturbed (usually) */
		(mtmp->msleeping || ++count > 5)) {
		You_hear(barracks_msg[rn2(3)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[3] = {
#if 0 /*JP*/
		"a sound reminiscent of an elephant stepping on a peanut.",
		"a sound reminiscent of a seal barking.",
		"Doctor Dolittle!",
#else
		"�ۂ��s�[�i�b�c�̏�ŗx��悤�ȉ��𕷂����D",
		"�A�V�J���i����悤�ȉ��𕷂����D",
		"�h���g���搶�̐��𕷂����I",
#endif
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You_hear(zoo_msg[rn2(2)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_shop && !rn2(200)) {
	if (!(sroom = search_special(ANY_SHOP))) {
	    /* strange... */
	    level.flags.has_shop = 0;
	    return;
	}
	if (tended_shop(sroom) &&
		!index(u.ushops, ROOM_INDEX(sroom) + ROOMOFFSET)) {
	    static const char * const shop_msg[3] = {
#if 0 /*JP*/
		    "someone cursing shoplifters.",
		    "the chime of a cash register.",
		    "Neiman and Marcus arguing!",
#else
		    "�N�����D�_���̂̂��鐺�𕷂����D",
		    "���W�̃`�[���Ƃ������𕷂����D",
		    "�C�g�[�ƃ��[�J�h�[�̋c�_�𕷂����I",
#endif
	    };
	    You_hear(shop_msg[rn2(2)+hallu]);
	}
	return;
    }
    if (Is_oracle_level(&u.uz) && !rn2(400)) {
	/* make sure the Oracle is still here */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->data == &mons[PM_ORACLE])
		break;
	/* and don't produce silly effects when she's clearly visible */
	if (mtmp && (hallu || !canseemon(mtmp))) {
	    static const char * const ora_msg[5] = {
#if 0 /*JP*/
		    "a strange wind.",		/* Jupiter at Dodona */
		    "convulsive ravings.",	/* Apollo at Delphi */
		    "snoring snakes.",		/* AEsculapius at Epidaurus */
		    "someone say \"No more woodchucks!\"",
		    "a loud ZOT!"		/* both rec.humor.oracle */
#else
		"��ȕ��̉��𕷂����D",
		"�������̐��𕷂����D",
		"�ւ̂��т��𕷂����D",
		"�N�����u�����E�b�h�`���b�N�͂���Ȃ��I�v�ƌ����Ă��鐺�𕷂����D",
		"�傫�Ȃy�n�s�𕷂����I"
#endif
	    };
	    You_hear(ora_msg[rn2(3)+hallu*2]);
	}
	return;
    }
#ifdef BLACKMARKET
    if (!Is_blackmarket(&u.uz) && at_dgn_entrance("One-eyed Sam's Market") &&
        !rn2(200)) {
      static const char *blkmar_msg[3] = {
#if 0 /*JP*/
        "You hear someone complaining about the prices.",
        "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
        "You feel like searching for more gold.",
#else
        "���Ȃ��͒N�����l�i�ɂ��ċ�s�������Ă���̂𕷂����D",
        "�N���̂����₫�����������F�u�H�Ƃ͂�������900�S�[���h���D�v",
        "���Ȃ��͍X�ɑ����̋��݂�k�������悤�ȋC�������D",
#endif
      };
      pline(blkmar_msg[rn2(2)+hallu]);
    }
#endif /* BLACKMARKET */
}

#endif /* OVL0 */
#ifdef OVLB

static const char * const h_sounds[] = {
#if 0 /*JP*/
    "beep", "boing", "sing", "belche", "creak", "cough", "rattle",
    "ululate", "pop", "jingle", "sniffle", "tinkle", "eep"
#else
    "�s�[�b�Ɩ���","�������Ă�","�̂���","�L�[�L�[�Ɩ���",
    "��������","�S���S������","�z�[�z�[����","�|���Ɩ���",
    "�K�����K�����Ɩ���","�N���N������","�`�����`�����Ɩ���",
    "�C�[�b�Ɩ���"
#endif
};

const char *
growl_sound(mtmp)
register struct monst *mtmp;
{
	const char *ret;

	switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_HISS:
/*JP
	    ret = "hiss";
*/
	    ret = "�V�[�b�Ɩ���";
	    break;
	case MS_BARK:
	case MS_GROWL:
/*JP
	    ret = "growl";
*/
	    ret = "�͂������i����";
	    break;
	case MS_ROAR:
/*JP
	    ret = "roar";
*/
	    ret = "�i����";
	    break;
	case MS_BUZZ:
/*JP
	    ret = "buzz";
*/
	    ret = "�u�[�b�Ɩ���";
	    break;
	case MS_SQEEK:
/*JP
	    ret = "squeal";
*/
	    ret = "�L�[�L�[����";
	    break;
	case MS_SQAWK:
/*JP
	    ret = "screech";
*/
	    ret = "���؂萺�𗧂Ă�";
	    break;
	case MS_NEIGH:
/*JP
	    ret = "neigh";
*/
	    ret = "���ȂȂ���";
	    break;
	case MS_WAIL:
/*JP
	    ret = "wail";
*/
	    ret = "�߂�������";
	    break;
	case MS_SILENT:
/*JP
		ret = "commotion";
*/
		ret = "��������";
		break;
	default:
/*JP
		ret = "scream";
*/
		ret = "���؂萺��������";
	}
	return ret;
}

/* the sounds of a seriously abused pet, including player attacking it */
void
growl(mtmp)
register struct monst *mtmp;
{
    register const char *growl_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	growl_verb = h_sounds[rn2(SIZE(h_sounds))];
    else
	growl_verb = growl_sound(mtmp);
    if (growl_verb) {
/*JP
	pline("%s %s!", Monnam(mtmp), vtense((char *)0, growl_verb));
*/
	pline("%s��%s�I", Monnam(mtmp), makeplural(growl_verb));
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 18);
    }
}

/* the sounds of mistreated pets */
void
yelp(mtmp)
register struct monst *mtmp;
{
    register const char *yelp_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	yelp_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
/*JP
	    yelp_verb = "yowl";
*/
	    yelp_verb = "�߂�������";
	    break;
	case MS_BARK:
	case MS_GROWL:
/*JP
	    yelp_verb = "yelp";
*/
	    yelp_verb = "�L�����L��������";
	    break;
	case MS_ROAR:
/*JP
	    yelp_verb = "snarl";
*/
	    yelp_verb = "���Ȃ���";
	    break;
	case MS_SQEEK:
/*JP
	    yelp_verb = "squeal";
*/
	    yelp_verb = "�L�[�L�[����";
	    break;
	case MS_SQAWK:
/*JP
	    yelp_verb = "screak";
*/
	    yelp_verb = "���؂萺�𗧂Ă�";
	    break;
	case MS_WAIL:
/*JP
	    yelp_verb = "wail";
*/
	    yelp_verb = "�߂�������";
	    break;
    }
    if (yelp_verb) {
/*JP
	pline("%s %s!", Monnam(mtmp), vtense((char *)0, yelp_verb));
*/
	pline("%s��%s�I", Monnam(mtmp), yelp_verb);
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 12);
    }
}

/* the sounds of distressed pets */
void
whimper(mtmp)
register struct monst *mtmp;
{
    register const char *whimper_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	whimper_verb = h_sounds[rn2(SIZE(h_sounds))];
    else switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_GROWL:
/*JP
	    whimper_verb = "whimper";
*/
	    whimper_verb = "�N���N������";
	    break;
	case MS_BARK:
/*JP
	    whimper_verb = "whine";
*/
	    whimper_verb = "�N�[���Ɩ���";
	    break;
	case MS_SQEEK:
/*JP
	    whimper_verb = "squeal";
*/
	    whimper_verb = "�L�[�L�[����";
	    break;
    }
    if (whimper_verb) {
/*JP
	pline("%s %s.", Monnam(mtmp), vtense((char *)0, whimper_verb));
*/
	pline("%s��%s�D", Monnam(mtmp), whimper_verb);
	if(flags.run) nomul(0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 6);
    }
}

/* pet makes "I'm hungry" noises */
void
beg(mtmp)
register struct monst *mtmp;
{
    if (mtmp->msleeping || !mtmp->mcanmove ||
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent(mtmp->data) && mtmp->data->msound <= MS_ANIMAL)
	(void) domonnoise(mtmp);
    else if (mtmp->data->msound >= MS_HUMANOID) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
/*JP
	verbalize("I'm hungry.");
*/
	verbalize("�͂�؂����D");
    }
}

static int
domonnoise(mtmp)
register struct monst *mtmp;
{
    register const char *pline_msg = 0,	/* Monnam(mtmp) will be prepended */
			*verbl_msg = 0;	/* verbalize() */
    struct permonst *ptr = mtmp->data;
    char verbuf[BUFSZ];

    /* presumably nearness and sleep checks have already been made */
    if (!flags.soundok) return(0);
    if (is_silent(ptr)) return(0);

    /* Make sure its your role's quest quardian; adjust if not */
    if (ptr->msound == MS_GUARDIAN && ptr != &mons[urole.guardnum]) {
    	int mndx = monsndx(ptr);
    	ptr = &mons[genus(mndx,1)];
    }

    /* be sure to do this before talking; the monster might teleport away, in
     * which case we want to check its pre-teleport position
     */
    if (!canspotmon(mtmp))
	map_invisible(mtmp->mx, mtmp->my);

    switch (ptr->msound) {
	case MS_ORACLE:
	    return doconsult(mtmp);
	case MS_PRIEST:
	    priest_talk(mtmp);
	    break;
	case MS_LEADER:
	case MS_NEMESIS:
	case MS_GUARDIAN:
	    quest_chat(mtmp);
	    break;
	case MS_SELL: /* pitch, pay, total */
	    shk_chat(mtmp);
	    break;
	case MS_VAMPIRE:
	    {
	    /* vampire messages are varied by tameness, peacefulness, and time of night */
		boolean isnight = night();
		boolean kindred =    (Upolyd && (u.umonnum == PM_VAMPIRE ||
				       u.umonnum == PM_VAMPIRE_LORD));
		boolean nightchild = (Upolyd && (u.umonnum == PM_WOLF ||
				       u.umonnum == PM_WINTER_WOLF ||
	    			       u.umonnum == PM_WINTER_WOLF_CUB));
#if 0 /*JP*/
		const char *racenoun = (flags.female && urace.individual.f) ?
					urace.individual.f : (urace.individual.m) ?
					urace.individual.m : urace.noun;
#else
		const char *racenoun = (flags.female) ? "���Ȃ�" : "���܂�" ;
#endif

		if (mtmp->mtame) {
			if (kindred) {
#if 0 /*JP*/
				Sprintf(verbuf, "Good %s to you Master%s",
					isnight ? "evening" : "day",
					isnight ? "!" : ".  Why do we not rest?");
#else
				Sprintf(verbuf, "����l�l�C%s%s",
					isnight ? "����΂��" : "����ɂ���",
					isnight ? "!" : "�D���x�݂ɂȂ�܂��񂩁H");
#endif
				verbl_msg = verbuf;
		    	} else {
#if 0 /*JP*/
		    	    Sprintf(verbuf,"%s%s",
				nightchild ? "Child of the night, " : "",
				midnight() ?
					"I can stand this craving no longer!" :
				isnight ?
					"I beg you, help me satisfy this growing craving!" :
					"I find myself growing a little weary.");
#else
		    	    Sprintf(verbuf,"%s%s",
				nightchild ? "��̎e��C" : "",
				midnight() ?
					"���͂���ȏ㊉�]��}�����Ȃ��I" :
				isnight ?
					"�ӂ��ꂠ���銉�]�𖞂����̂������Ă���Ȃ����H���ށI" :
					"���͏��X��ꂽ�悤���D");
#endif
				verbl_msg = verbuf;
			}
		} else if (mtmp->mpeaceful) {
			if (kindred && isnight) {
#if 0 /*JP*/
				Sprintf(verbuf, "Good feeding %s!",
	    				flags.female ? "sister" : "brother");
#else
				Sprintf(verbuf, "�悤�Z��I");
#endif
				verbl_msg = verbuf;
 			} else if (nightchild && isnight) {
#if 0 /*JP*/
				Sprintf(verbuf,
				    "How nice to hear you, child of the night!");
#else
				Sprintf(verbuf,
				    "��̎e��C���C�������ȁI");
#endif
				verbl_msg = verbuf;
	    		} else
/*JP
		    		verbl_msg = "I only drink... potions.";
*/
				verbl_msg = "���͖�D�D�������܂Ȃ�";
    	        } else {
			int vampindex;
	    		static const char * const vampmsg[] = {
			       /* These first two (0 and 1) are specially handled below */
#if 0 /*JP*/
	    			"I vant to suck your %s!",
	    			"I vill come after %s without regret!",
#else
				"���O��%s���悱���I",
				"������%s��ǌ������Ă��炨���I",
#endif
		    	       /* other famous vampire quotes can follow here if desired */
	    		};
			if (kindred)
#if 0 /*JP*/
			    verbl_msg = "This is my hunting ground that you dare to prowl!";
#else
			    verbl_msg = "���܂���������Ă��邱�̂�����͎��̎�ꂾ�I";
#endif
			else if (youmonst.data == &mons[PM_SILVER_DRAGON] ||
				 youmonst.data == &mons[PM_BABY_SILVER_DRAGON]) {
			    /* Silver dragons are silver in color, not made of silver */
#if 0 /*JP*/
			    Sprintf(verbuf, "%s! Your silver sheen does not frighten me!",
					youmonst.data == &mons[PM_SILVER_DRAGON] ?
					"Fool" : "Young Fool");
			    verbl_msg = verbuf; 
#else
			    verbl_msg = "�΂��߁I���܂��̋�̋P���ȂǕ|���Ȃ����I";
#endif
			} else {
			    vampindex = rn2(SIZE(vampmsg));
			    if (vampindex == 0) {
				Sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
	    			verbl_msg = verbuf;
			    } else if (vampindex == 1) {
				Sprintf(verbuf, vampmsg[vampindex],
/*JP
					Upolyd ? an(mons[u.umonnum].mname) : an(racenoun));
*/
					Upolyd ? jtrns_mon(mons[u.umonnum].mname) : racenoun);
	    			verbl_msg = verbuf;
		    	    } else
			    	verbl_msg = vampmsg[vampindex];
			}
	        }
	    }
	    break;
	case MS_WERE:
	    if (flags.moonphase == FULL_MOON && (night() ^ !rn2(13))) {
#if 0 /*JP*/
		pline("%s throws back %s head and lets out a blood curdling %s!",
		      Monnam(mtmp), mhis(mtmp),
		      ptr == &mons[PM_HUMAN_WERERAT] ? "shriek" : "howl");
#else
		pline("%s�͓����̂����炵�w�؂�����悤��%s���������I",
		      Monnam(mtmp),
		      ptr == &mons[PM_HUMAN_WERERAT] ? "���؂萺" : "���K");
#endif
		wake_nearto(mtmp->mx, mtmp->my, 11*11);
	    } else
		pline_msg =
/*JP
		     "whispers inaudibly.  All you can make out is \"moon\".";
*/
		     "�����Ƃ�Ȃ��悤�Ȑ��ł����₢���D ���낤���āw���x�Ƃ������t�����������Ƃꂽ�D";
	    break;
	case MS_BARK:
	    if (flags.moonphase == FULL_MOON && night()) {
/*JP
		pline_msg = "howls.";
*/
		pline_msg = "�i�����D";
	    } else if (mtmp->mpeaceful) {
		if (mtmp->mtame &&
			(mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			 moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5))
/*JP
		    pline_msg = "whines.";
*/
		    pline_msg = "�N���N�������D";
		else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000)
/*JP
		    pline_msg = "yips.";
*/
		    pline_msg = "�L�����L���������D";
		else {
		    if (mtmp->data != &mons[PM_DINGO])	/* dingos do not actually bark */
/*JP
			    pline_msg = "barks.";
*/
			    pline_msg = "���������i�����D";
		}
	    } else {
/*JP
		pline_msg = "growls.";
*/
		pline_msg = "�������i�����D";
	    }
	    break;
	case MS_MEW:
	    if (mtmp->mtame) {
		if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			mtmp->mtame < 5)
/*JP
		    pline_msg = "yowls.";
*/
		    pline_msg = "�߂��������D";
		else if (moves > EDOG(mtmp)->hungrytime)
/*JP
		    pline_msg = "meows.";
*/
		    pline_msg = "�j���[���Ɩ����D";
		else if (EDOG(mtmp)->hungrytime > moves + 1000)
/*JP
		    pline_msg = "purrs.";
*/
		    pline_msg = "�S���S���Ɩ����D";
		else
/*JP
		    pline_msg = "mews.";
*/
		    pline_msg = "�j���[�j���[�����D";
		break;
	    } /* else FALLTHRU */
	case MS_GROWL:
#ifdef EXOTIC_PETS
	    if (mtmp->mtame &&
		   (mtmp->data == &mons[PM_MONKEY] ||
		    mtmp->data == &mons[PM_APE] ||
		    mtmp->data == &mons[PM_CARNIVOROUS_APE])) {
		if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
			 moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5) {
/*JP
			pline_msg = "shrieks.";
*/
			pline_msg = "���؂萺���������D";
			wake_nearto(mtmp->mx, mtmp->my, 8*8);
		} else if (EDOG(mtmp)->hungrytime > moves + 1000) {
/*JP
			pline_msg = "chatters.";
*/
			pline_msg = "�L�B�L�B�����D";
		} else {
/*JP
			pline_msg = "hoots.";
*/
			pline_msg = "�z�[�b�C�z�[�b�Ɩ����D";
		}
	    } else
#endif
/*JP
	    pline_msg = mtmp->mpeaceful ? "snarls." : "growls!";
*/
	    pline_msg = mtmp->mpeaceful ? "���Ȃ����D" : "�������i�����I";
	    break;
	case MS_ROAR:
/*JP
	    pline_msg = mtmp->mpeaceful ? "snarls." : "roars!";
*/
	    pline_msg = mtmp->mpeaceful ? "���Ȃ����D" : "�ƂĂ��������i�����I";
	    break;
	case MS_SQEEK:
/*JP
	    pline_msg = "squeaks.";
*/
	    pline_msg = "�L�[�L�[�����D";
	    break;
	case MS_SQAWK:
	    if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful)
/*JP
	    	verbl_msg = "Nevermore!";
*/
		verbl_msg = "������x�ƁI";
	    else
/*JP
	    	pline_msg = "squawks.";
*/
		pline_msg = "�K�@�K�@�����D";
	    break;
	case MS_HISS:
	    if (!mtmp->mpeaceful)
/*JP
		pline_msg = "hisses!";
*/
		pline_msg = "�V�[�b�Ɩ����I";
	    else return 0;	/* no sound */
	    break;
	case MS_BUZZ:
/*JP
	    pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily.";
*/
	    pline_msg = mtmp->mpeaceful ? "�Ⴂ�H�������ĂĂ���D" : "�{�����悤�ȉH�������Ă��D";
	    break;
	case MS_GRUNT:
/*JP
	    pline_msg = "grunts.";
*/
	    pline_msg = "���Ȃ萺���������D";
	    break;
	case MS_NEIGH:
	    if (mtmp->mtame < 5)
/*JP
		pline_msg = "neighs.";
*/
		pline_msg = "�r�X�������ȂȂ����D";
	    else if (moves > EDOG(mtmp)->hungrytime)
/*JP
		pline_msg = "whinnies.";
*/
		pline_msg = "��X�������ȂȂ����D";
	    else
/*JP
		pline_msg = "whickers.";
*/
		pline_msg = "�q�q�[���Ƃ��ȂȂ����D";
	    break;
	case MS_WAIL:
/*JP
	    pline_msg = "wails mournfully.";
*/
	    pline_msg = "�߂����ɖ����D";
	    break;
	case MS_GURGLE:
/*JP
	    pline_msg = "gurgles.";
*/
	    pline_msg = "���ڂ��ڂƉ��𗧂Ă��D";
	    break;
	case MS_BURBLE:
/*JP
	    pline_msg = "burbles.";
*/
	    pline_msg = "�ԁ[�Ԃ��Ɩ����D";
	    break;
	case MS_SHRIEK:
/*JP
	    pline_msg = "shrieks.";
*/
	    pline_msg = "���؂萺���������D";
	    aggravate();
	    break;
	case MS_IMITATE:
/*JP
	    pline_msg = "imitates you.";
*/
	    pline_msg = "���Ȃ��̐^���������D";
	    break;
	case MS_BONES:
/*JP
	    pline("%s rattles noisily.", Monnam(mtmp));
*/
	    pline("%s�͍����J�^�J�^�Ɩ炵���D",Monnam(mtmp));
/*JP
	    You("freeze for a moment.");
*/
	    You("��u��������D");
	    nomul(-2);
	    break;
	case MS_LAUGH:
	    {
		static const char * const laugh_msg[4] = {
/*JP
		    "giggles.", "chuckles.", "snickers.", "laughs.",
*/
		    "���������΂����D", "�N�b�N�b�Ə΂����D", "�΂��ɂ����悤�ɏ΂����D", "�΂����D",
		};
		pline_msg = laugh_msg[rn2(4)];
	    }
	    break;
	case MS_MUMBLE:
/*JP
	    pline_msg = "mumbles incomprehensibly.";
*/
	    pline_msg = "�s���Ȍ��t���Ԃ₢���D";
	    break;
	case MS_DJINNI:
	    if (mtmp->mtame) {
/*JP
		verbl_msg = "Sorry, I'm all out of wishes.";
*/
		verbl_msg = "���߂��C�肢�͕i�؂�Ȃ񂾁D";
	    } else if (mtmp->mpeaceful) {
		if (ptr == &mons[PM_WATER_DEMON])
/*JP
		    pline_msg = "gurgles.";
*/
		    pline_msg = "�S�{�S�{�S�{�S�{�D";
		else
/*JP
		    verbl_msg = "I'm free!";
*/
		    verbl_msg = "���͎��R���I";
/*JP
	    } else verbl_msg = "This will teach you not to disturb me!";
*/
	    } else verbl_msg = "����ς킷�҂��ǂ��Ȃ邩�m�邪�����I";
	    break;
	case MS_BOAST:	/* giants */
	    if (!mtmp->mpeaceful) {
		switch (rn2(4)) {
#if 0 /*JP*/
		case 0: pline("%s boasts about %s gem collection.",
			      Monnam(mtmp), mhis(mtmp));
#else
		case 0: pline("%s�͎����̕�΂̃R���N�V���������������D",
			      Monnam(mtmp));
#endif
			break;
/*JP
		case 1: pline_msg = "complains about a diet of mutton.";
*/
		case 1: pline_msg = "�����r�΂���H�ׂĂ��鎖����s�����D";
			break;
/*JP
	       default: pline_msg = "shouts \"Fee Fie Foe Foo!\" and guffaws.";
*/
	       default: pline_msg = "�w����͂��͂��́I�x�Ƃ΂��΂������D";
			wake_nearto(mtmp->mx, mtmp->my, 7*7);
			break;
		}
		break;
	    }
	    /* else FALLTHRU */
	case MS_HUMANOID:
	    if (!mtmp->mpeaceful) {
		if (In_endgame(&u.uz) && is_mplayer(ptr)) {
		    mplayer_talk(mtmp);
		    break;
		} else return 0;	/* no sound */
	    }
	    /* Generic peaceful humanoid behaviour. */
	    if (mtmp->mflee)
/*JP
		pline_msg = "wants nothing to do with you.";
*/
		pline_msg = "���Ȃ��Ɗ֌W�����������Ȃ��悤���D";
	    else if (mtmp->mhp < mtmp->mhpmax/4)
/*JP
		pline_msg = "moans.";
*/
		pline_msg = "���߂������������D";
	    else if (mtmp->mconf || mtmp->mstun)
/*JP
		verbl_msg = !rn2(3) ? "Huh?" : rn2(2) ? "What?" : "Eh?";
*/
		verbl_msg = !rn2(3) ? "�ցH" : rn2(2) ? "���H" : "���H";
	    else if (!mtmp->mcansee)
/*JP
		verbl_msg = "I can't see!";
*/
		verbl_msg = "���������Ȃ��I";
	    else if (mtmp->mtrapped) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);

		if (t) t->tseen = 1;
/*JP
		verbl_msg = "I'm trapped!";
*/
	        verbl_msg = "㩂ɂ͂܂��Ă��܂����I";
	    } else if (mtmp->mhp < mtmp->mhpmax/2)
/*JP
		pline_msg = "asks for a potion of healing.";
*/
		pline_msg = "�񕜂̖�������ĂȂ����q�˂��D";
	    else if (mtmp->mtame && !mtmp->isminion &&
						moves > EDOG(mtmp)->hungrytime)
/*JP
		verbl_msg = "I'm hungry.";
*/
		verbl_msg = "�����������ȁD";
	    /* Specific monsters' interests */
	    else if (is_elf(ptr))
/*JP
		pline_msg = "curses orcs.";
*/
		pline_msg = "�I�[�N��������D";
	    else if (is_dwarf(ptr))
/*JP
		pline_msg = "talks about mining.";
*/
		pline_msg = "�̌@�ɂ��Ęb�����D";
	    else if (likes_magic(ptr))
/*JP
		pline_msg = "talks about spellcraft.";
*/
		pline_msg = "�����Z�p�ɂ��Ęb�����D";
	    else if (ptr->mlet == S_CENTAUR)
/*JP
		pline_msg = "discusses hunting.";
*/
		pline_msg = "�ɂ��ċc�_�����D";
	    else switch (monsndx(ptr)) {
		case PM_HOBBIT:
		    pline_msg = (mtmp->mhpmax - mtmp->mhp >= 10) ?
#if 0 /*JP*/
				"complains about unpleasant dungeon conditions."
				: "asks you about the One Ring.";
#else
				"�s�����Ȗ��{�̏�Ԃɂ��ĕs�����q�ׂ��D"
				: "�u��̎w�ցv�ɂ��Đq�˂��D";
#endif
		    break;
		case PM_ARCHEOLOGIST:
/*JP
    pline_msg = "describes a recent article in \"Spelunker Today\" magazine.";
*/
		    pline_msg = "�u�������A�v�̍ŐV�̋L�������M���Ă���D";
		    break;
#ifdef TOURIST
		case PM_TOURIST:
/*JP
		    verbl_msg = "Aloha.";
*/
		    verbl_msg = "�A���[�n�D";
		    break;
#endif
		default:
/*JP
		    pline_msg = "discusses dungeon exploration.";
*/
		    pline_msg = "���{�T���ɂ��ċc�_�����D";
		    break;
	    }
	    break;
	case MS_SEDUCE:
#ifdef SEDUCE
	    if (ptr->mlet != S_NYMPH &&
		could_seduce(mtmp, &youmonst, (struct attack *)0) == 1) {
			(void) doseduce(mtmp);
			break;
	    }
	    switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0)
#else
	    switch ((poly_gender() == 0) ? rn2(3) : 0)
#endif
	    {
		case 2:
#if 0 /*JP*/
			verbl_msg = "Hello, sailor.";
#else
			switch(poly_gender()){
			case 0:	      
			  verbl_msg = "����ɂ��́D���炢���j�ˁD";
			  break;
			case 1:
			  verbl_msg = "����ɂ��́C���삳��D";
			  break;
			default:
			  verbl_msg = "����ɂ��́D";
			  break;
			}
#endif
			break;
		case 1:
/*JP
			pline_msg = "comes on to you.";
*/
			pline_msg = "���Ȃ��̂ق��ւ���Ă����D";
			break;
		default:
/*JP
			pline_msg = "cajoles you.";
*/
			pline_msg = "���Ȃ��������Ă��D";
	    }
	    break;
#ifdef KOPS
	case MS_ARREST:
	    if (mtmp->mpeaceful)
#if 0 /*JP*/
		verbalize("Just the facts, %s.",
		      flags.female ? "Ma'am" : "Sir");
#else
		verbalize("�����������m�肽����ł���C%s�D",
		      flags.female ? "������" : "�U��");
#endif
	    else {
		static const char * const arrest_msg[3] = {
#if 0 /*JP*/
		    "Anything you say can be used against you.",
		    "You're under arrest!",
		    "Stop in the name of the Law!",
#else
		    "���܂��̌������Ƃ͂��܂��ɂƂ��ĕs���ȏ؋��ƂȂ邱�Ƃ�����I",
		    "���܂���ߕ߂���I",
		    "�@�̖��̂��ƒ����ɒ��~����I",
#endif
		};
		verbl_msg = arrest_msg[rn2(3)];
	    }
	    break;
#endif
	case MS_BRIBE:
	    if (mtmp->mpeaceful && !mtmp->mtame) {
		(void) demon_talk(mtmp);
		break;
	    }
	    /* fall through */
	case MS_CUSS:
	    if (!mtmp->mpeaceful)
		cuss(mtmp);
	    break;
	case MS_SPELL:
	    /* deliberately vague, since it's not actually casting any spell */
/*JP
	    pline_msg = "seems to mutter a cantrip.";
*/
	    pline_msg = "�ԂԂƂԂ₢�Ă���D";
	    break;
	case MS_NURSE:
	    if (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep)))
/*JP
		verbl_msg = "Put that weapon away before you hurt someone!";
*/
		verbl_msg = "����������߂Ȃ����I����͐l����������̂�I";
	    else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf)
		verbl_msg = Role_if(PM_HEALER) ?
#if 0 /*JP*/
			  "Doc, I can't help you unless you cooperate." :
			  "Please undress so I can examine you.";
#else
			  "�搶�C���Ȃ��̋��͂Ȃ��ł͂ǂ����悤������܂����D" :
			  "����E���ł��������D���Ȃ���f�@���܂���D";
#endif
#ifdef TOURIST
	    else if (uarmu)
/*JP
		verbl_msg = "Take off your shirt, please.";
*/
		verbl_msg = "�V���c��E���ł��������D";
#endif
/*JP
	    else verbl_msg = "Relax, this won't hurt a bit.";
*/
	    else verbl_msg = "�������āD�����Ƃ��ɂ��Ȃ����D";
	    break;
	case MS_GUARD:
#ifndef GOLDOBJ
	    if (u.ugold)
#else
	    if (money_cnt(invent))
#endif
/*JP
		verbl_msg = "Please drop that gold and follow me.";
*/
		verbl_msg = "����u���Ă��Ă��Ă��������D";
	    else
/*JP
		verbl_msg = "Please follow me.";
*/
		verbl_msg = "���Ă��Ă��������D";
	    break;
	case MS_SOLDIER:
	    {
		static const char * const soldier_foe_msg[3] = {
#if 0 /*JP*/
		    "Resistance is useless!",
		    "You're dog meat!",
		    "Surrender!",
#else
		    "��R���Ă����ʂ��I",
		    "���ɋ��ꂿ�܂��I",
		    "�~������I",
#endif
		},		  * const soldier_pax_msg[3] = {
#if 0 /*JP*/
		    "What lousy pay we're getting here!",
		    "The food's not fit for Orcs!",
		    "My feet hurt, I've been on them all day!",
#else
		    "�Ȃ�ċ������������񂾁I",
		    "�Ȃ񂾂��H�I�[�N�ł��򂦂˂����I",
		    "�������䂵���C�����Ƃ��̒��q���I",
#endif
		};
		verbl_msg = mtmp->mpeaceful ? soldier_pax_msg[rn2(3)]
					    : soldier_foe_msg[rn2(3)];
	    }
	    break;
	case MS_RIDER:
		if (ptr == &mons[PM_DEATH] && !rn2(10))
/*JP
			pline_msg = "is busy reading a copy of Sandman #8.";
*/
			pline_msg = "Sandman��8�͂�ǂނ̂ɖZ�����D";
#if 0 /*JP*/
		else verbl_msg = (ptr == &mons[PM_DEATH]) ? "WHO DO YOU THINK YOU ARE, WAR?" : "Who do you think you are, War?";
#else
		else verbl_msg = (ptr == &mons[PM_DEATH])
			? "������*���҂�*���l�������Ƃ����邩�H *�E�H�[*��D"
			: "���������҂����l�������Ƃ����邩�H �E�H�[��D";
#endif
		break;
    }

/*JP
    if (pline_msg) pline("%s %s", Monnam(mtmp), pline_msg);
*/
    if (pline_msg) pline("%s��%s", Monnam(mtmp), pline_msg);
    else if (verbl_msg) verbalize(verbl_msg);
    return(1);
}


int
dotalk()
{
    int result;
    boolean save_soundok = flags.soundok;
    flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;
    return result;
}

static int
dochat()
{
    register struct monst *mtmp;
    register int tx,ty;
    struct obj *otmp;

    if (is_silent(youmonst.data)) {
/*JP
	pline("As %s, you cannot speak.", an(youmonst.data->mname));
*/
	pline("���Ȃ���%s�Ȃ̂ŁC�b�����Ƃ��ł��Ȃ��D", jtrns_mon_gen(youmonst.data->mname, flags.female));
	return(0);
    }
    if (Strangled) {
/*JP
	You_cant("speak.  You're choking!");
*/
	You("�b���Ȃ��D���Ȃ��͎���i�߂��Ă���I");
	return(0);
    }
    if (u.uswallow) {
/*JP
	pline("They won't hear you out there.");
*/
	You("�O�֌����Ęb���������C�N����������Ȃ������D");
	return(0);
    }
    if (Underwater) {
/*JP
	Your("speech is unintelligible underwater.");
*/
	pline("���ʉ��ł́C���Ȃ��̘b�͂낭�ɗ�������Ȃ��D");
	return(0);
    }

    if (!Blind && (otmp = shop_object(u.ux, u.uy)) != (struct obj *)0) {
	/* standing on something in a shop and chatting causes the shopkeeper
	   to describe the price(s).  This can inhibit other chatting inside
	   a shop, but that shouldn't matter much.  shop_object() returns an
	   object iff inside a shop and the shopkeeper is present and willing
	   (not angry) and able (not asleep) to speak and the position contains
	   any objects other than just gold.
	*/
	price_quote(otmp);
	return(1);
    }

/*JP
    if (!getdir("Talk to whom? (in what direction)")) {
*/
    if (!getdir("�N�Ƙb���܂����H[���������Ă�]")) {
	/* decided not to chat */
	return(0);
    }

#ifdef STEED
    if (u.usteed && u.dz > 0)
	return (domonnoise(u.usteed));
#endif
    if (u.dz) {
/*JP
	pline("They won't hear you %s there.", u.dz < 0 ? "up" : "down");
*/
	pline("%s�����Ęb�����Ă��Ӗ����Ȃ��D", u.dz < 0 ? "���" : "����");
	return(0);
    }

    if (u.dx == 0 && u.dy == 0) {
/*
 * Let's not include this.  It raises all sorts of questions: can you wear
 * 2 helmets, 2 amulets, 3 pairs of gloves or 6 rings as a marilith,
 * etc...  --KAA
	if (u.umonnum == PM_ETTIN) {
	    You("discover that your other head makes boring conversation.");
	    return(1);
	}
*/
/*JP
	pline("Talking to yourself is a bad habit for a dungeoneer.");
*/
	pline("���{�T���҂ɂƂ��Ĉ�l���͈����Ȃ��D");
	return(0);
    }

    tx = u.ux+u.dx; ty = u.uy+u.dy;
    mtmp = m_at(tx, ty);

    if (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT)
	return(0);

    /* sleeping monsters won't talk, except priests (who wake up) */
    if ((!mtmp->mcanmove || mtmp->msleeping) && !mtmp->ispriest) {
	/* If it is unseen, the player can't tell the difference between
	   not noticing him and just not existing, so skip the message. */
	if (canspotmon(mtmp))
/*JP
	    pline("%s seems not to notice you.", Monnam(mtmp));
*/
	    pline("%s�͂��Ȃ��ɋC�����Ă��Ȃ��悤���D", Monnam(mtmp));
	return(0);
    }

    /* if this monster is waiting for something, prod it into action */
    mtmp->mstrategy &= ~STRAT_WAITMASK;

    if (mtmp->mtame && mtmp->meating) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
/*JP
	pline("%s is eating noisily.", Monnam(mtmp));
*/
	pline("%s�̓o���o���ƕ���H�ׂĂ���D", Monnam(mtmp));
	return (0);
    }

    return domonnoise(mtmp);
}

#ifdef USER_SOUNDS

extern void FDECL(play_usersound, (const char*, int));

typedef struct audio_mapping_rec {
#ifdef USER_SOUNDS_REGEX
	struct re_pattern_buffer regex;
#else
	char *pattern;
#endif
	char *filename;
	int volume;
	struct audio_mapping_rec *next;
} audio_mapping;

static audio_mapping *soundmap = 0;

char* sounddir = ".";

/* adds a sound file mapping, returns 0 on failure, 1 on success */
int
add_sound_mapping(mapping)
const char *mapping;
{
	char text[256];
	char filename[256];
	char filespec[256];
	int volume;

	if (sscanf(mapping, "MESG \"%255[^\"]\"%*[\t ]\"%255[^\"]\" %d",
		   text, filename, &volume) == 3) {
	    const char *err;
	    audio_mapping *new_map;

	    if (strlen(sounddir) + strlen(filename) > 254) {
		raw_print("sound file name too long");
		return 0;
	    }
	    Sprintf(filespec, "%s/%s", sounddir, filename);

	    if (can_read_file(filespec)) {
		new_map = (audio_mapping *)alloc(sizeof(audio_mapping));
#ifdef USER_SOUNDS_REGEX
		new_map->regex.translate = 0;
		new_map->regex.fastmap = 0;
		new_map->regex.buffer = 0;
		new_map->regex.allocated = 0;
		new_map->regex.regs_allocated = REGS_FIXED;
#else
		new_map->pattern = (char *)alloc(strlen(text) + 1);
		Strcpy(new_map->pattern, text);
#endif
		new_map->filename = strdup(filespec);
		new_map->volume = volume;
		new_map->next = soundmap;

#ifdef USER_SOUNDS_REGEX
		err = re_compile_pattern(text, strlen(text), &new_map->regex);
#else
		err = 0;
#endif
		if (err) {
		    raw_print(err);
		    free(new_map->filename);
		    free(new_map);
		    return 0;
		} else {
		    soundmap = new_map;
		}
	    } else {
		Sprintf(text, "cannot read %.243s", filespec);
		raw_print(text);
		return 0;
	    }
	} else {
	    raw_print("syntax error in SOUND");
	    return 0;
	}

	return 1;
}

void
play_sound_for_message(msg)
const char* msg;
{
	audio_mapping* cursor = soundmap;

	while (cursor) {
#ifdef USER_SOUNDS_REGEX
	    if (re_search(&cursor->regex, msg, strlen(msg), 0, 9999, 0) >= 0) {
#else
	    if (pmatch(cursor->pattern, msg)) {
#endif
		play_usersound(cursor->filename, cursor->volume);
	    }
	    cursor = cursor->next;
	}
}

#endif /* USER_SOUNDS */

#endif /* OVLB */

/*sounds.c*/
