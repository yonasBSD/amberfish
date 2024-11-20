#include "str.h"

void trim(string* s)
{
	const char* p = s->c_str();
	const char* q = p + s->length();
	do {
		if (*p == '\0') {
			s->clear();
			return;
		}
		if (!isspace(*p))
			break;
		p++;
	} while (true);
	do {
		if (p == q) {
			s->clear();
			return;
		}
		if (!isspace(*(q - 1)))
			break;
		q--;
	} while (true);
	size_t pqlen = q - p;
	char* tmp = (char*) malloc(pqlen + 1);
	memcpy(tmp, p, pqlen);
	tmp[pqlen] = '\0';
	*s = tmp;
	free(tmp);
}


