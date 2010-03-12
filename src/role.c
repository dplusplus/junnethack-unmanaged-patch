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
  ������̍ŏ��̈ꕶ���ڂ�����R�[�h�������ɑ��݂���̂ŁA
  �p�ꖼ���c���Ă����B (see you.h)
*/

const struct Role roles[] = {
{	{"Archeologist", 0}, {
#if 1 /*JP*/
	 "�l�Êw��", 0}, {
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
	{"�z��",	0},
	{"�J����",      0},
	{"������",      0},
	{"���@��",	0},
	{"�@���",	0},
	{"�T����",	0},
	{"���A�w��",    0},
	{"���p���W��",	0},
	{"�ْ�",	0} },
#endif

	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
#if 0 /*JP*/
	"Arc", "the College of Archeology", "the Tomb of the Toltec Kings",
#else
	"Arc", "�l�Êw��w", "�g���e�J���Ƃ̕�",
#endif
	PM_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL,
	NON_PM, PM_HUMAN_MUMMY, S_SNAKE, S_MUMMY,
	ART_ORB_OF_DETECTION,
	MH_HUMAN|MH_DWARF|MH_GNOME | ROLE_MALE|ROLE_FEMALE |
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
	 "��ؐl", 0}, {
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
	{"����",	"������"},
	{"���D��",	0},
	{"����",	0},
	{"�R��",	0},
	{"�N����",	0},
	{"����",	0},
	{"�E�C��",	0},
	{"���",	"�����"},
        {"������",	0} },
#endif
	"Mitra", "Crom", "Set", /* Hyborian */
#if 0 /*JP*/
	"Bar", "the Camp of the Duali Tribe", "the Duali Oasis",
#else
	"Bar", "�f���A�����̃L�����v", "�f���A�����̃I�A�V�X",
#endif
	PM_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
	ART_HEART_OF_AHRIMAN,
	MH_HUMAN|MH_ORC | ROLE_MALE|ROLE_FEMALE |
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
	 "���A�l", 0}, {
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
	{"�����l",	0},
	{"���Z��",	0},
	{"���Q��",	0},
	{"���Q��",	0},
	{"���s��",	0},
	{"���V��",	0},
	{"�V�q��",	0},
	{"���Q��",	0},
	{"����",	0} },
#endif
	"Anu", "_Ishtar", "Anshar", /* Babylonian */
#if 0 /*JP*/
	"Cav", "the Caves of the Ancestors", "the Dragon's Lair",
#else
	"Cav", "���Â̓��A", "���̉B���",
#endif
	PM_CAVEMAN, PM_CAVEWOMAN, NON_PM,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON,
	PM_BUGBEAR, PM_HILL_GIANT, S_HUMANOID, S_GIANT,
	ART_SCEPTRE_OF_MIGHT,
	MH_HUMAN|MH_DWARF|MH_GNOME | ROLE_MALE|ROLE_FEMALE |
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
	 "��t", 0}, {
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
	{"���K��",      0},
	{"��t���K��",	0},
	{"�Ō�t",	"�Ō�w"},
	{"��t����",	0},
	{"�򕨎�C",	0},
	{"��t��C",	"�Ō��C"},
	{"������",	0},
	{"���Ȉ�",	0},
	{"�O�Ȉ�",	0} },
#endif
	"_Athena", "Hermes", "Poseidon", /* Greek */
#if 0 /*JP*/
	"Hea", "the Temple of Epidaurus", "the Temple of Coeus",
#else
	"Hea", "�G�s�_�E���X���@", "�R�C�I�X���@",
#endif
	PM_HEALER, NON_PM, NON_PM,
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS,
	PM_GIANT_RAT, PM_SNAKE, S_RODENT, S_YETI,
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
	 "�R�m", 0}, {
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
	{"���K��",	0},
	{"����",	0},
	{"��m",	"����m"},
	{"�R��",	0},
	{"�d��m",	0},
	{"�R�m",	0},
	{"�d�R�m",	0},
	{"�M�R�m",	0},
	{"���R�m",	0} },
#endif
	"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
#if 0 /*JP*/
	"Kni", "Camelot Castle", "the Isle of Glass",
#else
	"Kni", "�L�������b�g��", "�K���X�̓�",
