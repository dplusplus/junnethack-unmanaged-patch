/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2003
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"
#include "func_tab.h"
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int NDECL(wiz_debug_cmd);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply); /**/
extern int NDECL(dorub); /**/
extern int NDECL(dojump); /**/
extern int NDECL(doextlist); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern int NDECL(dotakeoff); /**/
extern int NDECL(doremring); /**/
extern int NDECL(dowear); /**/
extern int NDECL(doputon); /**/
extern int NDECL(doddoremarm); /**/
extern int NDECL(dokick); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dothrow); /**/
extern int NDECL(doeat); /**/
extern int NDECL(done2); /**/
extern int NDECL(doengrave); /**/
extern int NDECL(dopickup); /**/
extern int NDECL(ddoinv); /**/
extern int NDECL(dotypeinv); /**/
extern int NDECL(dolook); /**/
extern int NDECL(doprgold); /**/
extern int NDECL(doprwep); /**/
extern int NDECL(doprarm); /**/
extern int NDECL(doprring); /**/
extern int NDECL(dopramulet); /**/
extern int NDECL(doprtool); /**/
extern int NDECL(dosuspend); /**/
extern int NDECL(doforce); /**/
extern int NDECL(doopen); /**/
extern int NDECL(doclose); /**/
extern int NDECL(dosh); /**/
extern int NDECL(dodiscovered); /**/
extern int NDECL(doset); /**/
extern int NDECL(dotogglepickup); /**/
extern int NDECL(dowhatis); /**/
extern int NDECL(doquickwhatis); /**/
extern int NDECL(dowhatdoes); /**/
extern int NDECL(dohelp); /**/
extern int NDECL(dohistory); /**/
extern int NDECL(doloot); /**/
extern int NDECL(dodrink); /**/
extern int NDECL(dodip); /**/
extern int NDECL(dosacrifice); /**/
extern int NDECL(dopray); /**/
extern int NDECL(doturn); /**/
extern int NDECL(doredraw); /**/
extern int NDECL(doread); /**/
extern int NDECL(dosave); /**/
extern int NDECL(dosearch); /**/
extern int NDECL(doidtrap); /**/
extern int NDECL(dopay); /**/
extern int NDECL(dosit); /**/
extern int NDECL(dotalk); /**/
extern int NDECL(docast); /**/
extern int NDECL(dovspell); /**/
extern int NDECL(dotele); /**/
extern int NDECL(dountrap); /**/
extern int NDECL(doversion); /**/
extern int NDECL(doextversion); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(dowield); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dozap); /**/
extern int NDECL(doorganize); /**/

#ifdef LIVELOG_SHOUT
extern int NDECL(doshout); /**/
#endif

#endif /* DUMB */

#ifdef OVL1
static int NDECL((*timed_occ_fn));
#endif /* OVL1 */

STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
STATIC_PTR int NDECL(domonability);
STATIC_PTR int NDECL(dooverview_or_wiz_where);
STATIC_PTR int NDECL(dotravel);
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
STATIC_PTR int NDECL(wiz_panic);
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_mon_polycontrol);
STATIC_PTR int NDECL(wiz_show_wmodes);
STATIC_PTR int NDECL(wiz_mazewalkmap);
extern char SpLev_Map[COLNO][ROWNO];
STATIC_PTR int NDECL(wiz_showkills);	/* showborn patch */
#ifdef SHOW_BORN
extern void FDECL(list_vanquished, (int, BOOLEAN_P)); /* showborn patch */
#endif /* SHOW_BORN */
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int NDECL(wiz_migrate_mons);
#endif
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *, long *, long *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(contained, (winid, const char *, long *, long *));
STATIC_PTR int NDECL(wiz_show_stats);
#  ifdef PORT_DEBUG
STATIC_DCL int NDECL(wiz_port_debug);
#  endif
# endif
STATIC_PTR int NDECL(enter_explore_mode);
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(doconduct); /**/
STATIC_PTR boolean NDECL(minimal_enlightenment);

#ifdef OVLB
STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
#ifdef UNIX
static void NDECL(end_of_input);
#endif
#endif /* OVLB */

static const char* readchar_queue="";

STATIC_DCL char *NDECL(parse);
STATIC_DCL boolean FDECL(help_dir, (CHAR_P,const char *));

#ifdef OVL1

STATIC_PTR int
doprev_message()
{
    return nh_doprev_message();
}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int NDECL((*fn));
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

#ifdef REDO

static char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

	    retval = (*extcmdlist[idx].ef_funct)();
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
/*JP
	putstr(datawin, 0, "            Extended Commands List");
*/
	putstr(datawin, 0, "            拡張コマンド一覧");
	putstr(datawin, 0, "");
/*JP
	putstr(datawin, 0, "    Press '#', then type:");
*/
	putstr(datawin, 0, "    '#'を押したあとタイプせよ:");
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
		Sprintf(buf, "    %-15s - %s.", efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

#ifdef TTY_GRAPHICS
#define MAX_EXT_CMD 40		/* Change if we ever have > 40 ext cmds */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				Sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    warning("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			Sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    Sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
/*JP
		    Sprintf(prompt, "%s or %s", choices[i-1]->ef_txt,
*/
		    Sprintf(prompt, "%s または %s", choices[i-1]->ef_txt,
				choices[i]->ef_txt);
		} else {
/*JP
		    Strcat(prompt," or ");
*/
		    Strcat(prompt," または ");
		    Strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		Sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
/*JP
	    Sprintf(prompt, "Extended Command: %s", cbuf);
*/
	    Sprintf(prompt, "拡張コマンド: %s", cbuf);
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((genericptr_t)pick_list);
#ifdef DEBUG
			warning("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((genericptr_t)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

/* #monster command - use special monster ability while polymorphed */
STATIC_PTR int
domonability()
{
	if (can_breathe(youmonst.data)) return dobreathe();
	else if (attacktype(youmonst.data, AT_SPIT)) return dospit();
	else if (youmonst.data->mlet == S_NYMPH) return doremove();
	else if (attacktype(youmonst.data, AT_GAZE)) return dogaze();
	else if (is_were(youmonst.data)) return dosummon();
	else if (webmaker(youmonst.data)) return dospinweb();
	else if (is_hider(youmonst.data)) return dohide();
	else if (is_mind_flayer(youmonst.data)) return domindblast();
	else if (u.umonnum == PM_GREMLIN) {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		if (split_mon(&youmonst, (struct monst *)0))
		    dryup(u.ux, u.uy, TRUE);
/*JP
	    } else There("is no fountain here.");
*/
	    } else pline("ここには泉はない。");
	} else if (is_unicorn(youmonst.data)) {
	    fix_attributes_and_properties((struct obj *)0,0);
	    return 1;
	} else if (youmonst.data->msound == MS_SHRIEK) {
/*JP
	    You("shriek.");
*/
	    You("金切り声をあげた。");
	    if(u.uburied)
/*JP
		pline("Unfortunately sound does not carry well through rock.");
*/
		pline("残念ながら音は岩をうまく伝わらない。");
	    else aggravate();
	} else if (Upolyd)
/*JP
		pline("Any special ability you may have is purely reflexive.");
*/
		pline("あなたの持っている特殊能力はどれも受動的だ。");
/*JP
	else You("don't have a special ability in your normal form!");
*/
	else You("特殊能力を持っていない！");
	return 0;
}

STATIC_PTR int
enter_explore_mode()
{
	if(!discover && !wizard) {
/*JP
		pline("Beware!  From explore mode there will be no return to normal game.");
*/
		pline("警告！発見モードに入ったら通常モードには戻れない。");
/*JP
		if (paranoid_yn("Do you want to enter explore mode?", iflags.paranoid_quit) == 'y') {
*/
		if (paranoid_yn("発見モードに移りますか？", iflags.paranoid_quit) == 'y') {
			clear_nhwindow(WIN_MESSAGE);
/*JP
			You("are now in non-scoring explore mode.");
*/
			You("スコアがのらない発見モードに移行した。");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
/*JP
			pline("Resuming normal game.");
*/
			pline("通常モードを続ける。");
		}
	}
	return 0;
}

STATIC_PTR int
dooverview_or_wiz_where()
{
#ifdef WIZARD
	//if (wizard) return wiz_where();
	//else
#endif
	dooverview();
	return 0;
}

#ifdef WIZARD

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
	    makewish(TRUE);
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
/*JP
	    pline("Unavailable command '^W'.");
*/
	    pline("'^W'コマンドは使えない。");
	return 0;
}

/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
/*JP
	else		pline("Unavailable command '^I'.");
*/
	else		pline("'^I'コマンドは使えない。");
	return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{
	if (wizard) {
	    struct trap *t;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
	    for (t = ftrap; t != 0; t = t->ntrap) {
		t->tseen = 1;
		map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
/*JP
	    pline("Unavailable command '^F'.");
*/
	    pline("'^F'コマンドは使えない。");
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
/*JP
	else		pline("Unavailable command '^G'.");
*/
	else		pline("'^G'コマンドは使えない。");
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, (schar *)0, (xchar *)0);
/*JP
	else	    pline("Unavailable command '^O'.");
*/
	else	    pline("'^O'コマンドは使えない。");
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
/*JP
	else	    pline("Unavailable command '^E'.");
*/
	else	    pline("'^E'コマンドは使えない。");
	return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
/*JP
	else		pline("Unavailable command '^V'.");
*/
	else		pline("'^V'コマンドは使えない。");
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
#if 0 /*JP*/
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
#else
    pline("怪物の変化制御: %s",
	  iflags.mon_polycontrol ? "オン" : "オフ");
#endif
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

/*JP
    getlin("To what experience level do you want to be set?", buf);
*/
    getlin("経験レベルをいくつに設定しますか？", buf);
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
	pline(Never_mind);
	return 0;
    }
    if (newlevel == u.ulevel) {
/*JP
	You("are already that experienced.");
*/
	You("すでにその経験レベルだ。");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
/*JP
	    You("are already as inexperienced as you can get.");
*/
	    You("すでに最低の経験レベルだ。");
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
/*JP
	    losexp("#levelchange");
*/
	    losexp("#levelchangeコマンドで");
    } else {
	if (u.ulevel >= MAXULEV) {
/*JP
	    You("are already as experienced as you can get.");
*/
	    You("すでに最大の経験レベルだ。");
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
/*JP
	if (yn("Do you want to call panic() and end your game?") == 'y')
*/
	if (yn("panic()関数を呼び出してゲームを終了させますか？") == 'y')
		panic("crash test.");
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			Sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #showkills command */
STATIC_PTR int wiz_showkills()		/* showborn patch */
{
	list_vanquished('y', FALSE);
	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
#if 0 /*JP*/
	You_[] = "You ",
	are[]  = "are ",  were[]  = "were ",
	have[] = "have ", had[]   = "had ",
	can[]  = "can ",  could[] = "could ";
#else
	You_[] = "あなたは",
	are[]  = "である",       were[]  = "であった",
	have[] = "をもっている", had[]   = "をもっていた",
	can[]  = "できる",       could[] = "できた",
	iru[]  = "いる",         ita[]   = "いた";
#endif
#if 0 /*JP*/
static const char
	have_been[]  = "have been ",
	have_never[] = "have never ", never[] = "never ";
#endif

#if 0 /*JP*/
#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#else
#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, suffix, final ? past : present)
#endif
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#if 0 /*JP*/
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)
#else
#define you_are_ing(goodthing) enl_msg(You_,iru,ita,goodthing)
#endif

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

#if 0 /*JP*/
	Sprintf(buf, "%s%s%s.", start, middle, end);
#else
	Sprintf(buf, "%s%s%s。", start, middle, end);
#endif
	putstr(en_win, 0, buf);
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    Sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

#if 0 /*JP*/
	    if (absamt <= 3) modif = "small";
	    else if (absamt <= 6) modif = "moderate";
	    else if (absamt <= 12) modif = "large";
	    else modif = "huge";
#else
	    if (absamt <= 3) modif = "僅かな";
	    else if (absamt <= 6) modif = "中程度の";
	    else if (absamt <= 12) modif = "大きな";
	    else modif = "強大な";
#endif
	}
#if 0 /*JP*/
	bonus = (incamt > 0) ? "bonus" : "penalty";
#else
	bonus = (incamt > 0) ? "ボーナス" : "ペナルティ";
#endif
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
#if 0 /*JP*/
	Sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp);
#else
	Sprintf(outbuf, "%sに%s%s", inctyp, modif, bonus);
#endif
	return outbuf;
}

