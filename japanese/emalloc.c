
/* EMS handler routine
   Copyright (c) Kouji Takada 1994-2000 */
/* JNetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include <dos.h>
#include <stdlib.h>
#include "emalloc.h"

#define	EMS_VECT	0x67
#define	EMS_STR		"EMMXXXX0"
static	int isems = 0;
static	unsigned int ems_frame = 0;
static	unsigned long emalloc_ptr = 0;
static	unsigned int ems_handle = 0;

#ifdef MOVERLAY
/* MOVERLAY API */
#define	MOVE_PAUSE_CACHE	4
extern	unsigned short _movefpause;
extern	void _movepause(void);
extern	void _moveresume(void);
#endif

/* ���K�����ꂽ�|�C���^���쐬���� */
static	void *mk_fp(unsigned int seg, unsigned int off)
{
	unsigned long temp;

	temp = (((unsigned long)seg) << 4) + ((unsigned long)off);
	return (void *)(((temp & 0xffff0L) << 12) + (temp & 0x0000fL));
}

/* �v���O�����I�����ɌĂ΂�AEMS ���J������ */
void	done_ems(void)
{
	union REGS inregs, outregs;
	struct SREGS segregs;
	int i;

	if (isems == 0) return;
	inregs.h.ah = 0x44;
	inregs.x.dx = ems_handle;
	inregs.x.bx = 0xffff;
	for (i = 0; i < 4; i++)
	{
		/* �y�[�W���A���}�b�v���� */
		inregs.h.al = i;
		int86(EMS_VECT, &inregs, &outregs);
		if (outregs.h.ah != 0) fprintf(stderr,
			"�x��: EMS ���A���}�b�v�o���܂��� (%04x)�B\n",
			outregs.h.ah & (i << 8));
	}

	/* �y�[�W���J������ */
	inregs.h.ah = 0x45;
	inregs.x.dx = ems_handle;
	int86(EMS_VECT, &inregs, &outregs);
	if (outregs.h.ah != 0)
	{
		fprintf(stderr, "�x��: EMS ���J���o���܂��� (%04x)�B\n",
			outregs.h.ah);
		return;
	}
/*	fprintf(stderr, "EMS �� %ud �o�C�g�g�p����܂����B\n", emalloc_ptr); */
	isems = 0;
}

/* EMS ������������ */
int	detect_ems(void)
{
	union REGS inregs, outregs;
	char *vect;
	int i;

	/* EMS �h���C�o��T�� */
	vect = (char *)_dos_getvect(EMS_VECT);
	vect = mk_fp(FP_SEG(vect), 0x000a);
	if (memcmp(vect, EMS_STR, 8) != 0)
	{
		fprintf(stderr, "EMS �����݂��܂���B\n");
		return 0;
	}

#ifdef MOVERLAY
	/* MOVERLAY ���擾���Ă��� EMS ���J�������� */
        _movefpause |= MOVE_PAUSE_CACHE;
	_movepause();
#endif
	/* �n�[�h�E�F�A�����킩�ǂ����m�F���� */
	inregs.h.ah = 0x40;
	int86(EMS_VECT, &inregs, &outregs);
	if (outregs.h.ah != 0)
	{
		fprintf(stderr, "EMS ���ُ�ł� (%04x)�B\n", outregs.h.ah);
#ifdef MOVERLAY
		_moveresume();
#endif
		return 0;
	}

	/* �S�y�[�W�̋󂫂����邩�m�F���� */
	inregs.h.ah = 0x42;
	int86(EMS_VECT, &inregs, &outregs);
	if (outregs.x.bx < 4)
	{
		fprintf(stderr, "EMS �̃t���[��������܂��� (%04x)�B\n",
			outregs.x.cx);
#ifdef MOVERLAY
		_moveresume();
#endif
		return 0;
	}

	/* �y�[�W�t���[�����S�ȏ゠�邩�ǂ����m�F���� */
	inregs.x.ax = 0x5801;
	int86(EMS_VECT, &inregs, &outregs);
	if (outregs.x.cx < 4)
	{
		fprintf("stderr, EMS �̃t���[��������܂��� (%04x)�B\n", outregs.x.cx);
		_moveresume();
		return 0;
	}

	/* �y�[�W�t���[���̐擪�A�h���X�𓾂� */
	inregs.h.ah = 0x41;
	int86(EMS_VECT, &inregs, &outregs);
	ems_frame = outregs.x.bx;

	/* �S�y�[�W�̃��������m�ۂ��� */
	inregs.h.ah = 0x43;
	inregs.x.bx = 4;
	int86(EMS_VECT, &inregs, &outregs);
	if (outregs.h.ah != 0)
	{
		fprintf(stderr, "EMS ���擾�o���܂��� (%04x)�B\n",
			outregs.h.ah);
#ifdef MOVERLAY
		_moveresume();
#endif
		_moveresume();
		return 0;
	}
	ems_handle = outregs.x.dx;

	/* �y�[�W���}�b�v���� */
	inregs.h.ah = 0x44;
	inregs.x.dx = ems_handle;
	for (i = 0; i < 4; i++)
	{
		inregs.h.al = i;
		inregs.x.bx = i;
		int86(EMS_VECT, &inregs, &outregs);
		if (outregs.h.ah != 0)
		{
			fprintf(stderr, "EMS ���}�b�v�o���܂��� (%04x)�B\n",
				outregs.h.ah & (i << 8));
			inregs.h.ah = 0x45;
			inregs.x.dx = ems_handle;
			int86(EMS_VECT, &inregs, &outregs);
#ifdef MOVERLAY
			_moveresume();
#endif
			return 0;
		}
	}

#ifdef MOVERLAY
	/* MOVERLAY �̃L���b�V�����ĂїL���ɂ��� */
	_moveresume();
#endif
	/* �v���O�����I�����Ɏ����I�ɌĂяo�� */
	atexit(done_ems);

	fprintf(stderr, "\nEMS ���f�[�^�̈�Ƃ��Ďg�p���܂��B\n");
	isems = 1;
	return 1;
}

/* EMS �̈悩��D�悵�ă��������m�ۂ��� */
void	*emalloc(unsigned int size)
{
	void *ptr;

	/* EMS ���g�p�\�Ȃ� */
	if (isems != 0)
	{
		/* EMS �ɋ󂫂������ */
		if ((emalloc_ptr + ((unsigned long)size)) <= 0xffffL)
		{
			/* �|�C���^������ĕԂ� */
			ptr = mk_fp(ems_frame, emalloc_ptr);
			emalloc_ptr += size;
			memset(ptr, 0, size);
			return ptr;
		}
	}
	/* EMS ���烁�������擾�o���Ȃ���΁A�q�[�v���g�� */
	ptr = alloc(size);
	memset(ptr, 0, size);
	return ptr;
}

