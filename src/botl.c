/*	SCCS Id: @(#)botl.c	3.4	1996/07/15	*/
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

#ifdef OVL0
extern const char *hu_stat[];	/* defined in eat.c */

const char * const enc_stat[] = {
#if 0 /*JP*/
	"",
	"Burdened",
	"Stressed",
	"Strained",
	"Overtaxed",
	"Overloaded"
#else
	"",
	"‚æ‚ë‚ß‚«",
	"ˆ³”—",
	"ŒÀŠE",
	"‰×d",
	"’´‰ß"
#endif
};

STATIC_DCL void NDECL(bot1);
STATIC_DCL void NDECL(bot2);
#endif /* OVL0 */

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)

extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
extern const struct text_color_option *text_colors;

struct color_option
text_color_of(text, color_options)
const char *text;
const struct text_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (strstri(color_options->text, text)
	 || strstri(text, color_options->text))
		return color_options->color_option;
	return text_color_of(text, color_options->next);
}

struct color_option
percentage_color_of(value, max, color_options)
int value, max;
const struct percent_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (100 * value <= color_options->percentage * max)
		return color_options->color_option;
	return percentage_color_of(value, max, color_options->next);
}

void
start_color_option(color_option)
struct color_option color_option;
{
	int i;
	if (color_option.color != NO_COLOR)
		term_start_color(color_option.color);
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_start_attr(i);
}

void
end_color_option(color_option)
struct color_option color_option;
{
	int i;
	if (color_option.color != NO_COLOR)
		term_end_color();
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_end_attr(i);
}

void
apply_color_option(color_option, newbot2)
struct color_option color_option;
const char *newbot2;
{
	if (!iflags.use_status_colors) return;
	curs(WIN_STATUS, 1, 1);
	start_color_option(color_option);
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

void
add_colored_text(text, newbot2)
const char *text;
char *newbot2;
{
	char *nb;
	struct color_option color_option;

	if (*text == '\0') return;

	/* don't add anything if it can't be displayed.
	 * Otherwise the color of invisible text may bleed into
	 * the statusline. */
	if (strlen(newbot2) >= min(MAXCO, CO)-1) return;

	if (!iflags.use_status_colors) {
		Sprintf(nb = eos(newbot2), " %s", text);
                return;
        }

	Strcat(nb = eos(newbot2), " ");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Strcat(nb = eos(nb), text);
	curs(WIN_STATUS, 1, 1);
       	color_option = text_color_of(text, text_colors);
	start_color_option(color_option);
	/* Trim the statusline to always have the end color
	 * to have effect. */
	newbot2[min(MAXCO, CO)-1] = '\0';
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else /* OVLB */
STATIC_OVL NEARDATA int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

STATIC_DCL const char *NDECL(rank);

#ifdef OVL1

/* convert experience level (1..30) to rank index (0..8) */
int
xlev_to_rank(xlev)
int xlev;
{
	return (xlev <= 2) ? 0 : (xlev <= 30) ? ((xlev + 2) / 4) : 8;
}

#if 0	/* not currently needed */
/* convert rank index (0..8) to experience level (1..30) */
int
rank_to_xlev(rank)
int rank;
{
	return (rank <= 0) ? 1 : (rank <= 8) ? ((rank * 4) - 2) : 30;
}
#endif

const char *
rank_of(lev, monnum, female)
	int lev;
	short monnum;
	boolean female;
{
	register struct Role *role;
	register int i;


	/* Find the role */
	for (role = (struct Role *) roles; role->name.m; role++)
	    if (monnum == role->malenum || monnum == role->femalenum)
	    	break;
	if (!role->name.m)
	    role = &urole;

	/* Find the rank */
	for (i = xlev_to_rank((int)lev); i >= 0; i--) {
	    if (female && role->rank[i].f) return (role->rank[i].f);
	    if (role->rank[i].m) return (role->rank[i].m);
	}

	/* Try the role name, instead */
	if (female && role->name.f) return (role->name.f);
	else if (role->name.m) return (role->name.m);
	return ("Player");
}


STATIC_OVL const char *
rank()
{
	return(rank_of(u.ulevel, Role_switch, flags.female));
}

int
title_to_mon(str, rank_indx, title_length)
const char *str;
int *rank_indx, *title_length;
{
	register int i, j;


	/* Loop through each of the roles */
	for (i = 0; roles[i].name.m; i++)
	    for (j = 0; j < 9; j++) {
	    	if (roles[i].rank[j].m && !strncmpi(str,
	    			roles[i].rank[j].m, strlen(roles[i].rank[j].m))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].m);
	    	    return roles[i].malenum;
	    	}
	    	if (roles[i].rank[j].f && !strncmpi(str,
	    			roles[i].rank[j].f, strlen(roles[i].rank[j].f))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].f);
	    	    return ((roles[i].femalenum != NON_PM) ?
	    	    		roles[i].femalenum : roles[i].malenum);
	    	}
	    }
	return NON_PM;
}