void
enlightenment(final)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
/*JP
	putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:");
*/
	putstr(en_win, 0, final ? "最終属性：" : "現在の属性：");
	putstr(en_win, 0, "");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
#if 0 /*JP*/
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
#else
				"エルベレスの御手",
				"調和の使者",
				"アリオッチの名誉"
#endif
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
/*JP
	if (u.ualign.record >= 20)	you_are("piously aligned");
*/
	if (u.ualign.record >= 20)	you_have("敬虔な信仰心");
/*JP
	else if (u.ualign.record > 13)	you_are("devoutly aligned");
*/
	else if (u.ualign.record > 13)	you_have("心からの信仰心");
/*JP
	else if (u.ualign.record > 8)	you_are("fervently aligned");
*/
	else if (u.ualign.record > 8)	you_have("熱心な信仰心");
/*JP
	else if (u.ualign.record > 3)	you_are("stridently aligned");
*/
	else if (u.ualign.record > 3)	you_have("おおげさな信仰心");
/*JP
	else if (u.ualign.record == 3)	you_are("aligned");
*/
	else if (u.ualign.record == 3)	you_have("普通の信仰心");
/*JP
	else if (u.ualign.record > 0)	you_are("haltingly aligned");
*/
	else if (u.ualign.record > 0)	you_have("躊躇しながらも信仰心");
/*JP
	else if (u.ualign.record == 0)	you_are("nominally aligned");
*/
	else if (u.ualign.record == 0)	you_have("形だけの信仰心");
/*JP
	else if (u.ualign.record >= -3)	you_have("strayed");
*/
	else if (u.ualign.record >= -3)	you_have("信仰に迷い");
/*JP
	else if (u.ualign.record >= -8)	you_have("sinned");
*/
	else if (u.ualign.record >= -8)	you_are_ing("罪を負って");
/*JP
	else you_have("transgressed");
*/
	else you_are_ing("信仰から逸脱して");
#ifdef WIZARD
	if (wizard) {
#if 0 /*JP*/
		Sprintf(buf, " %d / %ld", u.ualign.record, ALIGNLIM);
		enl_msg("Your alignment ", "is", "was", buf);
#else
		Sprintf(buf, "あなたの属性値は%d", u.ualign.record);
		enl_msg(buf, "である", "だった", "");
#endif
#if 0 /*JP*/
		Sprintf(buf, " %d - %d",
		        min_monster_difficulty(), max_monster_difficulty());
		enl_msg("Monster difficulty range ", "is", "was", buf);
#else
		Sprintf(buf, "怪物の難易度範囲は%d～%d",
		        min_monster_difficulty(), max_monster_difficulty());
		enl_msg(buf, "である", "だった", "");
#endif
#if 0 /*JP*/
		Sprintf(buf, " %d", level_difficulty());
		enl_msg("Level difficulty ", "is", "was", buf);
#else
		Sprintf(buf, "レベル難易度は%d", level_difficulty());
		enl_msg(buf, "である", "だった", "");
#endif
	}
#endif

	/*** Resistances to troubles ***/
#if 0 /*JP:T*/
	if (Fire_resistance) you_are("fire resistant");
	if (Cold_resistance) you_are("cold resistant");
	if (Sleep_resistance) you_are("sleep resistant");
	if (Disint_resistance) you_are("disintegration-resistant");
	if (Shock_resistance) you_are("shock resistant");
	if (Poison_resistance) you_are("poison resistant");
	if (Drain_resistance) you_are("level-drain resistant");
	if (Sick_resistance) you_are("immune to sickness");
	if (Antimagic) you_are("magic-protected");
	if (Acid_resistance) you_are("acid resistant");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Invulnerable) you_are("invulnerable");
	if (u.uedibility) you_can("recognize detrimental food");
#else
	if (Fire_resistance) you_have("火への耐性");
	if (Cold_resistance) you_have("寒さへの耐性");
	if (Sleep_resistance) you_have("眠りへの耐性");
	if (Disint_resistance) you_have("粉砕への耐性");
	if (Shock_resistance) you_have("電撃への耐性");
	if (Poison_resistance) you_have("毒への耐性");
	if (Drain_resistance) you_have("レベルダウンへの耐性");
	if (Sick_resistance) you_have("病気に対する免疫");
	if (Antimagic) you_have("魔法防御能力");
	if (Acid_resistance) you_have("酸への耐性");
	if (Stone_resistance)
		you_have("石化への耐性");
	if (Invulnerable) you_are("不死身");
	if (u.uedibility) you_can("有害な食料を識別");
#endif

	/*** Troubles ***/
	if (Halluc_resistance)
/*JP
		enl_msg("You resist", "", "ed", " hallucinations");
*/
		you_have("幻覚への耐性");
	if (final) {
/*JP
		if (Hallucination) you_are("hallucinating");
*/
		if (Hallucination) you_are("幻覚状態");
/*JP
		if (Stunned) you_are("stunned");
*/
		if (Stunned) you_are("くらくら状態");
/*JP
		if (Confusion) you_are("confused");
*/
		if (Confusion) you_are("混乱状態");
/*JP
		if (Blinded) you_are("blinded");
*/
		if (Blinded) you_are("盲目");
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE)
/*JP
				you_are("sick from food poisoning");
*/
				enl_msg("あなたは食中毒で気分が悪", "い", "かった", "");
			if (u.usick_type & SICK_NONVOMITABLE)
/*JP
				you_are("sick from illness");
*/
				enl_msg("あなたは病気で気分が悪", "い", "かった", "");
		}
	}
/*JP
	if (Stoned) you_are("turning to stone");
*/
	if (Stoned) enl_msg("あなたは石になっ", "た", "ている", "");
/*JP
	if (Slimed) you_are("turning into slime");
*/
	if (Slimed) you_are_ing("スライム状になって");
/*JP
	if (Strangled) you_are((u.uburied) ? "buried" : "being strangled");
*/
	if (Strangled) you_are_ing((u.uburied) ? "窒息して" : "首を絞められて");
	if (Glib) {
#if 0 /*JP:T*/
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		you_have(buf);
#else
		Sprintf(buf, "%sがぬるぬるして", body_part(FINGER));
		enl_msg(buf, iru, ita, "");
#endif
	}
/*JP
	if (Fumbling) enl_msg("You fumble", "", "d", "");
*/
	if (Fumbling) you_are_ing("不器用になって");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
#if 0 /*JP:T*/
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		you_have(buf);
#else
		Sprintf(buf, "%sを怪我して", makeplural(body_part(LEG)));
		you_are_ing(buf);
#endif
	}
#if defined(WIZARD) && defined(STEED)
	if (Wounded_legs && u.usteed && wizard) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
#if 0 /*JP:T*/
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
#else
	    enl_msg(buf, iru, ita, "は肢を怪我して");
#endif
	}
#endif
/*JP
	if (Sleeping) enl_msg("You ", "fall", "fell", " asleep");
*/
	if (Sleeping) you_are_ing("眠って");
/*JP
	if (Hunger) enl_msg("You hunger", "", "ed", " rapidly");
*/
	if (Hunger) enl_msg("あなたはすぐに腹が減る状態", "である", "だった", "");

	/*** Vision and senses ***/
/*JP
	if (See_invisible) enl_msg(You_, "see", "saw", " invisible");
*/
	if (See_invisible) enl_msg("あなたは透明なものを見られ", "る", "た", "");
/*JP
	if (Blind_telepat) you_are("telepathic");
*/
	if (Blind_telepat) you_have("テレパシー");
/*JP
	if (Warning) you_are("warned");
*/
	if (Warning) you_have("警戒能力");
	if (Warn_of_mon && flags.warntype) {
#if 0 /*JP*/
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			(flags.warntype & M2_GIANT) ? "giants" :
			(flags.warntype & M2_WERE) ? "lycanthropes" :
			(flags.warntype & M2_UNDEAD) ? "undead" :
			something);
#else
		Sprintf(buf, "%sの存在を感じる能力",
			(flags.warntype & M2_ORC) ? "オーク" :
			(flags.warntype & M2_DEMON) ? "悪魔" :
			(flags.warntype & M2_GIANT) ? "巨人" :
			(flags.warntype & M2_WERE) ? "獣人" :
			(flags.warntype & M2_UNDEAD) ? "不死の生物" :
			something);
#endif
		you_are(buf);
	} else if (Warn_of_mon && uwep) {
		const char *monster_name = get_warned_of_monster(uwep);
		if (monster_name) {
/*JP
			Sprintf(buf, "aware of the presence of %s", monster_name);
*/
			Sprintf(buf, "%sの存在を感じる能力", monster_name);
			you_are(buf);
		}
	}
/*JP
	if (Undead_warning) you_are("warned of undead");
*/
	if (Undead_warning) you_have("不死の生物への警戒能力");
/*JP
	if (Searching) you_have("automatic searching");
*/
	if (Searching) you_have("探査能力");
/*JP
	if (Clairvoyant) you_are("clairvoyant");
*/
	if (Clairvoyant) you_have("千里眼能力");
/*JP
	if (Infravision) you_have("infravision");
*/
	if (Infravision) you_have("赤外線を見る能力");
/*JP
	if (Detect_monsters) you_are("sensing the presence of monsters");
*/
	if (Detect_monsters) you_have("怪物を探す能力");
/*JP
	if (u.umconf) you_are("going to confuse monsters");
*/
	if (u.umconf) you_have("怪物を混乱させる能力");

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
/*JP
		you_are("poorly adorned");
*/
		you_are_ing("みすぼらしく着飾って");
	    else
/*JP
		you_are("adorned");
*/
		you_are_ing("着飾って");
	}
