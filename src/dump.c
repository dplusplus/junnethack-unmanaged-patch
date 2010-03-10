/*	SCCS Id: @(#)end.c	3.5	2010/03/08	*/
/* Copyright (c) Patric Mueller.			*/
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "date.h"

#ifdef DUMP_LOG
extern char msgs[][BUFSZ];
extern int lastmsg;
void FDECL(do_vanquished, (int, BOOLEAN_P));
#endif 

#ifdef DUMP_LOG
FILE *dump_fp = (FILE *)0;  /**< file pointer for text dumps */
FILE *html_dump_fp = (FILE *)0;  /**< file pointer for html dumps */
/* TODO:
 * - escape unmasked characters in html (done for map)
 * - tables for skills and spells
 * - menucolors for items?
 * - started/ended date at the top
 */

void
dump_init()
{
  if (dump_fn[0]) {
    int new_dump_fn_len = strlen(dump_fn)+strlen(plname);
    char *new_dump_fn = (char *) alloc((unsigned)(new_dump_fn_len+5+1)); /* space for ".html" */
    char *p = (char *) strstr(dump_fn, "%n");

    if (p) {
      char *q = new_dump_fn;
      strncpy(q, dump_fn, p-dump_fn);
      q += p-dump_fn;
      strncpy(q, plname, strlen(plname) + 1);
      regularize(q);
      q[strlen(plname)] = '\0';
      q += strlen(q);
      p += 2;	/* skip "%n" */
      strncpy(q, p, strlen(p));
      new_dump_fn[new_dump_fn_len] = '\0';
    } else {
      strcpy(new_dump_fn, dump_fn);
    }

#ifdef DUMP_TEXT_LOG
    dump_fp = fopen(new_dump_fn, "w");
    if (!dump_fp) {
/*JP
	pline("Can't open %s for output.", new_dump_fn);
*/
	pline("%s���o�͗p�ɊJ���Ȃ��B", new_dump_fn);
/*JP
	pline("Dump file not created.");
*/
	pline("�_���v�t�@�C�����쐬�ł��Ȃ������B");
    }
#endif
#ifdef DUMP_HTML_LOG
    html_dump_fp = fopen(strcat(new_dump_fn, ".html"), "w");
    if (!html_dump_fp) {
/*JP
	pline("Can't open %s for output.", new_dump_fn);
*/
	pline("%s���o�͗p�ɊJ���Ȃ��B", new_dump_fn);
/*JP
	pline("Html dump file not created.");
*/
	pline("HTML�_���v�t�@�C�����쐬�ł��Ȃ������B");
    }
#endif
    if (new_dump_fn) free(new_dump_fn);
  }
}
#endif

void
dump_exit()
{
#ifdef DUMP_LOG
	if (dump_fp) {
		dump_html("</body>\n</html>\n","");
		fclose (dump_fp);
	}
#endif
}

void
dump(pre, str)
const char *pre, *str;
{
#ifdef DUMP_LOG
  if (dump_fp)
    fprintf(dump_fp, "%s%s\n", pre, str);
  if (html_dump_fp)
    fprintf(html_dump_fp, "%s%s\n", pre, str);
#endif
}

/** Outputs a string only into the html dump. */
void
dump_html(format, str)
const char *format, *str;
{
#ifdef DUMP_LOG
  if (html_dump_fp)
    fprintf(html_dump_fp, format, str);
#endif
}

/** Outputs a string only into the text dump. */
void
dump_text(format, str)
const char *format, *str;
{
#ifdef DUMP_LOG
  if (dump_fp)
    fprintf(dump_fp, format, str);
#endif
}

/** Dumps one line as is. */
void
dump_line(pre, str)
const char *pre, *str;
{
#ifdef DUMP_LOG
  if (dump_fp)
    fprintf(dump_fp, "%s%s\n", pre, str);
  if (html_dump_fp)
    fprintf(html_dump_fp, "%s%s<br />\n", pre, str);
#endif
}

#ifdef MENU_COLOR
extern boolean get_menu_coloring(const char *str, int *color, int *attr);
#endif

