#ifndef _AF_UTIL_H
#define _AF_UTIL_H

void etymon_db_construct_path(int ftype, char* dbname, char* buf);

void etymon_tolower(char* s);

char*** etymon_split_options(int argc, char *argv[]);

#endif