#if 0 /*JP*/
	if (Invisible) you_are("invisible");
	else if (Invis) you_are("invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    you_are("visible");
	if (Displaced) you_are("displaced");
	if (Stealth) you_are("stealthy");
	if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if (Conflict) enl_msg("You cause", "", "d", " conflict");
#else
	if (Invisible) you_are("透明");
	else if (Invis) you_are("他人に対して透明");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    you_are("不透明");
	if (Displaced) you_have("幻影能力");
	if (Stealth) you_have("人目を盗む能力");
	if (Aggravate_monster) you_are_ing("反感をかって");
	if (Conflict) you_are_ing("闘争を引き起こして");
#endif

	/*** Transportation ***/
#if 0 /*JP*/
	if (Jumping) you_can("jump");
	if (Teleportation) you_can("teleport");
	if (Teleport_control) you_have("teleport control");
	if (Lev_at_will) you_are("levitating, at will");
	else if (Levitation) you_are("levitating");	/* without control */
	else if (Flying) you_can("fly");
	if (Wwalking) you_can("walk on water");
	if (Swimming) you_can("swim");        
	if (Breathless) you_can("survive without air");
	else if (Amphibious) you_can("breathe water");
	if (Passes_walls) you_can("walk through walls");
#else
	if (Jumping) you_can("跳躍することが");
	if (Teleportation) you_can("瞬間移動が");
	if (Teleport_control) you_have("瞬間移動の制御能力");
	if (Lev_at_will) you_have("上手く浮遊する能力");
	else if (Levitation) you_are("浮遊状態");	/* without control */
	else if (Flying) you_can("飛ぶことが");
	if (Wwalking) you_can("水の上を歩くことが");
	if (Swimming) you_can("泳ぐことが");        
	if (Breathless) you_can("空気なしで生き延びることが");
	else if (Amphibious) you_can("水中で呼吸が");
	if (Passes_walls) you_can("壁を通り抜けることが");
#endif
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
#if 0 /*JP*/
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
#else
	if (u.usteed && (final < 2 || strcmp(killer, "乗りものから落ちて"))) {
	    Sprintf(buf, "%sに乗って", y_monnam(u.usteed));
	    you_are_ing(buf);
	}
#endif
#endif
	if (u.uswallow) {
#if 0 /*JP*/
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    you_are(buf);
#else /*JP*/
	    Sprintf(buf, "%sに飲み込まれて", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), "(%u)", u.uswldtim);
#endif
	    you_are_ing(buf);
#endif /*JP*/
	} else if (u.ustuck) {
#if 0 /*JP*/
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    you_are(buf);
#else
	    Sprintf(buf, "%s%s",
		    a_monnam(u.ustuck),
		    (Upolyd && sticks(youmonst.data)) ? "を捕まえて" : "に捕まって");
	    you_are_ing(buf);
#endif
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
/*JP
	    you_have(enlght_combatinc("to hit", u.uhitinc, final, buf));
*/
	    you_have(enlght_combatinc("命中率", u.uhitinc, final, buf));
	if (u.udaminc)
/*JP
	    you_have(enlght_combatinc("damage", u.udaminc, final, buf));
*/
	    you_have(enlght_combatinc("ダメージ", u.udaminc, final, buf));
/*JP
	if (Slow_digestion) you_have("slower digestion");
*/
	if (Slow_digestion) enl_msg("食物の消化が遅", "い", "かった", "");
/*JP
	if (Regeneration) enl_msg("You regenerate", "", "d", "");
*/
	if (Regeneration) you_have("再生能力");
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;

	    if (prot < 0)
/*JP
		you_are("ineffectively protected");
*/
		you_are_ing("不十分に守られて");
	    else
/*JP
		you_are("protected");
*/
		you_are_ing("守られて");
	}
#if 0 /*JP*/
	if (Protection_from_shape_changers)
		you_are("protected from shape changers");
	if (Polymorph) you_are("polymorphing");
	if (Polymorph_control) you_have("polymorph control");
#else
	if (Protection_from_shape_changers)
		you_have("変化怪物への耐性");
	if (Polymorph) you_are_ing("変化して");
	if (Polymorph_control) you_have("変化の制御能力");
#endif
	if (u.ulycn >= LOW_PM) {
/*JP
		Strcpy(buf, an(mons[u.ulycn].mname));
*/
		Strcpy(buf, jtrns_mon_gen(mons[u.ulycn].mname, flags.female));
		you_are(buf);
	}
	if (Upolyd) {
#if 0 /*JP*/
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#else
	    if (u.ulycn >= LOW_PM) Strcpy(buf, "獣の姿を");
	    else Sprintf(buf, "%sに変化",
		jtrns_mon_gen(youmonst.data->mname, youmonst.female));
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
#if 0 /*JP*/
	    you_are(buf);
#else
	    enl_msg(buf, "している", "していた", "");
#endif
	}
#if 0 /*JP*/
	if (Unchanging) you_can("not change from your current form");
	if (Fast) you_are(Very_fast ? "very fast" : "fast");
	if (Reflecting) you_have("reflection");
	if (Free_action) you_have("free action");
	if (Fixed_abil) you_have("fixed abilities");
	if (Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if (u.twoweap) you_are("wielding two weapons at once");
#else
	if (Unchanging) enl_msg("今の姿から変化することができな",
				"い", "かった", "");
	if (Fast) you_have(Very_fast ? "とても素早く行動する能力" :
			   "素早く行動する能力");
	if (Reflecting) you_have("反射能力");
	if (Free_action) you_have("拘束されない能力");
	if (Fixed_abil) enl_msg("能力が変化しな", "い", "かった", "");
	if (Lifesaved)
		enl_msg("あなたの生命は保存されて", iru, ita,"");
	if (u.twoweap) you_are("二刀流");
#endif

	/*** Miscellany ***/
#if 0 /*JP*/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", Luck);
#endif
	    you_are(buf);
	}
#else
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%s",
		    ltmp >= 10 ? "猛烈に" : ltmp >= 5 ? "とても" : "",
		    Luck < 0 ? "不幸" : "幸福");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), "(%d)", Luck);
#endif
	    you_are(buf);
	}
#endif
#ifdef WIZARD
/*JP
	 else if (wizard) enl_msg("Your luck ", "is", "was", " zero");
*/
	 else if (wizard) enl_msg("あなたの運はゼロ", "である", "だった", "");
#endif
#if 0 /*JP*/
	if (u.moreluck > 0) you_have("extra luck");
	else if (u.moreluck < 0) you_have("reduced luck");
#else
 	if (u.moreluck > 0) you_have("さらなる幸運");
	else if (u.moreluck < 0) you_have("さらなる悪運");
#endif
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
/*JP
		enl_msg("Bad luck ", "does", "did", " not time out for you");
*/
		enl_msg("悪運は時間切れにならな", "い", "かった", "");
	    if (ltmp >= 0)
/*JP
		enl_msg("Good luck ", "does", "did", " not time out for you");
*/
		enl_msg("幸運は時間切れにならな", "い", "かった", "");
	}

#if 0 /*JP*/
	if (u.ugangr) {
	    Sprintf(buf, " %sangry with you",
		    u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    enl_msg(u_gname(), " is", " was", buf);
	} else
#else
	if (u.ugangr) {
	    Sprintf(buf, "%sは%s怒って", u_gname(),
		    u.ugangr > 6 ? "猛烈に" : u.ugangr > 3 ? "とても" : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), "(%d)", u.ugangr);
#endif
	    enl_msg(buf, iru, ita, "");
	} else
