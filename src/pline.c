/*	SCCS Id: @(#)pline.c	3.4	1999/11/28	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2005
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009
**	JNetHack may be freely redistributed.  See license for details. 
*/

#define NEED_VARARGS /* Uses ... */	/* comment line for pre-compiled headers */
#include "hack.h"
#include "epri.h"
#ifdef WIZARD
#include "edog.h"
#include "eshk.h"
#endif

#ifdef OVLB

static boolean no_repeat = FALSE;

static char *FDECL(You_buf, (int));

#if defined(DUMP_LOG) && defined(DUMPMSGS)
char msgs[DUMPMSGS][BUFSZ];
int msgs_count[DUMPMSGS];
int lastmsg = -1;
#endif

void
msgpline_add(typ, pattern)
     int typ;
     char *pattern;
{
    struct _plinemsg *tmp = (struct _plinemsg *) alloc(sizeof(struct _plinemsg));
    if (!tmp) return;
    tmp->msgtype = typ;
    tmp->pattern = strdup(pattern);
    tmp->next = pline_msg;
    pline_msg = tmp;
}

void
msgpline_free()
{
    struct _plinemsg *tmp = pline_msg;
    struct _plinemsg *tmp2;
    while (tmp) {
	free(tmp->pattern);
	tmp2 = tmp;
	tmp = tmp->next;
	free(tmp2);
    }
    pline_msg = NULL;
}

int
msgpline_type(msg)
     char *msg;
{
    struct _plinemsg *tmp = pline_msg;
    while (tmp) {
	if (pmatch(tmp->pattern, msg)) return tmp->msgtype;
	tmp = tmp->next;
    }
    return MSGTYP_NORMAL;
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vpline, (const char *, va_list));

void
pline VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vpline(line, VA_ARGS);
	VA_END();
}

char prevmsg[BUFSZ];

# ifdef USE_STDARG
static void
vpline(const char *line, va_list the_args) {
# else
static void
vpline(line, the_args) const char *line; va_list the_args; {
# endif

#else	/* USE_STDARG | USE_VARARG */

#define vpline pline

void
pline VA_DECL(const char *, line)
#endif	/* USE_STDARG | USE_VARARG */

	char pbuf[BUFSZ];
	int typ;
/* Do NOT use VA_START and VA_END in here... see above */

	if (!line || !*line) return;
	if (index(line, '%')) {
	    Vsprintf(pbuf,line,VA_ARGS);
	    line = pbuf;
	}
#if defined(DUMP_LOG) && defined(DUMPMSGS)
	if (DUMPMSGS > 0 && !program_state.gameover) {
		/* count identical messages */
		if (!strncmp(msgs[lastmsg], line, BUFSZ)) {
			msgs_count[lastmsg] += 1;
		} else {
			lastmsg = (lastmsg + 1) % DUMPMSGS;
			strncpy(msgs[lastmsg], line, BUFSZ);
			msgs_count[lastmsg] = 1;
		}
	}
#endif
	typ = msgpline_type(line);
	if (!iflags.window_inited) {
	    raw_print(line);
	    return;
	}
#ifndef MAC
	if (no_repeat && !strcmp(line, toplines))
	    return;
#endif /* MAC */
	if (vision_full_recalc) vision_recalc(0);
	if (u.ux) flush_screen(1);		/* %% */
	if (typ == MSGTYP_NOSHOW) return;
	if (typ == MSGTYP_NOREP && !strcmp(line, prevmsg)) return;
	putstr(WIN_MESSAGE, 0, line);
	strncpy(prevmsg, line, BUFSZ);
	if (typ == MSGTYP_STOP) display_nhwindow(WIN_MESSAGE, TRUE); /* --more-- */
}

/*VARARGS1*/
void
Norep VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, const char *);
	no_repeat = TRUE;
	vpline(line, VA_ARGS);
	no_repeat = FALSE;
	VA_END();
	return;
}

