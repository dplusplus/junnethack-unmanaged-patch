/*	SCCS Id: @(#)sp_lev.h	3.4	1996/05/08	*/
/* Copyright (c) 1989 by Jean-Christophe Collet			  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SP_LEV_H
#define SP_LEV_H

    /* wall directions */
#define W_NORTH		1
#define W_SOUTH		2
#define W_EAST		4
#define W_WEST		8
#define W_ANY		(W_NORTH|W_SOUTH|W_EAST|W_WEST)

    /* MAP limits */
#define MAP_X_LIM	76
#define MAP_Y_LIM	21

    /* Per level flags */
#define NOTELEPORT	0x00000001L
#define HARDFLOOR	0x00000002L
#define NOMMAP		0x00000004L
#define SHORTSIGHTED	0x00000008L
#define ARBOREAL	0x00000010L
#define NOFLIPX		0x00000020L
#define NOFLIPY		0x00000040L
#define MAZELEVEL	0x00000080L
#define PREMAPPED	0x00000100L
#define SHROUD		0x00000200L
#define STORMY		0x00000400L
#define GRAVEYARD	0x00000800L

/* different level layout initializers */
#define LVLINIT_NONE		0
#define LVLINIT_SOLIDFILL	1
#define LVLINIT_MAZEGRID	2
#define LVLINIT_MINES		3

/* max. layers of object containment */
#define MAX_CONTAINMENT 10

/* max. # of random registers */
#define MAX_REGISTERS	10

/* max. nested depth of subrooms */
#define MAX_NESTED_ROOMS 5

/* max. # of opcodes per special level */
#define SPCODER_MAX_RUNTIME	65536

/* Opcodes for creating the level
 * If you change these, also change opcodestr[] in util/lev_main.c
 */
enum opcode_defs {
    SPO_NULL = 0,
    SPO_MESSAGE,
    SPO_MONSTER,
    SPO_OBJECT,
    SPO_ENGRAVING,
    SPO_ROOM,
    SPO_SUBROOM,
    SPO_DOOR,
    SPO_STAIR,
    SPO_LADDER,
    SPO_ALTAR,
    SPO_FOUNTAIN,
    SPO_SINK,
    SPO_POOL,
    SPO_TRAP,
    SPO_GOLD,
    SPO_CORRIDOR,
    SPO_LEVREGION,
    SPO_DRAWBRIDGE,
    SPO_MAZEWALK,
    SPO_NON_DIGGABLE,
    SPO_NON_PASSWALL,
    SPO_WALLIFY,
    SPO_MAP,
    SPO_ROOM_DOOR,
    SPO_REGION,
    SPO_CMP,
    SPO_JMP,
    SPO_JL,
    SPO_JLE,
    SPO_JG,
    SPO_JGE,
    SPO_JE,
    SPO_JNE,
    SPO_SPILL,
    SPO_TERRAIN,
    SPO_REPLACETERRAIN,
    SPO_EXIT,
    SPO_ENDROOM,
    SPO_POP_CONTAINER,
    SPO_PUSH,
    SPO_POP,
    SPO_RN2,
    SPO_DEC,
    SPO_INC,
    SPO_MATH_ADD,
    SPO_MATH_SUB,
    SPO_MATH_MUL,
    SPO_MATH_DIV,
    SPO_MATH_MOD,
    SPO_COPY,
    SPO_MON_GENERATION,
    SPO_END_MONINVENT,
    SPO_GRAVE,
    SPO_FRAME_PUSH,
    SPO_FRAME_POP,
    SPO_CALL,
    SPO_RETURN,
    SPO_INITLEVEL,
    SPO_LEVEL_FLAGS,
    SPO_LEVEL_SOUNDS,
    SPO_WALLWALK,
    SPO_VAR_INIT, /* variable_name data */
    SPO_SHUFFLE_ARRAY,
    SPO_DICE,

    SPO_SEL_ADD,
    SPO_SEL_POINT,
    SPO_SEL_RECT,
    SPO_SEL_FILLRECT,
    SPO_SEL_LINE,
    SPO_SEL_RNDLINE,
    SPO_SEL_GROW,
    SPO_SEL_FLOOD,
    SPO_SEL_RNDCOORD,
    SPO_SEL_ELLIPSE,
    SPO_SEL_FILTER,

    MAX_SP_OPCODES
};

/* MONSTER and OBJECT can take a variable number of parameters,
 * they also pop different # of values from the stack. So,
 * first we pop a value that tells what the _next_ value will
 * mean.
 */
/* MONSTER */
#define SP_M_V_PEACEFUL         0
#define SP_M_V_ALIGN            1
#define SP_M_V_ASLEEP           2
#define SP_M_V_APPEAR           3
#define SP_M_V_NAME             4

