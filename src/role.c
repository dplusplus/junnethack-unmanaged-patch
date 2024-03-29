/*	SCCS Id: @(#)role.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

/*
**	Japanese version Copyright
**	(c) Issei Numata, Naoki Hamada, Shigehiro Miyashita, 1994-2000
**	For 3.4, Copyright (c) Kentaro Shirakata, 2002-2009
**	For UnNetHack 3.5 Copyright (c) Masaki Miyaso, 2009-2010
**	JNetHack may be freely redistributed.  See license for details. 
*/

#include "hack.h"


/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */
/*JP
  文字列の最初の一文字目を見るコードが随所に存在するので、
  英語名を残しておく。 (see you.h)
*/

const struct Role roles[] = {
{	{"Archeologist", 0}, {
#if 1 /*JP*/
	 "考古学者", 0}, {
#endif
#if 0 /*JP*/
	{"Digger",      0},
	{"Field Worker",0},
	{"Investigator",0},
	{"Exhumer",     0},
	{"Excavator",   0},
	{"Spelunker",   0},
	{"Speleologist",0},
	{"Collector",   0},
	{"Curator",     0} },
#else
	{"鉱員",	0},
	{"労働者",      0},
	{"調査者",      0},
	{"発掘者",	0},
	{"掘削者",	0},
	{"探検者",	0},
	{"洞窟学者",    0},
	{"美術収集者",	0},
	{"館長",	0} },
#endif

	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
#if 0 /*JP*/
	"Arc", "the College of Archeology", "the Tomb of the Toltec Kings",
#else
	"Arc", "考古学大学", "トルテカ王家の墓",
#endif
	PM_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL,
	ART_ORB_OF_DETECTION,
	MH_HUMAN|MH_DWARF|MH_GNOME|MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 10,  7,  7,  7 },
	{  20, 20, 20, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -4
},
{	{"Barbarian", 0}, {
#if 1 /*JP*/
	 "野蛮人", 0}, {
#endif
#if 0 /*JP*/
	{"Plunderer",   "Plunderess"},
	{"Pillager",    0},
	{"Bandit",      0},
	{"Brigand",     0},
	{"Raider",      0},
	{"Reaver",      0},
	{"Slayer",      0},
	{"Chieftain",   "Chieftainess"},
	{"Conqueror",   "Conqueress"} },
#else
	{"盗賊",	"女盗賊"},
	{"略奪者",	0},
	{"悪漢",	0},
	{"山賊",	0},
	{"侵略者",	0},
	{"強盗",	0},
	{"殺戮者",	0},
	{"首領",	"女首領"},
        {"征服王",	0} },
#endif
	"Mitra", "Crom", "Set", /* Hyborian */
#if 0 /*JP*/
	"Bar", "the Camp of the Duali Tribe", "the Duali Oasis",
#else
	"Bar", "デュアリ族のキャンプ", "デュアリ族のオアシス",
#endif
	PM_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON,
	ART_HEART_OF_AHRIMAN,
	MH_HUMAN|MH_ORC|MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  16,  7,  7, 15, 16,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0,10,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_HASTE_SELF,      -4
},
{	{"Caveman", "Cavewoman"}, {
#if 1 /*JP*/
	 "洞窟人", 0}, {
#endif
#if 0 /*JP*/
	{"Troglodyte",  0},
	{"Aborigine",   0},
	{"Wanderer",    0},
	{"Vagrant",     0},
	{"Wayfarer",    0},
	{"Roamer",      0},
	{"Nomad",       0},
	{"Rover",       0},
	{"Pioneer",     0} },
#else
	{"穴居人",	0},
	{"原住民",	0},
	{"放浪者",	0},
	{"浮浪者",	0},
	{"旅行者",	0},
	{"放遊者",	0},
	{"遊牧民",	0},
	{"流浪者",	0},
	{"先駆者",	0} },
#endif
	"Anu", "_Ishtar", "Anshar", /* Babylonian */
#if 0 /*JP*/
	"Cav", "the Caves of the Ancestors", "the Dragon's Lair",
#else
	"Cav", "太古の洞窟", "竜の隠れ家",
#endif
	PM_CAVEMAN, PM_CAVEWOMAN, NON_PM,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON,
	ART_SCEPTRE_OF_MIGHT,
	MH_HUMAN|MH_DWARF|MH_GNOME|MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7,  8,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 12, 0, 1,  8, A_INT, SPE_DIG,             -4
},
{	{"Healer", 0}, {
#if 1 /*JP*/
	 "薬師", 0}, {
#endif
#if 0 /*JP*/
	{"Rhizotomist",    0},
	{"Empiric",        0},
	{"Embalmer",       0},
	{"Dresser",        0},
	{"Medicus ossium", "Medica ossium"},
	{"Herbalist",      0},
	{"Magister",       "Magistra"},
	{"Physician",      0},
	{"Chirurgeon",     0} },
#else
	{"見習い",      0},
	{"医師見習い",	0},
	{"看護師",	"看護婦"},
	{"医師助手",	0},
	{"薬物主任",	0},
	{"医師主任",	"看護主任"},
	{"漢方医",	0},
	{"内科医",	0},
	{"外科医",	0} },
#endif
	"_Athena", "Hermes", "Poseidon", /* Greek */
#if 0 /*JP*/
	"Hea", "the Temple of Epidaurus", "the Temple of Coeus",
#else
	"Hea", "エピダウロス寺院", "コイオス寺院",
#endif
	PM_HEALER, NON_PM, NON_PM,
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS,
	ART_STAFF_OF_AESCULAPIUS,
	MH_HUMAN|MH_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 13,  7, 11, 16 },
	{  15, 20, 20, 15, 25, 5 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4
},
{	{"Knight", 0}, {
#if 1 /*JP*/
	 "騎士", 0}, {
#endif
#if 0 /*JP*/
	{"Gallant",     0},
	{"Esquire",     0},
	{"Bachelor",    0},
	{"Sergeant",    0},
	{"Knight",      0},
	{"Banneret",    0},
	{"Chevalier",   "Chevaliere"},
	{"Seignieur",   "Dame"},
	{"Paladin",     0} },
#else
	{"見習い",	0},
	{"歩兵",	0},
	{"戦士",	"女戦士"},
	{"騎兵",	0},
	{"重戦士",	0},
	{"騎士",	0},
	{"重騎士",	0},
	{"勲騎士",	0},
	{"聖騎士",	0} },
#endif
	"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
#if 0 /*JP*/
	"Kni", "Camelot Castle", "the Isle of Glass",
#else
	"Kni", "キャメロット城", "ガラスの島",
