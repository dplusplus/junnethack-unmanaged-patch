/*
**
**	$Id: jlib.c,v 1.14 2008/05/03 15:40:59 argrath Exp $
**
*/

/* Copyright (c) Issei Numata 1994-2000 */
/* JNetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <ctype.h>
#include "hack.h"

int xputc(CHAR_P);
int xputc2(int, int);


#define EUC	0
#define SJIS	1
#define JIS	2

/* internal kcode */
/* IC=0 EUC */
/* IC=1 SJIS */
#define IC ((unsigned char)("��"[0])==0x8a)

/* default input kcode */
#ifndef INPUT_KCODE
# ifdef MSDOS
#  define INPUT_KCODE SJIS
# else
#  define INPUT_KCODE EUC
# endif
#endif

/* default output kcode */
#ifndef OUTPUT_KCODE
# ifdef MSDOS
#  define OUTPUT_KCODE SJIS
# else
#  define OUTPUT_KCODE EUC
# endif
#endif

#ifdef WIN32_PLATFORM_PSPC 
#define putchar(x) {}
#endif

static int	output_kcode = OUTPUT_KCODE;
static int	input_kcode = INPUT_KCODE;

/*
**	Kanji code library....
*/

/*
 * ����������(��1������)���ǂ����𒲂ׂ�
 */
int
is_kanji(c)
     unsigned c;
{
    if(IC == EUC)
      return (c & 0x80);
    else
      return ((unsigned int)c>=0x81 && (unsigned int)c<=0x9f)
	|| ((unsigned int)c>=0xe0 && (unsigned int)c<=0xfc);
}

void
setkcode(c)
     int c;
{
    if(c == 'E' || c == 'e' )
      output_kcode = EUC;
    else if(c == 'J' || c == 'j')
      output_kcode = JIS;
    else if(c == 'S' || c == 's')
      output_kcode = SJIS;
    else if(c == 'I' || c == 'i')
#ifdef MSDOS
      output_kcode = SJIS;
#else
      output_kcode = IC;
#endif
    else{
	output_kcode = IC;
    }
    input_kcode = output_kcode;
}
/*
**	EUC->SJIS
*/

unsigned char *
e2sj(s)
     unsigned char *s;
{
    unsigned char h,l;
    static unsigned char sw[2];

    h = s[0] & 0x7f;
    l = s[1] & 0x7f;

    sw[0] = ((h - 1) >> 1)+ ((h <= 0x5e) ? 0x71 : 0xb1);
    sw[1] = l + ((h & 1) ? ((l < 0x60) ? 0x1f : 0x20) : 0x7e);

    return sw;
}
/*
**	SJIS->EUC
*/
unsigned char *
sj2e(s)
     unsigned char *s;
{
    unsigned int h,l;
    static unsigned char sw[2];

    h = s[0];
    l = s[1];

    h = h + h - ((h <=0x9f) ? 0x00e1 : 0x0161);
    if( l<0x9f )
      l = l - ((l > 0x7f) ? 0x20 : 0x1f);
    else{
	l = l-0x7e;
	++h;
    }
    sw[0] = h | 0x80;
    sw[1] = l | 0x80;
    return sw;
}
/*
**	translate string to internal kcode
*/
const char *
str2ic(s)
     const char *s;
{
    static unsigned char buf[1024];
    const unsigned char *up;
    unsigned char *p, *pp;
    int kin;

    if(!s)
      return s;

    buf[0] = '\0';

    if( IC==input_kcode ){
	strcpy((char *)buf, s);
	return (char *)buf;
    }

    p = buf;
    if( IC==EUC && input_kcode == SJIS ){
	while(*s){
	    up = s;
	    if(is_kanji(*up)){
		pp = sj2e((unsigned char *)s);
		*(p++) = pp[0];
		*(p++) = pp[1];
		s += 2;
	    }
	    else
	      *(p++) = (unsigned char)*(s++);
	}
    }
    else if( IC==EUC && input_kcode == JIS ){
	kin = 0;
	while(*s){
	    if(s[0] == 033 && s[1] == '$' && (s[2] == 'B' || s[3] == '@')){
		kin = 1;
		s += 3;
	    }
	    else if(s[0] == 033 && s[1] == '(' && (s[2] == 'B' || s[3] == 'J')){
		kin = 0;
		s += 3;
	    }
	    else if( kin )
	      *(p++) = (*(s++) | 0x80);
	    else
	      *(p++) = *(s++);
	}
    }
    else{
	strcpy((char *)buf, s);
	return (char *)buf;
    }

    *(p++) = '\0';
    return (char *)buf;
}

#ifdef SJIS_FILESYSTEM
/*
**	translate string to output kcode
*/
const char *
ic2str(s)
     const char *s;
{
    static unsigned char buf[1024];
    const unsigned char *up;
    unsigned char *p, *pp;
    int kin;

    if(!s)
      return s;

    buf[0] = '\0';

    p = buf;
    if( IC==EUC && output_kcode == SJIS ){
	while(*s){
	    up = s;
	    if( *up & 0x80 ){
		pp = e2sj((unsigned char *)s);
		*(p++) = pp[0];
		*(p++) = pp[1];
		s += 2;
	    }
	    else
	      *(p++) = (unsigned char)*(s++);
	}
    }
    else{
	strcpy((char *)buf, s);
	return (char *)buf;
    }

    *(p++) = '\0';
    return (char *)buf;
}
#endif /* MSDOS */

/*
**	primitive function
*/

static int kmode;	/* 0: Kanji out */
			/* 1: Kanji in */

static void
tty_reset()
{
    if(kmode && output_kcode==JIS ){
	putchar(033);
	putchar('(');
	putchar('B');
/*
    if (flags.DECgraphics){
      putchar(033);
      putchar('$');
      putchar(')');
      putchar('B');
    }
*/
    }
    kmode = 0;
}

/* print out 1 byte character to tty (no conversion) */
static void
tty_cputc(unsigned int c)
{
    if(kmode && output_kcode==JIS ){
	putchar(033);
	putchar('(');
	putchar('B');
    }
    kmode = 0;

#if defined(NO_TERMS) && (defined(MSDOS) || defined(WIN32CON))
    xputc(c);
#else
    putchar(c);
#endif
} 

/* print out 2 bytes character to tty (no conversion) */
static void
tty_cputc2(unsigned int c, unsigned int c2)
{
    kmode = 1;

#if defined(NO_TERMS) && (defined(MSDOS) || defined(WIN32CON))
    xputc2(c, c2);
#else
    putchar(c);
    putchar(c2);
#endif
} 

/* print out 1 byte character to tty (IC->output_kcode) */
static void
tty_jputc(unsigned int c)
{
    if(kmode && output_kcode==JIS ){
	putchar(033);
	putchar('(');
	putchar('B');
    }
    kmode = 0;

#if defined(NO_TERMS) && (defined(MSDOS) || defined(WIN32CON))
    xputc(c);
#else
    putchar(c);
#endif
}

/* print out 2 bytes character to tty (IC->output_kcode) */
static void
tty_jputc2(unsigned int c, unsigned int c2)
{
    if(!kmode && output_kcode==JIS ){
	putchar(033);
	putchar('$');
	putchar('B');
    }
    kmode = 1;

#if defined(NO_TERMS) && (defined(MSDOS) || defined(WIN32CON))
    xputc2(c, c2);
#else
    putchar(c);
    putchar(c2);
#endif
}

/*
 *  2�o�C�g�������o�b�t�@�����O���Ȃ���o�͂���
 *  �����R�[�h�ϊ����s��
 */
int
jbuffer(
     unsigned int c,
     unsigned int *buf,
     void (*reset)(),
     void (*f1)(unsigned int),
     void (*f2)(unsigned int, unsigned int))
{
    static unsigned int ibuf[2];
    unsigned int c1, c2;
    unsigned char uc[2];
    unsigned char *p;

    if(!buf) buf = ibuf;
    if(!reset) reset = tty_reset;
    if(!f1) f1 = tty_jputc;
    if(!f2) f2 = tty_jputc2;

    c = c & 0xff;

    if(!(buf[0]) && (is_kanji(c))){
	buf[1] = c;
	++buf[0];
	return 0;
    }
    else if(buf[0]){
	c1 = buf[1];
	c2 = c;

	if(IC == output_kcode)
	  ;
	else if(IC == EUC){
	    switch(output_kcode){
	      case JIS:
		c1 &= 0x7f;
		c2 &= 0x7f;
		break;
	      case SJIS:
		uc[0] = c1;
		uc[1] = c2;
		p = e2sj(uc);
		c1 = p[0];
		c2 = p[1];
		break;
	      default:
		impossible("Unknown kcode!");
		break;
	    }
	}
	else if(IC == SJIS){
	    uc[0] = c1;
	    uc[1] = c2;
	    p = sj2e(uc);
	    switch(output_kcode){
	      case JIS:
		c1 &= 0x7f;
		c2 &= 0x7f;
		break;
	      case EUC:
		break;
	      default:
		impossible("Unknown kcode!");
		break;
	    }
	}
	f2(c1, c2);
	buf[0] = 0;
	return 2;
    }
    else if(c){
	f1(c);
	return 1;
    }
    reset();
    return -1;
}