#endif
	PM_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_IXOTH,
	PM_QUASIT, PM_OCHRE_JELLY, S_IMP, S_JELLY,
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
	 "������", 0}, {
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
	{"�����]��",	0},
	{"���S��",	0},
	{"����ғ`",	0},
	{"�y�̏K����",	0},
	{"���̏K����",	0},
	{"���̏K����",	0},
	{"�؂̏K����",	0},
	{"�΂̏K����",	0},
	{"�Ƌ��F�`",	0} },
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
#if 0 /*JP*/
	"Mon", "the Monastery of Chan-Sune",
	  "the Monastery of the Earth-Lord",
#else
	"Mon", "�`�����E�X�[�C���@",
	  "�n���̏C���@",
#endif
	PM_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN,
	PM_EARTH_ELEMENTAL, PM_XORN, S_ELEMENTAL, S_XORN,
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
	 "�m��", "��m"}, {
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
	{"�C����",	"�C����"},
	{"����",	0},
	{"����",	0},
	{"�m��",	"��m"},
	{"���C�i��",	0},
	{"����",	"����"},
	{"�i��",	0},
	{"��i��",	0},
	{"��m��",      0} },
#endif
	0, 0, 0,	/* chosen randomly from among the other roles */
#if 0 /*JP*/
	"Pri", "the Great Temple", "the Temple of Nalzok",
#else
	"Pri", "�̑�Ȃ鎛�@", "�i���]�N���@",
#endif
	PM_PRIEST, PM_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_NALZOK,
	PM_HUMAN_ZOMBIE, PM_WRAITH, S_ZOMBIE, S_WRAITH,
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
	 "����", 0}, {
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
	{"�ǂ��͂�",	0},
	{"�Ђ�������",	0},
	{"�X��",	0},
	{"�����",	0},
	{"�����ǂ�",	0},
	{"��",	0},
	{"�D�_",	"���D�_"},
	{"����",	0},
	{"��D�_",	0} },
#endif
	"Issek", "Mog", "Kos", /* Nehwon */
#if 0 /*JP*/
	"Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall",
#else
	"Rog", "�����M���h", "�ÎE�҃M���h",
#endif
	PM_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN,
	PM_LEPRECHAUN, PM_GUARDIAN_NAGA, S_NYMPH, S_NAGA,
	ART_MASTER_KEY_OF_THIEVERY,
	MH_HUMAN|MH_ORC | ROLE_MALE|ROLE_FEMALE |
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
	 "�����W���[", 0}, {
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
	{"�V��",	0},
	{"������",	0},
	{"�擱",	0},
	{"��@",	0},
	{"�ˌ�",	0},
	{"�|��",	0},	/* One skilled at crossbows */
	{"�����|��",	0},
	{"�㋉�|��",	0},
	{"���l",	0} },
#endif
	"Mercury", "_Venus", "Mars", /* Roman/planets */
#if 0 /*JP*/
	"Ran", "Orion's camp", "the cave of the wumpus",
#else
	"Ran", "�I���I���̃L�����v", "�����p�X�̓��A",
#endif
	PM_RANGER, NON_PM, NON_PM /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,
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
	 "��", 0}, {
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
	{"���{",	0},  /* Banner Knight */
	{"�Q�l",	0},  /* no allegiance */
	{"�E��",	"���m��"},  /* secret society */
	{"���",	0},  /* heads a castle */
	{"�̎�",	0},  /* has a territory */
	{"����",	0},  /* heads a province */
	{"�喼",	"����"},  /* a samurai lord */
	{"����",	0},  /* Noble of the Court */
	{"���R",	"�剜"} },  /* supreme commander, warlord */
#endif
	"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
#if 0 /*JP*/
	"Sam", "the Castle of the Taro Clan", "the Shogun's Castle",
#else
	"Sam", "���Y�ꑰ�̏�", "���R�̏�",
#endif
	PM_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI,
	PM_WOLF, PM_STALKER, S_DOG, S_ELEMENTAL,
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
	 "�ό��q", 0}, {
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
	{"�v�[���Y",	"�v�[�q"},
	{"�ό��q",	0},
	{"���V���s��",  0},
	{"�՗���",      0},
	{"���s��",	0},
	{"���l",	0},
	{"�q�C��",	0},
	{"�T����",	0},
	{"�`����",	0} },
#endif
	"Blind Io", "_The Lady", "Offler", /* Discworld */
#if 0 /*JP*/
	"Tou", "Ankh-Morpork", "the Thieves' Guild Hall",