#endif
	PM_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_IXOTH,
	ART_MAGIC_MIRROR_OF_MERLIN,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  13,  7, 14,  8, 10, 17 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},
{	{"Monk", 0}, {
#if 1 /*JP*/
	 "武闘家", 0}, {
#endif
#if 0 /*JP*/
	{"Candidate",         0},
	{"Novice",            0},
	{"Initiate",          0},
	{"Student of Stones", 0},
	{"Student of Waters", 0},
	{"Student of Metals", 0},
	{"Student of Winds",  0},
	{"Student of Fire",   0},
	{"Master",            0} },
#endif
	{"入門希望者",	0},
	{"初心者",	0},
	{"入門者伝",	0},
	{"土の習い手",	0},
	{"水の習い手",	0},
	{"金の習い手",	0},
	{"木の習い手",	0},
	{"火の習い手",	0},
	{"免許皆伝",	0} },
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
#if 0 /*JP*/
	"Mon", "the Monastery of Chan-Sune",
	  "the Monastery of the Earth-Lord",
#else
	"Mon", "チャン・スー修道院",
	  "地王の修道院",
#endif
	PM_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN,
	ART_EYES_OF_THE_OVERWORLD,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  8,  8,  7,  7 },
	{  25, 10, 20, 20, 15, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -4
},
{	{"Priest", "Priestess"}, {
#if 1 /*JP*/
	 "僧侶", "尼僧"}, {
#endif
#if 0 /*JP*/
	{"Aspirant",    0},
	{"Acolyte",     0},
	{"Adept",       0},
	{"Priest",      "Priestess"},
	{"Curate",      0},
	{"Canon",       "Canoness"},
	{"Lama",        0},
	{"Patriarch",   "Matriarch"},
	{"High Priest", "High Priestess"} },
#else
	{"修道者",	"修道女"},
	{"侍者",	0},
	{"侍祭",	0},
	{"僧侶",	"尼僧"},
	{"助任司祭",	0},
	{"聖者",	"聖女"},
	{"司教",	0},
	{"大司教",	0},
	{"大僧上",      0} },
#endif
	0, 0, 0,	/* chosen randomly from among the other roles */
#if 0 /*JP*/
	"Pri", "the Great Temple", "the Temple of Nalzok",
#else
	"Pri", "偉大なる寺院", "ナルゾク寺院",
#endif
	PM_PRIEST, PM_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_NALZOK,
	ART_MITRE_OF_HOLINESS,
	MH_HUMAN|MH_ELF | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 10,  7,  7,  7 },
	{  15, 10, 30, 15, 20, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4
},
  /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
     retains its traditional meaning. */
{	{"Rogue", 0}, {
#if 1 /*JP*/
	 "盗賊", 0}, {
#endif
#if 0 /*JP*/
	{"Footpad",     0},
	{"Cutpurse",    0},
	{"Rogue",       0},
	{"Pilferer",    0},
	{"Robber",      0},
	{"Burglar",     0},
	{"Filcher",     0},
	{"Magsman",     "Magswoman"},
	{"Thief",       0} },
#else
	{"追いはぎ",	0},
	{"ひったくり",	0},
	{"スリ",	0},
	{"ごろつき",	0},
	{"こそどろ",	0},
	{"空巣",	0},
	{"泥棒",	"女泥棒"},
	{"強盗",	0},
	{"大泥棒",	0} },
#endif
	"Issek", "Mog", "Kos", /* Nehwon */
#if 0 /*JP*/
	"Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall",
#else
	"Rog", "盗賊ギルド", "暗殺者ギルド",
#endif
	PM_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN,
	ART_MASTER_KEY_OF_THIEVERY,
	MH_HUMAN|MH_ORC|MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  7,  6 },
	{  20, 10, 10, 30, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -4
},
{	{"Ranger", 0}, {
#if 1 /*JP*/
	 "レンジャー", 0}, {
#endif
#if 0	/* OBSOLETE */
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"},	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Lolth", /* Elven */
#endif
#if 0 /*JP*/
	{"Tenderfoot",    0},
	{"Lookout",       0},
	{"Trailblazer",   0},
	{"Reconnoiterer", "Reconnoiteress"},
	{"Scout",         0},
	{"Arbalester",    0},	/* One skilled at crossbows */
	{"Archer",        0},
	{"Sharpshooter",  0},
	{"Marksman",      "Markswoman"} },
#else
	{"新米",	0},
	{"見張り",	0},
	{"先導",	0},
	{"偵察",	0},
	{"斥候",	0},
	{"弓兵",	0},	/* One skilled at crossbows */
	{"中級弓兵",	0},
	{"上級弓兵",	0},
	{"名人",	0} },
#endif
	"Mercury", "_Venus", "Mars", /* Roman/planets */
#if 0 /*JP*/
	"Ran", "Orion's camp", "the cave of the wumpus",
#else
	"Ran", "オリオンのキャンプ", "ワンパスの洞窟",
#endif
	PM_RANGER, NON_PM, NON_PM /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,
	ART_LONGBOW_OF_DIANA,
	MH_HUMAN|MH_ELF|MH_GNOME|MH_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 6,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4
},
{	{"Samurai", 0}, {
#if 1 /*JP*/
	 "侍", 0}, {
#endif
#if 0 /*JP*/
	{"Hatamoto",    0},  /* Banner Knight */
	{"Ronin",       0},  /* no allegiance */
	{"Ninja",       "Kunoichi"},  /* secret society */
	{"Joshu",       0},  /* heads a castle */
	{"Ryoshu",      0},  /* has a territory */
	{"Kokushu",     0},  /* heads a province */
	{"Daimyo",      0},  /* a samurai lord */
	{"Kuge",        0},  /* Noble of the Court */
	{"Shogun",      0} },/* supreme commander, warlord */
#else
	{"旗本",	0},  /* Banner Knight */
	{"浪人",	0},  /* no allegiance */
	{"忍者",	"くノ一"},  /* secret society */
	{"城主",	0},  /* heads a castle */
	{"領主",	0},  /* has a territory */
	{"国主",	0},  /* heads a province */
	{"大名",	"腰元"},  /* a samurai lord */
	{"公家",	0},  /* Noble of the Court */
	{"将軍",	"大奥"} },  /* supreme commander, warlord */
#endif
	"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
#if 0 /*JP*/
	"Sam", "the Castle of the Taro Clan", "the Shogun's Castle",
#else
	"Sam", "太郎一族の城", "将軍の城",