/* work buffer for You(), &c and verbalize() */
static char *you_buf = 0;
static int you_buf_siz = 0;

static char *
You_buf(siz)
int siz;
{
	if (siz > you_buf_siz) {
		if (you_buf) free((genericptr_t) you_buf);
		you_buf_siz = siz + 10;
		you_buf = (char *) alloc((unsigned) you_buf_siz);
	}
	return you_buf;
}

void
free_youbuf()
{
	if (you_buf) free((genericptr_t) you_buf),  you_buf = (char *)0;
	you_buf_siz = 0;
}

/* `prefix' must be a string literal, not a pointer */
#define YouPrefix(pointer,prefix,text) \
 Strcpy((pointer = You_buf((int)(strlen(text) + sizeof prefix))), prefix)

#define YouMessage(pointer,prefix,text) \
 strcat((YouPrefix(pointer, prefix, text), pointer), text)

/*VARARGS1*/
void
You VA_DECL(const char *, line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "You ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "���Ȃ���", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
Your VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "Your ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "���Ȃ���", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_feel VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "You feel ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "���Ȃ���", line), VA_ARGS);
	VA_END();
}


/*VARARGS1*/
void
You_cant VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "You can't ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "���Ȃ���", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
pline_The VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "The ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
There VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
/*JP
	vpline(YouMessage(tmp, "There ", line), VA_ARGS);
*/
	vpline(YouMessage(tmp, "", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_hear VA_DECL(const char *,line)
	char *tmp;
#if 1 /*JP*/
	char *adj;
	char *p;
#endif
	VA_START(line);
	VA_INIT(line, const char *);
	if (Underwater)
/*JP
		YouPrefix(tmp, "You barely hear ", line);
*/
		adj = "��������";
	else if (u.usleep)
/*JP
		YouPrefix(tmp, "You dream that you hear ", line);
*/
		adj = "���̒���";
	else
/*JP
		YouPrefix(tmp, "You hear ", line);
*/
		adj = "";
#if 0 /*JP*/
	vpline(strcat(tmp, line), VA_ARGS);
#else
	tmp = You_buf(strlen(adj) + strlen(line) + sizeof("���Ȃ���   "));

	p = (char *)strstr(line, "����") ;
	if( p == NULL )
		Strcpy(tmp, "���Ȃ���");
	else
		Strcpy(tmp, "");
	if( p != NULL || (p = (char *)strstr(line, "����")) != NULL ){
		strncat(tmp, line, (p - line));
		strcat(tmp, adj);
		strcat(tmp, p);
	} else{
		Strcat(tmp, line);
	}
	vpline(tmp, VA_ARGS);
#endif
	VA_END();
}

/*VARARGS1*/
void
verbalize VA_DECL(const char *,line)
	char *tmp;
	if (!flags.soundok) return;
	VA_START(line);
	VA_INIT(line, const char *);
#if 0 /*JP*/
	tmp = You_buf((int)strlen(line) + sizeof "\"\"");
	Strcpy(tmp, "\"");
	Strcat(tmp, line);
	Strcat(tmp, "\"");
#else
	tmp = You_buf((int)strlen(line) + sizeof "�u�v");
	Strcpy(tmp, "�u");
	Strcat(tmp, line);
	Strcat(tmp, "�v");
#endif
	vpline(tmp, VA_ARGS);
	VA_END();
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vraw_printf,(const char *,va_list));

void
raw_printf VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vraw_printf(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vraw_printf(const char *line, va_list the_args) {
# else
static void
vraw_printf(line, the_args) const char *line; va_list the_args; {
# endif

#else  /* USE_STDARG | USE_VARARG */

void
raw_printf VA_DECL(const char *, line)
#endif
/* Do NOT use VA_START and VA_END in here... see above */

	if(!index(line, '%'))
	    raw_print(line);
	else {
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,line,VA_ARGS);
	    raw_print(pbuf);
	}
}


/*VARARGS1*/
void
impossible VA_DECL(const char *, s)
	VA_START(s);
	VA_INIT(s, const char *);
	if (program_state.in_impossible)
		panic("impossible called impossible");
	program_state.in_impossible = 1;
	{
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,s,VA_ARGS);
	    paniclog("impossible", pbuf);
	}
	vpline(s,VA_ARGS);