/** Dumps an object from the inventory. */
void
dump_object(c, str)
const char c;
const char *str;
{
#ifdef DUMP_LOG
	if (dump_fp)
		fprintf(dump_fp, "  %c - %s\n", c, str);
	if (html_dump_fp) {
#ifdef MENU_COLOR
		int color;
		int attr;
		if (iflags.use_menu_color &&
		    get_menu_coloring(str, &color, &attr)) {
			fprintf(html_dump_fp, "<span class=\"nh_color_%d\"><span class=\"nh_item_letter\">%c</span> - %s</span><br />\n", color, c, str);
		} else
#endif
		fprintf(html_dump_fp, "<span class=\"nh_item_letter\">%c</span> - %s<br />\n", c, str);
	}
#endif
}

/** Dumps a secondary title. */
void
dump_subtitle(str)
const char *str;
{
#ifdef DUMP_LOG
  dump_text("  %s\n", str);
  dump_html("<h3>%s</h3>\n", str);
#endif
}

/** Dump a title. Strips : from the end of str. */
void
dump_title(str)
char *str;
{
#ifdef DUMP_LOG
	int len = strlen(str);
	if (str[len-1] == ':') {
		str[len-1] = '\0';
	}
#if 1 /*JP*/
	else if (!strcmp(&str[len-2], "�F")) {
		str[len-2] = '\0';
	}
#endif
	if (dump_fp)
		fprintf(dump_fp, "%s\n", str);
	if (html_dump_fp)
		fprintf(html_dump_fp, "<h2>%s</h2>\n", str);
#endif
}

/** Starts a list in the dump. */
void
dump_list_start()
{
#ifdef DUMP_LOG
	if (html_dump_fp)
		fprintf(html_dump_fp, "<ul>\n");
#endif
}

/** Dumps a list item. */
void
dump_list_item(str)
const char *str;
{
#ifdef DUMP_LOG
	if (dump_fp)
		fprintf(dump_fp, "  %s\n", str);
	if (html_dump_fp)
		fprintf(html_dump_fp, "<li>%s</li>\n", str);
#endif
}

/** Ends a list in the dump. */
void
dump_list_end()
{
	dump_html("</ul>\n","");
}

/** Starts a blockquote in the dump. */
void
dump_blockquote_start()
{
#ifdef DUMP_LOG
	if (html_dump_fp)
		fprintf(html_dump_fp, "<blockquote>\n");
#endif
}

/** Ends a blockquote in the dump. */
void
dump_blockquote_end()
{
#ifdef DUMP_LOG
	dump_text("\n", "");
	dump_html("</blockquote>\n", "");
#endif
}

/** Dumps the HTML header. */
void
dump_header_html(title)
const char *title;
{
	dump_html("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n","");
	dump_html("<html xmlns=\"http://www.w3.org/1999/xhtml\">", "");
	dump_html("<head>\n", "");
/*JP
	dump_html("<title>UnNetHack " VERSION_STRING ": %s</title>\n", title);
*/
	dump_html("<title>JUnNetHack " VERSION_STRING ": %s</title>\n", title);
#if 0 /*JP*/
	dump_html("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n", "");
#else
/* internal kcode */
/* IC=0 EUC */
/* IC=1 SJIS */
#define IC ((unsigned char)("��"[0])==0x8a)
	dump_html("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=%s\" />\n", IC ? "Shift_JIS" : "EUC-JP");
#endif
/*JP
	dump_html("<meta name=\"generator\" content=\"UnNetHack " VERSION_STRING "\" />\n", "");
*/
	dump_html("<meta name=\"generator\" content=\"JUnNetHack " VERSION_STRING "\" />\n", "");
	dump_html("<meta name=\"date\" content=\"%s\" />\n", iso8601(0));
	dump_html("<link rel=\"stylesheet\" type=\"text/css\" href=\"unnethack_dump.css\" />\n", "");
	dump_html("</head>\n", "");
	dump_html("<body>\n", "");
}

static char html_escape_buf[BUFSZ];
/** Escape a single character for HTML. */
char* html_escape_character(const char c) {
	switch (c) {
		case '<':
			return "&lt;";
		case '>':
			return "&gt;";
		case '&':
			return "&amp;";
		case '\"':
			return "&quot;";
		case '\'':
			return "&#39;"; /* not &apos; */
		default:
			sprintf(html_escape_buf, "%c", c);
			return html_escape_buf;
	}
}

/*dump.c*/