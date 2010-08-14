/*	SCCS Id: @(#)engrave.c	3.4	2001/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2008
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009-2010
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"
#include "lev.h"
#include "decl.h"
#include <ctype.h>

#ifdef OVLB
/* random engravings */
static const char *random_mesg[] = {
#if 0 /*JP*/
	"Elbereth",
	/* trap engravings */
	"Vlad was here", "ad aerarium",
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"Kilroy was here",
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"You won't get it up the steps", /* Adventure */
	"Lasciate ogni speranza o voi ch'entrate.", /* Inferno */
	"Well Come", /* Prisoner */
	"We apologize for the inconvenience.", /* So Long... */
	"See you next Wednesday", /* Thriller */
	"notary sojak", /* Smokey Stover */
	"For a good time call 8?7-5309",
	"Please don't feed the animals.", /* Various zoos around the world */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
#ifdef MAIL
	"You've got mail!", /* AOL */
#endif
	"As if!", /* Clueless */
	/* From JNetHack */
	"Moon prism power make up!", /* Sailor Moon */
	"Lizard's Tail!", /* Mahoujin Guru Guru */
	"It's a show time!" /* St. Tail */
	/* From Slash'Em */
	/* [Tom] added these */
	"Y?v?l s??ks!", /* just kidding... */
	"T?m ?as h?r?",
	/* Tsanth added these */
	"Gazortenplatz", /* Tribute to David Fizz */
	"John 3:16", /* You see this everywhere; why not here? */
	"....TCELES B HSUP   A magic spell?", /* Final Fantasy I (US) */
	"Turn around.", /* Various people at various times in history */
	"UY WUZ HERE", /* :] */
	"I'll be back!", /* Terminator */
	"That is not dead which can eternal lie.", /* HPL */
	/* From NAO */
	"Arooo!  Werewolves of Yendor!", /* gang tag */
	"Dig for Victory here", /* pun, duh */
	"Don't go this way",
	"Gaius Julius Primigenius was here.  Why are you late?", /* pompeii */
	"Go left --->", "<--- Go right",
	"Haermund Hardaxe carved these runes", /* viking graffiti */
	"Here be dragons",
	"Need a light?  Come visit the Minetown branch of Izchak's Lighting Store!",
	"Save now, and do your homework!",
	"Snakes on the Astral Plane - Soon in a dungeon near you!",
	"There was a hole here.  It's gone now.",
	"The Vibrating Square",
	"This is a pit!",
	"This is not the dungeon you are looking for.",
	"This square deliberately left blank.",
	"Warning, Exploding runes!",
	"Watch out, there's a gnome with a wand of death behind that door!",
	"X marks the spot",
	"X <--- You are here.",
	"You are the one millionth visitor to this place!  Please wait 200 turns for your wand of wishing.",
	/* From UnNetHack */
	"She's watching you!", /* Portal */
	"The cake is a lie!", /* Portal */
	"cjohnson/tier3", /* Portal */
	"What's the sitch?", /* Kim Possible */
	"So not the drama.", /* Kim Possible */
	"Sudden decompression sucks!", /* Space Quest 3 death message */
	"Thanks for playing UnNetHack.  As usual, you've been a real pantload!" /* Space Quest 4 death message, paraphrased */
	"Colorless green ideas sleep furiously.", /* Noam Chomsky */
	"Time flies like an arrow.", /* linguistic humor */
	"All Your Base Are Belong To Us!", /* Zero Wing */
	"Numa Numa", /* Internet Meme */
	"I want to believe!", /* X-Files */
	"Trust No One", /* X-Files */
	"The Truth is Out There", /* X-Files */
	"Look behind you, a Three-Headed Monkey!", /* Monkey Island */
	"Ifnkovhgroghprm", /* Kings Quest I */
#else /*JP*/
	"Elbereth",
	/* trap engravings */
	"ヴラドはここにいる", "ad aerarium",
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"キルロイ参上",
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"ソレハ　デキマセン。", /* 昔のアドベンチャーゲーム */
	"ここから入らんとするものは全て希望を捨てよ。", /* 神曲:地獄編, ダンテ */
	"よく来たな", /* Prisoner */
	"ご迷惑をおかけしたことを深くお詫びいたします。", /* So Long... */
	"早く来い来い木曜日", /* アメリカ横断ウルトラクイズ */
	"のらくろ",
	"バイア☆ラ 8?7-5309",
	"動物にエサを与えないでください。", /* Various zoos around the world */
	"私負けましたわ", /* 回文 */
	"イチ推し！", /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
#ifdef MAIL
	"ユー・ガッタ・メール！", /* AOL */
#endif
	"冗談じゃない！", /* Clueless */ /* 参考: http://www.iris.dti.ne.jp/~yfurui/alicia/rep_asif.html */
	/* From JNetHack */
	"ムーン・クリスタル・パワー・メイクアップ！", /* Sailor Moon */
	"とかげのしっぽ！", /* Mahoujin Guru Guru */
	"イッツ・ア・ショウタイム！" /* St. Tail */
	/* From Slash'Em */
	/* [Tom] added these */
	"Y?v?l s??ks!", /* just kidding... */
	"T?m ?as h?r?",
	/* Tsanth added these */
	"Gazortenplatz", /* Tribute to David Fizz */
	"John 3:16", /* You see this everywhere; why not here? */
	"「．．．とくれせんたぼーび」って魔法の呪文か？", /* Final Fantasy I (US) */
	"様変り。", /* Various people at various times in history */
	"UY WUZ HERE", /* :] */
	"また戻る！", /* Terminator */
	"久遠に臥したるもの死する事なく。", /* HPL */
	/* From NAO (netHack.alt.org) */
	"ウオーーーン！  イェンダーの狼人間！", /* gang tag */
	"勝つためにここを掘れ！", /* pun, duh */
	"こっちに来るな",
	"ガイウス・ユリウス・プリーミゲニウス参上。 君は今頃来たのか？", /* pompeii */
	"左へ行け →", "← 右へ行け",
	"Haermund Hardaxeとルーンで刻まれている", /* viking graffiti */
	"ここで竜になれ",
	"灯りが欲しい？ 鉱山の街のIzchakの照明店へどうぞ！",
	"ここで保存して、宿題をしておけ！",
	"精霊界の上の蛇が― すぐに迷宮に入ってあなたのそばに！",
	"ここに穴があった。 今はもうない。",
	"振動する場所",
	"ここに落し穴がある！",
	"ここはあなたがお探しの迷宮ではありません。",
	"ここにはわざと何も置いていない。",
	"警告！ 爆発のルーン！",
	"注意しろ、その扉の後ろに死の杖を持ったノームがいるぞ！",
	"×印地点",
	"× ← 現在位置。",
	"あなたはこの場所に訪れた百万人目です！ 200ターン後に願いの杖をプレゼント。",
	/* From UnNetHack */
	"彼女はあなたを見ています！", /* Portal */
	"そのケーキは偽者です！", /* Portal */
	"cjohnson/tier3", /* Portal */
	"sitchって何だ？", /* Kim Possible */
	"ドラマじゃないから大丈夫！", /* Kim Possible */
	"突然、減圧により吸い込まれた！", /* Space Quest 3 death message */
	"UnNetHackをプレイしてくれてありがとう。 例によって、あなたは窒息した！" /* Space Quest 4 death message, paraphrased */
	"無色で緑色のアイデアは荒れ狂うように眠った。", /* Noam Chomsky 意味をなさない文でも人は文法的な適切さを判断できる例*/
	"Time flies like an arrow.", /* linguistic humor 「光陰矢のごとし」機械翻訳「時間蝿は矢を好む」 */
	"All Your Base Are Belong To Us!", /* Zero Wing */
	"マイアヒ", /* Internet Meme */
	"真実を求めて！", /* X-Files */
	"誰も信じるな", /* X-Files */
	"真実はそこにある", /* X-Files */
	"後ろを見ろ、三つ首の猿が！", /* Monkey Island */
	"Ifnkovhgroghprm", /* Kings Quest I */
#endif /*JP*/
};

