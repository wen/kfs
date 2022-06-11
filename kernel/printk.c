#include "printklocal.h"

static void pad(int howmany, const char *with)
{
	int n;

	while (howmany > 0) {
		n = (howmany >= PADSIZE) ? PADSIZE : howmany;
		kwrite(with, n);
		howmany -= n;
	}
}

static void pf_print(t_pf *pf)
{
	static const char blanks[PADSIZE] = BLKS;
	static const char zeroes[PADSIZE] = ZERO;

	pf->realsz = pf->dprec > pf->size ? pf->dprec : pf->size;
	if (pf->sign)
		++pf->realsz;
	if (pf->ox[1])
		pf->realsz += 2;
	pf->prsize = pf->width > pf->realsz ? pf->width : pf->realsz;
	if ((unsigned)pf->ret + pf->prsize > INT_MAX)
	{
		pf->ret = EOF;
		return ;
	}
	if ((pf->flags & (LADJUST | ZEROPAD)) == 0)
		pad(pf->width - pf->realsz, blanks);
	if (pf->sign)
		kwrite(&pf->sign, 1);
	if (pf->ox[1])
	{
		pf->ox[0] = '0';
		kwrite(pf->ox, 2);
	}
	if ((pf->flags & (LADJUST | ZEROPAD)) == ZEROPAD)
		pad(pf->width - pf->realsz, zeroes);
	pad(pf->dprec - pf->size, zeroes);
	kwrite(pf->cp, pf->size);
	if (pf->flags & LADJUST)
		pad(pf->width - pf->realsz, blanks);
	pf->ret += pf->prsize;
}

static void pf_number(t_pf *pf)
{
	if ((pf->dprec = pf->prec) >= 0)
		pf->flags &= ~ZEROPAD;
	pf->cp = pf->buf + BUF;
	if (pf->flags & INTMAX_SIZE) {
		if (pf->ujval != 0 || pf->prec != 0 \
			|| (pf->flags & ALT && pf->base == 8))
			pf->cp = ujtoa(pf, pf->ujval, pf->cp);
	} else {
		if (pf->ulval != 0 || pf->prec != 0 \
			|| (pf->flags & ALT && pf->base == 8))
			pf->cp = ultoa(pf, pf->ulval, pf->cp);
	}
	pf->size = pf->buf + BUF - pf->cp;
}

static void pf_nosign(t_pf *pf)
{
	pf->sign = '\0';
	pf_number(pf);
}

static void getaster(t_pf *pf, int *val)
{
	pf->n2 = 0;
	pf->cp = pf->fmt;
	while (ISDIGIT(*pf->cp))
	{
		pf->n2 = 10 * pf->n2 + TODIGIT(*pf->cp);
		++pf->cp;
	}
	*val = va_arg(pf->ap, int);
}

static void pf_parse_flags(t_pf *pf)
{
	if (pf->ch == ' ' && pf->sign == '\0') {
		pf->sign = ' ';
	} else if (pf->ch == '#') {
		pf->flags |= ALT;
	} else if (pf->ch == '*' || pf->ch == '-') {
		if (pf->ch == '*') {
			getaster(pf, &pf->width);
			if (pf->width >= 0) {
				pf->ch = *pf->fmt++;
				return;
			}
			pf->width = -pf->width;
		}
		pf->flags |= LADJUST;
	} else if (pf->ch == '+') {
		pf->sign = '+';
	} else if (pf->ch == '0') {
		pf->flags |= ZEROPAD;
	}
	pf->ch = *pf->fmt++;
}

static void pf_parse_precision_width(t_pf *pf)
{
	if (pf->ch == '.') {
		if ((pf->ch = *pf->fmt++) == '*') {
			getaster(pf, &pf->prec);
			pf->ch = *pf->fmt++;
			return;
		}
		pf->prec = 0;
		while (ISDIGIT(pf->ch)) {
			pf->prec = 10 * pf->prec + TODIGIT(pf->ch);
			pf->ch = *pf->fmt++;
		}
		return;
	}
	pf->n = 0;
	pf->n = 10 * pf->n + TODIGIT(pf->ch);
	pf->ch = *pf->fmt++;
	while (ISDIGIT(pf->ch)) {
		pf->n = 10 * pf->n + TODIGIT(pf->ch);
		pf->ch = *pf->fmt++;
	}
	pf->width = pf->n;
}