#endif
	PM_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI,
	ART_TSURUGI_OF_MURAMASA,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  10,  8,  7, 10, 17,  6 },
	{  30, 10,  8, 30, 14,  8 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -4
},
#ifdef TOURIST
{	{"Tourist", 0}, {
#if 1 /*JP*/
	 "観光客", 0}, {
#endif
#if 0 /*JP*/
	{"Rambler",     0},
	{"Sightseer",   0},
	{"Excursionist",0},
	{"Peregrinator","Peregrinatrix"},
	{"Traveler",    0},
	{"Journeyer",   0},
	{"Voyager",     0},
	{"Explorer",    0},
	{"Adventurer",  0} },
#else
	{"プー太郎",	"プー子"},
	{"観光客",	0},
	{"周遊旅行者",  0},
	{"遍歴者",      0},
	{"旅行者",	0},
	{"旅人",	0},
	{"航海者",	0},
	{"探検家",	0},
	{"冒険者",	0} },
#endif
	"Blind Io", "_The Lady", "Offler", /* Discworld */
#if 0 /*JP*/
	"Tou", "Ankh-Morpork", "the Thieves' Guild Hall",
#else
	"Tou", "アンクモルポーク", "盗賊ギルド",
#endif
	PM_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES,
	ART_YENDORIAN_EXPRESS_CARD,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  6,  7,  7, 10 },
	{  15, 10, 10, 15, 30, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4
},
#endif
{	{"Valkyrie", 0}, {
#if 1 /*JP*/
	 "ワルキューレ", 0}, {
#endif
#if 0 /*JP*/
	{"Stripling",   0},
	{"Skirmisher",  0},
	{"Fighter",     0},
	{"Man-at-arms", "Woman-at-arms"},
	{"Warrior",     0},
	{"Swashbuckler",0},
	{"Hero",        "Heroine"},
	{"Champion",    0},
	{"Lord",        "Lady"} },
#else
	{"見習い",	0},
	{"歩兵",	0},
	{"戦士",	"女戦士"},
	{"騎兵",      "女重騎兵"},
	{"戦闘兵",	0},
	{"攻撃兵",      0},
	{"英雄",	0},
	{"闘士",	"女闘士"},
	{"ロード",	"レディ"} },
#endif
	"Tyr", "Odin", "Loki", /* Norse */
#if 0 /*JP*/
	"Val", "the Shrine of Destiny", "the cave of Surtur",
#else
	"Val", "運命の聖堂", "スルトの洞窟",
#endif
	PM_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_WARRIOR, PM_LORD_SURTUR,
	ART_ORB_OF_FATE,
	MH_HUMAN|MH_DWARF | ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 10,  7 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  2, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_CONE_OF_COLD,    -4
},
{	{"Wizard", 0}, {
#if 1 /*JP*/
	 "魔法使い", 0}, {
#endif
#if 0 /*JP*/
	{"Evoker",      0},
	{"Conjurer",    0},
	{"Thaumaturge", 0},
	{"Magician",    0},
	{"Enchanter",   "Enchantress"},
	{"Sorcerer",    "Sorceress"},
	{"Necromancer", 0},
	{"Wizard",      0},
	{"Mage",        0} },
#else
	{"手品師",	0},
	{"奇術師",	0},
	{"占い師",	0},
	{"霊感師",	0},
	{"召喚師",	0},
	{"妖術師",      0},
	{"魔術師",      0},
	{"魔法使い",	"魔女"},
	{"大魔法使い",	0} },
#endif
	"Ptah", "Thoth", "Anhur", /* Egyptian */
#if 0 /*JP*/
	"Wiz", "the Lonely Tower", "the Tower of Darkness",
#else
	"Wiz", "調和の塔", "暗黒の塔",
#endif
	PM_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE,
	ART_EYE_OF_THE_AETHIOPICA,
	MH_HUMAN|MH_ELF|MH_GNOME|MH_ORC|MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_MAGIC_MISSILE,   -4
},
/* Array terminator */
{{0, 0}}
};


