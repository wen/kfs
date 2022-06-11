#ifndef PRINTKLOCAL_H
#define PRINTKLOCAL_H

#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include "tty.h"
#include "string.h"

#define BUF			32
#define U_LONG		unsigned long
#define FLAGS		"#0*- +"
#define MOD        	"hljz"
#define TYPES		"diouxXDOUcsp"
#define ALT			0x001
#define LADJUST		0x004
#define LONGDBL		0x008
#define LONGINT		0x010
#define LLONGINT	0x020
#define SHORTINT	0x040
#define ZEROPAD		0x080
#define FPT			0x100
#define SIZET		0x400
#define INTMAXT		0x1000
#define CHARINT		0x2000
#define INTMAX_SIZE	(INTMAXT|SIZET|LLONGINT)

#define XDIGS_LOWER	"0123456789abcdef"
#define XDIGS_UPPER	"0123456789ABCDEF"

#define PADSIZE		16
#define BLKS {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}
#define ZERO {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'}

typedef long ssize_t;

typedef struct	s_pf
{
	char		*fmt;
	int			ch;
	int			n;
	int			n2;
	char		*cp;
	int			flags;
	int			ret;
	int			width;
	int			prec;
	char		sign;
	U_LONG		ulval;
	uintmax_t	ujval;
	int			base;
	int			dprec;
	int			realsz;
	int			size;
	int			prsize;
	const char	*xdigs;
	char		buf[BUF];
	char		ox[2];
	va_list		ap;
	char		*dst;
	int			cursor;
}				t_pf;

char *ujtoa(t_pf *pf, uintmax_t val, char *endp);
char *ultoa(t_pf *pf, U_LONG val, char *endp);

#endif