static void pf_parse_mod(t_pf *pf)
{
	if (pf->ch == 'h') {
		if (pf->flags & SHORTINT) {
			pf->flags &= ~SHORTINT;
			pf->flags |= CHARINT;
		} else {
			pf->flags |= SHORTINT;
		}
	} else if (pf->ch == 'j') {
		pf->flags |= INTMAXT;
	} else if (pf->ch == 'l') {
		if (pf->flags & LONGINT) {
			pf->flags &= ~LONGINT;
			pf->flags |= LLONGINT;
		} else {
			pf->flags |= LONGINT;
		}
	} else if (pf->ch == 'z') {
		pf->flags |= SIZET;
	}
	pf->ch = *pf->fmt++;
}

static long sarg(t_pf *pf, int flags)
{
	if (flags & LONGINT)
		return (va_arg(pf->ap, long));
	if (flags & SHORTINT)
		return ((long)(short)va_arg(pf->ap, int));
	if (flags & CHARINT)
		return ((long)(signed char)va_arg(pf->ap, int));
	return ((long)va_arg(pf->ap, int));
}

static U_LONG uarg(t_pf *pf, int flags)
{
	if (flags & LONGINT)
		return (va_arg(pf->ap, U_LONG));
	if (flags & SHORTINT)
		return ((U_LONG)(unsigned short)va_arg(pf->ap, int));
	if (flags & CHARINT)
		return ((U_LONG)(unsigned char)va_arg(pf->ap, int));
	return ((U_LONG)va_arg(pf->ap, unsigned int));
}

static intmax_t sjarg(t_pf *pf, int flags)
{
	if (flags & INTMAXT)
		return (va_arg(pf->ap, intmax_t));
	if (flags & SIZET)
		return ((intmax_t)va_arg(pf->ap, ssize_t));
	return ((intmax_t)va_arg(pf->ap, long long));
}

static uintmax_t ujarg(t_pf *pf, int flags)
{
	if (flags & INTMAXT)
		return (va_arg(pf->ap, uintmax_t));
	if (flags & SIZET)
		return ((uintmax_t)va_arg(pf->ap, size_t));
	return ((uintmax_t)va_arg(pf->ap, unsigned long long));
}


static void	pf_get_ddi(t_pf *pf)
{
	if (pf->ch == 'D')
		pf->flags |= LONGINT;
	if (pf->flags & INTMAX_SIZE)
	{
		pf->ujval = sjarg(pf, pf->flags);
		if ((intmax_t)pf->ujval < 0)
		{
			pf->ujval = -pf->ujval;
			pf->sign = '-';
		}
	}
	else
	{
		pf->ulval = sarg(pf, pf->flags);
		if ((long)pf->ulval < 0)
		{
			pf->ulval = -pf->ulval;
			pf->sign = '-';
		}
	}
	pf->base = 10;
	pf_number(pf);
}

static void	pf_get_oo(t_pf *pf)
{
	if (pf->ch == 'O')
		pf->flags |= LONGINT;
	if (pf->flags & INTMAX_SIZE)
		pf->ujval = ujarg(pf, pf->flags);
	else
		pf->ulval = uarg(pf, pf->flags);
	pf->base = 8;
	pf_nosign(pf);
}

static void	pf_get_uu(t_pf *pf)
{
	if (pf->ch == 'U')
		pf->flags |= LONGINT;
	if (pf->flags & INTMAX_SIZE)
		pf->ujval = ujarg(pf, pf->flags);
	else
		pf->ulval = uarg(pf, pf->flags);
	pf->base = 10;
	pf_nosign(pf);
}