#endif /* OVL1 */
#ifdef OVLB

void
max_rank_sz()
{
	register int i, r, maxr = 0;
	for (i = 0; i < 9; i++) {
	    if (urole.rank[i].m && (r = strlen(urole.rank[i].m)) > maxr) maxr = r;
	    if (urole.rank[i].f && (r = strlen(urole.rank[i].f)) > maxr) maxr = r;
	}
	mrank_sz = maxr;
	return;
}

#endif /* OVLB */
#ifdef OVL0

#ifdef SCORE_ON_BOTL
long
botl_score()
{
    int deepest = deepest_lev_reached(FALSE);
#ifndef GOLDOBJ
    long ugold = u.ugold + hidden_gold();

    if ((ugold -= u.ugold0) < 0L) ugold = 0L;
    return ugold + u.urscore + (long)(50 * (deepest - 1))
#else
    long umoney = money_cnt(invent) + hidden_gold();

    if ((umoney -= u.umoney0) < 0L) umoney = 0L;
    return umoney + u.urscore + (long)(50 * (deepest - 1))
#endif
			  + (long)(deepest > 30 ? 10000 :
				   deepest > 20 ? 1000*(deepest - 20) : 0);
}
#endif

#ifdef DUMP_LOG
void bot1str(char *newbot1)
#else
STATIC_OVL void
bot1()
#endif
{
#ifndef DUMP_LOG
	char newbot1[MAXCO];
#endif
	register char *nb;
	register int i,j;

	Strcpy(newbot1, plname);
	if('a' <= newbot1[0] && newbot1[0] <= 'z') newbot1[0] += 'A'-'a';
#if 1 /*JP*/
	if(is_kanji1(newbot1, 9))
		newbot1[9] = '_';
#endif
	newbot1[10] = 0;
/*JP
	Sprintf(nb = eos(newbot1)," the ");
*/
	Sprintf(nb = eos(newbot1)," ");

	if (Upolyd) {
		char mbot[BUFSZ];
		int k = 0;

/*JP
		Strcpy(mbot, mons[u.umonnum].mname);
*/
		Strcpy(mbot, jtrns_mon_gen(mons[u.umonnum].mname, flags.female));
		while(mbot[k] != 0) {
		    if ((k == 0 || (k > 0 && mbot[k-1] == ' ')) &&
					'a' <= mbot[k] && mbot[k] <= 'z')
			mbot[k] += 'A' - 'a';
		    k++;
		}
		Sprintf(nb = eos(nb), mbot);
	} else
		Sprintf(nb = eos(nb), rank());

	Sprintf(nb = eos(nb),"  ");
	i = mrank_sz + 15;
	j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
	if((i - j) > 0)
		Sprintf(nb = eos(nb),"%*s", i-j, " ");	/* pad with spaces */
	if (ACURR(A_STR) > 18) {
		if (ACURR(A_STR) > STR18(100))
/*JP
		    Sprintf(nb = eos(nb),"St:%2d ",ACURR(A_STR)-100);
*/
		    Sprintf(nb = eos(nb),"‹­:%2d ",ACURR(A_STR)-100);
		else if (ACURR(A_STR) < STR18(100))
/*JP
		    Sprintf(nb = eos(nb), "St:18/%02d ",ACURR(A_STR)-18);
*/
		    Sprintf(nb = eos(nb), "‹­:18/%02d ",ACURR(A_STR)-18);
		else
#if 0 /*JP*/
		    Sprintf(nb = eos(nb),"St:18/** ");
#else
		    Sprintf(nb = eos(nb),"‹­:18/** ");
#endif
	} else
/*JP
		Sprintf(nb = eos(nb), "St:%-1d ",ACURR(A_STR));
*/
		Sprintf(nb = eos(nb), "‹­:%-1d ",ACURR(A_STR));
	Sprintf(nb = eos(nb),
/*JP
		"Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d",
*/
		"‘:%-1d ‘Ï:%-1d ’m:%-1d Œ«:%-1d –£:%-1d ",
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
#if 0 /*JP*/
	Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "  Chaotic" :
			(u.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Lawful");
#else
	Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "¬“×" :
			(u.ualign.type == A_NEUTRAL) ? "’†—§" : "’˜");
#endif
#ifdef SCORE_ON_BOTL
	if (flags.showscore)
/*JP
	    Sprintf(nb = eos(nb), " S:%ld", botl_score());
*/
	    Sprintf(nb = eos(nb), "%ld“_", botl_score());
#endif
#ifdef DUMP_LOG
}
STATIC_OVL void
bot1()
{
	char newbot1[MAXCO];

	bot1str(newbot1);
#endif
	curs(WIN_STATUS, 1, 0);
	putstr(WIN_STATUS, 0, newbot1);
}

/* provide the name of the current level for display by various ports */
int
describe_level(buf)
char *buf;
{
	int ret = 1;

	/* TODO:	Add in dungeon name */
	if (Is_knox(&u.uz))
/*JP
		Sprintf(buf, "%s ", dungeons[u.uz.dnum].dname);
*/
		Sprintf(buf, "%s ", jtrns_obj('d', dungeons[u.uz.dnum].dname));
	else if (In_quest(&u.uz))
/*JP
		Sprintf(buf, "Home %d ", dunlev(&u.uz));
*/
		Sprintf(buf, "ŒÌ‹½ %d ", dunlev(&u.uz));
	else if (In_endgame(&u.uz))
		Sprintf(buf,
/*JP
			Is_astralevel(&u.uz) ? "Astral Plane " : "End Game ");
*/
			Is_astralevel(&u.uz) ? "¸—ìŠE " : "ÅIŽŽ—û ");
	else if (Is_blackmarket(&u.uz))
/*JP
		Sprintf(buf, "Blackmarket ");
*/
		Sprintf(buf, "ˆÅŽs ");
	else if (Is_town_level(&u.uz))
/*JP
		Sprintf(buf, "Town ");
*/
		Sprintf(buf, "ŠX ");
	else if (Is_minetown_level(&u.uz))
/*JP
		Sprintf(buf, "Mine Town:%-2d ", depth(&u.uz));
*/
		Sprintf(buf, "zŽRŠX:%-2d ", depth(&u.uz));
	else {
#if 0/*JP*/
		char *dgn_name = dungeons[u.uz.dnum].dname;
		if (!strncmpi(dgn_name, "The ", 4)) { dgn_name += 4; }
		/* ports with more room may expand this one */
		Sprintf(buf, "%s:%-2d ", 
		        iflags.show_dgn_name ? dgn_name : "Dlvl",
		        depth(&u.uz));
#else
		/* ports with more room may expand this one */
		Sprintf(buf, "%s:%-2d ", iflags.show_dgn_name
		        ? jtrns_obj('d', dungeons[u.uz.dnum].dname)
		        : "’n‰º", depth(&u.uz));
#endif
		ret = 0;
	}
	return ret;
}

#ifdef DUMP_LOG
void bot2str(newbot2)
char* newbot2;
#else
STATIC_OVL void
bot2()
#endif
{
#ifndef DUMP_LOG
	char  newbot2[MAXCO];
#endif
	register char *nb;
	int hp, hpmax;
	int cap = near_capacity();
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	int save_botlx = flags.botlx;
#endif

	hp = Upolyd ? u.mh : u.uhp;
	hpmax = Upolyd ? u.mhmax : u.uhpmax;

	if(hp < 0) hp = 0;
	(void) describe_level(newbot2);
	Sprintf(nb = eos(newbot2), "%c:%-2ld", oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
		u.ugold
#else
		money_cnt(invent)
#endif
	       );

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	Strcat(nb = eos(newbot2), " HP:");
*/
	Strcat(nb = eos(newbot2), " ‘Ì:");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = 0;

	Sprintf(nb = eos(nb), "%d(%d)", hp, hpmax);
	apply_color_option(percentage_color_of(hp, hpmax, hp_colors), newbot2);
#else
/*JP
	Sprintf(nb = eos(nb), " HP:%d(%d)", hp, hpmax);
*/
	Sprintf(nb = eos(nb), " ‘Ì:%d(%d)", hp, hpmax);
#endif
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	Strcat(nb = eos(nb), " Pw:");
*/
	Strcat(nb = eos(nb), " –‚:");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Sprintf(nb = eos(nb), "%d(%d)", u.uen, u.uenmax);
	apply_color_option(percentage_color_of(u.uen, u.uenmax, pw_colors), newbot2);
#else
/*JP
	Sprintf(nb = eos(nb), " Pw:%d(%d)", u.uen, u.uenmax);
*/
	Sprintf(nb = eos(nb), " –‚:%d(%d)", u.uen, u.uenmax);
#endif
/*JP
	Sprintf(nb = eos(nb), " AC:%-2d", u.uac);
*/
	Sprintf(nb = eos(nb), " ŠZ:%-2d", u.uac);
	if (Upolyd)
		Sprintf(nb = eos(nb), " HD:%d", mons[u.umonnum].mlevel);
#ifdef EXP_ON_BOTL
	else if(flags.showexp)
/*JP
		Sprintf(nb = eos(nb), " Xp:%u/%-1ld", u.ulevel,u.uexp);
*/
		Sprintf(nb = eos(nb), " ŒoŒ±:%u/%-1ld", u.ulevel,u.uexp);
#endif
	else
/*JP
		Sprintf(nb = eos(nb), " Exp:%u", u.ulevel);
*/
		Sprintf(nb = eos(nb), " ŒoŒ±:%u", u.ulevel);

	if(flags.time)
/*JP
		Sprintf(nb = eos(nb), " T:%ld", moves);
*/
		Sprintf(nb = eos(nb), " •à:%ld", moves);

#ifdef REALTIME_ON_BOTL
	if(iflags.showrealtime) {
		time_t currenttime = get_realtime();
		Sprintf(nb = eos(nb), " %ld:%2.2ld", currenttime / 3600, 
		        (currenttime % 3600) / 60);
	}
#endif

	if(strcmp(hu_stat[u.uhs], "        "))
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text(hu_stat[u.uhs], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", hu_stat[u.uhs]);
#endif
	if(Confusion)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
		add_colored_text("Conf", newbot2);
*/
		add_colored_text("¬—", newbot2);
#else
/*JP
		Strcat(nb = eos(nb), " Conf");
*/
		Strcat(nb = eos(nb), " ¬—");
#endif
	if(Sick) {
		if (u.usick_type & SICK_VOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
			add_colored_text("FoodPois", newbot2);
*/
			add_colored_text("H“Å", newbot2);
#else
/*JP
			Strcat(nb = eos(nb), " FoodPois");
*/
			Strcat(nb = eos(nb), " H“Å");
#endif
		if (u.usick_type & SICK_NONVOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
			add_colored_text("Ill", newbot2);
*/
			add_colored_text("•a‹C", newbot2);
#else
/*JP
			Strcat(nb = eos(nb), " Ill");
*/
			Strcat(nb = eos(nb), " •a‹C");
#endif
	}
	if(Blind)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	     	add_colored_text("Blind", newbot2);
*/
	     	add_colored_text("–Ó–Ú", newbot2);
#else
/*JP
		Strcat(nb = eos(nb), " Blind");
*/
		Strcat(nb = eos(nb), " –Ó–Ú");
#endif
	if(Stunned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	     	add_colored_text("Stun", newbot2);
*/
	     	add_colored_text("á¿ò", newbot2);
#else
/*JP
		Strcat(nb = eos(nb), " Stun");
*/
		Strcat(nb = eos(nb), " á¿ò");
#endif
	if(Hallucination)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	     	add_colored_text("Hallu", newbot2);
*/
	     	add_colored_text("Œ¶Šo", newbot2);
#else
/*JP
		Strcat(nb = eos(nb), " Hallu");
*/
		Strcat(nb = eos(nb), " Œ¶Šo");
#endif
	if(Slimed)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
/*JP
	     	add_colored_text("Slime", newbot2);
*/
	     	add_colored_text("—n‰ð", newbot2);
#else
/*JP
		Strcat(nb = eos(nb), " Slime");
*/
		Strcat(nb = eos(nb), " —n‰ð");
#endif
	if(cap > UNENCUMBERED)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text(enc_stat[cap], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", enc_stat[cap]);
#endif
#ifdef DUMP_LOG
}
STATIC_OVL void
bot2()
{
	char newbot2[MAXCO];
	int save_botlx = flags.botlx;
	bot2str(newbot2);
#endif
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = save_botlx;
}

void
bot()
{
	bot1();
	bot2();
	flags.botl = flags.botlx = 0;
}

#endif /* OVL0 */

/*botl.c*/
