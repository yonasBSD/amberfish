#include "err.h"

char *prgname;

void write_header(ostream &out)
{
	out << "HTTP/1.1 200 OK" << crlf;
	out << "Cache-Control: no-cache, no-store, must-revalidate" << crlf;
}