#endif
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (!final) {
#if 0 /*JP*/
#if 0
	    /* "can [not] safely pray" vs "could [not] have safely prayed" */
	    Sprintf(buf, "%s%ssafely pray%s", can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");
#else
	    Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not ");
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif
	    you_can(buf);
#else /*JP*/
	    Strcpy(buf, "あなたは安全に祈ることが");
	    if(can_pray(FALSE))
	      enl_msg(buf, "できる", "できた", "");
	    else
	      enl_msg(buf, "できない", "できなかった", "");
#endif
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
#if 0 /*JP*/
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
#else
	    p = "死んだ後復活していた";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "生き延びた";  break;
	    case 1:  Strcpy(buf, "一度");  break;
	    case 2:  Strcpy(buf, "二度");  break;
	    case 3:  Strcpy(buf, "三度");  break;
	    default: Sprintf(buf, "%d回", u.umortality);
		     break;
#endif
	    }
	} else {		/* game ended in character's death */
#if 0 /*JP*/
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  warning("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
#else
	    p = "死んでいる";
	    switch (u.umortality) {
	    case 0:  warning("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, "(%d回！)", u.umortality);
		     break;
	}
#endif
	}
/*JP
	if (p) enl_msg(You_, "have been killed ", p, buf);
*/
	if (p) enl_msg(You_, "死んでいる", p, buf);
    }

	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

#ifdef DUMP_LOG
void
dump_enlightenment(final)
int final;
{
	int ltmp;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	const char *enc_stat[] = { /* copied from botl.c */
#if 0 /*JP*/
	     "",
	     "burdened",
	     "stressed",
	     "strained",
	     "overtaxed",
	     "overloaded"
#else
	     "",
	     "よろめき",
	     "圧迫",
	     "限界",
	     "荷重",
	     "超過"
#endif
	};
#if 0 /*JP*/
	char *youwere = "  You were ";
	char *youhave = "  You have ";
	char *youhad  = "  You had ";
	char *youcould = "  You could ";
#else
	char *youwere, *youhad, *youcould;
	youwere = youhad = youcould = "  あなたは";
#define DE_WERE "であった"
#define DE_HAD "をもっていた"
#define DE_COULD "できた"
#define DE_WERE_ING "いた"
#endif

/*JP
	dump("", "Final attributes");
*/
	dump("", "最終属性");

#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
#if 0 /*JP*/
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
#else
				"エルベレスの御手" DE_WERE,
				"調和の使者" DE_WERE,
				"アリオッチの名誉" DE_WERE
#endif
	    };
	    dump(youwere,
		(char *)hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
#endif

	if (u.ualign.record >= 20)
/*JP
		dump(youwere, "piously aligned");
*/
		dump(youhad, "敬虔な信仰心" DE_HAD);
	else if (u.ualign.record > 13)
/*JP
	    dump(youwere, "devoutly aligned");
*/
	    dump(youhad, "心からの信仰心" DE_HAD);
	else if (u.ualign.record > 8)
/*JP
	    dump(youwere, "fervently aligned");
*/
	    dump(youhad, "熱心な信仰心" DE_HAD);
	else if (u.ualign.record > 3)
/*JP
	    dump(youwere, "stridently aligned");
*/
	    dump(youhad, "おおげさな信仰心" DE_HAD);
	else if (u.ualign.record == 3)
/*JP
	    dump(youwere, "aligned");
*/
	    dump(youhad, "普通の信仰心" DE_HAD);
	else if (u.ualign.record > 0)
/*JP
	    dump(youwere, "haltingly aligned");
*/
	    dump(youhad, "躊躇しながらも信仰心" DE_HAD);
	else if (u.ualign.record == 0)
/*JP
	    dump(youwere, "nominally aligned");
*/
	    dump(youhad, "形だけの信仰心" DE_HAD);
/*JP
	else if (u.ualign.record >= -3)	dump(youhave, "strayed");
*/
	else if (u.ualign.record >= -3)	dump(youhad, "信仰に迷い" DE_HAD);
/*JP
	else if (u.ualign.record >= -8)	dump(youhave, "sinned");
*/
	else if (u.ualign.record >= -8)	dump(youhad, "罪を負って" DE_WERE_ING);
/*JP
	else dump("  You have ", "transgressed");
*/
	else dump(youhad, "信仰から逸脱して" DE_WERE_ING);
#if 0 /*JP*/
	Sprintf(buf, " %d", u.ualign.record);
	dump("  Your alignment was ", buf);
#else
	Sprintf(buf, "%dだった", u.ualign.record);
	dump("  あなたの属性値は", buf);
#endif


	/*** Resistances to troubles ***/
/*JP
	if (Fire_resistance) dump(youwere, "fire resistant");
*/
	if (Fire_resistance) dump(youhad, "火への耐性" DE_HAD);
/*JP
	if (Cold_resistance) dump(youwere, "cold resistant");
*/
	if (Cold_resistance) dump(youhad, "寒さへの耐性" DE_HAD);
/*JP
	if (Sleep_resistance) dump(youwere, "sleep resistant");
*/
	if (Sleep_resistance) dump(youhad, "眠りへの耐性" DE_HAD);
/*JP
	if (Disint_resistance) dump(youwere, "disintegration-resistant");
*/
	if (Disint_resistance) dump(youhad, "粉砕への耐性" DE_HAD);
/*JP
	if (Shock_resistance) dump(youwere, "shock resistant");
*/
	if (Shock_resistance) dump(youhad, "電撃への耐性" DE_HAD);
/*JP
	if (Poison_resistance) dump(youwere, "poison resistant");
*/
	if (Poison_resistance) dump(youhad, "毒への耐性" DE_HAD);
/*JP
	if (Drain_resistance) dump(youwere, "level-drain resistant");
*/
	if (Drain_resistance) dump(youhad, "レベルダウンへの耐性" DE_HAD);
/*JP
	if (Sick_resistance) dump(youwere, "immune to sickness");
*/
	if (Sick_resistance) dump(youhad, "病気に対する免疫" DE_HAD);
/*JP
	if (Acid_resistance) dump(youwere, "acid resistant");
*/
	if (Acid_resistance) dump(youhad, "酸への耐性" DE_HAD);
/*JP
	if (Stone_resistance) dump(youwere, "petrification resistant");
*/
	if (Stone_resistance) dump(youhad, "石化への耐性" DE_HAD);
/*JP
	if (Invulnerable) dump(youwere, "invulnerable");
*/
	if (Invulnerable) dump(youwere, "不死身" DE_WERE);
/*JP
	if (u.uedibility) dump(youcould, "recognize detrimental food");
*/
	if (u.uedibility) dump(youcould, "有害な食料を識別" DE_COULD);

	/*** Troubles ***/
/*JP
	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations");
*/
	if (Halluc_resistance) 	dump(youhad, "幻覚への耐性" DE_HAD);
/*JP
	if (Hallucination) dump(youwere, "hallucinating");
*/
	if (Hallucination) dump(youwere, "幻覚状態" DE_WERE);
/*JP
	if (Stunned) dump(youwere, "stunned");
*/
	if (Stunned) dump(youwere, "くらくら状態" DE_WERE);
/*JP
	if (Confusion) dump(youwere, "confused");
*/
	if (Confusion) dump(youwere, "混乱状態" DE_WERE);
/*JP
	if (Blinded) dump(youwere, "blinded");
*/
	if (Blinded) dump(youwere, "盲目" DE_WERE);
	if (Sick) {
		if (u.usick_type & SICK_VOMITABLE)
/*JP
			dump(youwere, "sick from food poisoning");
*/
			dump(youwere, "食中毒で気分が悪かった");
		if (u.usick_type & SICK_NONVOMITABLE)
/*JP
			dump(youwere, "sick from illness");
*/
			dump(youwere, "病気で気分が悪かった");
	}
/*JP
	if (Stoned) dump(youwere, "turning to stone");
*/
	if (Stoned) dump(youwere, "石になった");
/*JP
	if (Slimed) dump(youwere, "turning into slime");
*/
	if (Slimed) dump(youwere, "スライム状になって" DE_WERE_ING);
	if (Strangled)
/*JP
		dump(youwere, (u.uburied) ? "buried" : "being strangled");
*/
		dump(youwere, (u.uburied) ? "窒息して" DE_WERE_ING
			: "首を絞められて" DE_WERE_ING);
	if (Glib) {
#if 0 /*JP:T*/
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		dump(youhad, buf);
#else
		Sprintf(buf, "%sがぬるぬるしていた", body_part(FINGER));
		dump("", buf);
#endif
	}
/*JP
	if (Fumbling) dump("  ", "You fumbled");
*/
	if (Fumbling) dump(youwere, "不器用になって" DE_WERE_ING);
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
#if 0 /*JP:T*/
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		dump(youhad, buf);
#else
		Sprintf(buf, "%sを怪我して" DE_WERE_ING, makeplural(body_part(LEG)));
		dump(youhad, buf);
#endif
	}
#ifdef STEED
	if (Wounded_legs && u.usteed) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
#if 0 /*JP:T*/
	    *buf = highc(*buf);
	    Strcat(buf, " had wounded legs");
	    dump("  ", buf);
#else
	    Strcat(buf, "は脚を怪我していた");
	    dump("  ", buf);
#endif
	}
#endif
/*JP
	if (Sleeping) dump("  ", "You fell asleep");
*/
	if (Sleeping) dump(youwere, "眠って" DE_WERE_ING);
/*JP
	if (Hunger) dump("  ", "You hungered rapidly");
*/
	if (Hunger) dump(youwere, "すぐに腹が減る状態だった");

	/*** Vision and senses ***/
/*JP
	if (See_invisible) dump("  ", "You saw invisible");
*/
	if (See_invisible) dump(youwere, "透明なものを見られた");
/*JP
	if (Blind_telepat) dump(youwere, "telepathic");
*/
	if (Blind_telepat) dump(youhad, "テレパシー" DE_HAD);
/*JP
	if (Warning) dump(youwere, "warned");
*/
	if (Warning) dump(youwere, "警戒能力" DE_HAD);
	if (Warn_of_mon && flags.warntype) {
#if 0 /*JP*/
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			something); 
#else
		Sprintf(buf, "%sの存在を感じる能力" DE_HAD,
			(flags.warntype & M2_ORC) ? "オーク" :
			(flags.warntype & M2_DEMON) ? "悪魔" :
			something); 
#endif
		dump(youhad, buf);
	}
/*JP
	if (Undead_warning) dump(youwere, "warned of undead");
*/
	if (Undead_warning) dump(youhad, "不死の生物への警戒能力" DE_HAD);
/*JP
	if (Searching) dump(youhad, "automatic searching");
*/
	if (Searching) dump(youhad, "探査能力" DE_HAD);
/*JP
	if (Clairvoyant) dump(youwere, "clairvoyant");
*/
	if (Clairvoyant) dump(youhad, "千里眼能力" DE_HAD);
/*JP
	if (Infravision) dump(youhad, "infravision");
*/
	if (Infravision) dump(youhad, "赤外線を見る能力" DE_HAD);
	if (Detect_monsters)
/*JP
	  dump(youwere, "sensing the presence of monsters");
*/
	  dump(youhad, "怪物を探す能力" DE_HAD);
/*JP
	if (u.umconf) dump(youwere, "going to confuse monsters");
*/
	if (u.umconf) dump(youhad, "怪物を混乱させる能力" DE_HAD);

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;
	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
/*JP
		dump(youwere, "poorly adorned");
*/
		dump(youwere, "みずほらしく着飾って" DE_WERE_ING);
	    else
/*JP
		dump(youwere, "adorned");
*/
		dump(youwere, "着飾って" DE_WERE_ING);
	}
/*JP
	if (Invisible) dump(youwere, "invisible");
*/
	if (Invisible) dump(youwere, "透明" DE_WERE);
/*JP
	else if (Invis) dump(youwere, "invisible to others");
*/
	else if (Invis) dump(youwere, "他人に対して透明" DE_WERE);
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
/*JP
	    dump(youwere, "visible");
*/
	    dump(youwere, "不透明" DE_WERE);
/*JP
	if (Displaced) dump(youwere, "displaced");
*/
	if (Displaced) dump(youhad, "幻影能力" DE_HAD);
/*JP
	if (Stealth) dump(youwere, "stealthy");
*/
	if (Stealth) dump(youhad, "人目を盗む能力" DE_HAD);
/*JP
	if (Aggravate_monster) dump("  ", "You aggravated monsters");
*/
	if (Aggravate_monster) dump(youwere, "反感をかって" DE_WERE_ING);
/*JP
	if (Conflict) dump("  ", "You caused conflict");
*/
	if (Conflict) dump(youwere, "闘争を引き起こして" DE_WERE_ING);

	/*** Transportation ***/
/*JP
	if (Jumping) dump(youcould, "jump");
*/
	if (Jumping) dump(youcould, "跳躍することが" DE_COULD);
/*JP
	if (Teleportation) dump(youcould, "teleport");
*/
	if (Teleportation) dump(youcould, "瞬間移動が" DE_COULD);
/*JP
	if (Teleport_control) dump(youhad, "teleport control");
*/
	if (Teleport_control) dump(youhad, "瞬間移動の制御能力" DE_HAD);
/*JP
	if (Lev_at_will) dump(youwere, "levitating, at will");
*/
	if (Lev_at_will) dump(youhad, "上手く浮遊する能力" DE_HAD);
	else if (Levitation)
#if 0 /*JP*/
	  dump(youwere, "levitating");	/* without control */
#else
	  dump(youwere, "浮遊状態" DE_WERE);	/* without control */
#endif
/*JP
	else if (Flying) dump(youcould, "fly");
*/
	else if (Flying) dump(youcould, "飛ぶことが" DE_COULD);
/*JP
	if (Wwalking) dump(youcould, "walk on water");
*/
	if (Wwalking) dump(youcould, "水の上を歩くことが" DE_COULD);
/*JP
	if (Swimming) dump(youcould, "swim");
*/
	if (Swimming) dump(youcould, "泳ぐことが" DE_COULD);
/*JP
	if (Breathless) dump(youcould, "survive without air");
*/
	if (Breathless) dump(youcould, "空気なしで生き延びることが" DE_COULD);
/*JP
	else if (Amphibious) dump(youcould, "breathe water");
*/
	else if (Amphibious) dump(youcould, "水中で呼吸が" DE_COULD);
/*JP
	if (Passes_walls) dump(youcould, "walk through walls");
*/
	if (Passes_walls) dump(youcould, "壁を通り抜けることが" DE_COULD);
#ifdef STEED
#if 0 /*JP*/
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    dump(youwere, buf);
	}
#else
	if (u.usteed && (final < 2 || strcmp(killer, "乗りものから落ちて"))) {
	    Sprintf(buf, "%sに乗っていた", y_monnam(u.usteed));
	    dump(youwere, buf);
	}