char *
random_engraving(outbuf)
char *outbuf;
{
	const char *rumor;

	/* a random engraving may come from the "rumors" file,
	   or from the list above */
	if (!rn2(4) || !(rumor = getrumor(0, outbuf, TRUE)) || !*rumor)
	    Strcpy(outbuf, random_mesg[rn2(SIZE(random_mesg))]);

	wipeout_text(outbuf, (int)(strlen(outbuf) / 4), 0);
	return outbuf;
}

/* Partial rubouts for engraving characters. -3. */
static const struct {
	char		wipefrom;
	const char *	wipeto;
} rubouts[] = {
	{'A', "^"},     {'B', "Pb["},   {'C', "("},     {'D', "|)["},
	{'E', "|FL[_"}, {'F', "|-"},    {'G', "C("},    {'H', "|-"},
	{'I', "|"},     {'K', "|<"},    {'L', "|_"},    {'M', "|"},
	{'N', "|\\"},   {'O', "C("},    {'P', "F"},     {'Q', "C("},
	{'R', "PF"},    {'T', "|"},     {'U', "J"},     {'V', "/\\"},
	{'W', "V/\\"},  {'Z', "/"},
	{'b', "|"},     {'d', "c|"},    {'e', "c"},     {'g', "c"},
	{'h', "n"},     {'j', "i"},     {'k', "|"},     {'l', "|"},
	{'m', "nr"},    {'n', "r"},     {'o', "c"},     {'q', "c"},
	{'w', "v"},     {'y', "v"},
	{':', "."},     {';', ","},
	{'0', "C("},    {'1', "|"},     {'6', "o"},     {'7', "/"},
	{'8', "3o"}
};
/*JP*/

void
wipeout_text(engr, cnt, seed)
char *engr;
int cnt;
unsigned seed;		/* for semi-controlled randomization */
{
#if 0 /*JP*/
	char *s;
#else
	unsigned char *s;
#endif
	int i, j, nxt, use_rubout, lth = (int)strlen(engr);

	if (lth && cnt > 0) {
	    while (cnt--) {
		/* pick next character */
		if (!seed) {
		    /* random */
		    nxt = rn2(lth);
		    use_rubout = rn2(4);
		} else {
		    /* predictable; caller can reproduce the same sequence by
		       supplying the same arguments later, or a pseudo-random
		       sequence by varying any of them */
		    nxt = seed % lth;
		    seed *= 31,  seed %= (BUFSZ-1);
		    use_rubout = seed & 3;
		}
#if 0 /*JP*/
		s = &engr[nxt];
		if (*s == ' ') continue;
#else
		if (!seed)
		  j = rn2(2);
		else {
		    seed *= 31,  seed %= (BUFSZ-1);
		    j = seed % 2;
		}

		if(jrubout(engr, nxt, use_rubout, j)){
		    continue;
		}

		s = (unsigned char *)&engr[nxt];
		if (*s == ' ') continue;
#endif
		/* rub out unreadable & small punctuation marks */
		if (index("?.,'`-|_", *s)) {
		    *s = ' ';
		    continue;
		}

		if (!use_rubout)
		    i = SIZE(rubouts);
		else
		    for (i = 0; i < SIZE(rubouts); i++)
			if (*s == rubouts[i].wipefrom) {
			    /*
			     * Pick one of the substitutes at random.
			     */
			    if (!seed)
				j = rn2(strlen(rubouts[i].wipeto));
			    else {
				seed *= 31,  seed %= (BUFSZ-1);
				j = seed % (strlen(rubouts[i].wipeto));
			    }
			    *s = rubouts[i].wipeto[j];
			    break;
			}

		/* didn't pick rubout; use '?' for unreadable character */
		if (i == SIZE(rubouts)) *s = '?';
	    }
	}

	/* trim trailing spaces */
	while (lth && engr[lth-1] == ' ') engr[--lth] = 0;
}

boolean
can_reach_floor()
{
	return (boolean)(!u.uswallow &&
#ifdef STEED
			/* Restricted/unskilled riders can't reach the floor */
			!(u.usteed && P_SKILL(P_RIDING) < P_BASIC) &&
#endif
			 (!Levitation ||
			  Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)));
}
#endif /* OVLB */
#ifdef OVL0

const char *
surface(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];

	if ((x == u.ux) && (y == u.uy) && u.uswallow &&
		is_animal(u.ustuck->data))
/*JP
	    return "maw";
*/
	    return "胃袋";
	else if (IS_AIR(lev->typ) && Is_airlevel(&u.uz))
/*JP
	    return "air";
*/
	    return "空中";
	else if (is_pool(x,y))
/*JP
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "bottom" : "water";
*/
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "水の底" : "水中";
	else if (is_ice(x,y))
/*JP
	    return "ice";
*/
	    return "氷";
	else if (is_lava(x,y))
/*JP
	    return "lava";
*/
	    return "溶岩";
	else if (lev->typ == DRAWBRIDGE_DOWN)
/*JP
	    return "bridge";
*/
	    return "橋";
	else if(IS_ALTAR(levl[x][y].typ))
/*JP
	    return "altar";
*/
	    return "祭壇";
	else if(IS_GRAVE(levl[x][y].typ))
/*JP
	    return "headstone";
*/
	    return "墓石";
	else if(IS_FOUNTAIN(levl[x][y].typ))
/*JP
	    return "fountain";
*/
	    return "泉";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
/*JP
	    return "floor";
*/
	    return "床";
	else
/*JP
	    return "ground";
*/
	    return "地面";
}

const char *
ceiling(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];
	const char *what;

	/* other room types will no longer exist when we're interested --
	 * see check_special_room()
	 */
	if (*in_rooms(x,y,VAULT))
/*JP
	    what = "vault's ceiling";
*/
	    what = "倉庫の天井";
	else if (*in_rooms(x,y,TEMPLE))
/*JP
	    what = "temple's ceiling";
*/
	    what = "寺院の天井";
	else if (*in_rooms(x,y,SHOPBASE))
/*JP
	    what = "shop's ceiling";
*/
	    what = "店の天井";
	else if (IS_AIR(lev->typ))
/*JP
	    what = "sky";
*/
	    what = "空";
	else if (Underwater)
/*JP
	    what = "water's surface";
*/
	    what = "水面";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
/*JP
	    what = "ceiling";
*/
	    what = "天井";
	else
/*JP
	    what = "rock above";
*/
	    what = "上方の岩";

	return what;
}

struct engr *
engr_at(x, y)
xchar x, y;
{
	register struct engr *ep = head_engr;

	while(ep) {
		if(x == ep->engr_x && y == ep->engr_y)
			return(ep);
		ep = ep->nxt_engr;
	}
	return((struct engr *) 0);
}

#ifdef ELBERETH
/* Decide whether a particular string is engraved at a specified
 * location; a case-insensitive substring match used.
 * Ignore headstones, in case the player names herself "Elbereth".
 */
int
sengr_at(s, x, y)
	const char *s;
	xchar x, y;
{
	register struct engr *ep = engr_at(x,y);

	return (ep && ep->engr_type != HEADSTONE &&
		ep->engr_time <= moves && strstri(ep->engr_txt, s) != 0);
}
#endif /* ELBERETH */