static void	pf_get_xx(t_pf *pf)
{
	pf->xdigs = (pf->ch == 'X' ? XDIGS_UPPER : XDIGS_LOWER);
	if (pf->flags & INTMAX_SIZE)
		pf->ujval = ujarg(pf, pf->flags);
	else
		pf->ulval = uarg(pf, pf->flags);
	pf->base = 16;
	if (pf->flags & ALT \
		&& (pf->flags & INTMAX_SIZE ? pf->ujval != 0 : pf->ulval != 0))
		pf->ox[1] = pf->ch;
	pf_nosign(pf);
}

static void pf_get_cc(t_pf *pf)
{
	pf->cp = pf->buf;
	*pf->cp = va_arg(pf->ap, int);
	pf->size = 1;
	pf->sign = '\0';
}

static void pf_get_ss(t_pf *pf)
{
	if ((pf->cp = va_arg(pf->ap, char *)) == NULL)
		pf->cp = "(null)";
	pf->size = (pf->prec >= 0) ? strnlen(pf->cp, pf->prec) : strlen(pf->cp);
	pf->sign = '\0';
}

static void pf_get_p(t_pf *pf)
{
	pf->ujval = (uintmax_t)((uintptr_t)va_arg(pf->ap, void *));
	pf->base = 16;
	pf->xdigs = XDIGS_LOWER;
	pf->flags |= INTMAXT;
	pf->ox[1] = 'x';
	pf_nosign(pf);
}

static void pf_parse_type(t_pf *pf)
{
	if (pf->ch == 'c')
		pf_get_cc(pf);
	else if (pf->ch == 'D' || pf->ch == 'd' || pf->ch == 'i')
		pf_get_ddi(pf);
	else if (pf->ch == 'O' || pf->ch == 'o')
		pf_get_oo(pf);
	else if (pf->ch == 'p')
		pf_get_p(pf);
	else if (pf->ch == 's')
		pf_get_ss(pf);
	else if (pf->ch == 'U' || pf->ch == 'u')
		pf_get_uu(pf);
	else if (pf->ch == 'X' || pf->ch == 'x')
		pf_get_xx(pf);
}

static void pf_parse(t_pf *pf)
{
	while (1) {
		if (pf->ch != '\0' && strchr(FLAGS, pf->ch)) {
			pf_parse_flags(pf);
			continue;
		}
		if (pf->ch == '.' || (pf->ch >= '1' && pf->ch <= '9')) {
			pf_parse_precision_width(pf);
			continue;
		}
		if (pf->ch != '\0' && strchr(MOD, pf->ch)) {
			pf_parse_mod(pf);
			continue;
		}
		if (pf->ch != '\0' && strchr(TYPES, pf->ch))
			return (pf_parse_type(pf));
		if (pf->ch == '\0')
			return;
		pf->cp = pf->buf;
		*pf->cp = pf->ch;
		pf->size = 1;
		pf->sign = '\0';
		return;
	}
}

static void pf_raw_string(t_pf *pf)
{
	if ((unsigned)pf->ret + pf->n > INT_MAX) {
		pf->ret = EOF;
		return;
	}
	kwrite(pf->cp, pf->n);
	pf->ret += pf->n;
}

int printk(const char *fmt, ...)
{
	va_list ap;
	t_pf pf;

	va_start(ap, fmt);

	memset(&pf, '\0', sizeof(pf));
	pf.fmt = (char *)fmt;
	va_copy(pf.ap, ap);
	while (1) {
		pf.cp = pf.fmt;
		while ((pf.ch = *pf.fmt) != '\0' && pf.ch != '%')
			++pf.fmt;
		if ((pf.n = pf.fmt - pf.cp) != 0)
			pf_raw_string(&pf);
		if (pf.ret == EOF || pf.ch == '\0')
			break;
		++pf.fmt;
		pf.flags = 0;
		pf.dprec = 0;
		pf.width = 0;
		pf.prec = -1;
		pf.sign = '\0';
		pf.ox[1] = '\0';
		pf.ch = *pf.fmt++;
		pf_parse(&pf);
		if (pf.ch == '\0')
			break;
		else if (pf.ch == 'n')
			continue;
		pf_print(&pf);
		if (pf.ret == EOF)
			break;
	}

	va_end(pf.ap);
	va_end(ap);

	return pf.ret;
}