/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Role urole =
#if 0 /*JP*/
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
#else
{	{"Undefined", 0}, {"謎", 0}, { {0, 0}, {0, 0}, {0, 0},
#endif
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};



/* Table of all races */
const struct Race races[] = {
/*JP
  文字列の最初の一文字目を見るコードが随所に存在するので、
  英語名を残しておく。 (see you.h)
*/
#if 0 /*JP*/
{	"human", "human", "humanity", "Hum",
	{"man", "woman"},
#else
{	"human", "人間", "human", "humanity", "Hum",
	{"男", "女"},
#endif
	PM_HUMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MH_HUMAN, 0, MH_GNOME|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},
/*JP
{	"elf", "elven", "elvenkind", "Elf",
*/
{	"elf", "エルフ", "elven", "elvenkind", "Elf",
	{0, 0},
	PM_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MH_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MH_ELF, MH_ELF, MH_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 }		/* Energy */
},
/*JP
{	"dwarf", "dwarven", "dwarvenkind", "Dwa",
*/
{	"dwarf", "ドワーフ", "dwarven", "dwarvenkind", "Dwa",
	{0, 0},
	PM_DWARF, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MH_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	MH_DWARF, MH_DWARF|MH_GNOME, MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},
/*JP
{	"gnome", "gnomish", "gnomehood", "Gno",
*/
{	"gnome", "ノーム", "gnomish", "gnomehood", "Gno",
	{0, 0},
	PM_GNOME, NON_PM, PM_GNOME_MUMMY, PM_GNOME_ZOMBIE,
	MH_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MH_GNOME, MH_DWARF|MH_GNOME, MH_HUMAN,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{STR18(50),19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},
/*JP
{	"orc", "orcish", "orcdom", "Orc",
*/
{	"orc", "オーク", "orcish", "orcdom", "Orc",
	{0, 0},
	PM_ORC, NON_PM, PM_ORC_MUMMY, PM_ORC_ZOMBIE,
	MH_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MH_ORC, 0, MH_HUMAN|MH_ELF|MH_DWARF,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{STR18(50),16, 16, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},
/*JP
{	"vampire", "vampiric", "vampirehood", "Vam",
*/
{	"vampire", "吸血鬼", "vampiric", "vampirehood", "Vam",
	{0, 0},
	PM_VAMPIRE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MH_VAMPIRE | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MH_VAMPIRE, 0, MH_ELF|MH_GNOME|MH_DWARF|MH_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      4,      0,  0,  4,  3,  4 },
	{ STR19(19), 18, 18, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  3, 0,  4, 0,  4, 0 }		/* Energy */
},
/* Array terminator */
{ 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
/*JP
{	"something", "undefined", "something", "Xxx",
*/
{	"something", "謎", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
};


/* Table of all genders */
const struct Gender genders[] = {
/*JP: オプションで使っているので英語名を残しておく。 (see you.h) */
#if 0 /*JP*/
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
#else
	{"male",	"男性",	"彼",	"彼",	"彼の",	"Mal",	ROLE_MALE},
	{"female",	"女性",	"彼女",	"彼女",	"彼女の","Fem",	ROLE_FEMALE},
	{"neuter",	"中性",	"それ",	"それ",	"その",	"Ntr",	ROLE_NEUTER}
#endif
};


/* Table of all alignments */
const struct Align aligns[] = {
#if 0 /*JP*/
	{"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"unaligned",	"Una",	0,		A_NONE}
#else
	{"law",		"秩序",	"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"中立",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"混沌",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"無心",	"unaligned",	"Una",	0,		A_NONE}
#endif
};


/* Table of roleplay-conducts */

const struct Conduct conducts[] = {
#if 0 /*JP*/
{	"pacifism",	"pacifist", 	"peaceful",	TRUE,
	"You ","have been ","were ","a pacifist",
	"pretended to be a pacifist"},
#else
{	"平和主義",	"平和主義者", 	"平和的で",	TRUE,
	"あなたは","である","だった","平和主義者",
	"平和主義者に挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"sadism",	"sadist", 	"sadistic",	TRUE,
	"You ","have been ","were ","a sadist",
	"pretended to be a sadist"},
#else
{	"加虐趣味",	"サディスト", 	"加虐的で",	TRUE,
	"あなたは","である","だった","サディスト",
	"サディストに挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"atheism", 	"atheist", 	"atheistic",	TRUE,
	"You ","have been ","were ","an atheist",
	"pretended to be an atheist"},
#else
{	"無神論", 	"無神論者", 	"無信心で",	TRUE,
	"あなたは","である","だった","無神論者",
	"無神論者に挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"nudism", 	"nudist", 	"nude",		TRUE,
	"You ","have been ","were ","a nudist",
	"pretended to be a nudist"},
#else
{	"裸体主義", 	"ヌーディスト", 	"裸で",		TRUE,
	"あなたは","である","だった","ヌーディスト",
	"ヌーディストに挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"zen",		"zen master", 	"blindfolded",	TRUE,
	"You ","have followed ","followed ","the true Path of Zen",
	"left the true Path of Zen"},
#else
{	"禅",		"禅マスター", 	"盲目で",	TRUE,
	"あなたは","を求道している","を求道していた","禅の道",
	"禅の道から外れて"},
#endif /*JP*/

#if 0 /*JP*/
{	"asceticism",	"ascetic",	"hungry",	TRUE,
	"You ","have gone ","went ","without food",
	"pretended to be an ascet"},
#else
{	"苦行",	"苦行者",	"空腹で",	TRUE,
	"あなたは","ていない","なかった","食事をし",
	"苦行に挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"vegan",	"vegan",	"vegan",	TRUE,
	"You ","have followed ","followed ","a strict vegan diet",
	"pretended to be a vegan"},
#else
{	"厳格な菜食主義",	"厳格な菜食主義者",	"厳格な菜食主義で",	TRUE,
	"あなたは","である","だった","厳格な菜食主義者",
	"厳格な菜食主義に挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"vegetarian", 	"vegetarian",	"vegetarian",	TRUE,
	"You ","have been ","were ","vegetarian",
	"pretended to be a vegetarian"},
#else
{	"菜食主義", 	"菜食主義者",	"菜食主義で",	TRUE,
	"あなたは","である","だった","菜食主義者",
	"菜食主義に挑戦して頓挫して"},
#endif /*JP*/

#if 0 /*JP*/
{	"illiteracy", 	"illiterate",	"illiterate",	TRUE,
	"You ","have been ","were ","illiterate",
	"become literate"},
#else
{	"文盲", 	"文盲者",	"文盲で",	TRUE,
	"あなたは","ていない","なかった","読み書きし",
	"読み書きし始めて"},
#endif /*JP*/

#if 0 /*JP*/
{	"thievery",	"master thief",	"tricky",	TRUE,
	"You ","have been ","were ","very tricky",
	"pretended to be a master thief"}
#else
{	"盗み",	"盗賊頭",	"厄介で",	TRUE,
	"あなたは","である","だった","とても厄介者",
	"盗賊頭に挑戦して頓挫して"}
#endif /*JP*/
};

STATIC_DCL char * FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";


boolean
validrole(rolenum)
	int rolenum;
{
	return (rolenum >= 0 && rolenum < SIZE(roles)-1);
}


int
randrole()
{
	return (rn2(SIZE(roles)-1));
}


int
str2role(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; roles[i].name.m; i++) {
	    /* Does it match the male name? */
	    if (!strncmpi(str, roles[i].name.m, len))
		return i;
	    /* Or the female name? */
	    if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, roles[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validrace(rolenum, racenum)
	int rolenum, racenum;
{
	/* Assumes validrole */
	/* WAC -- checks ROLE_GENDMASK and ROLE_ALIGNMASK as well (otherwise, there 
	 * might not be an allowed gender or alignment for that role
	 */
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_RACEMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_GENDMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_ALIGNMASK));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK)*/
	    if (validrace(rolenum,i))
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK) {*/
	    if (validrace(rolenum,i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role has no permitted races? */
	return (rn2(SIZE(races)-1));
}


int
str2race(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; races[i].noun; i++) {
	    /* Does it match the noun? */
	    if (!strncmpi(str, races[i].noun, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, races[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validgend(rolenum, racenum, gendnum)
	int rolenum, racenum, gendnum;
{
	/* Assumes validrole and validrace */
	return (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		(roles[rolenum].allow & races[racenum].allow &
		 genders[gendnum].allow & ROLE_GENDMASK));
}


int
randgend(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid genders */
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) */
	    if (validgend(rolenum, racenum, i))
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {*/
	    if (validgend(rolenum, racenum, i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted genders? */
	return (rn2(ROLE_GENDERS));
}


int
str2gend(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_GENDERS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, genders[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, genders[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validalign(rolenum, racenum, alignnum)
	int rolenum, racenum, alignnum;
{
	/* Assumes validrole and validrace */
	return (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		(roles[rolenum].allow & races[racenum].allow &
		 aligns[alignnum].allow & ROLE_ALIGNMASK));
}


int
randalign(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid alignments */
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
	    	n++;

	/* Pick a random alignment */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted alignments? */
	return (rn2(ROLE_ALIGNS));
}


int
str2align(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, aligns[i].adj, len))
		return i;
#if 1 /*JP*/
	    if (!strncmpi(str, aligns[i].j, len))
		return i;
#endif

	    /* Or the filecode? */
	    if (!strcmpi(str, aligns[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (rolenum >= 0 && rolenum < SIZE(roles)-1) {
	allow = roles[rolenum].allow;

	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0;

    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum))
	    roles_ok++;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    roles_ok = rn2(roles_ok);
    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum)) {
	    if (roles_ok == 0)
		return i;
	    else
		roles_ok--;
	}
    }
    return ROLE_NONE;
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && racenum < SIZE(races)-1) {
	allow = races[racenum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum))
	    races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum)) {
	    if (races_ok == 0)
		return i;
	    else
		races_ok--;
	}
    }
    return ROLE_NONE;
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
	allow = genders[gendnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
		    
	if (!(allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_GENDMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum))
	    gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum)) {
	    if (gends_ok == 0)
		return i;
	    else
		gends_ok--;
	}
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
	allow = aligns[alignnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;

	if (!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_ALIGNMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i))
	    aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i)) {
	    if (aligns_ok == 0)
		return i;
	    else
		aligns_ok--;
	}
    }
    return ROLE_NONE;
}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
	/* If the role was explicitly specified as ROLE_RANDOM
	 * via -uXXXX-@ then choose the role in here to narrow down
	 * later choices. Pick a random role in this case.
	 */
	flags.initrole = pick_role(flags.initrace, flags.initgend,
					flags.initalign, PICK_RANDOM);
	if (flags.initrole < 0)
	    flags.initrole = randrole();
    }
    if (flags.initrole != ROLE_NONE) {
	if (flags.initrace == ROLE_NONE)
	     flags.initrace = pick_race(flags.initrole, flags.initgend,
						flags.initalign, PICK_RIGID);
	if (flags.initalign == ROLE_NONE)
	     flags.initalign = pick_align(flags.initrole, flags.initrace,
						flags.initgend, PICK_RIGID);
	if (flags.initgend == ROLE_NONE)
	     flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RIGID);
    }
}

#define BP_ALIGN	0
#define BP_GEND		1
#define BP_RACE		2
#define BP_ROLE		3
#define NUM_BP		4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
#if 0 /*JP*/
	const char *conj = "and ";
	if (num_post_attribs > 1
	    && post_attribs < num_post_attribs && post_attribs > 1)
	 	Strcat(buf, ","); 
	Strcat(buf, " ");
	--post_attribs;
	if (!post_attribs && num_post_attribs > 1) Strcat(buf, conj);
#else
	if(num_post_attribs > post_attribs){
		Strcat(buf, "、");
	}
	--post_attribs;
#endif
	return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
	int gendcount = 0;
	if (validrole(rolenum)) {
		if (roles[rolenum].allow & ROLE_MALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_FEMALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_NEUTER) ++gendcount;
	}
	return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
	int aligncount = 0;
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (races[racenum].allow & ROLE_CHAOTIC) ++aligncount;
		if (races[racenum].allow & ROLE_LAWFUL) ++aligncount;
		if (races[racenum].allow & ROLE_NEUTRAL) ++aligncount;
	}
	return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum, alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	int k, gendercount = 0, aligncount = 0;
	char buf[BUFSZ];
/*JP
	static char err_ret[] = " character's";
*/
	static char err_ret[] = "キャラクタの";
	boolean donefirst = FALSE;

	if (!suppliedbuf || buflen < 1) return err_ret;

	/* initialize these static variables each time this is called */
	post_attribs = 0;
	for (k=0; k < NUM_BP; ++k)
		pa[k] = 0;
	buf[0] = '\0';
	*suppliedbuf = '\0';
	
	/* How many alignments are allowed for the desired race? */
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
		aligncount = race_alignmentcount(racenum);

	if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM) {
		/* if race specified, and multiple choice of alignments for it */
		if ((racenum >= 0) && (aligncount > 1)) {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
#else
			if (donefirst) Strcat(buf, "の");
			Strcat(buf, aligns[alignnum].j);
#endif
			donefirst = TRUE;
		} else {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
#else
			if (donefirst) Strcat(buf, "の");
			Strcat(buf, aligns[alignnum].j);
#endif
			donefirst = TRUE;
		}
	} else {
		/* if alignment not specified, but race is specified
			and only one choice of alignment for that race then
			don't include it in the later list */
		if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM) &&
			ok_race(rolenum, racenum, gendnum, alignnum))
		      && (aligncount > 1))
		     || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
			pa[BP_ALIGN] = 1;
			post_attribs++;
		}
	}
	/* <your lawful> */

	/* How many genders are allowed for the desired role? */
	if (validrole(rolenum))
		gendercount = role_gendercount(rolenum);

	if (gendnum != ROLE_NONE  && gendnum != ROLE_RANDOM) {
		if (validrole(rolenum)) {
		     /* if role specified, and multiple choice of genders for it,
			and name of role itself does not distinguish gender */
			if ((rolenum != ROLE_NONE) && (gendercount > 1)
						&& !roles[rolenum].name.f) {
#if 0 /*JP*/
				if (donefirst) Strcat(buf, " ");
				Strcat(buf, genders[gendnum].adj);
#else
				if (donefirst) Strcat(buf, "の");
				Strcat(buf, genders[gendnum].j);
#endif
				donefirst = TRUE;
			}
	        } else {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
	        	Strcat(buf, genders[gendnum].adj);
#else
			if (donefirst) Strcat(buf, "の");
			Strcat(buf, genders[gendnum].j);
#endif
			donefirst = TRUE;
	        }
	} else {
		/* if gender not specified, but role is specified
			and only one choice of gender then
			don't include it in the later list */
		if ((validrole(rolenum) && (gendercount > 1)) || !validrole(rolenum)) {
			pa[BP_GEND] = 1;
			post_attribs++;
		}
	}
	/* <your lawful female> */

	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (validrole(rolenum) && ok_race(rolenum, racenum, gendnum, alignnum)) {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " "); 
			Strcat(buf, (rolenum == ROLE_NONE) ?
				races[racenum].noun :
				races[racenum].adj);