#ifdef ELBERETH_CONDUCT
/** Return the number of distinct times Elbereth is engraved at
 * the specified location. Case insensitive.  Counts an engraving
 * as being present even if it's still being written: if you're
 * killed while trying to write Elbereth, it still violates the
 * conduct (mainly because it's easier to implement that way).
 */
static
unsigned
nengr_at(x, y)
	xchar x, y;
{
	const char *s = "Elbereth";
	register struct engr *ep = engr_at(x, y);
	unsigned count = 0;
	const char *p;
	
	if (!ep || HEADSTONE == ep->engr_type)
		return 0;
	
	p = ep->engr_txt;	
	while (p = strstri(p, s)) {
		count++;
		p += 8;
	}

	return count;
}
#endif /* ELBERETH_CONDUCT */

#endif /* OVL0 */
#ifdef OVL2

void
u_wipe_engr(cnt)
register int cnt;
{
	if (can_reach_floor())
		wipe_engr_at(u.ux, u.uy, cnt);
}

#endif /* OVL2 */
#ifdef OVL1

void
wipe_engr_at(x,y,cnt)
register xchar x,y,cnt;
{
	register struct engr *ep = engr_at(x,y);

	/* Headstones are indelible */
	if(ep && ep->engr_type != HEADSTONE){
	    if(ep->engr_type != BURN || is_ice(x,y)) {
		if(ep->engr_type != DUST && ep->engr_type != ENGR_BLOOD) {
			cnt = rn2(1 + 50/(cnt+1)) ? 0 : 1;
		}
		wipeout_text(ep->engr_txt, (int)cnt, 0);
		while(ep->engr_txt[0] == ' ')
			ep->engr_txt++;
		if(!ep->engr_txt[0]) del_engr(ep);
	    }
	}
}

#endif /* OVL1 */
#ifdef OVL2

void
read_engr_at(x,y)
register int x,y;
{
	register struct engr *ep = engr_at(x,y);
	register int	sensed = 0;
	char buf[BUFSZ];
	
	/* Sensing an engraving does not require sight,
	 * nor does it necessarily imply comprehension (literacy).
	 */
	if(ep && ep->engr_txt[0]) {
	    switch(ep->engr_type) {
	    case DUST:
		if(!Blind) {
			sensed = 1;
#if 0 /*JP*/
			pline("%s is written here in the %s.", Something,
				is_ice(x,y) ? "frost" : "dust");
#else
			pline("何かの文字が%sに書いてある。",
				is_ice(x,y) ? "氷" : "ほこり");
#endif
		}
		break;
	    case ENGRAVE:
	    case HEADSTONE:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
#if 0 /*JP*/
			pline("%s is engraved here on the %s.",
				Something,
				surface(x,y));
#else
			pline("何かの文字が%sに刻まれている。",
				surface(x,y));
#endif
		}
		break;
	    case BURN:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
#if 0 /*JP*/
			pline("Some text has been %s into the %s here.",
				is_ice(x,y) ? "melted" : "burned",
				surface(x,y));
#else
			pline("何かの文字が%s%sいる。",
				surface(x,y),
				is_ice(x,y) ? "に刻まれて" : "で燃えて");
#endif
		}
		break;
	    case MARK:
		if(!Blind) {
			sensed = 1;
/*JP
			pline("There's some graffiti on the %s here.",
*/
			pline("%sに落書がある。",
				surface(x,y));
		}
		break;
	    case ENGR_BLOOD:
		/* "It's a message!  Scrawled in blood!"
		 * "What's it say?"
		 * "It says... `See you next Wednesday.'" -- Thriller
		 */
		if(!Blind) {
			sensed = 1;
/*JP
			You("see a message scrawled in blood here.");
*/
			You("血文字がなぐり書きされているのを見つけた。");
		}
		break;
	    default:
		warning("%s is written in a very strange way.",
				Something);
		sensed = 1;
	    }
	    if (sensed) {
	    	char *et;
/*JP
	    	unsigned maxelen = BUFSZ - sizeof("You feel the words: \"\". ");
*/
	    	unsigned maxelen = BUFSZ - sizeof("あなたは次のように感じた：「」");
	    	if (strlen(ep->engr_txt) > maxelen) {
	    		(void) strncpy(buf,  ep->engr_txt, (int)maxelen);
			buf[maxelen] = '\0';
			et = buf;
		} else
			et = ep->engr_txt;
		if (u.roleplay.illiterate && strcmp(et, "X")) {
/*JP
			pline("But you cannot read.");
*/
			pline("しかしあなたには読めなかった。");
		} else {
#if 0 /*JP*/
			You("%s: \"%s\".",
					(Blind) ? "feel the words" : "read",  et);
#else
			You("%s：「%s」",
					(Blind) ? "次のように感じた" : "読んだ",  et);
#endif
		}
		if(flags.run > 1) nomul(0, 0);
		if (moves > 5) check_tutorial_message(QT_T_ENGRAVING);
	    }
	}
}

#endif /* OVL2 */
#ifdef OVLB

void
make_engr_at(x,y,s,e_time,e_type)
register int x,y;
register const char *s;
register long e_time;
register xchar e_type;
{
	register struct engr *ep;

	if ((ep = engr_at(x,y)) != 0)
	    del_engr(ep);
	ep = newengr(strlen(s) + 1);
	ep->nxt_engr = head_engr;
	head_engr = ep;
	ep->engr_x = x;
	ep->engr_y = y;
	ep->engr_txt = (char *)(ep + 1);
	Strcpy(ep->engr_txt, s);
	/* engraving Elbereth shows wisdom */
	if (!in_mklev && !strcmp(s, "Elbereth")) exercise(A_WIS, TRUE);
	ep->engr_time = e_time;
	ep->engr_type = e_type > 0 ? e_type : rnd(N_ENGRAVE-1);
	ep->engr_lth = strlen(s) + 1;
}

/* delete any engraving at location <x,y> */
void
del_engr_at(x, y)
int x, y;
{
	register struct engr *ep = engr_at(x, y);

	if (ep) del_engr(ep);
}

/*
 *	freehand - returns true if player has a free hand
 */
int
freehand()
{

	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || !uarms->cursed)));
/*	if ((uwep && bimanual(uwep)) ||
	    (uwep && uarms))
		return(0);
	else
		return(1);*/
}

static NEARDATA const char styluses[] =
	{ ALL_CLASSES, ALLOW_NONE, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS,
	  GEM_CLASS, RING_CLASS, 0 };

/* Mohs' Hardness Scale:
 *  1 - Talc		 6 - Orthoclase
 *  2 - Gypsum		 7 - Quartz
 *  3 - Calcite		 8 - Topaz
 *  4 - Fluorite	 9 - Corundum
 *  5 - Apatite		10 - Diamond
 *
 * Since granite is a igneous rock hardness ~ 7, anything >= 8 should
 * probably be able to scratch the rock.
 * Devaluation of less hard gems is not easily possible because obj struct
 * does not contain individual oc_cost currently. 7/91
 *
 * steel     -	5-8.5	(usu. weapon)
 * diamond    - 10			* jade	     -	5-6	 (nephrite)
 * ruby       -  9	(corundum)	* turquoise  -	5-6
 * sapphire   -  9	(corundum)	* opal	     -	5-6
 * topaz      -  8			* glass      - ~5.5
 * emerald    -  7.5-8	(beryl)		* dilithium  -	4-5??
 * aquamarine -  7.5-8	(beryl)		* iron	     -	4-5
 * garnet     -  7.25	(var. 6.5-8)	* fluorite   -	4
 * agate      -  7	(quartz)	* brass      -	3-4
 * amethyst   -  7	(quartz)	* gold	     -	2.5-3
 * jasper     -  7	(quartz)	* silver     -	2.5-3
 * onyx       -  7	(quartz)	* copper     -	2.5-3
 * moonstone  -  6	(orthoclase)	* amber      -	2-2.5
 */

