/*	SCCS Id: @(#)lev_main.c	3.4	2002/03/27	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the main function for the parser
 * and some useful functions needed by yacc
 */
#define SPEC_LEV	/* for MPW */
/* although, why don't we move those special defines here.. and in dgn_main? */

#include <stdarg.h>

#include "hack.h"
#include "date.h"
#include "sp_lev.h"
#ifdef STRICT_REF_DEF
#include "tcap.h"
#endif

#ifdef MAC
# if defined(__SC__) || defined(__MRC__)
#  define MPWTOOL
#  define PREFIX ":dungeon:"	/* place output files here */
#  include <CursorCtl.h>
# else
#  if !defined(__MACH__)
#   define PREFIX ":lib:"	/* place output files here */
#  endif
# endif
#endif

#ifdef WIN_CE
#define PREFIX "\\nethack\\dat\\"
#endif

#ifndef MPWTOOL
# define SpinCursor(x)
#endif

#if defined(AMIGA) && defined(DLB)
# define PREFIX "NH:slib/"
#endif

#ifndef O_WRONLY
#include <fcntl.h>
#endif
#ifndef O_CREAT	/* some older BSD systems do not define O_CREAT in <fcntl.h> */
#include <sys/file.h>
#endif
#ifndef O_BINARY	/* used for micros, no-op for others */
# define O_BINARY 0
#endif

#if defined(MICRO) || defined(WIN32)
# define OMASK FCMASK
#else
# define OMASK 0644
#endif

#define ERR		(-1)

#define NewTab(type, size)	(type **) alloc(sizeof(type *) * size)
#define Free(ptr)		if(ptr) free((genericptr_t) (ptr))
#define Write(fd, item, size)	if (write(fd, (genericptr_t)(item), size) != size) return FALSE;

#if defined(__BORLANDC__) && !defined(_WIN32)
extern unsigned _stklen = STKSIZ;
#endif
#define MAX_ERRORS	25

extern int  NDECL (yyparse);
extern void FDECL (init_yyin, (FILE *));
extern void FDECL (init_yyout, (FILE *));

int  FDECL (main, (int, char **));
void FDECL (yyerror, (const char *));
void FDECL (yywarning, (const char *));
int  NDECL (yywrap);
int FDECL(get_floor_type, (CHAR_P));
int FDECL(get_room_type, (char *));
int FDECL(get_trap_type, (char *));
int FDECL(get_monster_id, (char *,CHAR_P));
int FDECL(get_object_id, (char *,CHAR_P));
boolean FDECL(check_monster_char, (CHAR_P));
boolean FDECL(check_object_char, (CHAR_P));
char FDECL(what_map_char, (CHAR_P));
void FDECL(scan_map, (char *, sp_lev *));
boolean FDECL(check_subrooms, (sp_lev *));
boolean FDECL(write_level_file, (char *,sp_lev *));

struct lc_funcdefs *FDECL(funcdef_new,(long,char *));
void FDECL(funcdef_free_all,(struct lc_funcdefs *));
struct lc_funcdefs *FDECL(funcdef_defined,(struct lc_funcdefs *,char *, int));

struct lc_vardefs *FDECL(vardef_new,(long,char *));
void FDECL(vardef_free_all,(struct lc_vardefs *));
struct lc_vardefs *FDECL(vardef_defined,(struct lc_vardefs *,char *, int));

void FDECL(splev_add_from, (sp_lev *, sp_lev *));

extern void NDECL(monst_init);
extern void NDECL(objects_init);
extern void NDECL(decl_init);

void FDECL(add_opcode, (sp_lev *, int, genericptr_t));

static boolean FDECL(write_common_data, (int,sp_lev *));
static boolean FDECL(write_maze, (int,sp_lev *));
static void NDECL(init_obj_classes);

static struct {
	const char *name;
	int type;
} trap_types[] = {
	{ "arrow",	ARROW_TRAP },
	{ "dart",	DART_TRAP },
	{ "falling rock", ROCKTRAP },
	{ "board",	SQKY_BOARD },
	{ "bear",	BEAR_TRAP },
	{ "land mine",	LANDMINE },
	{ "rolling boulder",	ROLLING_BOULDER_TRAP },
	{ "sleep gas",	SLP_GAS_TRAP },
	{ "rust",	RUST_TRAP },
	{ "fire",	FIRE_TRAP },
	{ "pit",	PIT },
	{ "spiked pit",	SPIKED_PIT },
	{ "hole",	HOLE },
	{ "trap door",	TRAPDOOR },
	{ "teleport",	TELEP_TRAP },
	{ "level teleport", LEVEL_TELEP },
	{ "magic portal",   MAGIC_PORTAL },
	{ "web",	WEB },
	{ "statue",	STATUE_TRAP },
	{ "magic",	MAGIC_TRAP },
	{ "anti magic",	ANTI_MAGIC },
	{ "polymorph",	POLY_TRAP },
	{ 0, 0 }
};