/*
 *  2�o�C�g�������o�b�t�@�����O���Ȃ���o�͂���
 *  �����R�[�h�ϊ��͍s��Ȃ�
 */
int
cbuffer(
     unsigned int c,
     unsigned int *buf,
     void (*reset)(),
     void (*f1)(unsigned int),
     void (*f2)(unsigned int, unsigned int))
{
    static unsigned int ibuf[2];

    if(!buf) buf = ibuf;
    if(!reset) reset = tty_reset;
    if(!f1) f1 = tty_cputc;
    if(!f2) f2 = tty_cputc2;

    if(!(buf[0]) && is_kanji(c)){
	buf[1] = c;
	++buf[0];
	return 0;
    }
    else if(buf[0]){
	f2(buf[1], c);
	buf[0] = 0;
	return 2;
    }
    else if(c){
	f1(c);
	return 1;
    }
    reset();
    return -1;
}

void
jputchar(int c)
{
    static unsigned int buf[2];
    jbuffer((unsigned int)(c & 0xff), buf, NULL, NULL, NULL);
}
void
cputchar(int c)
{
    static unsigned int buf[2];
    cbuffer((unsigned int)(c & 0xff), buf, NULL, NULL, NULL);
}

void
jputs(s)
     const char *s;
{
    while(*s)
      jputchar((unsigned char)*s++);
    jputchar('\n');
}

int
is_kanji2(s, pos)
     const char *s;
     int pos;
{
    unsigned char *str;

    str = (unsigned char *)s;
    while(*str && pos>0){
	if(is_kanji(*str)){
	    str+=2;
	    pos-=2;
	}
	else{
	    ++str;
	    --pos;
	}
    }
    if(pos<0)
      return 1;
    else
      return 0;
}

int
is_kanji1(s, pos)
     const char *s;
     int pos;
{
    unsigned char *str;

    str = (unsigned char *)s;
    while(*str && pos>0){
	if(is_kanji(*str)){
	    str+=2;
	    pos-=2;
	}
	else{
	    ++str;
	    --pos;
	}
    }
    if(!pos && is_kanji(*str))
      return 1;
    else
      return 0;
}

/*
** 8�r�b�g�X���[��isspace()
*/
int
isspace_8(c)
     int c;
{
    unsigned int *up;

    up = (unsigned int *)&c;
    return *up<0x80 ? isspace(*up) : 0;
}
/*
** split string(str) including japanese before pos and return to
** str1, str2.
*/
void
split_japanese( str, str1, str2, pos )
     char *str;
     char *str1;
     char *str2;
     int pos;
{
    int len, i, j, k, mlen;
    char *pstr;
    char *pnstr;
    int	sq_brac;

retry:
    len = strlen((char *)str);

    if( len < pos ){
	strcpy(str1,str);
	*str2 = '\0';
	return;
    }

    if(pos > 20)
      mlen = 20;
    else
      mlen = pos;

    i = pos;
    if(is_kanji2(str, i))
      --i;

/* 1:
** search space character
*/
    j = 0;
    while( j<mlen ){
	if(isspace_8(str[i-j])){
	    --j;
	    goto found;
	}
	else if(is_kanji1(str,i-j)){
	    if(!strncmp(str+i-j,"�@",2)){
		j -= 2;
		goto found;
	    }
	}
	++j;
    }

/* 3:
** search second bytes of japanese
*/
    j = 0;
    sq_brac = 0;
    while( j<mlen ){
	if(str[i-j] == '['){
	    sq_brac = 0;
	    goto found;
	}
	else if(str[i-j] == ']')
	  sq_brac = 1;
	
	if(!sq_brac && is_kanji1(str,i-j)){
	    goto found;
	}

	++j;
    }
    if(pos > 2){
	pos -= 2;
	goto retry;
    }
found:

    if(pos > 2){
	if((str[i-j] == ']' ||
	    str[i-j] == ')' ||
	    str[i-j] == '}')){
	    pos -= 2;
	    goto retry;
	}
	else if(!strncmp(str+i-j, "�n", 2) ||
		!strncmp(str+i-j, "�j", 2) ||
		!strncmp(str+i-j, "�p", 2)){
	    pos -= 2;
	    goto retry;
	}
	else if(!strncmp(str+i-j,"�H",2) ||
		!strncmp(str+i-j,"�A",2) ||
		!strncmp(str+i-j,"�B",2) ||
		!strncmp(str+i-j,"�C",2) ||
		!strncmp(str+i-j,"�D",2)){
	    pos -= 2;
	    goto retry;
	}
    }

    if(i == j){
	/* �����_���Ȃ����� */
	strcpy(str1, str);
	str2[0] = '\0';
	return;
    }

    pstr = str;

    pnstr = str1;
    for( k=0 ; k<i-j ; ++k )
      *(pnstr++) = *(pstr++);
    *(pnstr++) = '\0';

    pnstr = str2;
    for( ; str[k] ; ++k )
      *(pnstr++) = *(pstr++);
    *(pnstr++) = '\0';
}

void 
jrndm_replace(c)
     char *c;
{
    unsigned char cc[3];

    if(IC==SJIS)
      memcpy(cc, (char *)sj2e(c), 2);
    else
      memcpy(cc, c, 2);

    cc[0] &= 0x7f;
    cc[1] &= 0x7f;

    switch(cc[0]){
      case 0x21:
	cc[1] = rn2(94) + 0x21;
	break;
      case 0x23:
	if(cc[1] <= 0x39) /* �O�`�X */
	  cc[1] = rn2(10) + 0x30;
	else if(cc[1] <= 0x5A) /* �`�`�y */
	  cc[1] = rn2(26) + 0x41;
	else if(cc[2] <= 0x7A) /* ���`�� */
	  cc[1] = rn2(26) + 0x61;
	break;
      case 0x24:
      case 0x25:
	cc[1] = rn2(83) + 0x21; /* ���`�� or �A�`�� */
	break;
      case 0x26:
	if(cc[1] <= 0x30)
	  cc[1] = rn2(24) + 0x21; /* ���`�� �M���V������ */
	else
	  cc[1] = rn2(24) + 0x41; /* ���`�� �M���V������ */
	break;
      case 0x27:
	if(cc[1] <= 0x40)
	  cc[1] = rn2(33) + 0x21; /* �@�`�` ���V�A���� */
	else
	  cc[1] = rn2(33) + 0x51; /* �p�`�� ���V�A���� */
	break;
      case 0x4f:
	cc[1] = rn2(51) + 0x21; /* �@�` �r */
	break;
      case 0x74:
	cc[1] = rn2(4) + 0x21; /* � � � � ��4����*/
	break;
      default:
	if(cc[0] >= 0x30 && cc[1] <= 0x74)
	  cc[1] = rn2(94) + 0x21;
	break;
    }

    cc[0] |= 0x80;
    cc[1] |= 0x80;

    if(IC==SJIS)
      memcpy(c, (char *)e2sj(cc), 2);
    else
      memcpy(c, cc, 2);
}

/*
 * "put off"��Ώۂɂ���ēK�؂ɘa�󂷂�
 */
const char *
joffmsg(otmp, joshi)
register struct obj *otmp;
const char **joshi;
{
    static char buf[BUFSZ];

    *joshi = "��";

    if(otmp->oclass == RING_CLASS){
	Sprintf(buf, "%s����͂���", body_part(FINGER));
	return buf;
    }
    if( otmp->oclass == AMULET_CLASS){
	return "�͂���";
    }
    else if(is_helmet(otmp))
      return "���";
    else if(is_gloves(otmp))
      return "�͂���";
    else if(otmp->oclass == WEAPON_CLASS||is_shield(otmp)){
	*joshi = "��";
	return "����������";
    }
    else if(is_suit(otmp))
      return "�E��";
    else
      return "�͂���";
}

/*
 * "put on"��Ώۂɂ���ēK�؂ɘa�󂷂�
 */
const char *
jonmsg(otmp, joshi)
register struct obj *otmp;
const char **joshi;
{
    static char buf[BUFSZ];

    *joshi = "��";

    if(otmp->oclass == RING_CLASS){
	Sprintf(buf, "%s�ɂ͂߂�", body_part(FINGER));
	return buf;
    }
    else if(otmp->oclass == AMULET_CLASS)
      return "�g�ɂ���";
    else if(is_gloves(otmp))
      return "�g�ɂ���";
    else if(is_shield(otmp)){
	*joshi = "��";
	return "�g�����";
    }
    else if(is_helmet(otmp))
      return "���Ԃ�";
    else if(otmp->oclass == WEAPON_CLASS){
	Sprintf(buf, "%s�ɂ���", body_part(HAND));
	return buf;
    }
    else if(is_boots(otmp))
      return "����";
    else if(is_suit(otmp))
      return "����";
    else
      return "�g�ɂ���";
}

/*
 * ������Ԃ�
 */
