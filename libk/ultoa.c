#include "printflocal.h"

static char	*ultoa_dec(U_LONG val, char *cp)
{
	long sval;

	if (val < 10) {
		*--cp = TOCHAR(val);
		return cp;
	}
	if (val > LONG_MAX) {
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

static char *ultoa_oct(U_LONG val, char *cp, int octzero)
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

static char *ultoa_hex(U_LONG val, char *cp, const char *xdigs)
{
	while (1) {
		*--cp = xdigs[val & 15];
		val >>= 4;
		if (val == 0)
			break;
	}

	return cp;
}

char *ultoa(t_pf *pf, U_LONG val, char *endp)
{
	char *cp = endp;

	if (pf->base == 10)
		cp = ultoa_dec(val, cp);
	else if (pf->base == 8)
		cp = ultoa_oct(val, cp, pf->flags & ALT);
	else if (pf->base == 16)
		cp = ultoa_hex(val, cp, pf->xdigs);

	return cp;
}