#if 0 /*JP*/
	pline("Program in disorder; you probably should S)ave and restart the process.");
#else
	pline("�v���O�����ɏ�Q���� - �Z�[�u(S)���čĎ��s�����ق����悳�������B");
#endif
	program_state.in_impossible = 0;
	VA_END();
}

void
warning VA_DECL(const char *, s)
	char str[BUFSZ];
	VA_START(s);
	VA_INIT(s, const char *);
	{
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,s,VA_ARGS);
	    paniclog("warning", pbuf);
	}
	Vsprintf(str,s,VA_ARGS);
	pline("Warning: %s\n", str);
	VA_END();
}

const char *
align_str(alignment)
    aligntyp alignment;
{
    switch ((int)alignment) {
#if 0 /*JP*/
	case A_CHAOTIC: return "chaotic";
	case A_NEUTRAL: return "neutral";
	case A_LAWFUL:	return "lawful";
	case A_NONE:	return "unaligned";
#else
	case A_CHAOTIC: return "����";
	case A_NEUTRAL: return "����";
	case A_LAWFUL:	return "����";
	case A_NONE:	return "���S";
#endif
    }
/*JP
    return "unknown";
*/
    return "�s��";
}

void
mstatusline(mtmp)
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
#if 0 /*JP*/
	if (mtmp->mtame) {	  Strcat(info, ", tame");
#ifdef WIZARD
	    if (wizard) {
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport);
		Strcat(info, ")");
	    }
#endif
	}
#else
	if (mtmp->mtame) {	  Strcat(info, ", �����Ȃ炳��Ă���");
#ifdef WIZARD
	    if (wizard) {
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport);
		Strcat(info, ")");
	    }
#endif
	}
#endif /*JP*/
/*JP
	else if (mtmp->mpeaceful) Strcat(info, ", peaceful");
*/
	else if (mtmp->mpeaceful) Strcat(info, ", �F�D�I");
/*JP
	if (mtmp->meating)	  Strcat(info, ", eating");
*/
	if (mtmp->meating)	  Strcat(info, ", �H����");
/*JP
	if (mtmp->mcan)		  Strcat(info, ", cancelled");
*/
	if (mtmp->mcan)		  Strcat(info, ", ����");
/*JP
	if (mtmp->mconf)	  Strcat(info, ", confused");
*/
	if (mtmp->mconf)	  Strcat(info, ", �������");
#if 0 /*JP:T*/
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", blind");
#else
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", �Ӗ�");
#endif
/*JP
	if (mtmp->mstun)	  Strcat(info, ", stunned");
*/
	if (mtmp->mstun)	  Strcat(info, ", ���炭����");
#if 0 /*JP*/
	if (mtmp->msleeping)	  Strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", can't move");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", meditating");
	else if (mtmp->mflee)	  Strcat(info, ", scared");
#else /*JP*/
	if (mtmp->msleeping)	  Strcat(info, ", �������");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", �����Ȃ�");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", ���z��");
	else if (mtmp->mflee)	  Strcat(info, ", �����Ă���");
#endif
/*JP
	if (mtmp->mtrapped)	  Strcat(info, ", trapped");
*/
	if (mtmp->mtrapped)	  Strcat(info, ", 㩂ɂ������Ă���");
#if 0 /*JP:T*/
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
#else
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", �f����" :
					mtmp->mspeed == MSLOW ? ", �x��" :
					", ���x�s��");
#endif
/*JP
	if (mtmp->mundetected)	  Strcat(info, ", concealed");
*/
	if (mtmp->mundetected)	  Strcat(info, ", �B��Ă���");