#else
			if (donefirst) Strcat(buf, "の"); 
			Strcat(buf, races[racenum].j);
#endif
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, races[racenum].noun);
#else
			if (donefirst) Strcat(buf, "の"); 
			Strcat(buf, races[racenum].j);
#endif
			donefirst = TRUE;
		} else {
			pa[BP_RACE] = 1;
			post_attribs++;
		}
	} else {
		pa[BP_RACE] = 1;
		post_attribs++;
	}
	/* <your lawful female gnomish> || <your lawful female gnome> */

	if (validrole(rolenum)) {
/*JP
		if (donefirst) Strcat(buf, " ");
*/
		if (donefirst) Strcat(buf, "の");
		if (gendnum != ROLE_NONE) {
#if 0 /*JP*/
		    if (gendnum == 1  && roles[rolenum].name.f)
			Strcat(buf, roles[rolenum].name.f);
		    else
  			Strcat(buf, roles[rolenum].name.m);
#else
		    if (gendnum == 1  && roles[rolenum].jname.f)
			Strcat(buf, roles[rolenum].jname.f);
		    else
  			Strcat(buf, roles[rolenum].jname.m);
#endif
		} else {
#if 0 /*JP*/
			if (roles[rolenum].name.f) {
				Strcat(buf, roles[rolenum].name.m);
				Strcat(buf, "/");
				Strcat(buf, roles[rolenum].name.f);
			} else 
				Strcat(buf, roles[rolenum].name.m);
#else
			if (roles[rolenum].jname.f) {
				Strcat(buf, roles[rolenum].jname.m);
				Strcat(buf, "/");
				Strcat(buf, roles[rolenum].jname.f);
			} else 
				Strcat(buf, roles[rolenum].jname.m);
#endif
		}
		donefirst = TRUE;
	} else if (rolenum == ROLE_NONE) {
		pa[BP_ROLE] = 1;
		post_attribs++;
	}
	
	if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM) && !validrole(rolenum)) {
#if 0 /*JP*/
		if (donefirst) Strcat(buf, " ");
		Strcat(buf, "character");
#else
		if (donefirst) Strcat(buf, "の");
		Strcat(buf, "キャラクタ");
#endif
		donefirst = TRUE;
	}
