/*
**
**	$Id: jconj.c,v 1.11 2005/06/10 22:02:04 argrath Exp $
**
*/

/* Copyright (c) Issei Numata 1994-2000 */
/* JNetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define EUC	0
#define SJIS	1
#define JIS	2

/* internal kcode */
/* IC=0 EUC */
/* IC=1 SJIS */
#define IC ((unsigned char)("��"[0])==0x8a)

#define J_A	0
#define J_KA	(1*5)
#define J_SA	(2*5)
#define J_TA	(3*5)
#define J_NA	(4*5)
#define J_HA	(5*5)
#define J_MA	(6*5)
#define J_YA	(7*5)
#define J_RA	(8*5)
#define J_WA	(9*5)

#define J_GA	(10*5)
#define J_ZA	(11*5)
#define J_DA	(12*5)
#define J_BA	(13*5)
#define J_PA	(14*5)

static unsigned char hira_tab[][2]={
    {0xa4, 0xa2}, {0xa4, 0xa4}, {0xa4, 0xa6}, {0xa4, 0xa8}, {0xa4, 0xaa}, 
    {0xa4, 0xab}, {0xa4, 0xad}, {0xa4, 0xaf}, {0xa4, 0xb1}, {0xa4, 0xb3}, 
    {0xa4, 0xb5}, {0xa4, 0xb7}, {0xa4, 0xb9}, {0xa4, 0xbb}, {0xa4, 0xbd}, 
    {0xa4, 0xbf}, {0xa4, 0xc1}, {0xa4, 0xc4}, {0xa4, 0xc6}, {0xa4, 0xc8}, 
    {0xa4, 0xca}, {0xa4, 0xcb}, {0xa4, 0xcc}, {0xa4, 0xcd}, {0xa4, 0xce}, 
    {0xa4, 0xcf}, {0xa4, 0xd2}, {0xa4, 0xd5}, {0xa4, 0xd8}, {0xa4, 0xdb}, 
    {0xa4, 0xde}, {0xa4, 0xdf}, {0xa4, 0xe0}, {0xa4, 0xe1}, {0xa4, 0xe2}, 
    {0xa4, 0xe4}, {0xa4, 0xa4}, {0xa4, 0xe6}, {0xa4, 0xa8}, {0xa4, 0xe8}, 
    {0xa4, 0xe9}, {0xa4, 0xea}, {0xa4, 0xeb}, {0xa4, 0xec}, {0xa4, 0xed}, 
    {0xa4, 0xef}, {0xa4, 0xa4}, {0xa4, 0xa6}, {0xa4, 0xa8}, {0xa4, 0xaa}, 
    {0xa4, 0xac}, {0xa4, 0xae}, {0xa4, 0xb0}, {0xa4, 0xb2}, {0xa4, 0xb4}, 
    {0xa4, 0xb6}, {0xa4, 0xb8}, {0xa4, 0xba}, {0xa4, 0xbc}, {0xa4, 0xbe}, 
    {0xa4, 0xc0}, {0xa4, 0xc2}, {0xa4, 0xc5}, {0xa4, 0xc7}, {0xa4, 0xc9}, 
    {0xa4, 0xd0}, {0xa4, 0xd3}, {0xa4, 0xd6}, {0xa4, 0xd9}, {0xa4, 0xdc}, 
    {0xa4, 0xd1}, {0xa4, 0xd4}, {0xa4, 0xd7}, {0xa4, 0xda}, {0xa4, 0xdd},
};

#define FIFTH	0 /* �ܒi */
#define UPPER	1 /* ���i */
#define LOWER	2 /* ����i */
#define SAHEN	3 /* �T�� */
#define KAHEN	4 /* �J�� */

#define NORMAL	0 /* �����遨�`�� */
#define SOKUON	1 /* ���ށ��`�� */
#define HATSUON	2 /* �ł��`���� */
#define ION	3 /* �������`���� */