#else
	"Tou", "�A���N�����|�[�N", "�����M���h",
#endif
	PM_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES,
	PM_GIANT_SPIDER, PM_FOREST_CENTAUR, S_SPIDER, S_CENTAUR,
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
	 "�����L���[��", 0}, {
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
	{"���K��",	0},
	{"����",	0},
	{"��m",	"����m"},
	{"�R��",      "���d�R��"},
	{"�퓬��",	0},
	{"�U����",      0},
	{"�p�Y",	0},
	{"���m",	"�����m"},
	{"���[�h",	"���f�B"} },
#endif
	"Tyr", "Odin", "Loki", /* Norse */
#if 0 /*JP*/
	"Val", "the Shrine of Destiny", "the cave of Surtur",
#else
	"Val", "�^���̐���", "�X���g�̓��A",
#endif
	PM_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_WARRIOR, PM_LORD_SURTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
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
	 "���@�g��", 0}, {
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
	{"��i�t",	0},
	{"��p�t",	0},
	{"�肢�t",	0},
	{"�슴�t",	0},
	{"�����t",	0},
	{"�d�p�t",      0},
	{"���p�t",      0},
	{"���@�g��",	"����"},
	{"�喂�@�g��",	0} },
#endif
	"Ptah", "Thoth", "Anhur", /* Egyptian */
#if 0 /*JP*/
	"Wiz", "the Lonely Tower", "the Tower of Darkness",
#else
	"Wiz", "���a�̓�", "�Í��̓�",
