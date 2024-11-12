#include <stdlib.h>
#include <limits.h>
#include <errno.h>

int str_to_long(const char *s, long *val, int base)
{
	char *endp;

	if (!*s)
		return -1;

	*val = strtol(s, &endp, base);
	if (*endp)
		return -1;
	if ( errno == ERANGE &&
	     (*val == LONG_MIN || *val == LONG_MAX) )
		return -1;

	return 0;
}

int str_to_int(const char *s, int *val, int base)
{
	long v;

	if (str_to_long(s, &v, base) < 0)
		return -1;

	if (v < INT_MIN || v > INT_MAX)
		return -1;

	*val = (int) v;
	return 0;
}