#endif /*JP*/
#endif
	if (u.uswallow) {
/*JP
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
*/
	    Sprintf(buf, "%sに飲み込まれていた", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    dump(youwere, buf);
	} else if (u.ustuck) {
#if 0 /*JP*/
	    Sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
#else
	    Sprintf(buf, "%s%sいた",
		    a_monnam(u.ustuck),
		    (Upolyd && sticks(youmonst.data)) ? "を捕まえて" : "に捕まって");
#endif
	    dump(youwere, buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
#if 0 /*JP*/
	    dump(youhad,
		enlght_combatinc("to hit", u.uhitinc, final, buf));
#else
	{
		enlght_combatinc("命中率", u.uhitinc, final, buf);
	    Strcat(buf, DE_HAD);
	    dump(youhad, buf);
	}
#endif /*JP*/
	if (u.udaminc)
#if 0 /*JP*/
	    dump(youhad,
		enlght_combatinc("damage", u.udaminc, final, buf));
#else
	{
	    enlght_combatinc("ダメージ", u.udaminc, final, buf);
	    Strcat(buf, DE_HAD);
	    dump(youhad, buf);
	}
#endif /*JP*/
/*JP
	if (Slow_digestion) dump(youhad, "slower digestion");
*/
	if (Slow_digestion) dump(youhad, "食物の消化が遅かった");
/*JP
	if (Regeneration) dump("  ", "You regenerated");
*/
	if (Regeneration) dump(youhad, "再生能力" DE_HAD);
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    
	    if (prot < 0)
/*JP
		dump(youwere, "ineffectively protected");
*/
		dump(youwere, "不十分に守られて" DE_WERE_ING);
	    else
/*JP
		dump(youwere, "protected");
*/
		dump(youwere, "守られて" DE_WERE_ING);
	}
	if (Protection_from_shape_changers)
/*JP
		dump(youwere, "protected from shape changers");
*/
		dump(youwere, "変化怪物への耐性" DE_HAD);
/*JP
	if (Polymorph) dump(youwere, "polymorphing");
*/
	if (Polymorph) dump(youwere, "変化して" DE_WERE_ING);
/*JP
	if (Polymorph_control) dump(youhad, "polymorph control");
*/
	if (Polymorph_control) dump(youhad, "変化の制御能力" DE_HAD);
	if (u.ulycn >= LOW_PM) {
#if 0/*JP*/
		Strcpy(buf, an(mons[u.ulycn].mname));
#else
		Strcpy(buf, jtrns_mon_gen(mons[u.ulycn].mname, flags.female));
		Strcat(buf, DE_WERE);
#endif
		dump(youwere, buf);
	}
	if (Upolyd) {
#if 0/*JP*/
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s",
			 an(youmonst.data->mname));
#else
	    if (u.umonnum == u.ulycn) Strcpy(buf, "獣の姿をしていた");
	    else Sprintf(buf, "%sに変化していた", a_monnam(&youmonst));
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    dump(youwere, buf);
	}
	if (Unchanging)
/*JP
	  dump(youcould, "not change from your current form");
*/
	  dump(youcould, "今の姿から変化することができなかった");
#if 0/*JP*/
	if (Fast) dump(youwere, Very_fast ? "very fast" : "fast");
#else
	if (Fast) {
		Sprintf(buf, "%s素早く行動する能力" DE_HAD,
		    Very_fast ? "とても" : "");
		dump(youwere, buf);
	}
#endif
/*JP
	if (Reflecting) dump(youhad, "reflection");
*/
	if (Reflecting) dump(youhad, "反射能力" DE_HAD);
/*JP
	if (Free_action) dump(youhad, "free action");
*/
	if (Free_action) dump(youhad, "拘束されない能力" DE_HAD);
/*JP
	if (Fixed_abil) dump(youhad, "fixed abilities");
*/
	if (Fixed_abil) dump(youhad, "能力が変化しなかった");
	if (Lifesaved)
/*JP
		dump("  ", "Your life would have been saved");
*/
		dump("  ", "あなたの生命は保存されて" DE_WERE_ING);
/*JP
	if (u.twoweap) dump(youwere, "wielding two weapons at once");
*/
	if (u.twoweap) dump(youwere, "二刀流" DE_WERE);

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
#if 0 /*JP*/
	    Sprintf(buf, "%s%slucky (%d)",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "", Luck);
#else
	    Sprintf(buf, "%s%s(%d)",
		    ltmp >= 10 ? "猛烈に" : ltmp >= 5 ? "とても" : "",
		    Luck < 0 ? "不幸" : "幸福", Luck);
#endif
	    dump(youwere, buf);
	}
#ifdef WIZARD
/*JP
	 else if (wizard) dump("  ", "Your luck was zero");
*/
	 else if (wizard) dump("  ", "あなたの運はゼロだった");
#endif
/*JP
	if (u.moreluck > 0) dump(youhad, "extra luck");
*/
	if (u.moreluck > 0) dump(youhad, "さらなる幸運");
/*JP
	else if (u.moreluck < 0) dump(youhad, "reduced luck");
*/
	else if (u.moreluck < 0) dump(youhad, "さらなる悪運");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
/*JP
		dump("  ", "Bad luck did not time out for you");
*/
		dump("  ", "悪運は時間切れにならなかった");
	    if (ltmp >= 0)
/*JP
		dump("  ", "Good luck did not time out for you");
*/
		dump("  ", "幸運は時間切れにならなかった");
	}

	if (u.ugangr) {
#if 0 /*JP*/
	    Sprintf(buf, " %sangry with you",
		u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    Sprintf(buf2, "%s was %s", u_gname(), buf);
	    dump("  ", buf2);
#else /*JP*/
	    Sprintf(buf, "%sは%s怒っていた", u_gname(),
		u.ugangr > 6 ? "猛烈に" : u.ugangr > 3 ? "とても" : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    dump("  ", buf);
#endif /*JP*/
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* quit/escaped/ascended */
#if 0 /*JP*/
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
#else
	    p = "死んだ後復活していた";
	    switch (u.umortality) {
	    case 0:  p = "生き延びた";  break;
	    case 1:  Strcpy(buf, "一度");  break;
	    case 2:  Strcpy(buf, "二度");  break;
	    case 3:  Strcpy(buf, "三度");  break;
	    default: Sprintf(buf, "%d回", u.umortality);
		     break;
	    }
#endif
	} else {		/* game ended in character's death */
#if 0 /*JP*/
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  warning("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
#else
	    p = "死んでいる";
	    switch (u.umortality) {
	    case 0:  warning("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, "(%d回！)", u.umortality);
		     break;
	    }
#endif
	}
	if (p) {
/*JP
	  Sprintf(buf2, "You %s %s", p, buf);
*/
	  Sprintf(buf2, "あなたは%s%s", buf, p);
	  dump("  ", buf2);
	}
    }
	dump("", "");
	return;

} /* dump_enlightenment */
#endif

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL boolean
minimal_enlightenment()
{
	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char untabbed_fmtstr[] = "%-15s: %-12s";
	static const char untabbed_deity_fmtstr[] = "%-17s%s";
	static const char tabbed_fmtstr[] = "%s:\t%-12s";
	static const char tabbed_deity_fmtstr[] = "%s\t%s";
	static const char *fmtstr;
	static const char *deity_fmtstr;

	fmtstr = iflags.menu_tab_sep ? tabbed_fmtstr : untabbed_fmtstr;
	deity_fmtstr = iflags.menu_tab_sep ?
			tabbed_deity_fmtstr : untabbed_deity_fmtstr; 
	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
/*JP
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Starting", FALSE);
*/
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "開始", FALSE);

	/* Starting name, race, role, gender */
/*JP
	Sprintf(buf, fmtstr, "name", plname);
*/
	Sprintf(buf, fmtstr, "名前", plname);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
/*JP
	Sprintf(buf, fmtstr, "race", urace.noun);
*/
	Sprintf(buf, fmtstr, "種族", urace.j);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
#if 0 /*JP*/
	Sprintf(buf, fmtstr, "role",
		(flags.initgend && urole.name.f) ? urole.name.f : urole.name.m);
#else
	Sprintf(buf, fmtstr, "職業",
		(flags.initgend && urole.jname.f) ? urole.jname.f : urole.jname.m);
#endif
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
/*JP
	Sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj);
*/
	Sprintf(buf, fmtstr, "性別", genders[flags.initgend].j);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Starting alignment */
/*JP
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL]));
*/
	Sprintf(buf, fmtstr, "属性", align_str(u.ualignbase[A_ORIGINAL]));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Current name, race, role, gender */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
#if 0 /*JP*/
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Current", FALSE);
	Sprintf(buf, fmtstr, "race", Upolyd ? youmonst.data->mname : urace.noun);
#else
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "現在", FALSE);
	Sprintf(buf, fmtstr, "種族", Upolyd ? jtrns_mon_gen(youmonst.data->mname, flags.female) : urace.j);
#endif
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd) {
#if 0 /*JP*/
	    Sprintf(buf, fmtstr, "role (base)",
		(u.mfemale && urole.name.f) ? urole.name.f : urole.name.m);
#else
	    Sprintf(buf, fmtstr, "職業(基本)",
		(u.mfemale && urole.jname.f) ? urole.jname.f : urole.jname.m);
#endif
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	} else {
#if 0 /*JP*/
	    Sprintf(buf, fmtstr, "role",
		(flags.female && urole.name.f) ? urole.name.f : urole.name.m);
#else
	    Sprintf(buf, fmtstr, "職業",
		(flags.female && urole.jname.f) ? urole.jname.f : urole.jname.m);
#endif
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	/* don't want poly_gender() here; it forces `2' for non-humanoids */
	genidx = is_neuter(youmonst.data) ? 2 : flags.female;
/*JP
	Sprintf(buf, fmtstr, "gender", genders[genidx].adj);
*/
	Sprintf(buf, fmtstr, "性別", genders[genidx].j);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd && (int)u.mfemale != genidx) {
/*JP
	    Sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj);
*/
	    Sprintf(buf, fmtstr, "性別(基本)", genders[u.mfemale].j);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Current alignment */
/*JP
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type));
*/
	Sprintf(buf, fmtstr, "属性", align_str(u.ualign.type));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Current position of hero */
	if (wizard) {
		Sprintf(buf2, "(%2d,%2d)", u.ux, u.uy);
/*JP
		Sprintf(buf, fmtstr, "position", buf2);
*/
		Sprintf(buf, fmtstr, "位置", buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
/*JP
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
*/
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "神", FALSE);
	Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
#if 0 /*JP*/
		&& u.ualign.type == A_CHAOTIC) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (c)" : "");
#else
		&& u.ualign.type == A_CHAOTIC) ? " (初、現)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (初)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (現)" : "");
#endif
/*JP
	Sprintf(buf, fmtstr, "Chaotic", buf2);
*/
	Sprintf(buf, fmtstr, "混沌", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
#if 0 /*JP*/
		&& u.ualign.type == A_NEUTRAL) ? " (初、現)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (初)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (現)" : "");
#else
		&& u.ualign.type == A_NEUTRAL) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (c)" : "");
#endif
/*JP
	Sprintf(buf, fmtstr, "Neutral", buf2);
*/
	Sprintf(buf, fmtstr, "中立", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
#if 0 /*JP*/
		u.ualign.type == A_LAWFUL)  ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (c)" : "");