static int engrave(const char *, boolean);

/** return 1 if action took 1 (or more) moves, 0 if error or aborted */
int
doengrave()
{
	return engrave(NULL, FALSE);
}
int
doengrave_elbereth()
{
	return engrave("Elbereth", TRUE);
}

static
int
engrave(engraving, fingers)
const char *engraving;
boolean fingers;
{
	boolean dengr = FALSE;	/* TRUE if we wipe out the current engraving */
	boolean doblind = FALSE;/* TRUE if engraving blinds the player */
	boolean doknown = FALSE;/* TRUE if we identify the stylus */
	boolean eow = FALSE;	/* TRUE if we are overwriting oep */
	boolean jello = FALSE;	/* TRUE if we are engraving in slime */
	boolean ptext = TRUE;	/* TRUE if we must prompt for engrave text */
	boolean teleengr =FALSE;/* TRUE if we move the old engraving */
	boolean zapwand = FALSE;/* TRUE if we remove a wand charge */
	xchar type = DUST;	/* Type of engraving made */
	char buf[BUFSZ];	/* Buffer for final/poly engraving text */
	char ebuf[BUFSZ];	/* Buffer for initial engraving text */
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char post_engr_text[BUFSZ]; /* Text displayed after engraving prompt */
	const char *eword;	/* What to engrave */
	const char *everb;	/* Present tense of engraving type */
	const char *eloc; 	/* Where to engrave in the ground */
	const char *eground;	/* Type of the ground (ie dust/floor/...) */
	char *sp;		/* Place holder for space count of engr text */
	int len;		/* # of nonspace chars of new engraving text */
	int maxelen;		/* Max allowable length of engraving text */
	struct engr *oep = engr_at(u.ux,u.uy);
				/* The current engraving */
	struct obj *otmp;	/* Object selected with which to engrave */
	char *writer;

	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	buf[0] = (char)0;
	ebuf[0] = (char)0;
	post_engr_text[0] = (char)0;
	maxelen = BUFSZ - 1;
	if (is_demon(youmonst.data) || youmonst.data->mlet == S_VAMPIRE)
	    type = ENGR_BLOOD;

	/* Can the adventurer engrave at all? */

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
/*JP
			pline("What would you write?  \"Jonah was here\"?");
*/
			pline("何を書くんだい？「ヨナはここにいる」？");
			return(0);
		} else if (is_whirly(u.ustuck->data)) {
/*JP
			You_cant("reach the %s.", surface(u.ux,u.uy));
*/
			You("%sに届かない。", surface(u.ux,u.uy));
			return(0);
		} else
			jello = TRUE;
	} else if (is_lava(u.ux, u.uy)) {
/*JP
		You_cant("write on the lava!");
*/
		You("溶岩には書けない！");
		return(0);
	} else if (Underwater) {
/*JP
		You_cant("write underwater!");
*/
		You("水中では書けない！");
		return(0);
	} else if (is_pool(u.ux,u.uy) || IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
/*JP
		You_cant("write on the water!");
*/
		You("水には書けない！");
		return(0);
	}
	if(Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)/* in bubble */) {
/*JP
		You_cant("write in thin air!");
*/
		You("空中には書けない！");
		return(0);
	}
	if (cantwield(youmonst.data)) {
/*JP
		You_cant("even hold anything!");
*/
		You("何も持てない！");
		return(0);
	}
	if (check_capacity((char *)0)) return (0);

	/* One may write with finger, or weapon, or wand, or..., or...
	 * Edited by GAN 10/20/86 so as not to change weapon wielded.
	 */

	if (fingers) {
		if (uwep && uwep->otyp == ATHAME) {
			otmp = uwep;
		} else {
			otmp = &zeroobj;
		}
	} else {
		otmp = getobj(styluses, "write with");
	}
	if(!otmp) return(0);		/* otmp == zeroobj if fingers */

	if (otmp == &zeroobj) writer = makeplural(body_part(FINGER));
	else writer = xname(otmp);

	/* There's no reason you should be able to write with a wand
	 * while both your hands are tied up.
	 */
	if (!freehand() && otmp != uwep && !otmp->owornmask) {
/*JP
		You("have no free %s to write with!", body_part(HAND));
*/
		Your("%sは塞がっているので書けない！", body_part(HAND));
		return(0);
	}

	if (jello) {
/*JP
		You("tickle %s with your %s.", mon_nam(u.ustuck), writer);
*/
		You("%sで%sをくすぐった。", writer, mon_nam(u.ustuck));
/*JP
		Your("message dissolves...");
*/
		Your("メッセージは消えた．．．");
		return(0);
	}
	if (otmp->oclass != WAND_CLASS && !can_reach_floor()) {
/*JP
		You_cant("reach the %s!", surface(u.ux,u.uy));
*/
		You("%sに届かない！", surface(u.ux,u.uy));
		return(0);
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
/*JP
		You("make a motion towards the altar with your %s.", writer);
*/
		You("%sを使って祭壇に書こうとした。", writer);
		altar_wrath(u.ux, u.uy);
		return(0);
	}
	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (otmp == &zeroobj) { /* using only finger */
#if 0 /*JP:T*/
		You("would only make a small smudge on the %s.",
			surface(u.ux, u.uy));
#else
		You("は%sに小さなしみをつけることしかできなかった。",
			surface(u.ux, u.uy));
#endif
		return(0);
	    } else if (!levl[u.ux][u.uy].disturbed) {
/*JP
		You("disturb the undead!");
*/
		You("不死の者の眠りを妨げた！");
		levl[u.ux][u.uy].disturbed = 1;
		(void) makemon(&mons[PM_GHOUL], u.ux, u.uy, NO_MM_FLAGS);
		exercise(A_WIS, FALSE);
		return(1);
	    }
	}

	/* SPFX for items */

	switch (otmp->oclass) {
	    default:
	    case AMULET_CLASS:
	    case CHAIN_CLASS:
	    case POTION_CLASS:
	    case COIN_CLASS:
		break;

	    case RING_CLASS:
		/* "diamond" rings and others should work */
	    case GEM_CLASS:
		/* diamonds & other hard gems should work */
		if (objects[otmp->otyp].oc_tough) {
			type = ENGRAVE;
			break;
		}
		break;

	    case ARMOR_CLASS:
		if (is_boots(otmp)) {
			type = DUST;
			break;
		}
		/* fall through */
	    /* Objects too large to engrave with */
	    case BALL_CLASS:
	    case ROCK_CLASS:
/*JP
		You_cant("engrave with such a large object!");
*/
		pline("そんな大きなものを使って文字を刻めない！");
		ptext = FALSE;
		break;

	    /* Objects too silly to engrave with */
	    case FOOD_CLASS:
	    case SCROLL_CLASS:
	    case SPBOOK_CLASS:
#if 0 /*JP*/
		Your("%s would get %s.", xname(otmp),
			is_ice(u.ux,u.uy) ? "all frosty" : "too dirty");
#else
		Your("%sは%sなった。", xname(otmp),
			is_ice(u.ux,u.uy) ? "氷づけに" : "汚なく");
#endif
		ptext = FALSE;
		break;

	    case RANDOM_CLASS:	/* This should mean fingers */
		break;

	    /* The charge is removed from the wand before prompting for
	     * the engraving text, because all kinds of setup decisions
	     * and pre-engraving messages are based upon knowing what type
	     * of engraving the wand is going to do.  Also, the player
	     * will have potentially seen "You wrest .." message, and
	     * therefore will know they are using a charge.
	     */
	    case WAND_CLASS:
		if (zappable(otmp)) {
		    check_unpaid(otmp);
		    zapwand = TRUE;
		    if (Levitation) ptext = FALSE;

		    switch (otmp->otyp) {
		    /* DUST wands */
		    default:
			break;

			/* NODIR wands */
		    case WAN_LIGHT:
		    case WAN_SECRET_DOOR_DETECTION:
		    case WAN_CREATE_MONSTER:
		    case WAN_WISHING:
		    case WAN_ENLIGHTENMENT:
			zapnodir(otmp);
			break;

			/* IMMEDIATE wands */
			/* If wand is "IMMEDIATE", remember to affect the
			 * previous engraving even if turning to dust.
			 */
		    case WAN_STRIKING:
			Strcpy(post_engr_text,
/*JP
			"The wand unsuccessfully fights your attempt to write!"
*/
			"あなたが書こうとすると杖は抵抗した！"
			);
			break;
		    case WAN_SLOW_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
/*JP
				   "The bugs on the %s slow down!",
*/
				   "%sの上の虫の動きが遅くなった！",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SPEED_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
/*JP
				   "The bugs on the %s speed up!",
*/
				   "%sの上の虫の動きが速くなった！",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_POLYMORPH:
			if(oep)  {
			    if (!Blind) {
				type = (xchar)0;	/* random */
				(void) random_engraving(buf);
			    }
			    dengr = TRUE;
			}
			break;
		    case WAN_NOTHING:
		    case WAN_UNDEAD_TURNING:
		    case WAN_OPENING:
		    case WAN_LOCKING:
		    case WAN_PROBING:
			break;

			/* RAY wands */
		    case WAN_MAGIC_MISSILE:
			ptext = TRUE;
			if (!Blind) {
			   Sprintf(post_engr_text,
/*JP
				   "The %s is riddled by bullet holes!",
*/
				   "%sは散弾で細かい穴だらけになった！",
				   surface(u.ux, u.uy));
			}
			break;

		    /* can't tell sleep from death - Eric Backus */
		    case WAN_SLEEP:
		    case WAN_DEATH:
			if (!Blind) {
			   Sprintf(post_engr_text,
/*JP
				   "The bugs on the %s stop moving!",
*/
				   "%sの上の虫の動きが止まった！",
				   surface(u.ux, u.uy));
			}
			break;

		    case WAN_COLD:
			if (!Blind)
			    Strcpy(post_engr_text,
/*JP
				"A few ice cubes drop from the wand.");
*/
				"氷のかけらが杖からこぼれ落ちた。");
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_CANCELLATION:
		    case WAN_MAKE_INVISIBLE:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
/*JP
				pline_The("engraving on the %s vanishes!",
*/
				pline("%sの上の文字は消えた！",
					surface(u.ux,u.uy));
			    dengr = TRUE;
			}
			break;
		    case WAN_TELEPORTATION:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
/*JP
				pline_The("engraving on the %s vanishes!",
*/
				pline("%sの上の文字は消えた！",
					surface(u.ux,u.uy));
			    teleengr = TRUE;
			}
			break;

		    /* type = ENGRAVE wands */
		    case WAN_DIGGING:
			ptext = TRUE;
			type  = ENGRAVE;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
#if 0 /*JP*/
				pline("This %s is a wand of digging!",
				xname(otmp));
#else
				pline("これは穴掘りの杖だ！");
#endif
			    doknown = TRUE;
			}
			if (!Blind)
