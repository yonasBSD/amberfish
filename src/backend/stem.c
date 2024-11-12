#include "config.h"

int afnop = 0;  /* placeholder to avoid compiler warnings */

#ifdef HAVE_STEMMER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include "porter.cc"
#pragma GCC diagnostic pop
#endif