/*JP
	if (mtmp->minvis)	  Strcat(info, ", invisible");
*/
	if (mtmp->minvis)	  Strcat(info, ", �s��");
#if 0 /*JP:T*/
	if (mtmp == u.ustuck)	  Strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
#else
	if (mtmp == u.ustuck)	  Strcat(info,  
			(sticks(youmonst.data)) ? ", ���Ȃ����͂܂��Ă���" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", ���ݍ���ł���" :
				", ��������ł���") :
				", �͂܂��Ă���");
#endif

#ifdef STEED
#if 0 /*JP*/
	if (mtmp == u.usteed)	  Strcat(info, ", carrying you");
#else
	if (mtmp == u.usteed)	  Strcat(info, ", ���Ȃ����悹�Ă���");
#endif
#endif
#ifdef WIZARD
	if (wizard &&
	    mtmp->isshk && ESHK(mtmp)->cheapskate) {
/*JP
		Strcat(info, ", cheapskate");
*/
		Strcat(info, ", �P�`�ł���");
	}
#endif

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	Strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
	    (SUPPRESS_IT|SUPPRESS_INVISIBLE), FALSE));

/*JP
	pline("Status of %s (%s):  Level %d  HP %d(%d)  AC %d%s.",
*/
	pline("%s�̏�� (%s)�F Level %d  HP %d(%d)  AC %d%s",
		monnambuf,
		align_str(alignment),
		mtmp->m_lev,
		mtmp->mhp,
		mtmp->mhpmax,
		find_mac(mtmp),
		info);
}

