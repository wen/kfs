#ifndef PRINTFLOCAL_H
#define PRINTFLOCAL_H
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include "libft.h"
#define BUF			32
#define U_LONG		unsigned long
#define FLAGS		"#0*- +"
#define MOD        	"hljz"
#define TYPES		"diouxXDOUcspn"
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

typedef struct	s_pf
{
	int			fd;
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
	t_vec		*v;
	char		*dst;
	int			cursor;
}				t_pf;

void		pf_init1(t_pf *pf);
void		pf_parse(t_pf *pf);
void		pf_parse_flags(t_pf *pf);
void		pf_parse_precision_width(t_pf *pf);
void		pf_parse_mod(t_pf *pf);
void		pf_parse_type(t_pf *pf);
void		pf_get_cc(t_pf *pf);
void		pf_get_ss(t_pf *pf);
void		pf_get_p(t_pf *pf);
void		pf_set_n(t_pf *pf);
void		pf_number(t_pf *pf);
void		pf_nosign(t_pf *pf);
void		pf_print(t_pf *pf);
void		spf_print(t_pf *pf);
void		aspf_print(t_pf *pf);
void		pf_raw_string(t_pf *pf);
void		spf_raw_string(t_pf *pf);
void		aspf_raw_string(t_pf *pf);
long		sarg(t_pf *pf, int flags);
U_LONG		uarg(t_pf *pf, int flags);
intmax_t	sjarg(t_pf *pf, int flags);
uintmax_t	ujarg(t_pf *pf, int flags);
void		getaster(t_pf *pf, int *val);
char		*ultoa(t_pf *pf, U_LONG val, char *endp);
char		*ujtoa(t_pf *pf, uintmax_t val, char *endp);

#endif