const char *
numeral(obj)
register struct obj *obj;
{
    switch(obj->oclass){
      case WEAPON_CLASS:
      case WAND_CLASS:
	if(obj->otyp==SHURIKEN)
	  return "��";
	else
	  return "�{";
      case ARMOR_CLASS:
	return "��";
      case GEM_CLASS:
      case ROCK_CLASS:
      case BALL_CLASS:
	return "��";
      case SCROLL_CLASS:
	return "��";
      case SPBOOK_CLASS:
	return "��";
      case POTION_CLASS:
      case RING_CLASS:
      case AMULET_CLASS:
      case FOOD_CLASS:
	if(obj->quan < 10L)
	  return "��";
	else
	  return "";
      case COIN_CLASS:
	return "��";
      default:
	switch(obj->otyp){
	  case CREDIT_CARD:
	  case TOWEL:
	  case BLINDFOLD:
	    return "��";
	  case SKELETON_KEY:
	  case TALLOW_CANDLE:
	  case WAX_CANDLE:
	  case PICK_AXE:
	  case UNICORN_HORN:
	  case LEASH:
	  case STETHOSCOPE:
	  case MAGIC_MARKER:
	    return "�{";
	  case CRYSTAL_BALL:
	    return "��";
	    break;
	  default:
	    if(obj->quan < 10L)
	      return "��";
	    else
	      return "";
	}
    }
}

/*-------------------------------------------------------------------------
	�S�p�����̂�����p�^�[��
  -------------------------------------------------------------------------*/
static const char *ro0 =
	"                        �E  �E�C�D  �D��                �E      "
	"            �R�J    �T�J    �O�G�}  �^���b()�|�]�|�]            "
	"�]  �b      �d�E�E          �e  �f   (  )            [  ]    {  "
	"}           �q  �r          �u  �v  �i�m�j�n�|��    �{�|�^�_�|�F"
	"�|  ���^<    >  ���������`��E�d�B  ���{        ��  �K�b�x���r�b"
	"�b�b    �K�^��//    �~  �b��        ��  ()�b��  ��  <>  ��  �R��"
	"��  �ȁځ�  �ɁP��  �~�F�����|->�|<-�b�ȁb�Ɂ�                  "
	"                            ���߁��߁�������(    )  ���i||��    "
	"                            �O/  /���|  ������<>�Ɂ��R��        "
	"                                    �^�Q�b�Q    ��  ��  ��  ��  "
	"<<��>>��/   �`���`���d�E    ��                              �K�`"
	"�K�^||���b���I|)�s�{�����a                  ()�b                "
	"                                            ()�b�L�b�܁ځ܁��ځ{"
	"�u���܂��P /���R����                            ���Ȃd�R( �܂k )"
	"�e�߁u���b  ||����  ��  |<  |_  �m�u||�u�b()�e���n���o  ���܁P�b"
	"�i�� /�_VV�m�^�_�Ɂb�P��                        ��  ����    ����"
	"��  ���|��  �b���E���E����|<    ��||��||��  ����������  �R  �{�|"
	"��||�A  ��vv�^><���m�|��                    �߂��ǁR���R�M���M��"
	"�]  �񃉂�  �̂�J�R��  �L����  <   ��  |+����  �j�[��  �{����  "
	"l   ��  �i�X��  ��t+��  �Ăy��  �i����  �T��      ���[��  �[ ("
	"��  ��  ��  �i  �b���߂̂��J)�m�悯��  ��  �ĉ���  ��  �n����  "
	"��  �^�_��  ��  �͂܂�  ��  �L��7+�݂�����()���L���{�{��  ��  "
	"�B�����イ�����m��  �I��V�����H�I�̂���ƃi/v�`            "
	"                                �C�|�t�m�m���m����  ���~���ۂs��"
	"�{  ��  �i���J  �����L  �m�t�N  �m�N�P  �j ]�R  �i++�T  �J���V  "
	"�t�l�X  �����Z  �R�m�\  �N���^  �i��`  �J�m�J�m�c  �����e  �b�R"
	"�g  �{�m�|�Q�t�����s    �m�R�n  �n  ���k�q  �q  �P�m�t  �t  �^�_"
	"�w  �w  �����z  �z  �R�t�S�߁ځR�m�R�����{  ��  ���ۃR�ۃR�߃R��"
	"���t���m�m�����m�R�����t�u�t�L�G�A�G���t�R�m�E  �i���N�m        "
	"                        ���Ȃd�R�b�P�ځȂe�k�V��||���n�|�b  |<  "
	"/ �_�u�m||�ȃj�߂b()�����e�c>_�ځP�b�Ɂb�����^�_�b�{��          "
	"                            |3B                                 "
	"                                                                "
	"                                                                "
	"                                                                "
	"                ���ȄC���d�R�b�P�L  �e�k�E  �K  �܂�||�u�I  |<  "
	"J|�P�m�u||���b()�����e��( �܁P�b���m���O�^�_�k�ۂ��b||�ۄW�Y�]  "
	"�]||�b��-)  |O������                                            "
	"                ��  ( ���]�s�|      ���|�u  �{                  "
	"            ��  �s                                  ��  ��  ��  "
	"��                                                              "
	"        �|  ��                  ��  ��  ��  ��  ������  ��  ��  "
	"��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  "
	"��  ��  ";