#if 1 /*JP*/
	Strcat(buf, "の");
#endif
	/* <your lawful female gnomish cavewoman> || <your lawful female gnome>
	 *    || <your lawful female character>
	 */
	if (buflen > (int) (strlen(buf) + 1)) {
		Strcpy(suppliedbuf, buf);
		return suppliedbuf;
	} else
		return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
/*JP
	const char *defprompt = "Shall I pick a character for you? [yntq] ";
*/
	const char *defprompt = "適当にキャラクタを選んでよいですか？[yntq] ";
	int num_post_attribs = 0;
	char tmpbuf[BUFSZ];
	
	if (buflen < QBUFSZ)
		return (char *)defprompt;

#if 0 /*JP*/
	Strcpy(tmpbuf, "Shall I pick ");
	if (racenum != ROLE_NONE || validrole(rolenum))
		Strcat(tmpbuf, "your ");
	else {
		Strcat(tmpbuf, "a ");
	}
#else
	Strcpy(tmpbuf, "");
#endif
	/* <your> */

	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
	Sprintf(buf, "%s", s_suffix(tmpbuf));

	/* buf should now be:
	 * < your lawful female gnomish cavewoman's> || <your lawful female gnome's>
	 *    || <your lawful female character's>
	 *
         * Now append the post attributes to it
	 */

	num_post_attribs = post_attribs;
	if (post_attribs) {
		if (pa[BP_RACE]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "race");
*/
			Strcat(buf, "種族");
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "role");
*/
			Strcat(buf, "職業");
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "gender");
*/
			Strcat(buf, "性別");
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "alignment");
*/
			Strcat(buf, "属性");
		}
	}
/*JP
	Strcat(buf, " for you? [yntq] ");
*/
	Strcat(buf, "を適当に選んでよろしいですか？[yntq] ");
	return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
	char *sptr, *eptr;
	int i;

	/* Look for tokens delimited by '-' */
	if ((eptr = index(plname, '-')) != (char *) 0)
	    *eptr++ = '\0';
	while (eptr) {
	    /* Isolate the next token */
	    sptr = eptr;
	    if ((eptr = index(sptr, '-')) != (char *)0)
		*eptr++ = '\0';

	    /* Try to match it to something */
	    if ((i = str2role(sptr)) != ROLE_NONE)
		flags.initrole = i;
	    else if ((i = str2race(sptr)) != ROLE_NONE)
		flags.initrace = i;
	    else if ((i = str2gend(sptr)) != ROLE_NONE)
		flags.initgend = i;
	    else if ((i = str2align(sptr)) != ROLE_NONE)
		flags.initalign = i;
	}
	if(!plname[0]) {
	    askname();
	    plnamesuffix();
	}

	/* commas in the plname confuse the record file, convert to spaces */
	for (sptr = plname; *sptr; sptr++) {
		if (*sptr == ',') *sptr = ' ';
	}
}