static struct {
	const char *name;
	int type;
} room_types[] = {
	/* for historical reasons, room types are not contiguous numbers */
	/* (type 1 is skipped) */
	{ "ordinary",	 OROOM },
	{ "throne",	 COURT },
	{ "swamp",	 SWAMP },
	{ "vault",	 VAULT },
	{ "beehive",	 BEEHIVE },
	{ "morgue",	 MORGUE },
	{ "barracks",	 BARRACKS },
	{ "zoo",	 ZOO },
	{ "delphi",	 DELPHI },
	{ "temple",	 TEMPLE },
	{ "lemurepit",   LEMUREPIT },
	{ "anthole",	 ANTHOLE },
	{ "cocknest",	 COCKNEST },
	{ "garden",	 GARDEN },
	{ "leprehall",	 LEPREHALL },
	{ "shop",	 SHOPBASE },
	{ "armor shop",	 ARMORSHOP },
	{ "scroll shop", SCROLLSHOP },
	{ "potion shop", POTIONSHOP },
	{ "weapon shop", WEAPONSHOP },
	{ "food shop",	 FOODSHOP },
	{ "ring shop",	 RINGSHOP },
	{ "wand shop",	 WANDSHOP },
	{ "tool shop",	 TOOLSHOP },
	{ "book shop",	 BOOKSHOP },
	{ "tin shop",	 TINSHOP },
	{ "music shop",	 INSTRUMENTSHOP },
	{ "candle shop", CANDLESHOP },
	{ "pet shop",	 PETSHOP },	/* Stephen White */
#ifdef BLACKMARKET
	{ "black market", BLACKSHOP },
#endif /* BLACKMARKET */
	{ 0, 0 }
};

const char *fname = "(stdin)";
int fatal_error = 0;
int got_errors = 0;
int be_verbose = 0;
int decompile = 0;

#ifdef FLEX23_BUG
/* Flex 2.3 bug work around; not needed for 2.3.6 or later */
int yy_more_len = 0;
#endif

extern unsigned int max_x_map, max_y_map;

extern int line_number, colon_line_number;

struct lc_vardefs *variable_definitions = NULL;


