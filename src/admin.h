#ifndef _AF_ADMIN_H
#define _AF_ADMIN_H

#include "defs.h"

typedef struct {
	ETYMON_LOG log;
	char* dbname; /* database name */
	int memory; /* maximum amount of memory to use during operations (MB) */
	int phrase;
} ETYMON_DB_OPTIONS;

int etymon_db_create(ETYMON_DB_OPTIONS* opt);

void etymon_db_list(ETYMON_DB_OPTIONS* opt);

#endif