#if 0 /*JP*/
			    Strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"Chips fly out from the headstone." :
				is_ice(u.ux,u.uy) ?
				"Ice chips fly up from the ice surface!" :
				"Gravel flies up from the floor.");
#else
			    Strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"墓石から破片が飛び散った。" :
				is_ice(u.ux,u.uy) ?
				"氷の表面から氷のかけらが飛び散った。" :
			        "砂利が床から飛び散った。");
#endif
			else
/*JP
			    Strcpy(post_engr_text, "You hear drilling!");
*/
			    Strcpy(post_engr_text, "穴が開く音を聞いた！");
			break;

		    /* type = BURN wands */
		    case WAN_FIRE:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
/*JP
			    pline("This %s is a wand of fire!", xname(otmp));
*/
			    pline("これは炎の杖だ！");
			    doknown = TRUE;
			}
#if 0 /*JP*/
			Strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
#else
			Strcpy(post_engr_text,
				Blind ? "杖が暖かくなったような気がした。" :
					"炎が杖から飛び散った。");
#endif
			break;
		    case WAN_LIGHTNING:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
#if 0 /*JP*/
				pline("This %s is a wand of lightning!",
					xname(otmp));
#else
				pline("これは雷の杖だ！");
#endif
			    doknown = TRUE;
			}
			if (!Blind) {
#if 0 /*JP*/
			    Strcpy(post_engr_text,
				    "Lightning arcs from the wand.");
#else
			    Strcpy(post_engr_text,
				    "火花が杖から飛び散った。");
#endif
			    doblind = TRUE;
			} else
/*JP
			    Strcpy(post_engr_text, "You hear crackling!");
*/
			    Strcpy(post_engr_text, "パチパチという音を聞いた！");
			break;

		    /* type = MARK wands */
		    /* type = ENGR_BLOOD wands */
		    }
		} else /* end if zappable */
		    if (!can_reach_floor()) {
/*JP
			You_cant("reach the %s!", surface(u.ux,u.uy));
*/
			You("%sに届かない！", surface(u.ux,u.uy));
			return(0);
		    }
		break;

	    case WEAPON_CLASS:
		if (is_blade(otmp)) {
		    if ((int)otmp->spe > -3)
			type = ENGRAVE;
		    else
/*JP
			Your("%s too dull for engraving.", aobjnam(otmp,"are"));
*/
			pline("%sは刃がボロボロで、文字を彫れない。",xname(otmp));
		}
		break;

	    case TOOL_CLASS:
		if(otmp == ublindf) {
		    pline(
/*JP
		"That is a bit difficult to engrave with, don't you think?");
*/
		"ちょっとそれで彫るのは大変だろう、そう思わない？");
		    return(0);
		}
		switch (otmp->otyp)  {
		    case MAGIC_MARKER:
			if (otmp->spe <= 0)
/*JP
			    Your("marker has dried out.");
*/
			    Your("マーカは乾ききった。");
			else
			    type = MARK;
			break;
		    case TOWEL:
			/* Can't really engrave with a towel */
			ptext = FALSE;
			if (oep)
			    if ((oep->engr_type == DUST ) ||
				(oep->engr_type == ENGR_BLOOD) ||
				(oep->engr_type == MARK )) {
				if (!Blind)
/*JP
				    You("wipe out the message here.");
*/
				    You("メッセージを拭きとった。");
				else
#if 0 /*JP*/
				    Your("%s %s %s.", xname(otmp),
					 otense(otmp, "get"),
					 is_ice(u.ux,u.uy) ?
					 "frosty" : "dusty");
#else
				    pline("%sは%sなった。", xname(otmp),
					  is_ice(u.ux,u.uy) ?
					  "氷づけに" : "ほこりまみれに");
#endif
				dengr = TRUE;
			    } else
/*JP
				Your("%s can't wipe out this engraving.",
*/
				pline("この文字は%sでは拭きとれない。",
				     xname(otmp));
			else
#if 0 /*JP*/
			    Your("%s %s %s.", xname(otmp), otense(otmp, "get"),
				  is_ice(u.ux,u.uy) ? "frosty" : "dusty");
#else
			    pline("%sは%sなった。", xname(otmp),
				  is_ice(u.ux,u.uy) ? "氷づけに" : "ほこりまみれに");
#endif
			break;
		    default:
			break;
		}
		break;

	    case VENOM_CLASS:
#ifdef WIZARD
		if (wizard) {
/*JP
		    pline("Writing a poison pen letter??");
*/
		    pline("ふむ。これこそ本当の毒舌だ。");
		    break;
		}
#endif
	    case ILLOBJ_CLASS:
		warning("You're engraving with an illegal object!");
		break;
	}

	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (type == ENGRAVE || type == 0)
		type = HEADSTONE;
	    else {
		/* ensures the "cannot wipe out" case */
		type = DUST;
		dengr = FALSE;
		teleengr = FALSE;
		buf[0] = (char)0;
	    }
	}

	/* End of implement setup */

	/* Identify stylus */
	if (doknown) {
	    makeknown(otmp->otyp);
	    more_experienced(0,0,10);
	}

	if (teleengr) {
	    rloc_engr(oep);
	    oep = (struct engr *)0;
	}

	if (dengr) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Something has changed the engraving here */
	if (*buf) {
	    make_engr_at(u.ux, u.uy, buf, moves, type);
/*JP
	    pline_The("engraving now reads: \"%s\".", buf);
*/
	    pline("刻まれた文字を読んだ：「%s」。", buf);
	    ptext = FALSE;
	}

	if (zapwand && (otmp->spe < 0)) {
#if 0 /*JP*/
	    pline("%s %sturns to dust.",
		  The(xname(otmp)), Blind ? "" : "glows violently, then ");
#else
	    pline("%sは%sちりとなった。",
		  xname(otmp), Blind ? "" : "激しく輝き、");
#endif
	    if (!IS_GRAVE(levl[u.ux][u.uy].typ))
#if 0 /*JP*/
		You("are not going to get anywhere trying to write in the %s with your dust.",
		    is_ice(u.ux,u.uy) ? "frost" : "dust");
#else
		You("塵で%sに何か書こうとしたが、できなかった。",
		    is_ice(u.ux,u.uy) ? "氷" : "ほこり");
#endif
	    useup(otmp);
	    ptext = FALSE;
	}

	if (!ptext) {		/* Early exit for some implements. */
	    if (otmp->oclass == WAND_CLASS && !can_reach_floor())
/*JP
		You_cant("reach the %s!", surface(u.ux,u.uy));
*/
		You("%sに届かない！", surface(u.ux,u.uy));
	    return(1);
	}

	/* Special effects should have deleted the current engraving (if
	 * possible) by now.
	 */

	if (oep) {
	    register char c = 'n';

	    /* Give player the choice to add to engraving. */

	    if (type == HEADSTONE || engraving) {
		/* no choice, only append */
		c = 'y';
	    } else if ( (type == oep->engr_type) && (!Blind ||
		 (oep->engr_type == BURN) || (oep->engr_type == ENGRAVE)) ) {
/*JP
		c = yn_function("Do you want to add to the current engraving?",
*/
		c = yn_function("何か書き加えますか？",
				ynqchars, 'y');
		if (c == 'q') {
		    pline(Never_mind);
		    return(0);
		}
	    }

	    if (c == 'n' || Blind) {

		if( (oep->engr_type == DUST) || (oep->engr_type == ENGR_BLOOD) ||
		    (oep->engr_type == MARK) ) {
		    if (!Blind) {
#if 0 /*JP*/
			You("wipe out the message that was %s here.",
			    ((oep->engr_type == DUST)  ? "written in the dust" :
			    ((oep->engr_type == ENGR_BLOOD) ? "scrawled in blood"   :
							 "written")));
#else
			You("%sメッセージを拭きとった。",
			    ((oep->engr_type == DUST)  ? "ほこりに書かれている" :
			    ((oep->engr_type == BLOOD) ? "血文字でなぐり書きされている"   :
							 "書かれている")));
#endif
			del_engr(oep);
			oep = (struct engr *)0;
		    } else
		   /* Don't delete engr until after we *know* we're engraving */
			eow = TRUE;
		} else
		    if ( (type == DUST) || (type == MARK) || (type == ENGR_BLOOD) ) {
#if 0 /*JP*/
			You(
			 "cannot wipe out the message that is %s the %s here.",
			 oep->engr_type == BURN ?
			   (is_ice(u.ux,u.uy) ? "melted into" : "burned into") :
			   "engraved in", surface(u.ux,u.uy));
#else
			You("%sメッセージを拭きとれなかった。",
			    ((oep->engr_type == DUST)  ? "ほこりに書かれている" :
			    ((oep->engr_type == BLOOD) ? "血文字でなぐり書きされている"   :
							 "書かれている")));
#endif
			return(1);
		    } else
			if ( (type != oep->engr_type) || (c == 'n') ) {
			    if (!Blind || can_reach_floor())
/*JP
				You("will overwrite the current message.");
*/
				You("メッセージを上書きしようとした。");
			    eow = TRUE;
			}
	    }
	}

/*JP
	eword = (u.roleplay.illiterate ? "your name " : "");
*/
	eword = (u.roleplay.illiterate ? "あなたの名前を" : "");
	eground = surface(u.ux,u.uy);
	switch(type){
	    default:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "write");
		eloc = (oep && !eow ? "to the weird writing on" :
		    "strangely onto");
#else
		everb = (oep && !eow ? "書き加える" : "書く");
		eloc = (oep && !eow ? "の変な何かに" : "の奇妙な何かに");
#endif
		break;
	    case DUST:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "write");
		eloc = (oep && !eow ? "to the writing in" : "into");
		eground = (is_ice(u.ux,u.uy) ? "frost" : "dust");
#else
		everb = (oep && !eow ? "書き加える" : "書く");
		eloc = (oep && !eow ? "に書かれたものに" : "に");
		eground = (is_ice(u.ux,u.uy) ? "氷" : "ほこり");
#endif
		break;
	    case HEADSTONE:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "engrave");
		eloc = (oep && !eow ? "to the epitaph on" : "on");
#else
		everb = (oep && !eow ? "刻み加える" : "刻む");
		eloc = (oep && !eow ? "の墓碑銘に" : "の銘を");
#endif
		break;
	    case ENGRAVE:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "engrave");
		eloc = (oep && !eow ? "to the engraving in" : "into");
#else
		everb = (oep && !eow ? "刻み加える" : "刻む");
		eloc = (oep && !eow ? "に刻まれたものに" : "に");
#endif
		break;
	    case BURN:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" :
			( is_ice(u.ux,u.uy) ? "melt" : "burn" ) );
		eloc = (oep && !eow ? ( is_ice(u.ux,u.uy) ?
			"to the text melted into" : "to the text burned into" ) :
			"into");
#else
		everb = (oep && !eow ?
			( is_ice(u.ux,u.uy) ? "彫り加える" : "焼き加える" ) :
			( is_ice(u.ux,u.uy) ? "彫る" : "焼き付ける" ) );
		eloc = (oep && !eow ? ( is_ice(u.ux,u.uy) ?
			"に彫りこまれた文字に" : "に焼き付けられた文字に" ) : "に");
#endif
		break;
	    case MARK:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "scribble");
		eloc = (oep && !eow ? "to the graffiti on" : "onto");
#else
		everb = (oep && !eow ? "書き加える" : "落書きする");
		eloc = (oep && !eow ? "の落書きに" : "に");
#endif
		break;
	    case ENGR_BLOOD:
#if 0 /*JP*/
		everb = (oep && !eow ? "add" : "scrawl");
		eloc = (oep && !eow ? "to the scrawl on" : "onto");
#else
		everb = (oep && !eow ? "書き加える" : "なぐり書きする");
		eloc = (oep && !eow ? "のなぐり書きに" : "に");
#endif
		break;
	}

	/* Tell adventurer what is going on */
	if (engraving && otmp == &zeroobj)