/*
 *	Special setup modifications here:
 *
 *	Unfortunately, this is going to have to be done
 *	on each newgame or restore, because you lose the permonst mods
 *	across a save/restore.  :-)
 *
 *	1 - The Rogue Leader is the Tourist Nemesis.
 *	2 - Priests start with a random alignment - convert the leader and
 *	    guardians here.
 *	3 - Elves can have one of two different leaders, but can't work it
 *	    out here because it requires hacking the level file data (see
 *	    sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
	int alignmnt;

	/* Strip the role letter out of the player name.
	 * This is included for backwards compatibility.
	 */
	plnamesuffix();

	/* Check for a valid role.  Try flags.initrole first. */
	if (!validrole(flags.initrole)) {
	    /* Try the player letter second */
	    if ((flags.initrole = str2role(pl_character)) < 0)
	    	/* None specified; pick a random role */
	    	flags.initrole = randrole();
	}

	/* We now have a valid role index.  Copy the role name back. */
	/* This should become OBSOLETE */
	Strcpy(pl_character, roles[flags.initrole].name.m);
	pl_character[PL_CSIZ-1] = '\0';

	/* Check for a valid race */
	if (!validrace(flags.initrole, flags.initrace))
	    flags.initrace = randrace(flags.initrole);

	/* Check for a valid gender.  If new game, check both initgend
	 * and female.  On restore, assume flags.female is correct. */
	if (flags.pantheon == -1) {	/* new game */
	    if (!validgend(flags.initrole, flags.initrace, flags.female))
		flags.female = !flags.female;
	}
	if (!validgend(flags.initrole, flags.initrace, flags.initgend))
	    /* Note that there is no way to check for an unspecified gender. */
	    flags.initgend = flags.female;

	/* Check for a valid alignment */
	if (!validalign(flags.initrole, flags.initrace, flags.initalign))
	    /* Pick a random alignment */
	    flags.initalign = randalign(flags.initrole, flags.initrace);
	alignmnt = aligns[flags.initalign].value;

	/* Initialize urole and urace */
	urole = roles[flags.initrole];
	urace = races[flags.initrace];

	/* Fix up the quest leader */
	if (urole.ldrnum != NON_PM) {
	    mons[urole.ldrnum].msound = MS_LEADER;
	    mons[urole.ldrnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.ldrnum].mflags3 |= M3_CLOSE;
	    mons[urole.ldrnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest guardians */
	if (urole.guardnum != NON_PM) {
	    mons[urole.guardnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.guardnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest nemesis */
	if (urole.neminum != NON_PM) {
	    mons[urole.neminum].msound = MS_NEMESIS;
	    mons[urole.neminum].mflags2 &= ~(M2_PEACEFUL);
	    mons[urole.neminum].mflags2 |= (M2_NASTY|M2_STALK|M2_HOSTILE);
	    mons[urole.neminum].mflags3 |= M3_WANTSARTI | M3_WAITFORU;
	}

	/* Fix up the god names */
	if (flags.pantheon == -1) {		/* new game */
	    flags.pantheon = flags.initrole;	/* use own gods */
	    while (!roles[flags.pantheon].lgod)	/* unless they're missing */
		flags.pantheon = randrole();
	}
	if (!urole.lgod) {
	    urole.lgod = roles[flags.pantheon].lgod;
	    urole.ngod = roles[flags.pantheon].ngod;
	    urole.cgod = roles[flags.pantheon].cgod;
	}

	/* Fix up infravision */
	if (mons[urace.malenum].mflags3 & M3_INFRAVISION) {
	    /* although an infravision intrinsic is possible, infravision
	     * is purely a property of the physical race.  This means that we
	     * must put the infravision flag in the player's current race
	     * (either that or have separate permonst entries for
	     * elven/non-elven members of each class).  The side effect is that
	     * all NPCs of that class will have (probably bogus) infravision,
	     * but since infravision has no effect for NPCs anyway we can
	     * ignore this.
	     */
	    mons[urole.malenum].mflags3 |= M3_INFRAVISION;
	    if (urole.femalenum != NON_PM)
	    	mons[urole.femalenum].mflags3 |= M3_INFRAVISION;
	}

	/* Fix up initial roleplay flags */
	if (Role_if(PM_MONK)) 
	    flags.vegetarian = TRUE;
	flags.vegan |= flags.ascet;
	flags.vegetarian |= flags.vegan;

	/* Artifacts are fixed in hack_artifacts() */

	/* Success! */
	return;
}

#if 0 /*JP*/
/*
  あいさつは日本語として自然になるよう大きく仕様を変更
 */
const char *
Hello(mtmp)
struct monst *mtmp;
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Salutations"); /* Olde English */
	case PM_MONK:
	    return ("Namaste"); /* Sanskrit */
	case PM_SAMURAI:
	    return (mtmp && mtmp->data == &mons[PM_SHOPKEEPER] ?
	    		"Irasshaimase" : "Konnichi wa"); /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");       /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return (
#ifdef MAIL
	    		mtmp && mtmp->data == &mons[PM_MAIL_DAEMON] ? "Hallo" :
#endif
	    		"Velkommen");   /* Norse */
	default:
	    return ("Hello");
	}
}

const char *
Goodbye()
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Fare thee well");  /* Olde English */
	case PM_MONK:
	    return ("Punardarsanaya");  /* Sanskrit */
	case PM_SAMURAI:
	    return ("Sayonara");        /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");           /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return ("Farvel");          /* Norse */
	default:
	    return ("Goodbye");
	}
}
#else /*JP*/
const char *
Hello(mtmp, nameflg)
struct monst *mtmp;
int nameflg;
{
    static char helo_buf[BUFSZ];


    switch (Role_switch) {
    case PM_KNIGHT:
	if(nameflg)
	    Sprintf(helo_buf, "よくぞ参った%sよ", plname);
	else
	    Sprintf(helo_buf, "よくぞ参った");
	break;
    case PM_MONK:
	if(nameflg)
	    Sprintf(helo_buf, "ナマステー%s", plname);
	else
	    Sprintf(helo_buf, "ナマステー");
	break;
    case PM_SAMURAI:
	if(nameflg)
	    Sprintf(helo_buf, "よくぞ参られた%sよ", plname);
	else
	    Sprintf(helo_buf, "よくぞ参られた");
	break;
#ifdef TOURIST
    case PM_TOURIST:
	if(nameflg)
	    Sprintf(helo_buf, "アローハ%s", plname);
	else
	    Sprintf(helo_buf, "アローハ");
	break;
#endif
    case PM_VALKYRIE:
	if(nameflg)
	    Sprintf(helo_buf, "魂の守護者%sよ", plname);
	else
	    Sprintf(helo_buf, "魂の守護者");
	break;
    default:
	if(nameflg)
	    Sprintf(helo_buf, "ようこそ%s", plname);
	else
	    Sprintf(helo_buf, "ようこそ");
	break;
    }

    return helo_buf;
}

const char *
Goodbye(int nameflg)
{
    static char helo_buf[BUFSZ];

    switch (Role_switch) {
    case PM_KNIGHT:
	if(nameflg)
	    Sprintf(helo_buf, "さらば敬虔なる騎士の%sよ", plname);
	else
	    Sprintf(helo_buf, "さらば敬虔なる");
	break;
    case PM_MONK:
	if(nameflg)
	    Sprintf(helo_buf, "さらば悟りを開かんとする%sよ", plname);
	else
	    Sprintf(helo_buf, "さらば悟りを開かんとする");
	break;
    case PM_SAMURAI:
	if(nameflg)
	    Sprintf(helo_buf, "さらば武士道を志す%sよ", plname);
	else
	    Sprintf(helo_buf, "さらば武士道を志す");
	break;
#ifdef TOURIST
    case PM_TOURIST:
	if(nameflg)
	    Sprintf(helo_buf, "アローハ%s", plname);
	else
	    Sprintf(helo_buf, "アローハ");
	break;
#endif
    case PM_VALKYRIE:
	if(nameflg)
	    Sprintf(helo_buf, "さらば魂の守護者%sよ", plname);
	else
	    Sprintf(helo_buf, "さらば魂の守護者");
	break;
    default:
	if(nameflg)
	    Sprintf(helo_buf, "さようなら%s", plname);
	else
	    Sprintf(helo_buf, "さようなら");
	break;
    }

    return helo_buf;
}
#endif /*JP*/

