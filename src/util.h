#ifndef _AF_UTIL_H
#define _AF_UTIL_H

/* new */

#include "defs.h"

typedef Uint4 Afdbver_t;

FILE *afopendbf(const char *db, int type, const char *mode);
int afclosedbf(Affile *f);
int afdbver(FILE *info);
int afmakefn(int type, const char *db, char *buf);

/* old */

void etymon_db_construct_path(int ftype, const char* dbname, char* buf);
void etymon_tolower(char* s);
char*** etymon_split_options(int argc, char *argv[]);

#endif