static const char *ro1 =
	"������  ���\B ���ߎ�S��  ����    �  ��  �T  ������  ��  ���"
	"��  ��  ���Q�ؐh�ʓy��l�y  �~�Ǐ������S�H�䎅�{��  �����  �I��"
	"�l���~��������  ������  �剹�v���،��l  ��  �C����  ��  ����|��"
	"�я���  �����  ��  ��  ���܊�      �c  �c����  ��谎���c����  "
	"��  ��  ��  �M  ����# ����  ��  ��  �L  ��  �|  �m�q�v  ��  �ы�"
	"��  ��  ���ダ�X�d�ň�  �L  ���受���|�b�H���p  �X  �A  ��  ���]"
	"�}  ����������  �~���H  �q�ʘ�  �H  ��  �풹�K  ��  ��谂d�ΚD  "
	"��  ���L��  ��  ���֏��V�ʊ���  �܃��剤��  �񃀌R  �J�]�C  �H��"
	"��  �{�C�L���i�c�����R  �v  ��  �i  �g  �p  �T�M�q�Ńq�ŉ�  �s��"
	"���i���[��  ���t���M�n�ڏ��Z���Ց�����Z��  ��  �c  ���͙�����\"
	"�~���M���ǐS��  �  ������  ��  �΋��ΐ��k����  ��泖L�F��  ��  "
	"��  �����ǒ��M  ��  �V�����j��  �v��đ�e����S�b  ������  �指"
	"��t�H�y���H��  ��  �撹���R�c  ��  ��  ��  ������  ��  �T  ���y"
	"�Ăy��  ���ň��S���M�}  �����s�g�C�q��  ��  ������  �\�d�͌�����"
	"�g���S�珗���  �S���ѓl�R���c�؉��ؚF����l  ���D  �ؐ�щ�"
	"��  ��  ��  ��  ��  ��  ��  �������ʉ�  ���L��  �D  �J  �����䔰"
	"��  ����I  ���R�b�l��𒎉���L��谐H����n��/|�։]�p  ��  �S  "
	"��  �M�񏬚햔�y��  �D  ��  �O�������ȝ��S�l��  �Z����  �ʉΓc��"
	"�䔒�����  ��  ��  �F  ��  毙{��͗[�g��  �g  ���ݍ���������  "
	"�ΐ��|�M�s�\�����母��嗢  �������\�y�j�؎s�������b�`  �q  ����"
	"���s�L  �o  �e�A��  ���t��  ��谐�  ��  �p  ��  �Ԍ���  ��e�  "
	"������  �u�R���؋q�ř��ŌT  ��  ��  �c�j��  ���H�P���Q  ��  ����"
	"��  ��  ��  ��  ��  ��  �ԊQ��  �������\��  ��  �v��؎锒�X�T  "
	"�Y  ����������  �b���ؕS��  ��  �|�ă�����  �H  ���C���k����  "
	"�����r  谗͌�  ��  �r  ������  �~����  �s�\�슱��  ��  ��  ��  "
	"��  ��  ��  �j  ��  ����谌���  �v  ��  �y  ��  ��  �b�M��  ��  "
	"��  ��  �  ���R��H�����M�Ċ�  谌������c�L��  ��  ��  ��  ��  "
	"��  ����H���q����  �����ʊ���  �������i�ڍ��R�ΏK����  ��谌���"
	"�F�Ō��Ŏ~  �x  ��  �g  ���呴  ���  ��  �R�x���z�����ȐS�R  "
	"�ؙ{�������ڑ����ؑ���  ��  ��  泟t�����V�������l�҉ї���  ����"
	"�H  �v�����ȊL  ���Ȏԋ���R�H�{�n��Z�����\��  �`  ���x��  ����"
	"�x  �^  �����`  �q�}�l���r��`  ���X�`  ��  ��  �v���m����  �_  "
	"�؋g��  ���g�ΐ�،߉�  ���Ŋe  �p  �g�d��  ��  �N�X�C��C�ؔT  "
	"�y  �C  �R�����S�����؂T��  �y  ��  �v�Ή�������g�|�y  �y  ���\"
	"�����b���߃L�y�����Ù��d��  ��  ��  �����g  ���ߑ���������  �䎦"
	"�N�c������  ������  �v  ��  ���Y���Z��  �����́��������K���\�ی�"
	"��  �v  �|���|�j��  �}�S�����v  �F����  �Z  ��  �v  ��  �]���́�"
	"��  ��  ��  ���틽�����H�h�n�C�ŋ^  ���Y���Ė؁�����c��������  "
	"��  �Đ��  俗͓y���c�\����ʂs�Ҍ����������ю��l���]  �b����  "
	"�  �l����  ��俋�  �S�ʌ��������i L��  ��  ������  ���v��Ó�"
	"�g��n��n�u�n��ڒU�[  �g���H  ���H�[  �[  �[  �[  ���L��  ����"
	"���ћ��o��  ���������v�����Ԍ��\�\  ��  �v�Đ��؎��i���؋��H��  "
	"���E�d  ����N�r��  �N  �\�g���߃l  �n  ������  ���Y�˝��y�����\"
	"�Y  ��  �O�c�e���v�R��  �}  �㎩��  谔T�坼�U���\  �v  �c�\�ގ|"
	"��  �������ĎԎ��T  ���y�Y  ��  ���\���|�h���Ԗ����ŕv���]  ��  "
	"�����g泑���Ԏ蔒�������s  �C�ؐl�P��r�_  �~�����g�M  �����  "
	"�C���g  ��  ��  ������  ��  ��  �����b�y�����M��S  ���n��  ��  "
	"��  �  ��  ��  �쌢��  �Ό������ڎ��ˌ��ڌZ�����b�L�Ԋ���  ����"
	"��  ���Ŕn���b���Y�e�ʔ��ʊ���t�|����  ��  ���􉤌������M���O��"
	"���F��  �畽��  ��m�������Ï��Îq�Z�R  ���ԋ|�Z�P���Ý��،ß���"
	"�Z  �S�Ӛ�  ���t�Ì���  �g�X���𑫚����Ì�谌ٕœ��x��  �_  ��  "
	"���P�����܌��������牵�]����،��  ���ӑ��Ό��ጾ�����ǓьӉ�  "
	"�����Z����  �����K  ���Y�n���H�͌�̓����ʎq���R�c���ʐΌ���"
	"�y���y�@���q�q���q�y�~���s�ۃG�����ȓy�h����������讋|�����j���r"
	"��  ��  ��  �G�����������  ��  ��  �X  �i  �V�H�V���V���V�J�V�i"
	"�c����ې΍X�э��čN���H�������������Έ�y�T�~���������󍂌��M��"
	"䊐�e���s�����i�H�L�L�i��B �эF���L�΍L�����升�烐�H�ŉѓ�����"
	"�]����l|���͌����֌���  �l  �V����  �ԁ������ÌZ��  ���g���ʉюE"
	"�э�����  �����V�����v�]���ܐS��  ���D��  ��  �~��q�Ńw������"
	"�y�\涍���������涍���������؍��؍���  �������Ó��q�]�S���j�C��"
	"��  ��  �R���i�G㳍��؊��V�������Ώ�������L���ߓy��������  ��  "
	"�  �f  ��  �g��������h  �ќc�I����  �؜��~�ʐ�  �݉Ήуc����"
	"�Ι����Η[���Ď����c��  �ߜ��Ԝ�B �Ր�  �y  �؍˔�  �L�ˉ�  ��  "
	"��  �E  �_  ���L���V�R���  �Ί�H���C���  �����  �������  "
	"����R  ��  ���̖؏����\��  ���q�M  ���Ѝ�  �ݗ[��  �@  �؂��џt"
	"�Y  ��谔������ʉ���������||������ߘ�  ���c���ێQ  �U  �̝�"
	"�؜��Εĉ����F���ڜO�Z���V���^  �v�L�ш�H  �ԋҎa���e���Y�m�C�q"
	"�i  �C���R  =]  �䒆���i�C���\�ۏ��䏗�s�������ꛙ����  ���Ўm�S"
	"�c�S�|  �m���q�����ҕ���q���؎x��  �e�q[+EE�t  �l�~�у��􏬎���"
	"�������x���|���p�j�����i�����������u�������L�L�Ս�谐H�i�~�Ę���"
	"��  ��  ���X�~�q�y�����S��  ������  ��  ���J  ���ʛ����Ύ�����"
	"�c  �S�l���ڎ�  ��h�V�[����H�T�}�Ɠc�|��ԗR���Z�J�����{����"
	"�K�ۖ�v������  �ѐS�V���o  �����ҊL�v  ��  ��  �ώv���ؔV  ��  "
	"�c��������  �^  �g���ɏE�ԝ��]�l��  �l�y�����y�E���˘j�쏎  ����"
	"��  �C�̙�  �����؎ىΎٍ����юىюڋ��ՉE  �f  �|�H��S��  ����"
	"��  �p�L��  �e���  ����яd���d�����  ���S��  ��  �Z  �O����  "
	"�؋j����J�����l��  �g  ��  ���ސ�c���c�H�S��  �B  �єT�щΎ��~"
	"�l  �H������O  �S  ��  ��  谌����A�Ԏ���  ��  �яB���ؓыS�C�\"
	"�d���  ��{�e�D��  ���ؙq�\�~><�V�����]�痢���[�z���e  ��  �f  "
	"�l�Z���h��  �x  �x  �y�R�s�{�{  ��  �R����v�w  ����C  �n���  "
	"��  ���参  �e�{��  �y���[  �@  ���ԛ�  ��  �ы���Ś�{�l���ˋ�"
	"���ҏ�  ��  �������Ҏ�  ��  ��  ��  ���Ҋ��͗]�����m���\�]  �@�S"
	"�����]  ��  ��  ��  ���҃`�O������  ���g��  ���|���嗧��������  "
	"��  �n����  ��  ���y���؛��Ə͜c������  ��  ���葖  ������  ����"
	"���i�،��؏і؏͖؏ŏ�  ��  ��  �����΋��  ��  �������ڐΏѐΏ�"
	"�l�r�ћ�������  �ď�����������  ��  ��  �s�d���ߌ��������������r"
	"�  �������ы������d������  �v�ф��ۃi��������  �k�{��  ��  ��  "
	"��  �������ݐ�  �J  ��ؖ؏䑈  ��  �c��������  �����������葮  "
	"��  �H�Ў�  ��  �e����冎��Ǝ��ƃN�b�p����  �H���C�����㒇�\��  "
	"�����C�����C�Z  ��  �n���^  �C  �h�ғ�  �с��`  ������ؗR�b���c"
	"���l�\��  ���\��  �S  �V  �h�����c���I�����  ���\�J�C�m�փC��"
	"��  ��  ��m�������C��  �b����  �\  ��  �v�n�i  ����c�œѓᚘ��"
	"�ʐ���  ������\��  �  �t���Ό��ڐ��ę��H����  �  �љ���谋���"
	"�L  �������L�R�@�R���ĝ��؋摖��谋�  �؜c�؏���  ��ŏ�谋�  "
	"�o  �K  ���{������  �c�v���D��  �s  �ۗ͏�����  ��  ��  ��������"
	"�������؍Ȗؐ��~����  �����������M�Đ����m�����ߎl  �����܌�"
	"����  �ѐ���������  �ьZ����谖����Џ��̐��{��  ��  �؋Ҍ�  "
	"��  �|�̎��ӕv�����L�y���������ӐΕŎ����o  ��  ��><��  ���t����"
	"�|�����Z�J�����F�p�Ò��P�C�R���Y�\�C����j  �R������ml|�P���ˉH"
	"�F  �ؑS�ؒO������  ��  ��  ��  �O  ��  ���D����|�O���򎅜��r��"
	"����[���M����  ���S�L�������F  ��  ���������l���r��l|�r�g�a  "
	"�^�������l�P���P���P�ėЏ��]��y�R����  �����c���ѕD��  �D�[�D��"
	"�Α^�l���ъ��Ċ��y���������ь��ˊ�  ��  �P  �]  �q  ��  �Ǝ捇��"
	"��  �m  ��  ��  ��  ���]���Е��S���S�R����  ��\�a  �i�ؒU���ȓ�"
	"�P�ʑq  ��  ��  �Εi�N�����R�ؖڌ��S�đ��������@������  �s  ���O"
	"�q  �i�ؑs�ߓy�D�V  ��  ���q�J���n�a��  �]  �]  ��  ��  �L�]��  "
	"��  ��  �L  ���Ŏ��S��  �R����  ���D��  �J  ���Z�L�����������"
	"�l�R�Ê��O  �i�q�q�n�U����  �ؐ���  ��  �[�c��  ��  ������  �L  "
	"��  ������  �؛��M���؍���  �n���n�P�C�{�y�y����������R���Ў�  "
	"��S�\�F�ٜ��v���v����  �����  ��  ��ߑ�L��  �  �  �㍕����"
	"�C�T�����l�i�|��ѐ����Ŋ咹��  ��  �葁�  �~����  ��  ��  ��  "
	"���y��泌������V�  ���Ꭸ  ��  ���ь��n���Őm�U�K  ��  谐����Z"
	"�ȋ������R  �ؕ��w����  ����ؑ���谜O  ���V���v�y�U�R�U��ؓ�  "
	"�v����  �r  �R�D������|�P���莨  ���U�D���������i������  �|�P"
	"�ċғ�৘�  �d�t�c�͌�����  ����y��|�玨�S�m����  ���m��谒�  "
	"�������m���r�n��}�،��c�PTT�|�}�{  �  �ю������z�������r�ڃL�\"
	"�C���R  ���S�R  �ڒU�؎��  ��  ���ߌ���ѐ������n��؉J������  "
	"��  ��  �L  �ꘫ�X�k��  ������  �А�В������|���|�����c�e����  "
	"��  �\��쌎��  �А��c���ڒ����S�������䒎����������������������"
	"�d  ���ŉG�����͕�  ��  ���V�V�񉤜c�C�L���^��  ��  ��  ��谖ؒ�"
	";g  ��  ���T�T  ��  �ؕꍑ�J�؋K�c  ��  �ؐΏ\爒�  ���������؏t"
	"�M  �y�����⏗�����R�C�����Ћ���谒�������  ��  ��  ��  ��L����"
	"��  �D  ���Л�������M�p������  ��  ��  ��  ��  �V���Β�l��ђ�"
	"����M�쌾�����鑫���h  ��  �ё������  ��  ��  ����������  ����"
	"�|�R��  ������  �܌��U  �U  �ԎU��  �����ȎM�y�^��  ��  ����"
	"�o  ��ÊL��ԉ]�^�Ő�����]���t�a  ���o�J�b�Z  ���y��  �]  ����"
	"���ґ�  �S  �ؓy�x  ᠓���  �L�җ]  ��  ���x�Ύ��Ζ��z�͛����\��"
	"�����z�S��  ���Z��  ��  �m�t��  �  ��  �咷��  ��  �R����  �t  "
	"��  ��  ��  ��  ��  ���M�ʙ���  ��  �Β��Γo��  ������  ��  ��  "
	"��  �ē����[��  �d  ��  ��  �����_  �O�����B��  �G  ���o��  ����"
	"�n  ��  ��  �d�͙c  ���y������  ��  ��  ��  ���������e  ��  ����"
	"�㉺��  ��������e�S��  ��  �f�ډљX�n  ��  ��  �����ؖ���  ��[]"
	"����i  ���R���T��  ��  ��  ���Ó�  �R����  ������  �  ��  �ԕ�"
	"��E�_  ���ԑ厦�N���c�l�P���~  �  ��  ��  ��  ���D�U  �n�쎅�T"
	"�c���c����  �Ԍ��v谏�  ��  ���q��T��  �q���C  ��  ���H++  ��  "
	"�t���m�_����������  �p�����p�n�S�E  ��  ��  �l���S�������`�c��  "
	"�߃����S�O  �R  �R  �Đ�A  �M�����y�ѓy���߃c���_  �������q�c��"
	"�_  �ȒC�i��������  �b  ��  ��  �b  ��  ��  ���b�Δ�g���n  �b  "
	"�d�T��  ��  �O�I��  �L���ؕs�s�M�Дږk�����s��ԓьȈʌ��������^"
	"��  �^  �^  �L  �`���m��L��������  ���T�ѕ�����H  �m��������  "
	"��  ��  ��  ���P��  �Ĕ��M����  ��  �\  ��  �\  ��  �U�V�n੗F  "
	"��  ��  �ΒJ��  ��  ��  ��  ��  ���{�Γc���c�m�_���{��  �  �є�"
	"镗F����i  �F  ��  �唰�㒹�V  ��  �����  ��  ��  �L���ʖ�����"
	"�Ж}���ʉ�����  :e  �}  �Д�)e  �����c���q  �M�t��  �L���a  �ѕ�"
	"�Εŕ��ŐH����  ��  �΍��ʎM�ʐΔ�  ��������c����  ���ț����  "
	"��S�˔��  ��  �񕶏�q�K  ������x�ΔڕK  ��  �\  ���b��  ��  "
	"���L�  �k]E��  �ʉ�����p  ���ќe����  �c�����䛙�ڗr�v��  �ؓ~"
	"��  ���X���d镔���c��  �  �����|�S�S  ��  ��  �c  �؉�b���"
	"���N��  �H�c�\  �՜c�[  ��  �[  �[�Z������  ����涎ٛ����c  ����"
	"�я��c  ���c������  �������V�����ѐ���  ��  �p�����n  ���Ŗ���"
	"�󊢃n���C����  ��V����y  ���c�i�Л��t�z  �v  ��  ���ҕ����t  "
	"�����t  �{���g�ݕv  �������L��  ���g��  �t  ��  ��  ���T���C���"
	"��  �����\����  ������  �H  �㌢���c�e��Гc�����l�c����l�畜  "
	"��  ���䃀  ��  �C�����܋��t������  ��  ��  ��  ��  �щΚS  �ĕ�"
	"�Ĉَ����J�������厨��  ��  �u  ��  �ǋЊ�  �ǜO��  �a�H��  ���"
	"��y�؁��S�L�  �  �  �ߐΖ����ǖږΜ�����G  ��������G  �G  "
	"��  ��  �G  �X  �Ɨ͏��ƃ��O�v�֋x���ɕ���ᚘ���  �~���p�\��  "
	"�ԕ�b  �������y�����T  �����O����  䞃���  ��  �ș����؍K�����v"
	"��  �R��R���  �����  ��  �������i��  ��  ��  ��  �Ε����  "
	"��  ��  ��  ��  ��  ������毁�  ��  �H�  �����V  ������  ����"
	"�y�������Ж`�S�S�S  �˕���  �S���Õ�����ڎ��������d  ���^��"
	"�L  ������  �����K�ŃR�q��  ������  ��  �؃g������  �ќc��������"
	"�t  �e���  �W  �����  �ԉH�{  ���M���薃�Ζ��S���ї�  ��������"
	"�؝��x  ��  �^  ���z��  �؛����L�ؐ������C  �������V�k����  ��  "
	"��  ��  �s  �������C�N����  ��  ��  ��  ��  ��  �S����  ��  �R�b"
	"�~�K�~�K�t  ��  �O  �h  �����Ėю�  �ږ������[  ���������\�I�J��"
	"�����؋����D���Ǚk�U�[���@���������M��  ���������������q�D��N�Y"
	"�؛厅���  ���S���ʔ�  ��  ��  �S���q�M������  �S�ڎ�㦖��щ�  "
	"�M���\���ҍ������؍H��  �H��  �ˑ�Đn���L�働��S����F]  ���n"
	"�Z���  ��  ���뎨  ���\��  �V  ��  �e�t��  �y  ��  ��谗��K  "
	"��  ��  �O  �O  �R  ��  ���O�ԑO��调C�E�J  �j  �i���L  ��R��  "
	"���S��  �E���ؗR�E  �T  �U  �U���c  �l�E�l�J���G��  �b  ��  �L�"
	"魒��^�N�}���z  �T�ӌ�  �Z  �\�ŗf  ���͏���J  ���p��  ��  ��  "
	"�B  ��  �r  �r  �e  �e  ���b��㴉�����谐��ؗe  ���������T  ��  "
	"��  �H  �~  ��  �J����  �J  �H���H�ْ�  ��  ��  ��  ��  ��  ����"
	"�J�c�e  ���e��  �ъe�サ�K  ��  茞���  ��  �  �b���у��j�䛙��"
	"�؎q���؉������כ�����  �l���b����谐�y��  �������j��  ��  �c�e"
	"��  �[  ��  �[  ���c�[  �ė��琶���b��  �C  �g�v��TK�g�j�t����  "
	"��  �c�R��瘵  �ēl�S  ��  �  ������  ����  ��  ������  �U��"
	"����  ���i������  �ʗ���  ���ѕđC��  �b�i�ԘӕđC�đC���C��  "
	"��  ��  �c���đ卡  ��  ��  ��  ���͗�  ��  ���߃l����  ���ߊ��"
	"�J�߉J����  ���ߙʗљʗџe�����͗�  ��ߛ������S�đC�A  �Γ���  "
	"������  �A  ��  ����������  �D  �ΌˊL�e���e�J�H��  ��  ���Y���O"
	"�ǌ��ď��Y  ��  ���J��  ��  ��  �y�q������ꋗ�  ������  �l������"
	"������  ��  �у�����s���L�L�́���  �ؙ��A���ݖ��U  �������  "
	"��  ��  �^  ��  ��  ";