struct _jconj_tab {
    const char *main;
    int column;
/* 0: fifth conj. 1:upper conj. 2:lower conj. 3:SAHEN 4:KAHEN */
    int katsuyo_type;
/* 0: normal 1: sokuon 2: hatson 3: ion */
    int onbin_type;
} jconj_tab[] = {
/* �� */
    {"������", J_KA, LOWER, NORMAL},
    {"�J����", J_KA, LOWER, NORMAL},
    {"�����", J_RA, LOWER, NORMAL},
    {"�����", J_RA, LOWER, NORMAL},
    {"����", J_KA, FIFTH, ION},
    {"�ł�", J_TA, FIFTH, HATSUON},
    {"�u��", J_KA, FIFTH, ION},
    {"�[�߂�", J_MA, LOWER, NORMAL},
    {"������", J_TA, UPPER, NORMAL},
    {"����", J_SA, FIFTH, NORMAL},
/* �� */
    {"����", J_KA, FIFTH, ION},
    {"������", J_KA, UPPER, NORMAL},
    {"���Ԃ�", J_RA, FIFTH, HATSUON},
    {"�\����", J_A, LOWER, NORMAL},
    {"���݂�", J_KA, FIFTH, ION},
    {"����", J_MA, FIFTH, SOKUON},
    {"����", J_KA, UPPER, NORMAL},
    {"����", J_KA, KAHEN, NORMAL}, 
    {"�ӂ�", J_KA, FIFTH, ION}, 
    {"������", J_A, LOWER, NORMAL},
    {"������", J_RA, FIFTH, HATSUON},
    {"���܂�", J_SA, FIFTH, NORMAL},
    {"����", J_MA, FIFTH, SOKUON},
    {"�E��", J_SA, FIFTH, NORMAL},
    {"��", J_SA, FIFTH, NORMAL},
/* �� */
    {"������", J_KA, LOWER, NORMAL},
    {"�K�т�", J_BA, UPPER, NORMAL},
    {"����", J_NA, FIFTH, SOKUON},
    {"����", J_RA, FIFTH, HATSUON},
    {"����", J_SA, SAHEN, NORMAL}, 
/* �� */
    {"�o��", J_SA, FIFTH, NORMAL},
    {"�H�ׂ�", J_HA, LOWER, NORMAL}, 
    {"�g��", J_WA, FIFTH, HATSUON},
    {"����", J_KA, LOWER, NORMAL},
    {"�܂���", J_KA, FIFTH, ION},
    {"�o��", J_NA, LOWER, NORMAL},
    {"����", J_KA, FIFTH, ION},
    {"�n����", J_KA, LOWER, NORMAL},
    {"�Ƃ΂�", J_SA, FIFTH, NORMAL},
    {"���", J_BA, FIFTH, SOKUON},
    {"���", J_RA, FIFTH, HATSUON},
/* �� */
    {"������", J_GA, LOWER, NORMAL},
    {"���Â���", J_KA, LOWER, NORMAL},
    {"����", J_RA, FIFTH, HATSUON},
    {"�E��", J_GA, FIFTH, ION},
    {"�G�炷", J_SA, FIFTH, NORMAL},
    {"�h��", J_RA, FIFTH, HATSUON},
    {"����", J_MA, FIFTH, SOKUON},
/* �� */
    {"�͂�����", J_RA, FIFTH, HATSUON},
    {"����", J_KA, FIFTH, ION},
    {"�͂���", J_MA, FIFTH, SOKUON},
    {"�͂���", J_SA, FIFTH, NORMAL},
    {"�O��", J_SA, FIFTH, NORMAL},
    {"�͂߂�", J_MA, UPPER, NORMAL},
    {"�Z��", J_SA, FIFTH, NORMAL},
    {"�Ђ�������", J_KA, LOWER, NORMAL},
    {"�E��", J_WA, FIFTH, HATSUON},
    {"����", J_MA, FIFTH, SOKUON},
    {"�U�肩����", J_SA, FIFTH, NORMAL},
    {"�@��", J_RA, FIFTH, HATSUON},
/* �� */
    {"����", J_KA, FIFTH, ION},
    {"���", J_RA, FIFTH, HATSUON},
    {"��", J_SA, FIFTH, NORMAL},
    {"�g�ɂ���", J_KA, LOWER, NORMAL},
    {"����", J_TA, FIFTH, HATSUON},
/* �� */
    {"�Ă�", J_KA, FIFTH, ION},
    {"�Ă�", J_BA, FIFTH, SOKUON},
    {"�ǂ�", J_MA, FIFTH, SOKUON},
/* �� */
/* �� */
    {(void*)0, 0, 0, 0},
};

extern unsigned char *e2sj(unsigned char *s);
extern unsigned char *sj2e(unsigned char *s);