#if 0 /*JP*/
	    You("%s \"%s\" %swith your %s %s the %s.", everb, engraving,
		eword, makeplural(body_part(FINGER)), eloc, eground);
#else
	    You("%sで%s%s%s\"%s\"と%s。", body_part(FINGER), eground,
		eloc, eword, engraving, jpast(everb));
#endif
	else if (engraving && otmp != &zeroobj)
#if 0 /*JP*/
	    You("%s \"%s\" %swith %s %s the %s.", everb, engraving,
		eword, doname(otmp), eloc, eground);
#else
	    You("%sで%s%s%s\"%s\"と%s。", doname(otmp), eground,
		eloc, eword, engraving, jpast(everb));
#endif
	else if (otmp != &zeroobj)
#if 0 /*JP*/
	    You("%s %swith %s %s the %s.", everb, eword, doname(otmp),
		eloc, eground);
#else
	    You("%sで%s%s%s%s。", doname(otmp), eground,
		eloc, eword, jpast(everb));
#endif
	else
#if 0 /*JP*/
	    You("%s %swith your %s %s the %s.", everb, eword,
		makeplural(body_part(FINGER)), eloc, eground);
#else
	    You("%sで%s%s%s%s。", body_part(FINGER), eground,
		eloc, eword, jpast(everb));
#endif

	/* Prompt for engraving! (if literate) */
	if(u.roleplay.illiterate) {
	    Sprintf(ebuf,"X");
	} else if (engraving) {
	    Sprintf(ebuf, engraving);
	} else {
#if 0 /*JP*/
	    Sprintf(qbuf,"What do you want to %s %s the %s here?", everb,
		eloc, eground);
#else
	    Sprintf(qbuf,"%s%s何と%sか？", eground, eloc, jpolite(everb));
#endif
	    getlin(qbuf, ebuf);
	}

	/* Count the actual # of chars engraved not including spaces */
	len = strlen(ebuf);
	for (sp = ebuf; *sp; sp++) if (isspace(*sp)) len -= 1;

	if (len == 0 || index(ebuf, '\033')) {
	    if (zapwand) {
		if (!Blind)
#if 0 /*JP*/
		    pline("%s, then %s.",
			  Tobjnam(otmp, "glow"), otense(otmp, "fade"));
#else
		    pline("%sは輝いたが、すぐに消えた。", xname(otmp));
#endif
		return(1);
	    } else {
		pline(Never_mind);
		return(0);
	    }
	}

	/* A single `x' is the traditional signature of an illiterate person */
	if (len != 1 || (!index(ebuf, 'x') && !index(ebuf, 'X')))
	    violated(CONDUCT_ILLITERACY);

	/* Mix up engraving if surface or state of mind is unsound.
	   Note: this won't add or remove any spaces. */
	for (sp = ebuf; *sp; sp++) {
#if 0 /*JP*/
	    if (isspace(*sp)) continue;
#else
	    if (isspace_8(*sp)) continue;
#endif
	    if (((type == DUST || type == ENGR_BLOOD) && !rn2(25)) ||
		    (Blind && !rn2(11)) || (Confusion && !rn2(7)) ||
		    (Stunned && !rn2(4)) || (Hallucination && !rn2(2)))
#if 0 /*JP*/
		*sp = ' ' + rnd(96 - 2);	/* ASCII '!' thru '~'
						   (excludes ' ' and DEL) */
#else
	    {
		if(is_kanji1(ebuf, sp-ebuf))
		  jrndm_replace(sp);
		else if(is_kanji2(ebuf, sp-ebuf))
		  jrndm_replace(sp-1);
		else
		  *sp = '!' + rn2(93); /* ASCII-code only */
	    }
#endif
	}

	/* Previous engraving is overwritten */
	if (eow) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Figure out how long it took to engrave, and if player has
	 * engraved too much.
	 */
	switch(type){
	    default:
		multi = -(len/10);
/*JP
		if (multi) nomovemsg = "You finish your weird engraving.";
*/
		if (multi) nomovemsg = "あなたは奇妙な刻みを終えた。";
		break;
	    case DUST:
		multi = -(len/10);
/*JP
		if (multi) nomovemsg = "You finish writing in the dust.";
*/
		if (multi) nomovemsg = "あなたはほこりに書き終えた。";
		break;
	    case HEADSTONE:
	    case ENGRAVE:
		multi = -(len/10);
		if ((otmp->oclass == WEAPON_CLASS) &&
		    ((otmp->otyp != ATHAME) || otmp->cursed)) {
		    multi = -len;
		    maxelen = ((otmp->spe + 3) * 2) + 1;
			/* -2 = 3, -1 = 5, 0 = 7, +1 = 9, +2 = 11
			 * Note: this does not allow a +0 anything (except
			 *	 an athame) to engrave "Elbereth" all at once.
			 *	 However, you could now engrave "Elb", then
			 *	 "ere", then "th".
			 */
/*JP
		    Your("%s dull.", aobjnam(otmp, "get"));
*/
		    Your("%sは刃こぼれした。", xname(otmp));
		    if (otmp->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
/*JP
			    You("damage it, you pay for it!");
*/
			    You("未払いのものに傷をつけてしまった。支払いをしなきゃ！");
			    bill_dummy_object(otmp);
			}
		    }
		    if (len > maxelen) {
			multi = -maxelen;
			otmp->spe = -3;
		    } else if (len > 1)
			otmp->spe -= len >> 1;
		    else otmp->spe -= 1; /* Prevent infinite engraving */
		} else
		    if ( (otmp->oclass == RING_CLASS) ||
			 (otmp->oclass == GEM_CLASS) )
			multi = -len;
/*JP
		if (multi) nomovemsg = "You finish engraving.";
*/
		if (multi) nomovemsg = "あなたは刻み終えた。";
		break;
	    case BURN:
		multi = -(len/10);
		if (multi)
		    nomovemsg = is_ice(u.ux,u.uy) ?
#if 0 /*JP*/
			"You finish melting your message into the ice.":
			"You finish burning your message into the floor.";
#else
			"氷へメッセージを刻み終えた。":
			"床へメッセージを焼きいれ終えた。";
#endif
		break;
	    case MARK:
		multi = -(len/10);
		if ((otmp->oclass == TOOL_CLASS) &&
		    (otmp->otyp == MAGIC_MARKER)) {
		    maxelen = (otmp->spe) * 2; /* one charge / 2 letters */
		    if (len > maxelen) {
/*JP
			Your("marker dries out.");
*/
			Your("マーカは乾ききった。");
			otmp->spe = 0;
			multi = -(maxelen/10);
		    } else
			if (len > 1) otmp->spe -= len >> 1;
			else otmp->spe -= 1; /* Prevent infinite graffiti */
		}
/*JP
		if (multi) nomovemsg = "You finish defacing the dungeon.";
*/
		if (multi) nomovemsg = "あなたは迷宮への落書を書き終えた。";
		break;
	    case ENGR_BLOOD:
		multi = -(len/10);
/*JP
		if (multi) nomovemsg = "You finish scrawling.";
*/
		if (multi) nomovemsg = "はしり書きを書き終えた。";
		break;
	}

	/* Chop engraving down to size if necessary */
	if (len > maxelen) {
	    for (sp = ebuf; (maxelen && *sp); sp++)
/*JP
		if (!isspace(*sp)) maxelen--;
*/
		if (!isspace_8(*sp)) maxelen--;
	    if (!maxelen && *sp) {
#if 1 /*JP*/
		if(is_kanji2(ebuf, sp - ebuf))
		    --sp;
#endif
		*sp = (char)0;
/*JP
		if (multi) nomovemsg = "You cannot write any more.";
*/
		if (multi) nomovemsg = "これ以上何も書けなかった。";
/*JP
		You("only are able to write \"%s\"", ebuf);
*/
		You("「%s」とまでしか書けなかった。", ebuf);
	    }
	}

	/* Add to existing engraving */
	if (oep) Strcpy(buf, oep->engr_txt);

	(void) strncat(buf, ebuf, (BUFSZ - (int)strlen(buf) - 1));

