#ifndef _AF_OPEN_NEW_H
#define _AF_OPEN_NEW_H

#include "defs.h"

/* Move these to general definitions file. */
#define AF_PATH_SIZE (1024)
#define AF_MAX_OPEN_DBS (256)

typedef struct {
	char name[AF_PATH_SIZE];
	int mode;
} AF_DB;

/*
 *  mode
 *  "r"   Read-only
 *  "r+"  Read and write
 *  "w"   Create new/overwrite
 */
int af_open(const char *dbname, const char* mode, AF_DB *db);

int _af_open_init();

int _af_open_finish();

#endif