static const char *ro2 =
	"�T  �����s  �w���胋    ��      �m�_��k��  ��  ��  ��  ��  �ɗ\"
	"��  ���G][  �H�z��  ���{��  ��  �񊎐l^^�T  �ʐl�C�g�C�J�C��C��"
	"�C�n�Y���  ��  ��  ��  ��  ��  ��  �~���g  ��  ��  ��  ��  ��  "
	"�S  �L  �r  �ؘ������  ��  ��  �l���e  ��  �T  ������  ��  ��  "
	"��  ��  ��  �Z  ��  ��  �w���f  ��  ��  ��  ��  �{  ��  �_  ��  "
	"�R���\���F  �F  ��  ��  ��  ��  �`  �S  ��  ��  ��  ���      "
	"��  �K  ��  �  ��  ��  ��  ��  ��  �j�c�@  �  �_  ��  ��      "
	"��  �i  ��  ��  ��      ��  �X  �P�Y�Z  ��  ��  ��  �J  �O  ���T"
	"�k�ك� ]�c����  �R�c�c���R����f��  ��ʙk�\�k���  �P�ٖ�      "
	"��  ��  ��  ��  �Z  ��  ��  ��  �h  �X  �g���ؙ{�C�{�c�{�k�v����"
	"�n  �`���탊��  ������  �ナ��  ��  �󙃛�  ��  ��  ��  ��  �O��"
	"�  ��  �  ��  �[  �@  �@���@����  �  ��  �h  �h  ��͏��͋g��"
	"�v�͛�  ���͏�  �J�ΐ�  ��      �c  �M      �  �m�t�ܐn�����c��"
	"�ᙯ��  ���q���k�����b���y���M��  �P�k���i��\�O������O�\�\  "
	"���y������|]��  �^�ŒJ�Ŋ�  �m�P�ʜc�ʑ��ʉę�  �ʎz�ʝƃ�  �Q  "
	"�Z  ��  �P�x����    ���������������������������働�҃������E����"
	"���������������ڃ��i���烍�������Z���b�������{���\�������z���o"
	"���t����\�����������������������������ڑ���������V��  ���s"
	"��  ��  ��  ��  �X  �F  ��  ��  �M  ��  ��  ��  ��  调���::��  "
	"��  �P  �O  ��  �b  ��  �q  �e  ��  ��  ��  �H  ��  �@  �P  ��  "
	"��  �`  �h  ��  �G  �L  ��  ��  �V����  ��  ��  ��  ��  ��      "
	"��  �A  ��  ��  �c  ���p�z  ��  ��  ��  ��  ��  �  �i�؍�  �J  "
	"�@  ��          ��  �����d  ��  ��  ��      �G      �Y  ��      "
	"�  �R���������ߚ������L���ᚘ�K�����������蚘�����Ꚙ  ���ޚ���"
	"�y���y���y�R�y���y�ғy�~�y�s�y�Ø�  ��y�y��y�t�y��y��y���y�s"
	"�y���y��㉺�y��y�p�y��y���y���y���y�ԓy���y���y���y�D�y��y��"
	"�y�G�y�Γy�C�P�t�y���y���y�a�y��y�ԓy���y���y����  �y���y����  "
	"�i  �}  �b  �  �A  �i  �n  ��  ��  ��  �s  ������  �D����  �q  "
	"�v�^��      ���e�ї[�ʑ��v  ��  ��\��T��  ������l���\����"
	"�s���  �U  ��  ��  ��  �������ُ�  �C����  ���䏗�U���ꏗ�Ηr��"
	"���O���C��  ��  ����������  ��  ���j�������p��  ��  ��  ��  �я�"
	"��  �M  �i  ��  �P�x���n��  ��  ��      �[  ��  ��  ��  �d  �  "
	"�j���j����  �@  ��  ����    ��  ��  �T�q�q  ��  �z�q��  ���ۛ�q"
	"��  �w  ���q��  ��  �~�q�~�b�C  ��      �  ��          ��  �Q  "
	"��  �H�c�P�و�  �L  �L  ������  ��  �Ɛ��P���r  ��㛗�c��  ����"
	"���䛙  ���ě��L��  ����  ��  �R�i�R���R���R��y  ��  ���R�R��"
	"�R�R�R���R���R���R���R�َR�荇  �R����      �R�X�R�ђ�  �R�J�{  "
	"��  ��  ��  ��  ��  ��  �  ��  �R���  ��  �Ì��i  �[  ��  �S  "
	"��  ��  ��  �a  ��  ��  �o  �`  �@  ��  ��  �^  �o      �  �^  "
	"    ��  <<  �H����  �@�b���Ѓ��ЋЎ��z�Д���    ��谋Љ�����В�"
	"�Ж��К���  ���T�Г��ƋЕ���++TT�O����  �������ʛ��r������������"
	"�������u���c�������z�������n��ᢛ���������g��ḛ�谛�  ����    "
	"�M�R�i�\�O  �O              ��  �|  �z�|�|����  �F  �e  ��  ���|"
	"���|�����  ��  �_�ʃm  �g�c�C  �e���e���e���e���e��e���e�ݜe�{"
	"�e  �k  ��  �e  �e�c�e�ʜe  ��  ��  ��  �N  ��  ���  ���S��  "
	"��  ��  ��  ��  ��S�����U  ��  ��  ��  ��  �i  �\�u�C�S�e  �x�S"
	"��  �́���  ��  ���S��  ��  �{  ��  ��  �r�S��  ��  ��  ��  ��  "
	"��  ��  ��  ��  �W  ��  ��  ���S�G  �b  ��  ��  ��  ��  ��  ��  "
	"�  �M  ��  ��  �c  �t�S�H  ��  ��  ��  ��  ��  ��  ��������u  "
	"�C  ��  �S  ��  ���S��  ��  �q  ��  �O  �I  ��  �N  �Q  �a  �a  "
	"�]  �K  ��  ��  ��  ��  ��  �[  �f  ��      �[  �i  ��  �d  ��  "
	"�n  �{  ��  �K  ��  ��  ��  ��  ��  �i  ��  ��  �  �g  ��  ��  "
	"��  ��  ��  ��  �  ��  ��  ��  �T  ��  ��  ��  ��  ��  ��  �r��"
	"谜���  ��  �Y  ��  �ˍ��L���L���L���L�H�L����  ��  ��  ��  ��  "
	"�\  ��  �l  ��  ��  �s  �c  ��  �\  �z�荇���  ��  ��  �q  ��  "
	"�t  ��  ��  ��͗�  �e  �g  ��  �H  �\  ��  ��  ��  ������  ��  "
	"�P�x���y��  ��  ��  ��  ��  ���虯  ��  ��  ��  ��  ��  �  �F  "
	"��  �  ��  �_  ��P��  �`  �[�ʊ����i  ��  ��  �q  ��  ��  �H  "
	"��  ��  ��  ��  �H�c�o  �z  �  �  ��  ��  ��  �  ��  ��  �K  "
	"�B  �  ��  �~  �o���o�L��  ��  ��  ��  �o  ��  ��  �A  �A  ��  "
	"��  �y����  ��  �^  ��  ������  ��  ���P��  ��  �C���c���𝼕�  "
	"�����]���]�������Н�������  �@���ǎ������p�l�r�l�΋Ғf  ���O���s"
	"�������ѕ������[���ԕ��ԃ`����  ������  ���V���������ؓ�����i  "
	"���K���܈�  �����W  ��������  ����������C������  ��  ��  �R  "
	"�p  �R  ��  ��  ��  �U  �i  �  ��  �Ǔ���  ��  ��  ��  �A  �L  "
	"�`  ��  �d���\���������o���Ǒ�  ��  ��  �J��z  �s���T�ؖؓ��ؗ�"
	"�ؓ��؊��،Ȗ؍H�؜T�؎R�ؖ��؉���  �،��ؗ\�؏��ؕ��ؕ��،˖؏�"
	"�؜O�U���؉؉���  �ؑ��ؙ��؋�؊��ؓ�ؐ˖؎��؏o�ؕ�ؕt�ؗ�"
	"�؉Z�ؑ\�؊��؋��؉H�ؑC�ؚ�؍l�؎��؏[�ؑ��ؖ��ؓ��؊p�؝ۖؒ�"
	"�؍��؈�؛��؝��ؓߖؒ�ؚ�ؕ��؛t�і}�ؘC�щ��ؘ��؎��،N�؋�"
	"�ؑ��؎ٞR  �؍��،��؉��؋�  �؍��؜��؏@�؈�؏f�؏��؞R��  "
	"�ؒm�ؑ�؏��ؔO�ؕ{�ؕ��ؖ�ؓT�؎��ؘӖؔT�؊F�،Ӗ؏H�؎��،_"
	"�ؐ�؎Җؐr�ؒ��泗і��ؖ�ؙ`�ؕ��؞j�؎��ؐ��؎M�h  �؋S���"
	"�؍��؍v�؍��؍��؊��؍�ؓ��؉H�ؔʖٖؔ؉Ζؐ�ؖ��؝Ӗؗe�ؗ�"
	"�ؗ��؊s�y  �؜c�،\��  �؎H�p�l�؎a�؏]�ؐ��ؙ��ؐʖؑ��ؐ�ё�"
	"�ؖ��ؖ��؉i�ؘK�؊��؊і؊�؍Ŗ؏��؟}���@�ؓo�ؓ���Ꟗؔ��ؒ�"
	"�،��؈Ӗ،h�ؕ��ؙ@�ؚ���焖؟@�؊ϖؙ���谖ؔJ�ؕo�֖ؖ؉��ؕi"
	"�؞ٖ؊y��胖ؗ�؉d�؝���  �؉J�،��ؗ��ʜc�؊��ጇ�󌇓�  �"
	"��  ��  �P�����������H�������@���o��谌��t��[���e���e��e�c�e��"
	"�e�t�e���e���e���e��e��e�d�e�o�e�T�e�T�{���t  ���t���t�N�����["
	"���іы���  ��  �щ����ј��іS�����������������čH���R  �y  ��  "
	"��  ��  �~  �S  �s  ��  ��  ��  ��  ��  ��  ��  �O  ��  �l  ��  "
	"��  ��  ��  ��  �c  �R  ��  ��  ��  ��  �s��  ��  ��  ��  ��  "
	"�{  ��@��  ��  ��  ��  �G  ��  ��  �ܟ���  ��  ��  ���y��  �V��"
	"�Đ씟  ��  ��  ��  ��  ��  ������  �  ��  ���͜�  �@  ��  �y�X"
	"��  �y  ��  ���y��  ��  �  �c  �R  ���U�H  ������  ��  ��  �q  "
	"������  �u  ��  �`  ���q�h  ��  �@�M�W����  ��  �J  ��  �q  ��  "
	"�P  ���ד�  �Y�ꋃ�Ӗ�  ��  ��  ��  �ܗW��  ��  ��  �Q  ������  "
	"��  ��  �j  �����b  �  ��  �z���~  �~  �q  ��  ��  ��  �U  ��  "
	"��  �d  ��  ��  ��  ��  ��  ���i�K  ��  �L  ��  ��          ��  "
	"�J  �o  �l  ��  �o  �R  ��  �\  �{  ��  ��      ��  ��  ��  �  "
	"��  �]�\�  ��  ���펅���[�ΉΏ��Ιc�Ιc�΋��Γ�Ε��Ε�Έ��x  "
	"�Ώ�Ίe��  �Κ�΍��Η��Κ􋐖���  ��  ��  �΍c��৉Ν�Ώd����"
	"�Α��΍v�Έѝ�  �Ζ��  �ΜT��ꟉΓ։ΔԉΘ��Ή��Κʉΐ��ΒB���"
	"��泏K谉Ξى�ḉ�茕�  �����ܔb�F  ��  ����੊�=|  �  ૚��М�"
	"�Д��������ው�ї������������S  �������`��  ��  �N  ��  ��  �b  "
	"��  �e  ��  ��  ��  ������  ��  �Z  ��  ��  ��  ��  ��  ��  ��  "
	"��  ��  ��  ������  �\  �  �J  �b  �l  ��  ��  ��  ��  ��  ��  "
	"��  ��  ��  �e  ��  ��  �Y  ��  ��  ��  �@  �R���R  �K  ���ݖ`  "
	"�`  ��  �S  ���L�n  ���ʋ�  ��  ��  焋ʉ�  ��  ��  ��  ���Z�h�Z"
	"���\���犢�����Ԋ������ъ��S���p�������H���Й����ꊢ������  ��  "
	"���ÍX���}�p�c���c�c�c��c��c�v�c�c�c���c���c�ߓc���c�e�c���c�]"
	"�c��c����  �c��c����  �c  ��  �������v���R������ޛ������Û���"
	"�����������U���~������Λ��S���r���ݛ��u���ۛ������ϛ��ś�������"
	"���ږ�  ��  ����������`���M���s���қ��q���ʛ������͛��ݛ��ț���"
	"��ᢛ������a���ݛ��^���{��谛�  ���ϛ�����N��谛��^    ᠓V᠟t"
	"���q���X�������唒�𔒊������͔���比����R���R䍔瘰�M���M"
	"�r�M���M���M��M���M�g�M���M���M�ڙa�ڛ��ڏ���  �ڌ��ړ�|���"
	"�ڍ��ږ��v�ږږ��ڒ�ڈ��ڙZ�ڑ��ڐڔږ�  �ڍK�ڎҖڐ���  ��  "
	"��  ��  �ڊ��ڋM�ڈ���谖ڙ@�ڌۖڌ��ږ��ؐǒ��i��冖���������"
	"�΍H�ΐؐΔ�ΜA�Ί����ݐΌ\�Α��ΉԔg�ΐΕ��Η��Μ_�Γ��Β�ΐr"
	"�Λݐ�毐΍��΍��ΑC�ΓW�Δn�ΝӐΕi�ΐ��ΐӐΐ��ꟐΓo�Ή��΋`"
	"���c�΋^�ΗѐΞَ������i�������o���Ꭶ�K���T�������_���_���򎦊�"
	"���Ď��d���L������  �b���烈�є�щ��ы���  �і��ѝۉящё��ђ�"
	"�ю��щ񎦉�љf�щP�э��ш�їe�ьb�э҉њ��эΉы}�ыT�������|"
	"���䌊�c���^�������N�������D��  ���ʌ��Ȍ������l�����������̌���"
	"���\���痧�����ї��藧����  ���S���ᗧ�����՗��Њ�  ��  ��  �b  "
	"��  ��  ��  ��  ��  �{  ��  ��  �  ��O�{  ��  �S  ��  ��  �C  "
	"��  ��  ��  ��  ��  ��  �Y  ��  ��  ��  ��  ��  ��  ��  �  ��  "
	"��  ��  �c  ��  �Ì���  �  �i  �t  ��  ��  ��  �I  ��  ��  ��  "
	"��  �L  ��  ��  ��  ��  �M  ��  ��  ������  ��  ���@��  ��  ��  "
	"��  ��  ��  ��  ��  ��  ��  �  ��  �ď\�Ĕ�Čˉ����č����ĕĎ�"
	"�ē��ĕS�čX�^�ĕė��ėǕđ��ď@�ĉԕď_�Đr�ĉƕėp�Ė��Ĕ���  "
	"�Ď����ݕĎG��  ���������������]���䎅�����c���L�D���������䎅��"
	"���������������s��  ���^�@�����g���󎅛ݎ��G�������Î��C���E����"
	"�ˎ��������ю��V���쎅�������������ʛ�n���ӎ��ߎ���������������"
	"���i���v��  �������Ў��v���n���Ɏ����c  ���J���W���ǎ�  ���c����"
	"�������֎��[�������K���ݎ��A���_���厅�U��꟎��Ԏ������K���\��  "
	"���p�������Ď��Ď��o���h���Č��Ŏ��^���̎��n�i�Ŏ��d���Ǝ�������"
	"�Ŏ������ʍH�ʚ��  �ʊL��  �ʓ܊�谙c����  �c�S�s  ��  ��  ��  "
	"��  ��  ��  ��  �F  ��  �v�n�v��r�Y�r  ���N�r���r�ߗr�N�r  �r  "
	"�r���r���r���r  �r���x�H���H���H���H�r�H��H�O�H�G�H���HꟉH�Ԕ�"
	"�V�|�V�јV���і��Ή]�Δb�ΘC�Ώ��ΐJ���Ύ��~���K���ߎ��㎨�R��  "
	"�m�����莨�O�]��������  ��  ���J��  ���L�q�䒷����쌎�H���S���y"
	"�����������ތ��b���D���ጎ�����R�����������i�����������݌������C"
	"���ጎ�錎�����T���ڌ��񌎕{���󌎌����v�������݌��k���M��魌���"
	"���ӌ������݌������ጎ���������䌎�v���Ԍ������\�����������a���"
	"���匎�@���Č����������l������Ḍ��������b�莺  ���`�P�l�P�O�P�t"
	"�P�j�P�Z�P谍���㎁�q��M���M���M�M�R�M�t�M�q�M�ՏM��M���M��"
	"�M�`�M���M�֏M�D�MḏM�ˊv���L�F��  ++����  �S  ��  �t  ��  ��  "
	"��  ��  ��  �f  ��  �~  ��  ��  ��  �t  ��  ��  �K  ��  ��  ��  "
	"��  ��  �e  ��  ��  �{  �@  ��  ��  ��  �  ��  ������  ��  ��  "
	"��  ��  ��  ��  ��  ��  ��  �]���e  ��  �E  �G  ��  ��  ��  ��  "
	"������  �f  ��  ��  ��  ��  ��  ��  �g  ��  ��䙖A  ��  ��O�k  "
	"��  ��  �d�R��  ��  ��  ��  �R  ��  �  �i  ��  �b  ��  �[  ��  "
	"�{  �D  ��  ���M��  ��  䞋��  ���V��  �C  �J  �`  ��  ��  ��  "
	"��  �h  ��  ��  �Q  �`  ��  ��  ��  �H�c��  �w  �  ��  �q  �S  "
	"�~  �P  ��  �e��\  �j�c��  ������  ��  �h  �K����  �  �a  ��  "
	"��  �ΐ̐�  ��  ��  �e  ���[�|����  �t  �y  �D  �ы��p  ��  ���"
	"�  ��  �N  �{  ��  �㎵�Ҏs�g�������g����  �����������R���l����"
	"���L���Ò��u���Ò������R���ߒ��ݒ���񒎐㒎�}�������𒎎钎�V"
	"�������������������C���[�������t���]���T�������Ւ�������������"
	"�������񒎓c���ݒ��D���Ȓ��Ւ��Ӓ��c���i���璎�G���`���q���䒎�"
	"�u  �������Y���������~�������풎谒��͒������������������K���"
	"���i���Ԓ���噌������������c���z��  ���h���ǒ������t���峎M��  "
	"��  ��  �������N���n�s���s��s  �s调l�c��  ���߈�  �l���l�p�єC"
	"�l���l��l�c�l�U�l���l���l���l��_��߃l�\�l���㉺�l�s�ߙg����"
	"�l�N��ߗ�  �݌T�l�Ք�߃l�ڃl�Ѓl�ȃl�R�l�G�l�ۈ�  ��  �l�J�l��"
	"�l�Ճl���\  ���߃l�K�l�K�l�d�l�P�l�c焈߃l���l�ăl�g�l�̃l�e�l�"
	"�l���k  �G���G���G�n��  ���茩�ތ��Ҍ��`��毌��i��  �o  ��  ����"
	"��谊p�Z�p���p���p�r�p��p册��g��������H�������匾��������"
	"�������䌾����  ���D���댾�@���{���錾�����Ό������������������T"
	"���ь��ތ�����ौ��P���茾�j�������F�������茾�s���錾�R�������G"
	"���k���F�������v���a���c���K���ӌ��ʌ������v�����������֌�������"
	"��਌��􌾖����o���֌��֌�����  ���i��焌��K�������o���̌���谒N"
	"���ƌ���������谌��^�J��Q�J�J  �R�����������H���s��泌�泎Ҝc  "
	"涍�涏�涊e涋x涕S涗�涙Z涔�涔�涔��L���L���L���L��L���L��"
	"�L��L�R��  �L  �L����  �L�ҊL���L���L��L���L�L�^�L���L�ӊL��"
	"�LᶊL�ĊL��L���L�i�L�̐Ԕ��ԎҔV  ���S�������ё��x���~���v����"
	"�������Α������瑫�����둫�}�������摫�Ǒ������Ǒ������ʑ�������"
	"���m���_���d���`���E��  �����������q���ґ��P���ʑ��]�������@����"
	"���L���Α����������i������冑�焑��đ��摫�����A���o���呫�W����"
	"�g�|�g�{�g�L�g�S�g���g��g��Ԃ��Ԗ��  �Ԏ��ԉԜc�Ԏ��Ԏ��Ԋe"
	"�ԛݎԖ�Ԏ�ԖƎ��V�Ԗ��ԗ��ԗ��ԕv�ԑt�ԓc�Ԛ���͎ԍ��ԓW�Ԑ�"
	"�Ԑ�Ԏ��ԋ��Ԋ��Ԏ��Ԟَԗ���Ḑh  ���h�h���h  �h����  ��  �c  "
	"��  �R  �O  ��  ��  �@  ��  ��  ��  ��  ��  ��  ��  ��  �Y  ��  "
	"��  �y�X�h  ��  ��  �R���c  �U  ��  ��  �`  ��  �i  ��  ��  ��  "
	"�  �@  ��  ��  �h  ��  ��  ��  ��  ��  ��  ��  �u  ��  ��  �J  "
	"�˔b�t  ��  �  ��  �d  �đC�ђ��ћ��џt�ъÓщѓі��ш�ђ�ѐ�"
	"�ё��їѓюM�ш�юM�ќc��泓іL����������  �їR�эK��  ��������"
	"�������͋��n����������ً��ҋ������ڋ��N�����������Ë�����������"
	"�������΋��c������ΓB�s������S������K������G���ы�䊋�������"
	"�������ŋ��V������ो������F���ԋ����������c�������`���r���v����"
	"�������e���������������a���h�����������֋��c���A���K������������"
	"��Ɤ��ԋ���������貋����������c�����  ������A���ً������l���"
	"���ˋ�Ꞌ�谋�����^���^�����������ǋ�  ���剺��R�啶�吅��b"
	"��s��ʖ�\�勤��C�剗��P�剂�刽�劈����L��Җ�j��  ���"
	"��n��  ��d��B��焐�  ��  ��  �~  ��  �S  �L  ��  �P  ��  ���y"
	"��  ��  ��  ��  ��  �c  �v  ��  �S  �@  ��  �B  �n  ��  ��  ����"
	"�  �d�b����  ��谛�调l�W��谉J谎�谉J��J�щJ���J���J�Z�J��"
	"�J���J��J�щJ�p�J���J���J�U�J焉J�ĉJ漉J�y�_�߉J�މJ���_�����"
	"����ʕ�ʌ��ʉ}�v�͊v���v�n�v���v���v�U�v��v���v���v�\�v�}�v��"
	"�v�B�v���v�H�v�_�v䧊v�p�v�B�v�J�v�̃�����P��  ������ꉹ������"
	"���٘��Ō��śݕŐb�ŋg�ŊܕœÕŉʕŕF�Ŏv�Ř��œ��ŕp��ḕ�谕�"
	"��ŉ����������䕗��[�����[�����H�ԐH��H��H���H�ÐH��H�]�H�P"
	"�H�F�H���H���H��H�ӐH�e�H�S�H���H�M�H���H�֐H��H�M�H���HꟐH�F"
	"�H�j���񈽍���n���q�n�n���n�l�n�j�n���n��n�z�n��n��n�e�n�B"
	"�n�۔n�Z�n�R�n���n谔n��n�G�n�ǔn�a�n���n�Q�n��n�[�n�ݔn���n�"
	"�n�K�n�@�n��nḔn�b�n���n谔n���n�퍜�����t���e���ڍ��K��再���"
	"���L���i���c镖�镕�镖�镙f镏�镚�镕�镕�镋�镋g镏�镙�镐{"
	"镈�镕o镑l||++馎s馋�香Z馐��  ���q���c���S�S���ϋS�S㦋S��"
	"�S  �S�}�������ዛ������΋��~�������닛�|���x���n�������ы���"
	"���������T���Ջ��Z�������V���񋛎Q�������Ջ��O�����������c���v"
	"���U���t�������狛�j���Ћ��M���M��  ���t�������_���N�����͋��["
	"���싛�\���L���{��Ḓ��{���{�����咹�ʒ��ʒ����훗��������������"
	"�Ò��������������ߒ��𒹍��������s�������䒹�䒹�p�������ޒ��L��"
	"�̒��������������ڒ��钹�����R����  �H����  �~�������㒹�Ғ�����"
	"�^���ʒ��������������Œ��Ԓ�������  �d��谒�������}�b���b�Ď�  "
	"���厭�Ď�����������    ���˘Қ��m�v���v�m  �m���  �і��t��"
	"�t�׍������o���荕�c���g�������������������}���̋�  �  ꃕ���  "
	"��ꆝ�ꆓ���ۓ~�V�p�l�R�@����  ��  ꏃqꏋ�ꏊ�ꏏ�ꏗ�ꏍ�ꏌ_"
	"ꏌ�ꏑ�ꏉ���Zꏙ������T  ���Ӌ�  ��  �^�ʃ^���g  �b��";

