/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <string.h>
#include "open_new.h"

enum O_MODE {
        O_READONLY = 0,
        O_READWRITE,
        O_CREATE
};

static AF_DB open_dbs[AF_MAX_OPEN_DBS];  /* Internal table of AFDBs */
static int open_dbs_o[AF_MAX_OPEN_DBS];  /* 1 if open; 0 if not open */

int _af_open_init()
{
	memset(&open_dbs_o, 0, sizeof open_dbs_o);

	return 0;
}

/*
 *  mode  returns
 *  "r"   O_READONLY
 *  "r+"  O_READWRITE
 *  "w"   O_CREATE
 *  or returns -1 if unknown
 */
static int parse_mode(const char *mode)
{
	switch (mode[0]) {
	case 'r':
		switch (mode[1]) {
		case '+':
			return O_READWRITE;
		default:
			return O_READONLY;
		}
	case 'w':
		return O_CREATE;
	default:
		return -1;
	}
}

int af_open(const char *dbname, const char* mode, AF_DB *db)
{
	if ((db->mode = parse_mode(mode)) < 0)
		return -1;

	if (strlen(dbname) < AF_PATH_SIZE)
		strcpy(db->name, dbname);
	else
		return -2;
	
	return 0;
}

int af_close(AF_DB *db)
{
	
	return 0;
}

int _af_open_finish()
{
	int x;

	for (x = 0; x < AF_MAX_OPEN_DBS; x++) {
		if (open_dbs_o)
			af_close(&open_dbs[x]);
	}
	
	return 0;
}