#endif
	PM_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE,
	PM_VAMPIRE_BAT, PM_XORN, S_BAT, S_WRAITH,
	ART_EYE_OF_THE_AETHIOPICA,
	MH_HUMAN|MH_ELF|MH_GNOME|MH_ORC | ROLE_MALE|ROLE_FEMALE |
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
{	{"Undefined", 0}, {"��", 0}, { {0, 0}, {0, 0}, {0, 0},
#endif
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 0, 0,
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
  ������̍ŏ��̈ꕶ���ڂ�����R�[�h�������ɑ��݂���̂ŁA
  �p�ꖼ���c���Ă����B (see you.h)
*/
#if 0 /*JP*/
{	"human", "human", "humanity", "Hum",
	{"man", "woman"},
#else
{	"human", "�l��", "human", "humanity", "Hum",
	{"�j", "��"},
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
{	"elf", "�G���t", "elven", "elvenkind", "Elf",
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
{	"dwarf", "�h���[�t", "dwarven", "dwarvenkind", "Dwa",
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
{	"gnome", "�m�[��", "gnomish", "gnomehood", "Gno",
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
{	"orc", "�I�[�N", "orcish", "orcdom", "Orc",
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
/* Array terminator */
{ 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
/*JP
{	"something", "undefined", "something", "Xxx",
*/
{	"something", "��", "undefined", "something", "Xxx",
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
/*JP: �I�v�V�����Ŏg���Ă���̂ŉp�ꖼ���c���Ă����B (see you.h) */
#if 0 /*JP*/
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
#else
	{"male",	"�j��",	"��",	"��",	"�ނ�",	"Mal",	ROLE_MALE},
	{"female",	"����",	"�ޏ�",	"�ޏ�",	"�ޏ���","Fem",	ROLE_FEMALE},
	{"neuter",	"����",	"����",	"����",	"����",	"Ntr",	ROLE_NEUTER}
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
	{"law",		"����",	"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"����",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"����",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"���S",	"unaligned",	"Una",	0,		A_NONE}
#endif
};


/* Table of roleplay-conducts */

const struct Conduct conducts[] = {
#if 0 /*JP*/
{	"pacifism",	"pacifist", 	"peaceful",	TRUE,
	"You ","have been ","were ","a pacifist",
	"pretended to be a pacifist"},
#else
{	"���a��`",	"���a��`��", 	"���a�I��",	TRUE,
	"���Ȃ���","�ł���","������","���a��`��",
	"���a��`�҂ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"sadism",	"sadist", 	"sadistic",	TRUE,
	"You ","have been ","were ","a sadist",
	"pretended to be a sadist"},
#else
{	"���s�",	"�T�f�B�X�g", 	"���s�I��",	TRUE,
	"���Ȃ���","�ł���","������","�T�f�B�X�g",
	"�T�f�B�X�g�ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"atheism", 	"atheist", 	"atheistic",	TRUE,
	"You ","have been ","were ","an atheist",
	"pretended to be an atheist"},
#else
{	"���_�_", 	"���_�_��", 	"���M�S��",	TRUE,
	"���Ȃ���","�ł���","������","���_�_��",
	"���_�_�҂ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"nudism", 	"nudist", 	"nude",		TRUE,
	"You ","have been ","were ","a nudist",
	"pretended to be a nudist"},
#else
{	"���̎�`", 	"�k�[�f�B�X�g", 	"����",		TRUE,
	"���Ȃ���","�ł���","������","�k�[�f�B�X�g",
	"�k�[�f�B�X�g�ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"zen",		"zen master", 	"blindfolded",	TRUE,
	"You ","have followed ","followed ","the true Path of Zen",
	"left the true Path of Zen"},
#else
{	"�T",		"�T�}�X�^�[", 	"�Ӗڂ�",	TRUE,
	"���Ȃ���","���������Ă���","���������Ă���","�T�̓�",
	"�T�̓�����O���"},
#endif /*JP*/

#if 0 /*JP*/
{	"asceticism",	"ascetic",	"hungry",	TRUE,
	"You ","have gone ","went ","without food",
	"pretended to be an ascet"},
#else
{	"��s",	"��s��",	"�󕠂�",	TRUE,
	"���Ȃ���","�Ă��Ȃ�","�Ȃ�����","�H������",
	"��s�ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"vegan",	"vegan",	"vegan",	TRUE,
	"You ","have followed ","followed ","a strict vegan diet",
	"pretended to be a vegan"},
#else
{	"���i�ȍؐH��`",	"���i�ȍؐH��`��",	"���i�ȍؐH��`��",	TRUE,
	"���Ȃ���","�ł���","������","���i�ȍؐH��`��",
	"���i�ȍؐH��`�ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"vegetarian", 	"vegetarian",	"vegetarian",	TRUE,
	"You ","have been ","were ","vegetarian",
	"pretended to be a vegetarian"},
#else
{	"�ؐH��`", 	"�ؐH��`��",	"�ؐH��`��",	TRUE,
	"���Ȃ���","�ł���","������","�ؐH��`��",
	"�ؐH��`�ɒ��킵�ēڍ�����"},
#endif /*JP*/

#if 0 /*JP*/
{	"illiteracy", 	"illiterate",	"illiterate",	TRUE,
	"You ","have been ","were ","illiterate",
	"become literate"},
#else
{	"����", 	"���ӎ�",	"���ӂ�",	TRUE,
	"���Ȃ���","�Ă��Ȃ�","�Ȃ�����","�ǂݏ�����",
	"�ǂݏ������n�߂�"},
#endif /*JP*/

#if 0 /*JP*/
{	"thievery",	"master thief",	"tricky",	TRUE,
	"You ","have been ","were ","very tricky",
	"pretended to be a master thief"}
#else
{	"����",	"������",	"����",	TRUE,
	"���Ȃ���","�ł���","������","�ƂĂ�����",
	"�������ɒ��킵�ēڍ�����"}
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
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_RACEMASK));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK)
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK) {
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
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK)
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {
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
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_RACEMASK))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_RACEMASK))
		continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
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
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_RACEMASK))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_RACEMASK))
		continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
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
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_GENDMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_GENDMASK))
		continue;
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
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_ALIGNMASK))
		continue;
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
		Strcat(buf, "�A");
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
	static char err_ret[] = "�L�����N�^��";
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
			if (donefirst) Strcat(buf, "��");
			Strcat(buf, aligns[alignnum].j);
#endif
			donefirst = TRUE;
		} else {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
#else
			if (donefirst) Strcat(buf, "��");
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
				if (donefirst) Strcat(buf, "��");
				Strcat(buf, genders[gendnum].j);
#endif
				donefirst = TRUE;
			}
	        } else {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
	        	Strcat(buf, genders[gendnum].adj);
#else
			if (donefirst) Strcat(buf, "��");
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
			if (donefirst) Strcat(buf, "��"); 
			Strcat(buf, races[racenum].j);
#endif
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
#if 0 /*JP*/
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, races[racenum].noun);
#else
			if (donefirst) Strcat(buf, "��"); 
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
		if (donefirst) Strcat(buf, "��");
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
		if (donefirst) Strcat(buf, "��");
		Strcat(buf, "�L�����N�^");
#endif
		donefirst = TRUE;
	}