#define SP_M_V_FEMALE		5
#define SP_M_V_INVIS		6
#define SP_M_V_CANCELLED	7
#define SP_M_V_REVIVED		8
#define SP_M_V_AVENGE		9
#define SP_M_V_FLEEING		10
#define SP_M_V_BLINDED		11
#define SP_M_V_PARALYZED	12
#define SP_M_V_STUNNED		13
#define SP_M_V_CONFUSED		14
#define SP_M_V_SEENTRAPS	15

#define SP_M_V_END              16 /* end of variable parameters */

/* OBJECT */
#define SP_O_V_SPE              0
#define SP_O_V_CURSE            1
#define SP_O_V_CORPSENM         2
#define SP_O_V_NAME             3
#define SP_O_V_QUAN		4
#define SP_O_V_BURIED		5
#define SP_O_V_LIT		6
#define SP_O_V_ERODED		7
#define SP_O_V_LOCKED		8
#define SP_O_V_TRAPPED		9
#define SP_O_V_RECHARGED	10
#define SP_O_V_INVIS		11
#define SP_O_V_GREASED		12
#define SP_O_V_BROKEN		13
#define SP_O_V_COORD		14
#define SP_O_V_END              15 /* end of variable parameters */


/* When creating objects, we need to know whether
 * it's a container and/or contents.
 */
#define SP_OBJ_CONTENT		0x1
#define SP_OBJ_CONTAINER	0x2



#define SPOVAR_NULL	0x00
#define SPOVAR_INT	0x01 /* l */
#define SPOVAR_STRING	0x02 /* str */
#define SPOVAR_VARIABLE	0x03 /* str (contains the variable name) */
#define SPOVAR_COORD	0x04 /* coordinate, encoded in l; use SP_COORD_X() and SP_COORD_Y() */
#define SPOVAR_REGION	0x05 /* region, encoded in l; use SP_REGION_X1() etc */
#define SPOVAR_MAPCHAR	0x06 /* map char, in l */
#define SPOVAR_MONST	0x07 /* monster class & specific monster, encoded in l; use SP_MONST_... */
#define SPOVAR_OBJ	0x08 /* object class & specific object type, encoded in l; use SP_OBJ_... */
#define SPOVAR_SEL	0x09 /* selection. char[COLNO][ROWNO] in str */
#define SPOVAR_ARRAY	0x40 /* used in splev_var & lc_vardefs, not in opvar */

#define SP_COORD_X(l)	(l & 0xff)
#define SP_COORD_Y(l)	((l >> 16) & 0xff)
#define SP_COORD_PACK(x,y) ((( x ) & 0xff) + ((( y ) & 0xff) << 16))

#define SP_REGION_X1(l)	(l & 0xff)
#define SP_REGION_Y1(l)	((l >> 8) & 0xff)
#define SP_REGION_X2(l)	((l >> 16) & 0xff)
#define SP_REGION_Y2(l)	((l >> 24) & 0xff)
#define SP_REGION_PACK(x1,y1,x2,y2) ((( x1 ) & 0xff) + ((( y1 ) & 0xff) << 8) + ((( x2 ) & 0xff) << 16) + ((( y2 ) & 0xff) << 24))

#define SP_MONST_CLASS(l) (l & 0xff)
#define SP_MONST_PM(l)	  ((l >> 8) & 0xffff)
#define SP_MONST_PACK(m,c) ((( m ) << 8) + ((char)( c )))

#define SP_OBJ_CLASS(l)	  (l & 0xff)
#define SP_OBJ_TYP(l)	  ((l >> 8) & 0xffff)
#define SP_OBJ_PACK(o,c)  ((( o ) << 8) + ((char)( c )))

#define SP_MAPCHAR_TYP(l) (l & 0xff)
#define SP_MAPCHAR_LIT(l) ((l >> 8) & 0xff)
#define SP_MAPCHAR_PACK(typ,lit) ((( lit ) << 8) + ((char)( typ )))

struct opvar {
    xchar spovartyp; /* one of SPOVAR_foo */
    union {
	char *str;
	long l;
    } vardata;
};

struct splev_var {
    struct splev_var *next;
    char *name;
    xchar svtyp; /* SPOVAR_foo */
    union {
	struct opvar *value;
	struct opvar **arrayvalues;
    } data;
    long array_len;
};

struct splevstack {
    long depth;
    long depth_alloc;
    struct opvar **stackdata;
};


struct sp_frame {
    struct sp_frame *next;
    struct splevstack *stack;
    struct splev_var *variables;
    long n_opcode;
};