#else
		u.ualign.type == A_LAWFUL)  ? " (初、現)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (初)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (現)" : "");
#endif
/*JP
	Sprintf(buf, fmtstr, "Lawful", buf2);
*/
	Sprintf(buf, fmtstr, "秩序", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

/*JP
	end_menu(tmpwin, "Base Attributes");
*/
	end_menu(tmpwin, "基本属性");
	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
	return (n != -1);
}

STATIC_PTR int
doattributes()
{
	if (!minimal_enlightenment())
		return 0;
	if (wizard || discover)
		enlightenment(0);
	return 0;
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window */
	en_win = create_nhwindow(NHW_MENU);
/*JP
	putstr(en_win, 0, "Voluntary challenges:");
*/
	putstr(en_win, 0, "自発的挑戦:");
	putstr(en_win, 0, "");

	if (!u.uconduct.food)
/*JP
	    enl_msg(You_, "have gone", "went", " without food");
*/
	    enl_msg("あなたは食事をし", "ていない", "なかった", "");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
/*JP
	    you_have_X("followed a strict vegan diet");
*/
	    you_are("厳格な菜食主義者");
	else if (!u.uconduct.unvegetarian)
/*JP
	    you_have_been("vegetarian");
*/
	    you_are("菜食主義者");

	if (!u.uconduct.gnostic)
/*JP
	    you_have_been("an atheist");
*/
	    you_are("無神論者");

	if (!u.uconduct.weaphit)
/*JP
	    you_have_never("hit with a wielded weapon");
*/
	    enl_msg("あなたは装備している武器で攻撃し", "ていない", "なかった", "");
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
#else
	    Sprintf(buf, "あなたは%ld回装備した武器を使用し",
		    u.uconduct.weaphit);
	    enl_msg(buf, "ている", "た", "");
#endif
	}
#endif
	if (!u.uconduct.killer)
/*JP
	    you_have_been("a pacifist");
*/
	    you_are("平和主義者");

	if (!u.uconduct.literate)
/*JP
	    you_have_been("illiterate");
*/
	    enl_msg("あなたは読み書きし", "ていない", "なかった", "");
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
#else
	    Sprintf(buf, "%ld回読んだり書いたりし",
		    u.uconduct.literate);
	    enl_msg(buf, "ている", "た", "");
#endif
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
/*JP
	    you_have_never("genocided any monsters");
*/
	    enl_msg("あなたは怪物を虐殺し", "ていない", "なかった", "");
	} else {
#if 0 /*JP*/
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
#else
	    Sprintf(buf, "%d種の怪物を虐殺し",
		    ngenocided);
	    enl_msg(buf, "ている", "た", "");
#endif
	}

	if (!u.uconduct.polypiles)
/*JP
	    you_have_never("polymorphed an object");
*/
	    enl_msg("あなたは物体を変化させ", "ていない", "なかった", "");
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
#else
	    Sprintf(buf, "%ld個の道具を変化させ",
		    u.uconduct.polypiles);
	    enl_msg(buf, "ている", "た", "");
#endif
	}
#endif

	if (!u.uconduct.polyselfs)
/*JP
	    you_have_never("changed form");
*/
	    enl_msg("あなたは変化し", "ていない", "なかった", "");
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
#else
	    Sprintf(buf, "%ld回姿を変え",
		    u.uconduct.polyselfs);
	    enl_msg(buf, "ている", "た", "");
#endif
	}
#endif

	if (!u.uconduct.wishes)
/*JP
	    you_have_X("used no wishes");
*/
	    enl_msg("あなたは願い事をし", "ていない", "なかった", "");
	else {
#if 0 /*JP*/
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    you_have_X(buf);
#else
	    Sprintf(buf, "%ld回願い事をし",
		    u.uconduct.wishes);
	    enl_msg(buf, "ている", "た", "");
#endif

	    if (!u.uconduct.wisharti)
#if 0 /*JP*/
		enl_msg(You_, "have not wished", "did not wish",
			" for any artifacts");
#else
		enl_msg("あなたは聖器を願", "っていない", "わなかった", "");
#endif
	}

	/* Pop up the window and wait for a key */
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
}

#ifdef DUMP_LOG
void
dump_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

/*JP
	dump("", "Voluntary challenges");
*/
	dump("", "自発的挑戦");

	if (!u.uconduct.food)
/*JP
	    dump("", "  You went without food");
*/
	    dump("", "  あなたは食事をしなかった");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
/*JP
	    dump("", "  You followed a strict vegan diet");
*/
	    dump("", "  あなたは厳格な菜食主義者であった");
	else if (!u.uconduct.unvegetarian)
/*JP
	    dump("", "  You were a vegetarian");
*/
	    dump("", "  あなたは菜食主義者であった");
	else if (Role_if(PM_MONK) && u.uconduct.unvegetarian < 10) {
#if 0 /*JP*/
	    sprintf(buf, "  You ate non-vegetarian food %ld time%s.", 
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
#else
	    sprintf(buf, "  あなたは植物性以外の食事を%ld回した。", 
		u.uconduct.unvegetarian);
#endif
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
/*JP
	    dump("", "  You were an atheist");
*/
	    dump("", "  あなたは無神論者であった");

	if (!u.uconduct.weaphit)
/*JP
	    dump("", "  You never hit with a wielded weapon");
*/
	    dump("", "  あなたは装備している武器で攻撃しなかった");
	else if (Role_if(PM_MONK) && u.uconduct.weaphit < 10) {
#if 0 /*JP*/
	    Sprintf(buf, "  You hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
#else
	    Sprintf(buf, "  あなたは%ld回装備した武器を使用した",
		    u.uconduct.weaphit);
#endif
	    dump("", buf);
	}
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%ld回装備した武器を使用した",
		    u.uconduct.weaphit);
	    dump("  あなたは", buf);
#endif
	}
#endif
	if (!u.uconduct.killer)
/*JP
	    dump("", "  You were a pacifist");
*/
	    dump("", "  あなたは平和主義者であった");

	if (!u.uconduct.literate)
/*JP
	    dump("", "  You were illiterate");
*/
	    dump("", "  あなたは読み書きしなかった");
#ifdef WIZARD
	else if (wizard) {
#if 0 /*JP*/
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%ld回読んだり書いたりした",
		    u.uconduct.literate);
	    dump("  あなたは", buf);
#endif
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
/*JP
	    dump("", "  You never genocided any monsters");
*/
	    dump("", "  あなたは怪物を虐殺しなかった");
	} else {
#if 0 /*JP*/
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%d種の怪物を虐殺した", ngenocided);
	    dump("  あなたは", buf);
#endif
	}

	if (!u.uconduct.polypiles)
/*JP
	    dump("", "  You never polymorphed an object");
*/
	    dump("", "  あなたは物体を変化させなかった");
	else {
#if 0 /*JP*/
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%ld個の道具を変化させた",
		    u.uconduct.polypiles);
	    dump("  あなたは", buf);
#endif
	}

	if (!u.uconduct.polyselfs)
/*JP
	    dump("", "  You never changed form");
*/
	    dump("", "  You never changed form");
	else {
#if 0 /*JP*/
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%ld回姿を変えた",
		    u.uconduct.polyselfs);
	    dump("  あなたは", buf);
#endif
	}

	if (!u.uconduct.wishes)
/*JP
	    dump("", "  You used no wishes");
*/
	    dump("", "  あなたは願い事をしなかった");
	else {
#if 0 /*JP*/
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    dump("  You ", buf);
#else
	    Sprintf(buf, "%ld回願い事をした", u.uconduct.wishes);
	    dump("  あなたは", buf);
#endif

	    if (!u.uconduct.wisharti)
/*JP
		dump("", "  You did not wish for any artifacts");
*/
		dump("", "  あなたは聖器を願わなかった");
	}

	dump("", "");
}
#endif /* DUMP_LOG */

#endif /* OVLB */
#ifdef OVL1

#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#ifndef C
#define C(c)		(0x1f & (c))
#endif

static const struct func_tab cmdlist[] = {
	{C('d'), FALSE, dokick}, /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	{C('e'), TRUE, wiz_detect},
	{C('f'), TRUE, wiz_map},
	{C('g'), TRUE, wiz_genesis},
	{C('i'), TRUE, wiz_identify},
#endif
	{C('l'), TRUE, doredraw}, /* if number_pad is set */
	{C('n'), TRUE, donamelevel}, /* if number_pad is set */
	{C('o'), TRUE, dooverview_or_wiz_where}, /* depending on wizard status */
	{C('p'), TRUE, doprev_message},
	{C('r'), TRUE, doredraw},
	{C('t'), TRUE, dotele},
#ifdef WIZARD
	{C('v'), TRUE, wiz_level_tele},
	{C('w'), TRUE, wiz_wish},
#endif
	{C('x'), TRUE, doattributes},
#ifdef SUSPEND
	{C('z'), TRUE, dosuspend},
#endif
	{'a', FALSE, doapply},
	{'A', FALSE, doddoremarm},
	{M('a'), TRUE, doorganize},
/*	'b', 'B' : go sw */
	{'c', FALSE, doclose},
	{'C', TRUE, do_mname},
	{M('c'), TRUE, dotalk},
	{'d', FALSE, dodrop},
	{'D', FALSE, doddrop},
	{M('d'), FALSE, dodip},
	{'e', FALSE, doeat},
	{'E', FALSE, doengrave},
	{M('e'), TRUE, enhance_weapon_skill},
	{'f', FALSE, dofire},
/*	'F' : fight (one time) */
	{M('f'), FALSE, doforce},
/*	'g', 'G' : multiple go */
/*	'h', 'H' : go west */
	{'h', TRUE, dohelp}, /* if number_pad is set */
	{'i', TRUE, ddoinv},
	{'I', TRUE, dotypeinv},		/* Robert Viduya */
	{M('i'), TRUE, doinvoke},
/*	'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	{'j', FALSE, dojump}, /* if number_pad is on */
	{M('j'), FALSE, dojump},
	{'k', FALSE, dokick}, /* if number_pad is on */
	{'l', FALSE, doloot}, /* if number_pad is on */
	{M('l'), FALSE, doloot},
/*	'n' prefixes a count if number_pad is on */
	{M('m'), TRUE, domonability},
	{'N', TRUE, ddocall}, /* if number_pad is on */
	{M('n'), TRUE, ddocall},
	{M('N'), TRUE, ddocall},
	{'o', FALSE, doopen},
	{'O', TRUE, doset},
	{M('o'), FALSE, dosacrifice},
	{'p', FALSE, dopay},
	{'P', FALSE, doputon},
	{M('p'), TRUE, dopray},
	{'q', FALSE, dodrink},
	{'Q', FALSE, dowieldquiver},
	{M('q'), TRUE, done2},
	{'r', FALSE, doread},
	{'R', FALSE, doremring},
	{M('r'), FALSE, dorub},
/*JP
	{'s', TRUE, dosearch, "searching"},
*/
	{'s', TRUE, dosearch, "捜す"},
	{'S', TRUE, dosave},
	{M('s'), FALSE, dosit},
	{'t', FALSE, dothrow},
	{'T', FALSE, dotakeoff},
	{M('t'), TRUE, doturn},
/*	'u', 'U' : go ne */
	{'u', FALSE, dountrap}, /* if number_pad is on */
	{M('u'), FALSE, dountrap},
	{'v', TRUE, doversion},
	{'V', TRUE, dohistory},
	{M('v'), TRUE, doextversion},
	{'w', FALSE, dowield},
	{'W', FALSE, dowear},
	{M('w'), FALSE, dowipe},
	{'x', FALSE, doswapweapon},
	{'X', TRUE, enter_explore_mode},
/*	'y', 'Y' : go nw */
	{'z', FALSE, dozap},
	{'Z', TRUE, docast},
	{'<', FALSE, doup},
	{'>', FALSE, dodown},
	{'/', TRUE, dowhatis},
	{'&', TRUE, dowhatdoes},
	{'?', TRUE, dohelp},
	{M('?'), TRUE, doextlist},
#ifdef SHELL
	{'!', TRUE, dosh},
#endif
#if 0 /*JP*/
	{'.', TRUE, donull, "waiting"},
	{' ', TRUE, donull, "waiting"},
#else
	{'.', TRUE, donull, "休憩する"},
	{' ', TRUE, donull, "休憩する"},
#endif
	{',', FALSE, dopickup},
	{':', TRUE, dolook},
	{';', TRUE, doquickwhatis},
	{'^', TRUE, doidtrap},
	{'\\', TRUE, dodiscovered},		/* Robert Viduya */
	{'@', TRUE, dotogglepickup},
#if 1 /*JP*/
	{'`', TRUE, dotogglelang},
#endif
	{M('2'), FALSE, dotwoweapon},
	{WEAPON_SYM,  TRUE, doprwep},
	{ARMOR_SYM,  TRUE, doprarm},
	{RING_SYM,  TRUE, doprring},
	{AMULET_SYM, TRUE, dopramulet},
	{TOOL_SYM, TRUE, doprtool},
	{'*', TRUE, doprinuse},	/* inventory of all equipment in use */
	{GOLD_SYM, TRUE, doprgold},
	{SPBOOK_SYM, TRUE, dovspell},			/* Mike Stephenson */
	{'#', TRUE, doextcmd},
	{'_', TRUE, dotravel},
	{0,0,0,0}
};