/* A function to break a specific roleplay-conduct */
void
violated(cdt)
int cdt;
{
	switch(cdt) {		
	case CONDUCT_PACIFISM:
	    u.uconduct.killer++;
	    if (u.roleplay.pacifist) {
/*JP
		You_feel("violent!");
*/
		You_feel("暴力的だと感じた！");
/*JP
		if (yn("Do you want to quit?") == 'y') {
*/
		if (yn("ゲームをやめる？") == 'y') {
		    killer_format = NO_KILLER_PREFIX;
/*JP
		    killer = "quit after an act of violence";
*/
		    killer = "暴力的行為のあと抜けた";
		    done(QUIT);
		}
	    if (u.uconduct.killer >= 10) u.roleplay.pacifist = FALSE;
	    }
	    break;

	case CONDUCT_NUDISM:
	    u.uconduct.armoruses++;
	    if (u.roleplay.nudist){
/*JP
		You("realize you were nude.");
*/
		You("裸だったことに気づいた。");
		makemon(&mons[PM_COBRA],u.ux, u.uy, NO_MM_FLAGS);
		mksobj_at(APPLE, u.ux, u.uy, FALSE, FALSE);
		u.roleplay.nudist = FALSE;
	    }
	    break;

	case CONDUCT_BLINDFOLDED:
	    u.uconduct.unblinded++;
	    if (u.roleplay.blindfolded){
/*JP
		pline("The Spirit of Zen leaves your body.");
*/
		pline("禅の精神はあなたの身体から去った。");
		makemon(mkclass(S_ZOMBIE, 0), u.ux, u.uy, NO_MM_FLAGS); /* Z */
		makemon(mkclass(S_EYE, 0), u.ux, u.uy, NO_MM_FLAGS);    /* e */
		makemon(mkclass(S_NYMPH, 0), u.ux, u.uy, NO_MM_FLAGS);  /* n */
		u.roleplay.blindfolded = FALSE;
	    }
	    break;

	case CONDUCT_VEGETARIAN:	/* replaces violated_vegetarian() */
	    if (u.roleplay.vegetarian)
/*JP
		You_feel("guilty.");
*/
		You_feel("罪を感じた。");
	    if (Role_if(PM_MONK))
		adjalign(-1);
	    u.uconduct.unvegetarian++;
	    u.uconduct.unvegan++;
	    u.uconduct.food++;
	    if (u.uconduct.unvegetarian >= 30) u.roleplay.vegetarian = FALSE;
	    if (u.uconduct.unvegan >= 20) u.roleplay.vegan = FALSE;
	    if (u.uconduct.food >= 10) u.roleplay.ascet = FALSE;
	    break;

	case CONDUCT_VEGAN:
	    if (u.roleplay.vegan)
/*JP
		You_feel("a bit guilty.");
*/
		You_feel("ちょっと罪を感じた。");
	    u.uconduct.unvegan++;
	    u.uconduct.food++;
	    if (u.uconduct.unvegan >= 20) u.roleplay.vegan = FALSE;
	    if (u.uconduct.food >= 10) u.roleplay.ascet = FALSE;
	    break;

	case CONDUCT_FOODLESS:
	    if (u.roleplay.ascet)
/*JP
		You_feel("a little bit guilty.");
*/
		You_feel("ほんのわずかな罪を感じた。");
	    u.uconduct.food++;
	    if (u.uconduct.food >= 10) u.roleplay.ascet = FALSE;
	    break;

	case CONDUCT_ILLITERACY:
	    u.uconduct.literate++;
	    if (u.roleplay.illiterate) {
		/* should be impossible */
/*JP
		pline("Literatally literature for literate illiterates!");
*/
		pline("学識ある文盲者のための洗練された文学！");
		exercise(A_WIS, TRUE);		
		}
	    break;

	case CONDUCT_THIEVERY:
	    u.uconduct.robbed++;
	    if (Role_if(PM_ROGUE))
/*JP
		You_feel("like an ordinary robber."); 
*/
		You_feel("並以下の強盗になってしまったような気がした。"); 
	    break;

	default: 
	    impossible("violated: unknown conduct");

	}
	return;
}

/* a function to check whether a specific conduct has been broken
 * return FALSE if broken
*/
boolean
successful_cdt(cdt)
int cdt;
{
	if ((cdt == CONDUCT_PACIFISM) && !u.uconduct.killer &&
			!num_genocides() && (u.uconduct.weaphit<=100)) 
	    return TRUE;
	if ((cdt == CONDUCT_SADISM) && !u.uconduct.killer &&
		 	(num_genocides() || (u.uconduct.weaphit>100)))
	    return TRUE;
	if ((cdt == CONDUCT_ATHEISM) && !u.uconduct.gnostic) return TRUE;
	if ((cdt == CONDUCT_NUDISM) && !u.uconduct.armoruses) return TRUE;
	if ((cdt == CONDUCT_BLINDFOLDED) && !u.uconduct.unblinded) return TRUE;
	if ((cdt == CONDUCT_VEGETARIAN) && !u.uconduct.unvegetarian) return TRUE;
	if ((cdt == CONDUCT_VEGAN) && !u.uconduct.unvegan) return TRUE;
	if ((cdt == CONDUCT_FOODLESS) && !u.uconduct.food) return TRUE;
	if ((cdt == CONDUCT_ILLITERACY) && !u.uconduct.literate) return TRUE;
	if ((cdt == CONDUCT_THIEVERY) && !u.uconduct.robbed) return TRUE;

	return FALSE;
}

/* a function to check whether a specific conduct *
 *  was selected at character creation		  */
boolean
intended_cdt(cdt)
int cdt;
{
	if ((cdt == CONDUCT_PACIFISM) && flags.pacifist) return TRUE;
	if ((cdt == CONDUCT_ATHEISM) && flags.atheist) return TRUE;
	if ((cdt == CONDUCT_NUDISM) && flags.nudist) return TRUE;
	if ((cdt == CONDUCT_BLINDFOLDED) && flags.blindfolded) return TRUE;
	if ((cdt == CONDUCT_FOODLESS) && flags.ascet) return TRUE;
	if ((cdt == CONDUCT_VEGAN) && flags.vegan) return TRUE;
	if ((cdt == CONDUCT_VEGETARIAN) && flags.vegetarian) return TRUE;
	if ((cdt == CONDUCT_ILLITERACY) && flags.illiterate) return TRUE;

	return FALSE;
}

/* a function to check whether it's superflous to list that conduct */ 
boolean
superfluous_cdt(cdt)
int cdt;
{
	if ((cdt == CONDUCT_VEGAN) && successful_cdt(CONDUCT_FOODLESS)) return TRUE;
	if ((cdt == CONDUCT_VEGETARIAN) && successful_cdt(CONDUCT_VEGAN)) return TRUE;
	if ((cdt == CONDUCT_THIEVERY) && !u.uevent.invoked) return TRUE;

	return FALSE;
}

/* tell if you failed a selected conduct */ 
boolean
failed_cdt(cdt)
int cdt;
{
	return (intended_cdt(cdt) && !successful_cdt(cdt));
}

/* role.c */