struct sp_coder {
    struct splevstack *stack;
    struct sp_frame *frame;
    int allow_flips;
    int premapped;
    struct mkroom *croom;
    struct mkroom *tmproomlist[MAX_NESTED_ROOMS+1];
    boolean failed_room[MAX_NESTED_ROOMS+1];
    int n_subroom;
    boolean exit_script;
    int  lvl_is_joined;

    int opcode;  /* current opcode */
    struct opvar *opdat; /* current push data (req. opcode == SPO_PUSH) */
};

/* special level coder CPU flags */
#define SP_CPUFLAG_LT	1
#define SP_CPUFLAG_GT	2
#define SP_CPUFLAG_EQ	4
#define SP_CPUFLAG_ZERO	8

/*
 * Structures manipulated by the special levels loader & compiler
 */

typedef struct {
    xchar x1,y1,x2,y2;
    xchar fg, lit;
    int roughness;
    xchar thick;
} randline;

typedef struct {
	int cmp_what;
	int cmp_val;
} opcmp;

typedef struct {
	long jmp_target;
} opjmp;

typedef union str_or_len {
	char *str;
	int   len;
} Str_or_Len;

typedef struct {
	xchar   init_style; /* one of LVLINIT_foo */
	char	fg, bg;
	boolean smoothed, joined;
	xchar	lit, walled;
	long	flags;
	schar	filling;
} lev_init;

typedef struct {
	xchar x, y, mask;
} door;

typedef struct {
	xchar wall, pos, secret, mask;
} room_door;

typedef struct {
	xchar x, y, type;
} trap;

typedef struct {
	Str_or_Len name, appear_as;
	short id;
	aligntyp align;
	xchar x, y, class, appear;
	schar peaceful, asleep;
        short female, invis, cancelled, revived, avenge, fleeing, blinded, paralyzed, stunned, confused;
        long seentraps;
	short has_invent;
} monster;

typedef struct {
	Str_or_Len name;
	int   corpsenm;
	short id, spe;
	xchar x, y, class, containment;
	schar curse_state;
	int   quan;
	short buried;
	short lit;
        short eroded, locked, trapped, recharged, invis, greased, broken;
} object;

typedef struct {
	xchar		x, y;
	aligntyp	align;
	xchar		shrine;
} altar;

typedef struct {
	xchar x, y, dir, db_open;
} drawbridge;

typedef struct {
	xchar x, y, dir, stocked, typ;
} walk;

typedef struct {
	xchar x1, y1, x2, y2;
} digpos;

typedef struct {
	xchar x, y, up;
} lad;

typedef struct {
	xchar x, y, up;
} stair;

typedef struct {
	xchar x1, y1, x2, y2;
	xchar rtype, rlit, rirreg;
} region;

typedef struct {
    xchar areatyp;
    xchar x1,y1,x2,y2;
    xchar ter, tlit;
} terrain;

typedef struct {
    xchar chance;
    xchar x1,y1,x2,y2;
    xchar fromter, toter, tolit;
} replaceterrain;

/* values for rtype are defined in dungeon.h */
typedef struct {
	struct { xchar x1, y1, x2, y2; } inarea;
	struct { xchar x1, y1, x2, y2; } delarea;
	boolean in_islev, del_islev;
	xchar rtype, padding;
	Str_or_Len rname;
} lev_region;

typedef struct {
	xchar x, y;
	int   amount;
} gold;

typedef struct {
	xchar x, y;
	Str_or_Len engr;
	xchar etype;
} engraving;

typedef struct {
	xchar x, y;
} fountain;

typedef struct {
	xchar x, y;
} sink;

typedef struct {
	xchar x, y;
} pool;

typedef struct _room {
	Str_or_Len name;
	Str_or_Len parent;
	xchar x, y, w, h;
	xchar xalign, yalign;
	xchar rtype, chance, rlit, filled;
} room;

typedef struct {
	schar zaligntyp;
	schar keep_region;
	schar halign, valign;
	char xsize, ysize;
	char **map;
} mazepart;

typedef struct {
	struct {
		xchar room;
		xchar wall;
		xchar door;
	} src, dest;
} corridor;

typedef struct {
    int opcode;
    struct opvar *opdat;
} _opcode;

typedef struct {
    _opcode  *opcodes;
    long     n_opcodes;
} sp_lev;

typedef struct {
	xchar x, y, direction, count, lit;
	char typ;
} spill;


/* only used by lev_comp */

struct lc_funcdefs {
    struct lc_funcdefs *next;
    char *name;
    long addr;
    sp_lev code;
    long n_called;
};

struct lc_vardefs {
    struct lc_vardefs *next;
    char *name;
    long var_type; /* SPOVAR_foo */
};

#endif /* SP_LEV_H */