struct ext_func_tab extcmdlist[] = {
#if 0 /*JP*/
	{"adjust", "adjust inventory letters", doorganize, TRUE},
	{"annotate", "name current level", donamelevel, TRUE},
	{"chat", "talk to someone", dotalk, TRUE},	/* converse? */
	{"conduct", "list which challenges you have adhered to", doconduct, TRUE},
	{"dip", "dip an object into something", dodip, FALSE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill,
							TRUE},
	{"force", "force a lock", doforce, FALSE},
	{"invoke", "invoke an object's powers", doinvoke, TRUE},
	{"jump", "jump to a location", dojump, FALSE},
	{"loot", "loot a box on the floor", doloot, FALSE},
	{"monster", "use a monster's special ability", domonability, TRUE},
	{"name", "name an item or type of object", ddocall, TRUE},
	{"offer", "offer a sacrifice to the gods", dosacrifice, FALSE},
	{"overview", "show an overview of the dungeon", dooverview, TRUE},
	{"pray", "pray to the gods for help", dopray, TRUE},
	{"quit", "exit without saving current game", done2, TRUE},
#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, FALSE},
#endif
	{"rub", "rub a lamp or a stone", dorub, FALSE},
#ifdef LIVELOG_SHOUT
	{"shout", "shout something", doshout, FALSE},
#endif
	{"sit", "sit down", dosit, FALSE},
	{"turn", "turn undead", doturn, TRUE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, FALSE},
	{"untrap", "untrap something", dountrap, FALSE},
	{"version", "list compile time options for this version of UnNetHack",
		doextversion, TRUE},
	{"wipe", "wipe off your face", dowipe, FALSE},
	{"?", "get this list of extended commands", doextlist, TRUE},
#else /*JP*/
	{"adjust", "持ち物一覧の調整", doorganize, TRUE},
	{"annotate", "今いる階に名前をつける", donamelevel, TRUE},
	{"chat", "誰かと話す", dotalk, TRUE},	/* converse? */
	{"conduct", "どういう行動をとったか見る", doconduct, TRUE},
	{"dip", "何かに物を浸す", dodip, FALSE},
	{"enhance", "武器熟練度を高める", enhance_weapon_skill, TRUE},
	{"force", "鍵をこじあける", doforce, FALSE},
	{"invoke", "物の特別な力を使う", doinvoke, TRUE},
	{"jump", "他の位置に飛びうつる", dojump, FALSE},
	{"loot", "床の上の箱を開ける", doloot, TRUE},
	{"monster", "怪物の特別能力を使う", domonability, TRUE},
	{"name", "アイテムや物に名前をつける", ddocall, TRUE},
	{"offer", "神に供物を捧げる", dosacrifice, FALSE},
	{"overview", "ダンジョンの概略を見る", dooverview, TRUE},
	{"pray", "神に祈る", dopray, TRUE},
	{"quit", "セーブしないで終了", done2, TRUE},
#ifdef STEED
	{"ride", "怪物に乗る(または降りる)", doride, FALSE},
#endif
	{"rub", "ランプをこする", dorub, FALSE},
#ifdef LIVELOG_SHOUT
	{"shout", "叫ぶ", doshout, FALSE},
#endif
	{"sit", "座る", dosit, FALSE},
	{"turn", "アンデットを土に返す", doturn, TRUE},
	{"twoweapon", "両手持ちの切り替え", dotwoweapon, FALSE},
	{"untrap", "罠をはずす", dountrap, FALSE},
	{"version", "コンパイル時のオプションを表示する",
		doextversion, TRUE},
	{"wipe", "顔を拭う", dowipe, FALSE},
	{"?", "この拡張コマンド一覧を表示する", doextlist, TRUE},
#endif /*JP*/
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef PORT_DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE}, /* showkills (showborn patch) */
        {(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#if defined(WIZARD)
static const struct ext_func_tab debug_extcmdlist[] = {
#if 0 /*JP*/
	{"levelchange", "change experience level", wiz_level_change, TRUE},
	{"lightsources", "show mobile light sources", wiz_light_sources, TRUE},
	{"mazewalkmap", "show MAZEWALK paths", wiz_mazewalkmap, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "migrate n random monsters", wiz_migrate_mons, TRUE},
#endif
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, TRUE},
	{"panic", "test panic routine (fatal to game)", wiz_panic, TRUE},
	{"polyself", "polymorph self", wiz_polyself, TRUE},
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, TRUE},
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, TRUE},
	{"showkills", "show list of monsters killed", wiz_showkills, TRUE},
	{"stats", "show memory statistics", wiz_show_stats, TRUE},
	{"timeout", "look at timeout queue", wiz_timeout_queue, TRUE},
	{"vision", "show vision array", wiz_show_vision, TRUE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, TRUE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, TRUE},
#else /*JP*/
	{"levelchange", "経験レベルを変える", wiz_level_change, TRUE},
	{"light sources", "移動光源を表示する", wiz_light_sources, TRUE},
	{"mazewalkmap", "MAZEWALK経路を表示", wiz_mazewalkmap, TRUE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "ランダムな怪物を移住させる", wiz_migrate_mons, TRUE},
#endif
	{"monpoly_control", "怪物の変化を制御する", wiz_mon_polycontrol, TRUE},
	{"panic", "パニックルーチンをテストする(致命的)", wiz_panic, TRUE},
	{"polyself", "変身する", wiz_polyself, TRUE},
#ifdef PORT_DEBUG
	{"portdebug", "ウィザードポートデバッグコマンド", wiz_port_debug, TRUE},
#endif
	{"seenv", "視線ベクトルを表示する", wiz_show_seenv, TRUE},
	{"showkills", "怪物の殺害数一覧を表示する", wiz_showkills, TRUE},
	{"stats", "メモリ状態を表示する", wiz_show_stats, TRUE},
	{"timeout", "タイムアウトキューを見る", wiz_timeout_queue, TRUE},
	{"vision", "視界配列を表示する", wiz_show_vision, TRUE},
#ifdef DEBUG
	{"wizdebug", "ウィザードデバッグモード", wiz_debug_cmd, TRUE},
#endif
	{"wmode", "壁モードを表示する", wiz_show_wmodes, TRUE},
#endif /*JP*/
	{(char *)0, (char *)0, donull, TRUE}
};

/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt[0] != '?'; n++)
	    ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    for (j = 0; j < n; j++)
		if (strcmp(debug_extcmdlist[i].ef_txt, extcmdlist[j].ef_txt) < 0) break;

	    /* insert i'th debug entry into extcmdlist[j], pushing down  */
	    for (k = n; k >= j; --k)
		extcmdlist[k+1] = extcmdlist[k];
	    extcmdlist[j] = debug_extcmdlist[i];
	    n++;	/* now an extra entry */
	}
}


static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
	}
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

static int
wiz_mazewalkmap()
{
	winid win;
	int x, y;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);

	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++)
		row[x] = SpLev_Map[x][y] ? '1' : '.';
	    if (y == u.uy)
		row[u.ux] = '@';
	    row[x] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}


/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	Sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	Sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return 0;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
}

#ifdef DEBUG_MIGRATING_MONS
static int
wiz_migrate_mons()
{
	int mcount = 0;
	char inbuf[BUFSZ];
	struct permonst *ptr;
	struct monst *mtmp;
	d_level tolevel;
	getlin("How many random monsters to migrate? [0]", inbuf);
	if (*inbuf == '\033') return 0;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return 0;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst();
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return 0;
}
#endif

#endif /* WIZARD */

#define unctrl(c)	((c) <= C('z') ? (0x60 | (c)) : (c))
#define unmeta(c)	(0x7f & (c))


void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
		firsttime = (cmd == 0);

	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}
	if (*cmd == '\033') {
		flags.move = FALSE;
		return;
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377)
#else
	if(!*cmd || *cmd == (char)0377 || (!flags.rest_on_space && *cmd == ' '))