int
main(argc, argv)
int argc;
char **argv;
{
	FILE *fin;
	int i;
	boolean errors_encountered = FALSE;
#if defined(MAC) && (defined(THINK_C) || defined(__MWERKS__))
	static char *mac_argv[] = {	"lev_comp",	/* dummy argv[0] */
				":dat:Arch.des",
				":dat:Barb.des",
				":dat:Caveman.des",
				":dat:Healer.des",
				":dat:Knight.des",
				":dat:Monk.des",
				":dat:Priest.des",
				":dat:Ranger.des",
				":dat:Rogue.des",
				":dat:Samurai.des",
				":dat:Tourist.des",
				":dat:Valkyrie.des",
				":dat:Wizard.des",
				":dat:bigroom.des",
				":dat:castle.des",
				":dat:endgame.des",
				":dat:gehennom.des",
				":dat:knox.des",
				":dat:medusa.des",
				":dat:mines.des",
				":dat:oracle.des",
				":dat:sokoban.des",
				":dat:tower.des",
				":dat:yendor.des"
				};

	argc = SIZE(mac_argv);
	argv = mac_argv;
#endif
	/* Note:  these initializers don't do anything except guarantee that
		we're linked properly.
	*/
	monst_init();
	objects_init();
	decl_init();
	/* this one does something... */
	init_obj_classes();

	init_yyout(stdout);
	if (argc == 1) {		/* Read standard input */
	    init_yyin(stdin);
	    (void) yyparse();
	    if (fatal_error > 0 || got_errors > 0) {
		    errors_encountered = TRUE;
	    }
	} else {			/* Otherwise every argument is a filename */
	    for(i=1; i<argc; i++) {
		    fname = argv[i];
		    if(!strcmp(fname, "-v")) {
			be_verbose++;
			continue;
		    }
		    if(!strcmp(fname, "-d")) {
			decompile = 1;
			continue;
		    }
		    fin = freopen(fname, "r", stdin);
		    if (!fin) {
			(void) fprintf(stderr,"Can't open \"%s\" for input.\n",
						fname);
			perror(fname);
			errors_encountered = TRUE;
		    } else {
			init_yyin(fin);
			(void) yyparse();
			line_number = 1;
			if (fatal_error > 0 || got_errors > 0) {
				errors_encountered = TRUE;
				fatal_error = 0;
			}
		    }
		    (void) fclose(fin);
	    }
	}
	exit(errors_encountered ? EXIT_FAILURE : EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}

/*
 * Each time the parser detects an error, it uses this function.
 * Here we take count of the errors. To continue farther than
 * MAX_ERRORS wouldn't be reasonable.
 * Assume that explicit calls from lev_comp.y have the 1st letter
 * capitalized, to allow printing of the line containing the start of
 * the current declaration, instead of the beginning of the next declaration.
 */
void
yyerror(s)
const char *s;
{
	(void) fprintf(stderr, "%s: line %d : %s\n", fname,
		(*s >= 'A' && *s <= 'Z') ? colon_line_number : line_number, s);
	if (++fatal_error > MAX_ERRORS) {
		(void) fprintf(stderr,"Too many errors, good bye!\n");
		exit(EXIT_FAILURE);
	}
}


void
lc_error(const char *fmt, ...)
{
    char buf[512];
    va_list argp;

    va_start(argp, fmt);
    (void) vsnprintf(buf, 511, fmt, argp);
    va_end(argp);

    yyerror(buf);
}


/*
 * Just display a warning (that is : a non fatal error)
 */
void
yywarning(s)
const char *s;
{
	(void) fprintf(stderr, "%s: line %d : WARNING : %s\n",
				fname, colon_line_number, s);
}

void
lc_warning(const char *fmt, ...)
{
    char buf[512];
    va_list argp;

    va_start(argp, fmt);
    (void) vsnprintf(buf, 511, fmt, argp);
    va_end(argp);

    yywarning(buf);
}


struct opvar *
set_opvar_int(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_INT;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_coord(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_COORD;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_region(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_REGION;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_mapchar(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_MAPCHAR;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_monst(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_MONST;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_obj(ov, val)
struct opvar *ov;
long  val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_OBJ;
        ov->vardata.l = val;
    }
    return ov;
}

struct opvar *
set_opvar_str(ov, val)
struct opvar *ov;
char *val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_STRING;
	ov->vardata.str = (val) ? strdup(val) : NULL;
    }
    return ov;
}

struct opvar *
set_opvar_var(ov, val)
struct opvar *ov;
char *val;
{
    if (ov) {
        ov->spovartyp = SPOVAR_VARIABLE;
	ov->vardata.str = (val) ? strdup(val) : NULL;
    }
    return ov;
}

#define New(type)		\
	(type *) memset((genericptr_t)alloc(sizeof(type)), 0, sizeof(type))

void
add_opvars(sp_lev *sp, const char *fmt, ...)
{
    const char *p;
    va_list argp;

    va_start(argp, fmt);

    for(p = fmt; *p != '\0'; p++) {
	switch(*p) {
	case ' ': break;
	case 'i':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_int(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'c':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_coord(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'r':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_region(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'm':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_mapchar(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'M':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_monst(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'O':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_obj(ov, va_arg(argp, long));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 's':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_str(ov, va_arg(argp, char *));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'v':
	    {
		struct opvar *ov = New(struct opvar);
		set_opvar_var(ov, va_arg(argp, char *));
		add_opcode(sp, SPO_PUSH, ov);
		break;
	    }
	case 'o':
	    {
		long i = va_arg(argp, int);
		if (i < 0 || i >= MAX_SP_OPCODES)
		    fprintf(stderr, "add_opvars: unknown opcode '%i'.\n", i);
		add_opcode(sp, i, NULL);
		break;
	    }
	default:
	    fprintf(stderr, "add_opvars: illegal format character '%c'.\n", *p);
	    break;
	}
    }

    va_end(argp);
}


struct lc_funcdefs *
funcdef_new(addr, name)
     long addr;
     char *name;
{
    struct lc_funcdefs *f = New(struct lc_funcdefs);
    if (!f) {
	lc_error("Could not alloc function definition for '%s'.", name);
	return NULL;
    }
    f->next = NULL;
    f->addr = addr;
    f->name = strdup(name);
    f->n_called = 0;
    f->code.opcodes = NULL;
    f->code.n_opcodes = 0;
    return f;
}

void
funcdef_free_all(fchain)
     struct lc_funcdefs *fchain;
{
    struct lc_funcdefs *tmp = fchain;
    struct lc_funcdefs *nxt;
    while (tmp) {
	nxt = tmp->next;
	Free(tmp->name);
	/* FIXME: free tmp->code */
	Free(tmp);
	tmp = nxt;
    }
}

struct lc_funcdefs *
funcdef_defined(f, name, casesense)
     struct lc_funcdefs *f;
     char *name;
     int casesense;
{
    while (f) {
	if (casesense) {
	    if (!strcmp(name, f->name)) return f;
	} else {
	    if (!strcasecmp(name, f->name)) return f;
	}
	f = f->next;
    }
    return NULL;
}


struct lc_vardefs *
vardef_new(typ, name)
     long typ;
     char *name;
{
    struct lc_vardefs *f = New(struct lc_vardefs);
    if (!f) {
	lc_error("Could not alloc variable definition for '%s'.", name);
	return NULL;
    }
    f->next = NULL;
    f->var_type = typ;
    f->name = strdup(name);
    return f;
}

void
vardef_free_all(fchain)
     struct lc_vardefs *fchain;
{
    struct lc_vardefs *tmp = fchain;
    struct lc_vardefs *nxt;
    while (tmp) {
	nxt = tmp->next;
	Free(tmp->name);
	Free(tmp);
	tmp = nxt;
    }
}

struct lc_vardefs *
vardef_defined(f, name, casesense)
     struct lc_vardefs *f;
     char *name;
     int casesense;
{
    while (f) {
	if (casesense) {
	    if (!strcmp(name, f->name)) return f;
	} else {
	    if (!strcasecmp(name, f->name)) return f;
	}
	f = f->next;
    }
    return NULL;
}

const char *
spovar2str(spovar)
     long spovar;
{
    static togl = 0;
    static char buf[2][128];
    char *n;
    int is_array = (spovar & SPOVAR_ARRAY);
    spovar &= ~SPOVAR_ARRAY;

    switch (spovar) {
    default:		  lc_error("spovar2str(%li)", spovar); break;
    case SPOVAR_INT:	  n = "integer"; break;
    case SPOVAR_STRING:   n = "string"; break;
    case SPOVAR_VARIABLE: n = "variable"; break;
    case SPOVAR_COORD:	  n = "coordinate"; break;
    case SPOVAR_REGION:	  n = "region"; break;
    case SPOVAR_MAPCHAR:  n = "mapchar"; break;
    case SPOVAR_MONST:	  n = "monster"; break;
    case SPOVAR_OBJ:	  n = "object"; break;
    }

    togl = ((togl + 1) % 2);

    snprintf(buf[togl], 127, "%s%s", n, (is_array ? " array" : ""));
    return buf[togl];
}

void
check_vardef_type(vd, varname, vartype)
     struct lc_vardefs *vd;
     char *varname;
     long vartype;
{
    struct lc_vardefs *tmp;
    if ((tmp = vardef_defined(vd, varname, 1))) {
	if (tmp->var_type != vartype)
	    lc_error("Trying to use variable '%s' as %s, when it is %s.",
		     varname, spovar2str(vartype), spovar2str(tmp->var_type));
    } else lc_error("Variable '%s' not defined.", varname);
}

struct lc_vardefs *
add_vardef_type(vd, varname, vartype)
     struct lc_vardefs *vd;
     char *varname;
     long vartype;
{
    struct lc_vardefs *tmp;
    if ((tmp = vardef_defined(vd, varname, 1))) {
	if (tmp->var_type != vartype)
	    lc_error("Trying to redefine variable '%s' as %s, when it is %s.",
		     varname, spovar2str(vartype), spovar2str(tmp->var_type));
    } else {
	tmp = vardef_new(vartype, varname);
	tmp->next = vd;
	return tmp;
    }
    return vd;
}

int
reverse_jmp_opcode(opcode)
     int opcode;
{
    switch (opcode) {
    case SPO_JE:  return SPO_JNE;
    case SPO_JNE: return SPO_JE;
    case SPO_JL:  return SPO_JGE;
    case SPO_JG:  return SPO_JLE;
    case SPO_JLE: return SPO_JG;
    case SPO_JGE: return SPO_JL;
    default: lc_error("Cannot reverse comparison jmp opcode %i.", opcode); return SPO_NULL;
    }
}

/* basically copied from src/sp_lev.c */
struct opvar *
opvar_clone(ov)
     struct opvar *ov;
{
    if (ov) {
	struct opvar *tmpov = (struct opvar *)alloc(sizeof(struct opvar));
	if (!tmpov) panic("could not alloc opvar struct");
	switch (ov->spovartyp) {
	case SPOVAR_COORD:
	case SPOVAR_REGION:
	case SPOVAR_MAPCHAR:
	case SPOVAR_MONST:
	case SPOVAR_OBJ:
	case SPOVAR_INT:
	    {
		tmpov->spovartyp = ov->spovartyp;
		tmpov->vardata.l = ov->vardata.l;
	    }
	    break;
	case SPOVAR_VARIABLE:
	case SPOVAR_STRING:
	    {
		int len = strlen(ov->vardata.str);
		tmpov->spovartyp = ov->spovartyp;
		tmpov->vardata.str = (char *)alloc(len+1);
		(void)memcpy((genericptr_t)tmpov->vardata.str,
			     (genericptr_t)ov->vardata.str, len);
		tmpov->vardata.str[len] = '\0';
	    }
	    break;
	default:
	    {
		lc_error("Unknown opvar_clone value type (%i)!", ov->spovartyp);
	    }
	}
	return tmpov;
    }
    return NULL;
}


void
splev_add_from(splev, from_splev)
     sp_lev *splev;
     sp_lev *from_splev;
{
    int i;
    if (splev && from_splev)
	for (i = 0; i < from_splev->n_opcodes; i++)
	    add_opcode(splev, from_splev->opcodes[i].opcode, opvar_clone(from_splev->opcodes[i].opdat));
}


/*
 * Find the type of floor, knowing its char representation.
 */
int
get_floor_type(c)
char c;
{
	int val;

	SpinCursor(3);
	val = what_map_char(c);
	if(val == INVALID_TYPE) {
	    val = ERR;
	    lc_warning("Invalid fill character '%c' in MAZE declaration", c);
	}
	return val;
}

/*
 * Find the type of a room in the table, knowing its name.
 */
int
get_room_type(s)
char *s;
{
	register int i;

	SpinCursor(3);
	for(i=0; room_types[i].name; i++)
	    if (!strcmp(s, room_types[i].name))
		return ((int) room_types[i].type);
	return ERR;
}

/*
 * Find the type of a trap in the table, knowing its name.
 */
int
get_trap_type(s)
char *s;
{
	register int i;

	SpinCursor(3);
	for (i=0; trap_types[i].name; i++)
	    if(!strcmp(s,trap_types[i].name))
		return trap_types[i].type;
	return ERR;
}

/*
 * Find the index of a monster in the table, knowing its name.
 */
int
get_monster_id(s, c)
char *s;
char c;
{
	register int i, class;

	SpinCursor(3);
	class = c ? def_char_to_monclass(c) : 0;
	if (class == MAXMCLASSES) return ERR;

	for (i = LOW_PM; i < NUMMONS; i++)
	    if (!class || class == mons[i].mlet)
		if (!strcmp(s, mons[i].mname)) return i;
	/* didn't find it; lets try case insensitive search */
	for (i = LOW_PM; i < NUMMONS; i++)
	    if (!class || class == mons[i].mlet)
		if (!strcasecmp(s, mons[i].mname)) {
		    if (be_verbose)
			lc_warning("Monster type \"%s\" matches \"%s\".", s, mons[i].mname);
		    return i;
		}
	return ERR;
}

/*
 * Find the index of an object in the table, knowing its name.
 */
int
get_object_id(s, c)
char *s;
char c;		/* class */
{
	int i, class;
	const char *objname;

	SpinCursor(3);
	class = (c > 0) ? def_char_to_objclass(c) : 0;
	if (class == MAXOCLASSES) return ERR;

	for (i = class ? bases[class] : 0; i < NUM_OBJECTS; i++) {
	    if (class && objects[i].oc_class != class) break;
	    objname = obj_descr[i].oc_name;
	    if (objname && !strcmp(s, objname))
		return i;
	}

	for (i = class ? bases[class] : 0; i < NUM_OBJECTS; i++) {
	    if (class && objects[i].oc_class != class) break;
	    objname = obj_descr[i].oc_name;
	    if (objname && !strcasecmp(s, objname)) {
		if (be_verbose)
		    lc_warning("Object type \"%s\" matches \"%s\".", s, objname);
		return i;
	    }
	}

	return ERR;
}

static void
init_obj_classes()
{
	int i, class, prev_class;

	prev_class = -1;
	for (i = 0; i < NUM_OBJECTS; i++) {
	    class = objects[i].oc_class;
	    if (class != prev_class) {
		bases[class] = i;
		prev_class = class;
	    }
	}
}

/*
 * Is the character 'c' a valid monster class ?
 */
boolean
check_monster_char(c)
char c;
{
	return (def_char_to_monclass(c) != MAXMCLASSES);
}

/*
 * Is the character 'c' a valid object class ?
 */
boolean
check_object_char(c)
char c;
{
	return (def_char_to_objclass(c) != MAXOCLASSES);
}

/*
 * Convert .des map letter into floor type.
 */
char
what_map_char(c)
char c;
{
	SpinCursor(3);
	switch(c) {
		  case ' '  : return(STONE);
		  case '#'  : return(CORR);
		  case '.'  : return(ROOM);
		  case '-'  : return(HWALL);
		  case '|'  : return(VWALL);
		  case '+'  : return(DOOR);
		  case 'A'  : return(AIR);
		  case 'B'  : return(CROSSWALL); /* hack: boundary location */
		  case 'C'  : return(CLOUD);
		  case 'S'  : return(SDOOR);
		  case 'H'  : return(SCORR);
		  case '{'  : return(FOUNTAIN);
		  case '\\' : return(THRONE);
		  case 'K'  :
#ifdef SINKS
		      return(SINK);
#else
		      lc_warning("Sinks ('K') are not allowed in this version!  Ignoring...");
		      return(ROOM);
#endif
		  case '}'  : return(MOAT);
		  case 'P'  : return(POOL);
		  case 'L'  : return(LAVAPOOL);
		  case 'I'  : return(ICE);
		  case 'W'  : return(WATER);
		  case 'T'  : return(TREE);
		  case 't'  : return(DEADTREE);
		  case 'F'  : return(IRONBARS);	/* Fe = iron */
		  case 'x'  : return(MAX_TYPE); /* 'see-through' */
	    }
	return(INVALID_TYPE);
}

void
add_opcode(sp, opc, dat)
sp_lev *sp;
int opc;
genericptr_t dat;
{
   long nop = sp->n_opcodes;
   _opcode *tmp;

   if ((opc < 0) || (opc >= MAX_SP_OPCODES))
       lc_error("Unknown opcode '%i'", opc);

   tmp = (_opcode *)alloc(sizeof(_opcode)*(nop+1));
   if (sp->opcodes && nop) {
       (void) memcpy(tmp, sp->opcodes, sizeof(_opcode)*nop);
       free(sp->opcodes);
   } else if (!tmp)
       lc_error("Could not alloc opcode space");

   sp->opcodes = tmp;

   sp->opcodes[nop].opcode = opc;
   sp->opcodes[nop].opdat = dat;

   sp->n_opcodes++;
}

/*
 * Yep! LEX gives us the map in a raw mode.
 * Just analyze it here.
 */
void
scan_map(map, sp)
char *map;
sp_lev *sp;
{
	register int i, len;
	register char *s1, *s2;
	long max_len = 0;
	long max_hig = 0;
	char msg[256];
	char *tmpmap[ROWNO];
	int dx,dy;
	char *mbuf;

	/* First, strip out digits 0-9 (line numbering) */
	for (s1 = s2 = map; *s1; s1++)
	    if (*s1 < '0' || *s1 > '9')
		*s2++ = *s1;
	*s2 = '\0';

	/* Second, find the max width of the map */
	s1 = map;
	while (s1 && *s1) {
		s2 = index(s1, '\n');
		if (s2) {
			len = (int) (s2 - s1);
			s1 = s2 + 1;
		} else {
			len = (int) strlen(s1);
			s1 = (char *) 0;
		}
		if (len > max_len) max_len = len;
	}

	/* Then parse it now */
	while (map && *map) {
		tmpmap[max_hig] = (char *) alloc(max_len);
		s1 = index(map, '\n');
		if (s1) {
			len = (int) (s1 - map);
			s1++;
		} else {
			len = (int) strlen(map);
			s1 = map + len;
		}
		for(i=0; i<len; i++)
		  if((tmpmap[max_hig][i] = what_map_char(map[i])) == INVALID_TYPE) {
		      lc_warning("Invalid character '%c' @ (%d, %d) - replacing with stone", map[i], max_hig, i);
		      tmpmap[max_hig][i] = STONE;
		    }
		while(i < max_len)
		    tmpmap[max_hig][i++] = STONE;
		map = s1;
		max_hig++;
	}

	/* Memorize boundaries */

	max_x_map = max_len - 1;
	max_y_map = max_hig - 1;


	if(max_len > MAP_X_LIM || max_hig > MAP_Y_LIM) {
	    lc_error("Map too large at (%d x %d), max is (%d x %d)", max_len, max_hig, MAP_X_LIM, MAP_Y_LIM);
	}

	mbuf = (char *) alloc(((max_hig-1) * max_len) + (max_len-1) + 2);
	for (dy = 0; dy < max_hig; dy++)
	    for (dx = 0; dx < max_len; dx++)
		mbuf[(dy * max_len) + dx] = (tmpmap[dy][dx] + 1);

	mbuf[((max_hig-1) * max_len) + (max_len-1) + 1] = '\0';

	add_opvars(sp, "siio", mbuf, max_hig, max_len, SPO_MAP);

	for (dy = 0; dy < max_hig; dy++)
	    Free(tmpmap[dy]);
	Free(mbuf);
}


/*
 * Output some info common to all special levels.
 */
static boolean
write_common_data(fd, lvl)
int fd;
sp_lev *lvl;
{
	static struct version_info version_data = {
			VERSION_NUMBER, VERSION_FEATURES,
			VERSION_SANITY1, VERSION_SANITY2
	};

	Write(fd, &version_data, sizeof version_data);
	/*Write(fd, &lvl->init_lev, sizeof(lev_init));*/
	return TRUE;
}


/*
 * Here we write the sp_lev structure in the specified file (fd).
 * Also, we have to free the memory allocated via alloc().
 */
static boolean
write_maze(fd, maze)
int fd;
sp_lev *maze;
{
        int i;
        uchar len;

        if (!write_common_data(fd, maze))
            return FALSE;

	Write(fd, &(maze->n_opcodes), sizeof(maze->n_opcodes));

        for (i = 0; i < maze->n_opcodes; i++) {
	   _opcode tmpo = maze->opcodes[i];

	   Write(fd, &(tmpo.opcode), sizeof(tmpo.opcode));

	   if (tmpo.opcode < SPO_NULL || tmpo.opcode >= MAX_SP_OPCODES)
	       panic("write_maze: unknown opcode (%i).", tmpo.opcode);

	   if (tmpo.opcode == SPO_PUSH) {
	       genericptr_t opdat = tmpo.opdat;
	       if (opdat) {
		   struct opvar *ov = (struct opvar *)opdat;
		   int size;
		   Write(fd, &(ov->spovartyp), sizeof(ov->spovartyp));
		   switch (ov->spovartyp) {
		   case SPOVAR_NULL: break;
		   case SPOVAR_COORD:
		   case SPOVAR_REGION:
		   case SPOVAR_MAPCHAR:
		   case SPOVAR_MONST:
		   case SPOVAR_OBJ:
		   case SPOVAR_INT:
		       Write(fd, &(ov->vardata.l), sizeof(ov->vardata.l));
		       break;
		   case SPOVAR_VARIABLE:
		   case SPOVAR_STRING:
		       if (ov->vardata.str)
			   size = strlen(ov->vardata.str);
		       else size = 0;
		       Write(fd, &size, sizeof(size));
		       if (size) {
			   Write(fd, ov->vardata.str, size);
			   Free(ov->vardata.str);
		       }
		       break;
		   default: panic("write_maze: unknown data type (%i).", ov->spovartyp);
		   }
	       } else panic("write_maze: PUSH with no data.");
	   } else {
	       /* sanity check */
	       genericptr_t opdat = tmpo.opdat;
	       if (opdat)
		   panic("write_maze: opcode (%i) has data.", tmpo.opcode);
	   }

	   Free(tmpo.opdat);

	}
        /* clear the struct for next user */
	Free(maze->opcodes);
	maze->opcodes = NULL;
        /*(void) memset((genericptr_t) &maze->init_lev, 0, sizeof maze->init_lev);*/

	return TRUE;
}


static boolean
decompile_maze(fd, maze)
int fd;
sp_lev *maze;
{
        int i;
        uchar len;
	char debuf[128];
	const char *opcodestr[MAX_SP_OPCODES] = {
	    "null",
	    "message",
	    "monster",
	    "object",
	    "engraving",
	    "room",
	    "subroom",
	    "door",
	    "stair",
	    "ladder",
	    "altar",
	    "fountain",
	    "sink",
	    "pool",
	    "trap",
	    "gold",
	    "corridor",
	    "levregion",
	    "drawbridge",
	    "mazewalk",
	    "non_diggable",
	    "non_passwall",
	    "wallify",
	    "map",
	    "room_door",
	    "region",
	    "cmp",
	    "jmp",
	    "jl",
	    "jle",
	    "jg",
	    "jge",
	    "je",
	    "jne",
	    "spill",
	    "terrain",
	    "replaceterrain",
	    "exit",
	    "endroom",
	    "pop_container",
	    "push",
	    "pop",
	    "rn2",
	    "dec",
	    "inc",
	    "add",
	    "sub",
	    "mul",
	    "div",
	    "mod",
	    "copy",
	    "mon_generation",
	    "end_moninvent",
	    "grave",
	    "frame_push",
	    "frame_pop",
	    "call",
	    "return",
	    "init_map",
	    "flags",
	    "sounds",
	    "wallwalk",
	    "var_init",
	    "shuffle_array",
	    "dice",
	    "selection_add",
	    "selection_point",
	    "selection_rect",
	    "selection_fillrect",
	    "selection_line",
	    "selection_rndline",
	    "selection_grow",
	    "selection_flood",
	    "selection_rndcoord",
	    "selection_ellipse",
	    "selection_filter",
	};

	/* don't bother with the header stuff */

        for (i=0;i<maze->n_opcodes;i++) {
	   _opcode tmpo = maze->opcodes[i];

	   if (tmpo.opcode < SPO_NULL || tmpo.opcode >= MAX_SP_OPCODES)
	       panic("decompile_maze: unknown opcode (%i).", tmpo.opcode);

	   if (tmpo.opcode == SPO_PUSH) {
	       genericptr_t opdat = tmpo.opdat;
	       if (opdat) {
		   struct opvar *ov = (struct opvar *)opdat;
		   int size;
		   switch (ov->spovartyp) {
		   case SPOVAR_NULL: break;
		   case SPOVAR_COORD:
		       snprintf(debuf, 127, "%li:\t%s\tcoord:(%i,%i)\n", i, opcodestr[tmpo.opcode],
				(ov->vardata.l & 0xff), ((ov->vardata.l >> 16) & 0xff));
			   Write(fd, debuf, strlen(debuf));
			   break;
		   case SPOVAR_REGION:
		       snprintf(debuf, 127, "%li:\t%s\tregion:(%i,%i,%i,%i)\n", i, opcodestr[tmpo.opcode],
				(ov->vardata.l & 0xff), ((ov->vardata.l >> 8) & 0xff),
				((ov->vardata.l >> 16) & 0xff), ((ov->vardata.l >> 24) & 0xff));
			   Write(fd, debuf, strlen(debuf));
			   break;
		   case SPOVAR_OBJ:
		       snprintf(debuf, 127, "%li:\t%s\tobj:(id=%i,class=\'%c\')\n",
				i, opcodestr[tmpo.opcode],
				SP_OBJ_TYP(ov->vardata.l), SP_OBJ_CLASS(ov->vardata.l));
		       Write(fd, debuf, strlen(debuf));
		       break;
		   case SPOVAR_MONST:
		       snprintf(debuf, 127, "%li:\t%s\tmonster:(pm=%i, class='%c')\n", i, opcodestr[tmpo.opcode],
				SP_MONST_PM(ov->vardata.l), SP_MONST_CLASS(ov->vardata.l));
		       Write(fd, debuf, strlen(debuf));
		       break;
		   case SPOVAR_MAPCHAR:
		       snprintf(debuf, 127, "%li:\t%s\tmapchar:(%li,%i)\n", i, opcodestr[tmpo.opcode],
				(int)SP_MAPCHAR_TYP(ov->vardata.l), (schar)SP_MAPCHAR_LIT(ov->vardata.l));
		       Write(fd, debuf, strlen(debuf));
		       break;
		   case SPOVAR_INT:
		       if (ov->vardata.l >= ' ' && ov->vardata.l <= '~')
			   snprintf(debuf, 127, "%li:\t%s\tint:%li\t# '%c'\n", i, opcodestr[tmpo.opcode], ov->vardata.l, (char)ov->vardata.l);
		       else
			   snprintf(debuf, 127, "%li:\t%s\tint:%li\n", i, opcodestr[tmpo.opcode], ov->vardata.l);
		       Write(fd, debuf, strlen(debuf));
		       break;
		   case SPOVAR_VARIABLE:
		   case SPOVAR_STRING:
		       if (ov->vardata.str)
			   size = strlen(ov->vardata.str);
		       else size = 0;
		       if (size) {
			   int x;
			   int ok = (size > 127) ? 0 : 1;
			   if (ok)
			       for (x = 0; x < size; x++)
				   if (ov->vardata.str[x] < ' ' || ov->vardata.str[x] > '~') {
				       ok = 0;
				       break;
				   }
			   if (ok) {
			       if (ov->spovartyp == SPOVAR_VARIABLE)
				   snprintf(debuf, 127, "%li:\t%s\tvar:$%s\n", i, opcodestr[tmpo.opcode], ov->vardata.str);
			       else
				   snprintf(debuf, 127, "%li:\t%s\tstr:\"%s\"\n", i, opcodestr[tmpo.opcode], ov->vardata.str);
			       Write(fd, debuf, strlen(debuf));
			   } else {
			       snprintf(debuf, 127, "%li:\t%s\tstr:", i, opcodestr[tmpo.opcode]);
			       Write(fd, debuf, strlen(debuf));
			       for (x = 0; x < size; x++) {
				   snprintf(debuf, 127, "%02x ", ov->vardata.str[x]);
				   Write(fd, debuf, strlen(debuf));
			       }
			       snprintf(debuf, 127, "\n");
			       Write(fd, debuf, strlen(debuf));
			   }
		       }
		       break;
		   default: panic("decompile_maze: unknown data type (%i).", ov->spovartyp);
		   }
	       } else panic("decompile_maze: PUSH with no data.");
	   } else {
	       /* sanity check */
	       genericptr_t opdat = tmpo.opdat;
	       if (opdat)
		   panic("decompile_maze: opcode (%i) has data.", tmpo.opcode);
	       snprintf(debuf, 127, "%li:\t%s\n", i, opcodestr[tmpo.opcode]);
	       Write(fd, debuf, strlen(debuf));
	   }

	}
	return TRUE;
}

/*
 * Open and write special level file.
 * Return TRUE on success, FALSE on failure.
 */
boolean
write_level_file(filename, lvl)
char *filename;
sp_lev *lvl;
{
	int fout;
	char lbuf[60];

	if (decompile) {
	    lbuf[0] = '\0';
#ifdef PREFIX
	    Strcat(lbuf, PREFIX);
#endif
	    Strcat(lbuf, filename);
	    Strcat(lbuf, "_lev.txt");
	    fout = open(lbuf, O_TRUNC|O_WRONLY|O_CREAT, OMASK);
	    if (fout < 0) return FALSE;
	    decompile_maze(fout, lvl);
	    (void) close(fout);
	}

	lbuf[0] = '\0';
#ifdef PREFIX
	Strcat(lbuf, PREFIX);
#endif
	Strcat(lbuf, filename);
	Strcat(lbuf, LEV_EXT);

	fout = open(lbuf, O_WRONLY|O_CREAT|O_BINARY, OMASK);
	if (fout < 0) return FALSE;

        if (!lvl) panic("write_level_file");

	if (be_verbose)
	    fprintf(stdout, "File: '%s', opcodes: %li\n", lbuf, lvl->n_opcodes);

        if (!write_maze(fout, lvl))
          return FALSE;

	(void) close(fout);

	return TRUE;
}

#ifdef STRICT_REF_DEF
/*
 * Any globals declared in hack.h and descendents which aren't defined
 * in the modules linked into lev_comp should be defined here.  These
 * definitions can be dummies:  their sizes shouldn't matter as long as
 * as their types are correct; actual values are irrelevant.
 */
#define ARBITRARY_SIZE 1
/* attrib.c */
struct attribs attrmax, attrmin;
/* files.c */
const char *configfile;
char lock[ARBITRARY_SIZE];
char SAVEF[ARBITRARY_SIZE];
# ifdef MICRO
char SAVEP[ARBITRARY_SIZE];
# endif
/* termcap.c */
struct tc_lcl_data tc_lcl_data;
# ifdef TEXTCOLOR
#  ifdef TOS
const char *hilites[CLR_MAX];
#  else
char NEARDATA *hilites[CLR_MAX];
#  endif
# endif
/* trap.c */
const char *traps[TRAPNUM];
/* window.c */
struct window_procs windowprocs;
/* xxxtty.c */
# ifdef DEFINE_OSPEED
short ospeed;
# endif
#endif	/* STRICT_REF_DEF */

/*lev_main.c*/
