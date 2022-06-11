#include "printflocal.h"

static char *ujtoa_dec(uintmax_t val, char *cp)
{
	int sval;

	if (val < 10) {
		*--cp = TOCHAR(val % 10);
		return (cp);
	}
	if (val > INTMAX_MAX) {
		*--cp = TOCHAR(val % 10);
		sval = val / 10;
	} else {
		sval = val;
	}
	while (1) {
		*--cp = TOCHAR(sval % 10);
		sval /= 10;
		if (sval == 0)
			break;
	}

	return cp;
}

static char *ujtoa_oct(uintmax_t val, char *cp, int octzero)
{
	while (1) {
		*--cp = TOCHAR(val & 7);
		val >>= 3;
		if (val == 0)
			break;
	}
	if (octzero && *cp != '0')
		*--cp = '0';

	return cp;
}

static char *ujtoa_hex(uintmax_t val, char *cp, const char *xdigs)
{
	while (1) {
		*--cp = xdigs[val & 15];
		val >>= 4;
		if (val == 0)
			break;
	}

	return cp;
}

char *ujtoa(t_pf *pf, uintmax_t val, char *endp)
{
	char *cp;

	if (val <= ULONG_MAX)
		return (ultoa(pf, (U_LONG)val, endp));
	cp = endp;
	if (pf->base == 10)
		cp = ujtoa_dec(val, cp);
	else if (pf->base == 8)
		cp = ujtoa_oct(val, cp, pf->flags & ALT);
	else if (pf->base == 16)
		cp = ujtoa_hex(val, cp, pf->xdigs);

	return cp;
}