#ifdef ELBERETH_CONDUCT
	{
		unsigned ecount1, ecount0 = nengr_at(u.ux, u.uy);
		make_engr_at(u.ux, u.uy, buf, (moves - multi), type);
		ecount1 = nengr_at(u.ux, u.uy);
		if (ecount1 > ecount0)
			u.uconduct.elbereths += (ecount1 - ecount0);
	}
#else
	make_engr_at(u.ux, u.uy, buf, (moves - multi), type);
#endif

	if (post_engr_text[0]) pline(post_engr_text);

	if (doblind && !resists_blnd(&youmonst)) {
/*JP
	    You("are blinded by the flash!");
*/
	    You("まばゆい光で目がくらんだ！");
	    make_blinded((long)rnd(50),FALSE);
	    if (!Blind) Your(vision_clears);
	}

	return(1);
}

void
save_engravings(fd, mode)
int fd, mode;
{
	register struct engr *ep = head_engr;
	register struct engr *ep2;
	unsigned no_more_engr = 0;

	while (ep) {
	    ep2 = ep->nxt_engr;
	    if (ep->engr_lth && ep->engr_txt[0] && perform_bwrite(mode)) {
		bwrite(fd, (genericptr_t)&(ep->engr_lth), sizeof(ep->engr_lth));
		bwrite(fd, (genericptr_t)ep, sizeof(struct engr) + ep->engr_lth);
	    }
	    if (release_data(mode))
		dealloc_engr(ep);
	    ep = ep2;
	}
	if (perform_bwrite(mode))
	    bwrite(fd, (genericptr_t)&no_more_engr, sizeof no_more_engr);
	if (release_data(mode))
	    head_engr = 0;
}

void
rest_engravings(fd)
int fd;
{
	register struct engr *ep;
	unsigned lth;

	head_engr = 0;
	while(1) {
		mread(fd, (genericptr_t) &lth, sizeof(unsigned));
		if(lth == 0) return;
		ep = newengr(lth);
		mread(fd, (genericptr_t) ep, sizeof(struct engr) + lth);
		ep->nxt_engr = head_engr;
		head_engr = ep;
		ep->engr_txt = (char *) (ep + 1);	/* Andreas Bormann */
		/* mark as finished for bones levels -- no problem for
		 * normal levels as the player must have finished engraving
		 * to be able to move again */
		ep->engr_time = moves;
	}
}

void
del_engr(ep)
register struct engr *ep;
{
	if (ep == head_engr) {
		head_engr = ep->nxt_engr;
	} else {
		register struct engr *ept;

		for (ept = head_engr; ept; ept = ept->nxt_engr)
		    if (ept->nxt_engr == ep) {
			ept->nxt_engr = ep->nxt_engr;
			break;
		    }
		if (!ept) {
		    warning("Error in del_engr?");
		    return;
		}
	}
	dealloc_engr(ep);
}

/* randomly relocate an engraving */
void
rloc_engr(ep)
struct engr *ep;
{
	int tx, ty, tryct = 200;

	do  {
	    if (--tryct < 0) return;
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	} while (engr_at(tx, ty) ||
		!goodpos(tx, ty, (struct monst *)0, 0));

	ep->engr_x = tx;
	ep->engr_y = ty;
}


/* Epitaphs for random headstones */
static const char *epitaphs[] = {
#if 0 /*JP*/
	"Rest in peace",
	"R.I.P.",
	"Rest In Pieces",
	"Note -- there are NO valuable items in this grave",
	"1994-1995. The Longest-Lived Hacker Ever",
	"The Grave of the Unknown Hacker",
	"We weren't sure who this was, but we buried him here anyway",
	"Sparky -- he was a very good dog",
	"Beware of Electric Third Rail",
	"Made in Taiwan",
	"Og friend. Og good dude. Og died. Og now food",
	"Beetlejuice Beetlejuice Beetlejuice",
	"Look out below!",
	"Please don't dig me up. I'm perfectly happy down here. -- Resident",
	"Postman, please note forwarding address: Gehennom, Asmodeus's Fortress, fifth lemure on the left",
	"Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go",
	"Be careful, or this could happen to you!",
	"Soon you'll join this fellow in hell! -- the Wizard of Yendor",
	"Caution! This grave contains toxic waste",
	"Sum quod eris",
	"Here lies an Atheist, all dressed up and no place to go",
	"Here lies Ezekiel, age 102.  The good die young.",
	"Here lies my wife: Here let her lie! Now she's at rest and so am I.",
	"Here lies Johnny Yeast. Pardon me for not rising.",
	"He always lied while on the earth and now he's lying in it",
	"I made an ash of myself",
	"Soon ripe. Soon rotten. Soon gone. But not forgotten.",
	"Here lies the body of Jonathan Blake. Stepped on the gas instead of the brake.",
	"Go away!"
#else
	"安らかに眠れ。",
	"注意 --墓の中に価値あるものは＊一つも＊ない。",
	"1994-1995 今迄でもっとも長生きしたハッカー。",
	"名もしらぬハッカーの墓。",
	"ここに眠っているのは誰かは知らない。しかし我々は埋葬した。",
	"スパーキー -- すばらしい犬だった。",
	"送電用の第三のレールに気をつけよ。",
	"Made in Taiwan.",
	"オレトモダチ、オレイイヤツ、オレ死んだ、オレ食料。",
	"ビートルジュースビートルジュースビートルジュース",
	"下を見ろ！",
	"掘らないでください。とても幸せなんです。-- 居住者",
	"郵便屋さん。電報は下記の住所に転送してください。ゲヘナ市アスモデウス要塞5番町。",
	"メリーさんの羊/ふわふわ真っ白/困ったときには/最初に逃げた",
	"気をつけよ！さもなくば同じことが起きる。-- 使用者",
	"もうすぐ仲間になれる！！ -- イェンダーの魔法使い",
	"気をつけよ！この墓は有毒な廃棄物が含まれている。",
	"涅槃で待つ",
	"無神論者ここに眠る。みんな着飾っているが、行く所がない。",
	"エゼキエルここに眠る。享年102歳。いいやつは早く死ぬ。",
	"わが妻ここに眠る。ここに眠らせた！妻は安息を得、私も安息を得た。",
	"ジョニー・イーストここに眠る。眠ったままで失礼します。",
	"彼はいつも土の上で眠っていたが、とうとう土の中で眠ることになった。",
	"自ら灰になった。",
	"早く咲けば、早く散る。早く去る。しかし決して忘れない。",
	"ジョナサン・ブレイクここに眠る。ブレーキではなくアクセルを踏んだ。",
	"立ちされ！"
#endif
};

/* Create a headstone at the given location.
 * The caller is responsible for newsym(x, y).
 */
void
make_grave(x, y, str)
int x, y;
const char *str;
{
	/* Can we put a grave here? */
	if ((levl[x][y].typ != ROOM && levl[x][y].typ != GRAVE) || t_at(x,y)) return;

	/* Make the grave */
	levl[x][y].typ = GRAVE;

	/* Engrave the headstone */
	if (!str) str = epitaphs[rn2(SIZE(epitaphs))];
	del_engr_at(x, y);
	make_engr_at(x, y, str, 0L, HEADSTONE);
	return;
}


#endif /* OVLB */

/*engrave.c*/