/*
**	conjection verb word
**
**	Example
**	arg1	arg2	result
**	�E��	�Ȃ�	�E���Ȃ�
**	�E��	��	�E����
**
*/
static char *
jconjsub( tab, jverb, sfx )
     struct _jconj_tab *tab;
     char *jverb;
     char *sfx;
{
    int len;
    unsigned char *p;
    static unsigned char tmp[1024];

    len = strlen(jverb);
    strcpy((char *)tmp, jverb );

    if(!strncmp(sfx, "��", 2)){
	strcat((char *)tmp, sfx);
	return (char *)tmp;
    }

    switch( tab->katsuyo_type ){
      case FIFTH:
	p = tmp + (len - 2);
	if(!strncmp(sfx, "��", 2)){
	    if(!IC){
		p[0] = 0xa4;
		p[1] = hira_tab[tab->column][1];
	    } else {
	      memcpy(p, e2sj(hira_tab[tab->column]), 2);
	    }

	    strcpy((char *)p + 2, sfx);
	    break;
	}
	else if(!strncmp(sfx, "��", 2) || !strncmp(sfx, "��", 2)){
	    switch( tab->onbin_type ){
	      case NORMAL:
		if(!IC){
		    p[1] = hira_tab[tab->column + 1][1];
		} else {
		    memcpy(p, e2sj(hira_tab[tab->column + 1]), 2);
		}
		break;
	      case SOKUON:
		if(!IC){
		    p[1] = 0xf3;
		} else {
		    memcpy(p, "��", 2);
		}
		break;
	      case HATSUON:
		if(!IC){
		    p[1] = 0xc3;
		} else {
		    memcpy(p, "��", 2);
		}
		break;
	      case ION:
		if(!IC){
		    p[1] = 0xa4;
		} else {
		    memcpy(p, "��", 2);
		}
		break;
	    }
	    strcpy((char *)p + 2, sfx);
	    if(tab->onbin_type == SOKUON ||
	       (tab->onbin_type == ION && tab->column >= J_GA)){
		if(!IC){
		  ++p[3];
		} else {
		  ++p[3];
		}
/*	  memcpy(p+2, e2sj(sj2e(p+2)+1), 2);*//* sj2e() returns ptr to char* */
	    }
	    break;
	}
	else if(!strncmp(sfx, "��", 2)){
	    if(!IC){
		p[1] = hira_tab[tab->column + 3][1];
	    } else {
		memcpy(p, e2sj(hira_tab[tab->column + 3]), 2);
	    }
	    strcpy((char *)p + 2, sfx);
	}
	else if(!strncmp(sfx, "��", 2)){
	    if(!IC){
		p[1]=hira_tab[tab->column + 3][1];
	    } else {
		memcpy(p, e2sj(hira_tab[tab->column + 3]), 2);
	    }
	    strcpy((char *)p + 2, sfx + 2);
	}
	else if(!strncmp(sfx, "��", 2)) {
	    if(!IC){
		p[1] = hira_tab[tab->column + 1][1];
	    } else {
		memcpy(p, e2sj(hira_tab[tab->column + 1]), 2);
	    }
	    strcpy((char *)p + 2, sfx);
	    break;
	}
	else if(!strncmp(sfx, "��", 2)) {
	    if(!IC){
		p[1] = hira_tab[tab->column + 4][1];
	    } else {
		memcpy(p, e2sj(hira_tab[tab->column + 4]), 2);
	    }
	    strcpy((char *)p + 2, sfx + 2);
	    break;
	}
	break;
      case LOWER:
      case UPPER:
      case KAHEN:
	p = tmp + (len - 2);
	if(!strncmp(sfx, "��", 2)){
	    strcpy((char *)p, "��");
	    strcpy((char *)p + 2, sfx);
	}
	else if(!strncmp(sfx, "��", 2) && tab->katsuyo_type == LOWER){
	    strcpy((char *)p, "��");
	    strcpy((char *)p + 2, sfx);
	}
	else
	  strcpy((char *)p, sfx);
	break;
      case SAHEN:
	p = tmp + (len - 4);
	if(!strncmp(sfx, "��", 2) ||
	   !strncmp(sfx, "��", 2) ||
	   !strncmp(sfx, "��", 2) ||
	   !strncmp(sfx, "��", 2) ||
	   !strncmp(sfx, "��", 2)){
	    strcpy((char *)p, "��");
	    strcpy((char *)p + 2, sfx);
	}
	else if(!strncmp(sfx, "��", 2) || !strncmp(sfx, "���", 4)){
	    strcpy((char *)p, "�����");
	}
	break;
    }
    return (char *)tmp;
}

