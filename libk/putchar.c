#include "tty.h"

int putchar(int ic)
{
	char c = (char)ic;

	kwrite(&c, 1);

	return ic;
}