#if 1 /*JP*/
	Strcat(buf, "��");
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
	const char *defprompt = "�K���ɃL�����N�^��I��ł悢�ł����H[yntq] ";
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
			Strcat(buf, "�푰");
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "role");
*/
			Strcat(buf, "�E��");
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "gender");
*/
			Strcat(buf, "����");
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
/*JP
			Strcat(buf, "alignment");
*/
			Strcat(buf, "����");
		}
	}
/*JP
	Strcat(buf, " for you? [yntq] ");
*/
	Strcat(buf, "��K���ɑI��ł�낵���ł����H[yntq] ");
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
  �������͓��{��Ƃ��Ď��R�ɂȂ�悤�傫���d�l��ύX
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
	    Sprintf(helo_buf, "�悭���Q����%s��", plname);
	else
	    Sprintf(helo_buf, "�悭���Q����");
	break;
    case PM_MONK:
	if(nameflg)
	    Sprintf(helo_buf, "�i�}�X�e�[%s", plname);
	else
	    Sprintf(helo_buf, "�i�}�X�e�[");
	break;
    case PM_SAMURAI:
	if(nameflg)
	    Sprintf(helo_buf, "�悭���Q��ꂽ%s��", plname);
	else
	    Sprintf(helo_buf, "�悭���Q��ꂽ");
	break;
#ifdef TOURIST
    case PM_TOURIST:
	if(nameflg)
	    Sprintf(helo_buf, "�A���[�n%s", plname);
	else
	    Sprintf(helo_buf, "�A���[�n");
	break;
#endif
    case PM_VALKYRIE:
	if(nameflg)
	    Sprintf(helo_buf, "���̎���%s��", plname);
	else
	    Sprintf(helo_buf, "���̎���");
	break;
    default:
	if(nameflg)
	    Sprintf(helo_buf, "�悤����%s", plname);
	else
	    Sprintf(helo_buf, "�悤����");
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
	    Sprintf(helo_buf, "����Όh�i�Ȃ�R�m��%s��", plname);
	else
	    Sprintf(helo_buf, "����Όh�i�Ȃ�");
	break;
    case PM_MONK:
	if(nameflg)
	    Sprintf(helo_buf, "����Ό����J����Ƃ���%s��", plname);
	else
	    Sprintf(helo_buf, "����Ό����J����Ƃ���");
	break;
    case PM_SAMURAI:
	if(nameflg)
	    Sprintf(helo_buf, "����Ε��m�����u��%s��", plname);
	else
	    Sprintf(helo_buf, "����Ε��m�����u��");
	break;
#ifdef TOURIST
    case PM_TOURIST:
	if(nameflg)
	    Sprintf(helo_buf, "�A���[�n%s", plname);
	else
	    Sprintf(helo_buf, "�A���[�n");
	break;
#endif
    case PM_VALKYRIE:
	if(nameflg)
	    Sprintf(helo_buf, "����΍��̎���%s��", plname);
	else
	    Sprintf(helo_buf, "����΍��̎���");
	break;
    default:
	if(nameflg)
	    Sprintf(helo_buf, "���悤�Ȃ�%s", plname);
	else
	    Sprintf(helo_buf, "���悤�Ȃ�");
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
		You_feel("�\�͓I���Ɗ������I");
/*JP
		if (yn("Do you want to quit?") == 'y') {
*/
		if (yn("�Q�[������߂�H") == 'y') {
		    killer_format = NO_KILLER_PREFIX;
/*JP
		    killer = "quit after an act of violence";
*/
		    killer = "�\�͓I�s�ׂ̂��Ɣ�����";
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
		You("�����������ƂɋC�Â����B");
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
		pline("�T�̐��_�͂��Ȃ��̐g�̂��狎�����B");
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
		You_feel("�߂��������B");
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
		You_feel("������ƍ߂��������B");
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
		You_feel("�ق�̂킸���ȍ߂��������B");
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
		pline("�w�����镶�ӎ҂̂��߂̐������ꂽ���w�I");
		exercise(A_WIS, TRUE);		
		}
	    break;

	case CONDUCT_THIEVERY:
	    u.uconduct.robbed++;
	    if (Role_if(PM_ROGUE))
/*JP
		You_feel("like an ordinary robber."); 
*/
		You_feel("���ȉ��̋����ɂȂ��Ă��܂����悤�ȋC�������B"); 
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