/* �����̕ω� */
const char *
jconj( jverb, sfx )
     const char *jverb;
     const char *sfx;
{
    struct _jconj_tab *tab;
    int len;

    len = strlen(jverb);
    for( tab = jconj_tab; tab->main != (void*)0; ++tab){
	if(!strcmp(jverb, tab->main)){
	    return jconjsub(tab, jverb, sfx);
	}
    }

    for( tab = jconj_tab; tab->main != (void*)0; ++tab){
	if(len - strlen(tab->main) > 0 &&
	   !strcmp(jverb + (len - strlen(tab->main)), tab->main)){
	    return jconjsub(tab, jverb, sfx);
	}
    }

#ifdef JAPANESETEST
    fprintf( stderr, "I don't know such word \"%s\"\n");
#endif
    return jverb;
}

/* �\ */
const char *
jcan(jverb)
     const char *jverb;
{
    const char *ret;
    static unsigned char tmp[1024];

    int len = strlen(jverb);
    if(!strcmp(jverb + len - 4, "����")){
	strncpy(tmp, jverb, len - 4);
	strcpy(tmp + len - 4, "�ł���");
	return tmp;
    } else {
	ret = jconj(jverb, "���");
	return ret;
    }
}

/* �s�\ */
const char *
jcannot(jverb)
     const char *jverb;
{
    static unsigned char tmp[1024];

    int len = strlen(jverb);
    if(!strcmp(jverb + len - 4, "����")){
	strncpy(tmp, jverb, len-4);
	strcpy(tmp +len-4, "�ł��Ȃ�");
	return tmp;
    } else {
	return jconj(jverb, "��Ȃ�");
    }
}

/* �ߋ� */
const char *
jpast(jverb)
     const char *jverb;
{
    return jconj(jverb, "��");
}

/* �h�� */
const char *
jpolite(jverb)
     const char *jverb;
{
    return jconj(jverb, "�܂�");
}


/*
**	conjection of adjective word
**
**	Example:
**
**	�`�e���I�p�@	   �����I�p�@
**
**	�Ԃ�		-> �Ԃ�		(�`�e��)
**	�Y���		-> �Y���	(�`�e����)
**	�Y�킾		-> �Y���	(�`�e����)
*/
const char *
jconj_adj( jadj )
     const char *jadj;
{
    int len;
    static unsigned char tmp[1024];

    strcpy((char *)tmp, jadj);
    len = strlen((char *)tmp);

    if(!strcmp((char *)tmp + len - 2, "��")){
	strcpy((char *)tmp + len - 2, "��");
    } else if(!strcmp((char *)tmp + len - 2, "��") ||
	      !strcmp((char *)tmp + len - 2, "��") ||
	      !strcmp((char *)tmp + len - 2, "��")){
	strcpy((char *)tmp + len - 2, "��");
    }

    return (char *)tmp;
}


#ifdef JAPANESETEST
unsigned char
*e2sj(unsigned char *s)
{
    return *s;
}

unsigned char
*sj2e(unsigned char *s)
{
    return *s;
}

void
main()
{
    struct _jconj_tab *tab;

    for(tab = jconj_tab; tab->main != (void*)0; ++tab){
	printf("%-10s �Ȃ� %s\n", tab->main, jconj(tab->main, "�Ȃ�"));
	printf("%-10s �܂� %s\n", tab->main, jconj(tab->main, "�܂�"));
	printf("%-10s ��   %s\n", tab->main, jconj(tab->main, "��"));
	printf("%-10s ��� %s\n", tab->main, jconj(tab->main, "���"));
	printf("%-10s �Ƃ� %s\n", tab->main, jconj(tab->main, "�Ƃ�"));
	printf("%-10s �悤 %s\n", tab->main, jconj(tab->main, "�悤"));
	printf("%-10s %s\n", tab->main, jcan(tab->main));
	printf("%-10s %s\n", tab->main, jcannot(tab->main));
    }
    printf("%s\n", jconj("�O���nethack�̖|�������", "�Ȃ�"));
    printf("%s\n", jconj("�O���nethack�̖|�������", "�܂�"));
    printf("%s\n", jconj("�O���nethack�̖|�������", "��"));
    printf("%s\n", jconj("�O���nethack�̖|�������", "���"));
    printf("%s\n", jconj("�O���nethack�̖|�������", "�Ƃ�"));
}
#endif