#endif
	{
		nhbell();
		flags.move = FALSE;
		return;		/* probably we just had an interrupt */
	}
	if (iflags.num_pad && iflags.num_pad_mode == 1) {
		/* This handles very old inconsistent DOS/Windows behaviour
		 * in a new way: earlier, the keyboard handler mapped these,
		 * which caused counts to be strange when entered from the
		 * number pad. Now do not map them until here. 
		 */
		switch (*cmd) {
		    case '5':       *cmd = 'g'; break;
		    case M('5'):    *cmd = 'G'; break;
		    case M('0'):    *cmd = 'I'; break;
        	}
        }
	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = iflags.travel1 = 0;
	switch (*cmd) {
	 case 'g':  if (movecmd(cmd[1])) {
			flags.run = 2;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '5':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	 case 'G':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 3;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '-':  if (!iflags.num_pad) break;	/* else FALLTHRU */
	/* Effects of movement commands and invisible monsters:
	 * m: always move onto space (even if 'I' remembered)
	 * F: always attack space (even if 'I' not remembered)
	 * normal movement: attack if 'I', move otherwise
	 */
	 case 'F':  if (movecmd(cmd[1])) {
			flags.forcefight = 1;
			do_walk = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'm':  if (movecmd(cmd[1]) || u.dz) {
			flags.run = 0;
			flags.nopick = 1;
			if (!u.dz) do_walk = TRUE;
			else cmd[0] = cmd[1];	/* "m<" or "m>" */
		    } else
			prefix_seen = TRUE;
		    break;
	 case 'M':  if (movecmd(lowc(cmd[1]))) {
			flags.run = 1;
			flags.nopick = 1;
			do_rush = TRUE;
		    } else
			prefix_seen = TRUE;
		    break;
	 case '0':  if (!iflags.num_pad) break;
		    (void)ddoinv(); /* a convenience borrowed from the PC */
		    flags.move = FALSE;
		    multi = 0;
		    return;
	 case CMD_TRAVEL:
		    if (iflags.travelcmd) {
			    flags.travel = 1;
			    iflags.travel1 = 1;
			    flags.run = 8;
			    flags.nopick = 1;
			    do_rush = TRUE;
			    break;
		    }
		    /*FALLTHRU*/
	 default:   if (movecmd(*cmd)) {	/* ordinary movement */
			flags.run = 0;	/* only matters here if it was 8 */
			do_walk = TRUE;
		    } else if (movecmd(iflags.num_pad ?
				       unmeta(*cmd) : lowc(*cmd))) {
			flags.run = 1;
			do_rush = TRUE;
		    } else if (movecmd(unctrl(*cmd))) {
			flags.run = 3;
			do_rush = TRUE;
		    }
		    break;
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    domove();
	    flags.forcefight = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == '\033') {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */

	/* handle all other commands */
	} else {
	    register const struct func_tab *tlist;
	    int res, NDECL((*func));
#ifdef QWERTZ
	    unsigned char cmdchar = *cmd & 0xff;
#endif

	    for (tlist = cmdlist; tlist->f_char; tlist++) {
#ifdef QWERTZ
		if(C(cmdchar)==C('y') && iflags.qwertz_layout)
			cmdchar+='z'-'y';
		if (cmdchar != (tlist->f_char & 0xff)) continue;
#else
		if ((*cmd & 0xff) != (tlist->f_char & 0xff)) continue;
#endif

		if (u.uburied && !tlist->can_if_buried) {
/*JP
		    You_cant("do that while you are buried!");
*/
		    You("埋まっている時にそんなことはできない！");
		    res = 0;
		} else {
		    /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
		    func = ((struct func_tab *)tlist)->f_funct;
		    if (tlist->f_text && !occupation && multi)
			set_occupation(func, tlist->f_text, multi);
		    res = (*func)();		/* perform the command */
		}
		if (!res) {
		    flags.move = FALSE;
		    multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}

	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
#if 0 /*JP*/
		!help_dir(0, "Invalid direction key!"))
#else
		!help_dir(0, "無効な方向指定です。"))
#endif
/*JP
	    Norep("Unknown command '%s'.", expcmd);
*/
	    Norep("'%s'コマンド？", expcmd);
	}
	/* didn't move */
	flags.move = FALSE;
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];
#ifdef JPEXTENSION
	if(Totter){	/* mirror move */
	    u.dx = -u.dx;
	}
#endif
	if (u.dx && u.dy && u.umonnum == PM_GRID_BUG) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt)) {
		pline(Never_mind);
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline(emsg);
		return 0;
	}
	return 1;
}

int
getdir(s)
const char *s;
{
	char dirsym;

#ifdef REDO
	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else
#endif
#if 0 /*JP*/
	    dirsym = yn_function ((s && *s != '^') ? s : "In what direction?",
					(char *)0, '\0');
#else
	    dirsym = yn_function ((s && *s != '^') ? s : "どの方向？",
					(char *)0, '\0');
#endif
#ifdef REDO
	savech(dirsym);
#endif
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		boolean did_help = FALSE;
		if(!index(quitchars, dirsym)) {
		    if (iflags.cmdassist) {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
#if 0 /*JP*/
					    "Invalid direction key!");
#else
					    "そっちの方向は無理だよ！");
#endif
		    }
/*JP
		    if (!did_help) pline("What a strange direction!");
*/
		    if (!did_help) pline("ずいぶんと奇妙な方向だ！");
		}
		return 0;
	}
	if(!u.dz && (Stunned || (Confusion && !rn2(5)))) confdir();
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	char ctrl;
	winid win;
	static const char wiz_only_list[] = "EFGIOVW";
	char buf[BUFSZ], buf2[BUFSZ], *expl;

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		Sprintf(buf, "cmdassist: %s", msg);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
	    ctrl = (sym - 'A') + 1;
	    if ((expl = dowhatdoes_core(ctrl, buf2))
		&& (!index(wiz_only_list, sym)
#ifdef WIZARD
		    || wizard
#endif
	                     )) {
#if 0 /*JP*/
		Sprintf(buf, "Are you trying to use ^%c%s?", sym,
			index(wiz_only_list, sym) ? "" :
			" as specified in the Guidebook");
#else
		Sprintf(buf, "^%cを%s使ってみる？", sym,
			index(wiz_only_list, sym) ? "" :
			"ガイドブックの記述どおりに");
#endif
		putstr(win, 0, buf);
		putstr(win, 0, "");
		putstr(win, 0, expl);
		putstr(win, 0, "");
/*JP
		putstr(win, 0, "To use that command, you press");
*/
		putstr(win, 0, "コマンドを使用するには、<Ctrl>キーと");
#if 0 /*JP*/
		Sprintf(buf,
			"the <Ctrl> key, and the <%c> key at the same time.", sym);
#else
		Sprintf(buf,
			"<%c>キーを同時に押してください。", sym);
#endif
		putstr(win, 0, buf);
		putstr(win, 0, "");
	    }
	}
	if (iflags.num_pad && u.umonnum == PM_GRID_BUG) {
#if 0 /*JP*/
	    putstr(win, 0, "Valid direction keys in your current form (with number_pad on) are:");
#else
	    putstr(win, 0, "有効な方向キー(number_pad オプション設定時):");
#endif
	    putstr(win, 0, "             8   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             2   ");
	} else if (u.umonnum == PM_GRID_BUG) {
#if 0 /*JP*/
	    putstr(win, 0, "Valid direction keys in your current form are:");
#else
	    putstr(win, 0, "有効な方向キー:");
#endif
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else if (iflags.num_pad) {
#if 0 /*JP*/
	    putstr(win, 0, "Valid direction keys (with number_pad on) are:");
#else
	    putstr(win, 0, "有効な方向キー(number_pad オプション設定時):");
#endif
	    putstr(win, 0, "          7  8  9");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          1  2  3");
	} else {
#if 0 /*JP*/
	    putstr(win, 0, "Valid direction keys are:");
#else
	    putstr(win, 0, "有効な方向キー:");
#endif
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
#if 0 /*JP*/
	putstr(win, 0, "          <  up");
	putstr(win, 0, "          >  down");
	putstr(win, 0, "          .  direct at yourself");
#else
	putstr(win, 0, "          <  上");
	putstr(win, 0, "          >  下");
	putstr(win, 0, "          .  自分自身に直接");
#endif
	putstr(win, 0, "");
#if 0 /*JP*/
	putstr(win, 0, "(Suppress this message with !cmdassist in config file.)");
#else
	putstr(win, 0, "(このメッセージを表示したくない場合は設定ファイルに !cmdassist を設定してください。)");
#endif
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x = (u.umonnum == PM_GRID_BUG) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
            x = sgn(x), y = sgn(y);
        } else {
            u.tx = u.ux+x;
            u.ty = u.uy+y;
            cmd[0] = CMD_TRAVEL;
            return cmd;
        }

        if(x == 0 && y == 0) {
            /* here */
            if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
                cmd[0]=mod == CLICK_1 ? 'q' : M('d');
                return cmd;
            } else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
                cmd[0]=M('s');
                return cmd;
            } else if((u.ux == xupstair && u.uy == yupstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
                      || (u.ux == xupladder && u.uy == yupladder)) {
                return "<";
            } else if((u.ux == xdnstair && u.uy == ydnstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
                      || (u.ux == xdnladder && u.uy == ydnladder)) {
                return ">";
            } else if(OBJ_AT(u.ux, u.uy)) {
                cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
                return cmd;
            } else {
                return "."; /* just rest */
            }
        }

        /* directional commands */

        dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
            cmd[1] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
            cmd[2] = 0;
            if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
                /* slight assistance to the player: choose kick/open for them */
                if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
                    cmd[0] = C('d');
                    return cmd;
                }
                if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
                    cmd[0] = 'o';
                    return cmd;
                }
            }
            if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
                cmd[0] = 's';
                cmd[1] = 0;
                return cmd;
            }
        }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
    } else {
	cmd[0] = (iflags.num_pad ? M(ndir[dir]) :
		(sdir[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	register int foo;
	boolean prezero = FALSE;

	multi = 0;
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
		    if (multi > 9) {
			clear_nhwindow(WIN_MESSAGE);
/*JP
			Sprintf(in_line, "Count: %d", multi);
*/
			Sprintf(in_line, "数: %d", multi);
			pline(in_line);
			mark_synch();
		    }
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else break;	/* not a digit */
	    }

	if (foo == '\033') {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    multi = last_multi = 0;
# ifdef REDO
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
# endif
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';
	if (foo == 'g' || foo == 'G' || foo == 'm' || foo == 'M' ||
	    foo == 'F' || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
#ifdef REDO
	    savech((char)foo);
#endif
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);
	if (prezero) in_line[0] = '\033';
	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	exit_nhwindows((char *)0);
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
#ifdef REDO
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);
#else
	    sym = Getchar();
#endif

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

STATIC_PTR int
dotravel()
{
	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return 0;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
/*JP
	pline("Where do you want to travel to?");
*/
	pline("どこに移動する？");
/*JP
	if (getpos(&cc, TRUE, "the desired destination") < 0) {
*/
	if (getpos(&cc, TRUE, "移動先") < 0) {
		/* user pressed ESC */
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void NDECL(win32con_debug_keystrokes);
extern void NDECL(win32con_handler_info);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		void NDECL((*fn));
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", win32con_debug_keystrokes},
		{"show keystroke handler information", win32con_handler_info},
#endif
		{(char *)0, (void NDECL((*)))0}		/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	if (num_menu_selections > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((genericptr_t) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(query,resp, def)
const char *query,*resp;
char def;
{
	char qbuf[QBUFSZ];
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	Strcat(qbuf,"...");
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
}
#endif

/**
 * Asks the player a yes/no question if paranoid is true.
 * @return 'y' or 'n'
 */
char
paranoid_yn(query,paranoid)
const char *query;
boolean paranoid;
{
	if (paranoid) {
		char buf[BUFSZ];
		char query_yesno[2*BUFSZ];
		/* put [yes/no] between question and question mark? */
		Sprintf(query_yesno, "%s [yes/no]", query);
		getlin (query_yesno, buf);
		(void) lcase (buf);
		return (!(strcmp (buf, "yes"))) ? 'y' : 'n';
	} else {
		return yn(query);
	}
}

/*cmd.c*/
