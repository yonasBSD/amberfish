#ifndef _AF_OPEN_H
#define _AF_OPEN_H

/***** new *****/

typedef struct {
	char *db;
	char *mode;
} Afopen;

typedef struct {
	int dbid;
} Afopen_r;

int afopen(const Afopen *r, Afopen_r *rr);

/***** old *****/

#include "defs.h"
#include "log.h"
#include "info.h"

/* maximum number of databases that can be open simultaneously */
#define ETYMON_AF_MAX_OPEN 255

/* short by one, because the lock file is temporary for the old interface */
#define ETYMON_AF_MAX_DB_FILES 10

typedef struct {
	char* dbname;
	char fn[ETYMON_AF_MAX_DB_FILES][ETYMON_MAX_PATH_SIZE];
	int fd[ETYMON_AF_MAX_DB_FILES];
	ETYMON_DB_INFO info;
	ETYMON_AF_FDEF_DISK* fdef;
	int fdef_count;
	int keep_open;
	int read_only;
} ETYMON_AF_STATE;

typedef struct {
	char* dbname;
	int read_only; /* open database for read-only operations */
	int create; /* create database, overwriting if one already exists */
	int keep_open; /* keep database files open, instead of re-opening for each operation */
	ETYMON_AF_LOG* log;
} ETYMON_AF_OPEN;

typedef struct {
	int db_id;
	ETYMON_AF_LOG* log;
} ETYMON_AF_CLOSE;

int etymon_af_open(ETYMON_AF_OPEN* opt);

int etymon_af_close(ETYMON_AF_CLOSE* opt);

int etymon_af_open_files(char* where, ETYMON_AF_LOG* log, int db_id, int flags);

int etymon_af_close_files(char* where, ETYMON_AF_LOG* log, int db_id);

#endif