void
ustatusline()
{
	char info[BUFSZ];

	info[0] = '\0';
	if (Sick) {
#if 0 /*JP*/
		Strcat(info, ", dying from");
#else
		Strcat(info, ", ");
#endif
		if (u.usick_type & SICK_VOMITABLE)
/*JP
			Strcat(info, " food poisoning");
*/
			Strcat(info, "�H����");
		if (u.usick_type & SICK_NONVOMITABLE) {
			if (u.usick_type & SICK_VOMITABLE)
/*JP
				Strcat(info, " and");
*/
				Strcat(info, "��");
/*JP
			Strcat(info, " illness");
*/
			Strcat(info, "�a�C");
		}
#if 1 /*JP*/
		Strcat(info, "�Ŏ��ɂ���");
#endif
	}
#if 0 /*JP*/
	if (Stoned)		Strcat(info, ", solidifying");
	if (Slimed)		Strcat(info, ", becoming slimy");
	if (Strangled)		Strcat(info, ", being strangled");
	if (Vomiting)		Strcat(info, ", nauseated"); /* !"nauseous" */
	if (Confusion)		Strcat(info, ", confused");
#else
	if (Stoned)		Strcat(info, ", �Ή�������");
	if (Slimed)		Strcat(info, ", �X���C���ɂȂ����");
	if (Strangled)		Strcat(info, ", ����i�߂��Ă���");
	if (Vomiting)		Strcat(info, ", �f���C������");
	if (Confusion)		Strcat(info, ", �������");
#endif
	if (Blind) {
/*JP
	    Strcat(info, ", blind");
*/
	    Strcat(info, ", �Ӗڏ��");
	    if (u.ucreamed) {
#if 0 /*JP*/
		if ((long)u.ucreamed < Blinded || Blindfolded
						|| !haseyes(youmonst.data))
		    Strcat(info, ", cover");
		Strcat(info, "ed by sticky goop");
#else
		Strcat(info, "�˂΂˂΂ׂƂ����̂�");
		if ((long)u.ucreamed < Blinded || Blindfolded
						|| !haseyes(youmonst.data))
		    Strcat(info, "�����Ă���A");
#endif
	    }	/* note: "goop" == "glop"; variation is intentional */
	}
/*JP
	if (Stunned)		Strcat(info, ", stunned");
*/
	if (Stunned)		Strcat(info, ", ���炭����");
#ifdef STEED
	if (!u.usteed)
#endif
	if (Wounded_legs) {
	    const char *what = body_part(LEG);
	    if ((Wounded_legs & BOTH_SIDES) == BOTH_SIDES)
		what = makeplural(what);
/*JP
				Sprintf(eos(info), ", injured %s", what);
*/
				Sprintf(eos(info), ", %s�ɂ��������Ă���", what);
	}
/*JP
	if (Glib)		Sprintf(eos(info), ", slippery %s",
*/
	if (Glib)		Sprintf(eos(info), ", %s���ʂ�ʂ�",
					makeplural(body_part(HAND)));
#if 0 /*JP*/
	if (u.utrap)		Strcat(info, ", trapped");
	if (Fast)		Strcat(info, Very_fast ?
						", very fast" : ", fast");
	if (u.uundetected)	Strcat(info, ", concealed");
	if (Invis)		Strcat(info, ", invisible");
#else
	if (u.utrap)		Strcat(info, ", 㩂ɂ������Ă���");
	if (Fast)		Strcat(info, Very_fast ?
						", �ƂĂ��f����" : ", �f����");
	if (u.uundetected)	Strcat(info, ", �B��Ă���");
	if (Invis)		Strcat(info, ", �s��");
#endif
	if (u.ustuck) {
#if 0 /*JP*/
	    if (sticks(youmonst.data))
		Strcat(info, ", holding ");
	    else
		Strcat(info, ", held by ");
	    Strcat(info, mon_nam(u.ustuck));
#else
	    Strcat(info, ", ");
	    Strcat(info, mon_nam(u.ustuck));
	    if (sticks(youmonst.data))
		Strcat(info, "��͂܂��Ă���");
	    else
		Strcat(info, "�ɒ͂܂����Ă���");
#endif
	}

/*JP
	pline("Status of %s (%s%s):  Level %d  HP %d(%d)  AC %d%s.",
*/
	pline("%s�̏�� (%s %s)�F Level %d  HP %d(%d)  AC %d%s",
#if 0 /*JP*/
		plname,
		    (u.ualign.record >= 20) ? "piously " :
		    (u.ualign.record > 13) ? "devoutly " :
		    (u.ualign.record > 8) ? "fervently " :
		    (u.ualign.record > 3) ? "stridently " :
		    (u.ualign.record == 3) ? "" :
		    (u.ualign.record >= 1) ? "haltingly " :
		    (u.ualign.record == 0) ? "nominally " :
					    "insufficiently ",
#else
	      plname,
		    (u.ualign.record >= 20) ? "�h�i" :
		    (u.ualign.record > 13) ? "�M�S�[��" :
		    (u.ualign.record > 8) ? "�M��" :
		    (u.ualign.record > 3) ? "���̂��񍂂�" :
		    (u.ualign.record == 3) ? "" :
		    (u.ualign.record >= 1) ? "�L������" :
		    (u.ualign.record == 0) ? "���f" :
					    "�s�K��",
#endif
		align_str(u.ualign.type),
		Upolyd ? mons[u.umonnum].mlevel : u.ulevel,
		Upolyd ? u.mh : u.uhp,
		Upolyd ? u.mhmax : u.uhpmax,
		u.uac,
		info);
}

void
self_invis_message()
{
#if 0 /*JP*/
	pline("%s %s.",
	    Hallucination ? "Far out, man!  You" : "Gee!  All of a sudden, you",
	    See_invisible ? "can see right through yourself" :
		"can't see yourself");
#else
	pline("%s���Ȃ���%s�B",
	    Hallucination ? "���[�I�I" : "���I�ˑR",
	    See_invisible ? "�������g�������ƌ����Ȃ��Ȃ���" :
		"�������g�������Ȃ��Ȃ���");
#endif
}

#endif /* OVLB */
/*pline.c*/