/*
 * ���������蕶�̕���������
 */
static int
kanji2index(c1, c2)
unsigned char c1, c2;
{
#if (('��')==0x8181)
	/* SJIS */
	if (c1 >= 0xe0) c1 -= 0x40;
	c1 -= 0x81;
	if (c2 >= 0x80) c2--;
	c2 -= 0x40;
	return ((int)c1 * (0xfc - 0x40 + 1 - 1)) + (int)c2;
#else
	/* EUC */
    	return (((int)c1 & 0x7f) - 0x21) * (0x7e - 0x21 + 1) +
	       (((int)c2 & 0x7f) - 0x21);
#endif
}

int
jrubout(engr, nxt, use_rubout, select_rnd)
     char *engr;
     int nxt;
     int use_rubout;
     int select_rnd;
{
    int j;
    unsigned char *s;
    const unsigned char *p;

    if(is_kanji2(engr, nxt)){
	return 1;
      }

    s = (unsigned char *)&engr[nxt];
    if (*s == ' ') return 1;

    if(!is_kanji1(engr, nxt)) return 0;

    j = kanji2index(engr[nxt], engr[nxt + 1]);
    if (j >= 0x0000 && j <= 0x02B1) p = &ro0[j << 2];
    else if (j >= 0x0582 && j <= 0x1116) p = &ro1[(j - 0x0582) << 2];
    else if (j >= 0x1142 && j <= 0x1E7F) p = &ro2[(j - 0x1142) << 2];
    else p = "�H";
    
    if (p[2] != ' ' || p[3] != ' ') p += select_rnd * 2;
    engr[nxt] = *p++;
    engr[nxt + 1] = *p;

    return 1;
}
