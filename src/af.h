#ifndef ETYMON_AF_AF_H
#define ETYMON_AF_AF_H

#include "af_auto.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#if (HAVE_LIBXERCES_C == 1)
#if (HAVE_XERCESC_UTIL_XERCESVERSION_HPP == 1)
#define ETYMON_AF_XML
#endif
#endif

#define ETYMON_AF_BANNER "Amberfish, Version " ETYMON_AF_VERSION
#define ETYMON_AF_COPYRIGHT "Copyright (C) 1999-2004 Etymon Systems, Inc.  All Rights Reserved."
#define ETYMON_AF_BANNER_STAMP ETYMON_AF_BANNER ".  " ETYMON_AF_COPYRIGHT

/* maximum char[] key size */
/* 11 is big enough to hold the default uint4 keys */
#define ETYMON_MAX_KEY_SIZE (11)

/* maximum char[] size for an absolute path */
#define ETYMON_MAX_PATH_SIZE (1024)

/* size of buffer to store version stamp in db info file */
#define ETYMON_MAX_VSTAMP_SIZE (256)

/* maximum char[] size for a field name (not an entire field path) */
#define ETYMON_AF_MAX_FIELDNAME_SIZE (32)

typedef struct {
	int (*error)(char*, int);
} ETYMON_LOG;

/* platform specific typedefs */

typedef char int1;
typedef unsigned char uint1;

#if (SIZEOF_INT == 2)
typedef int int2;
typedef unsigned int uint2;
#else
#if (SIZEOF_SHORT_INT == 2)
typedef short int int2;
typedef unsigned short int uint2;
#endif
#endif

#if (SIZEOF_INT == 4)
typedef int int4;
typedef unsigned int uint4;
#else
#if (SIZEOF_LONG_INT == 4)
typedef long int int4;
typedef unsigned long int uint4;
#endif
#endif

/* 64-bit file system support */
#ifdef O_LARGEFILE
#define ETYMON_AF_O_LARGEFILE O_LARGEFILE
#define etymon_af_lseek(A, B, C)  lseek64(A, B, C)
#define etymon_af_fstat(A, B)  fstat64(A, B)
#define etymon_af_ftruncate(A, B)  ftruncate64(A, B)
#define etymon_af_readdir(A)  readdir64(A)
#define etymon_af_off_t  off64_t
typedef struct stat64  ETYMON_AF_STAT;
typedef struct dirent64  ETYMON_AF_DIRENT;
#else
#define ETYMON_AF_O_LARGEFILE (0)
#define etymon_af_lseek(A, B, C)  lseek(A, B, C)
#define etymon_af_fstat(A, B)  fstat(A, B)
#define etymon_af_ftruncate(A, B)  ftruncate(A, B)
#define etymon_af_readdir(A)  readdir(A)
#define etymon_af_off_t  off_t
typedef struct stat  ETYMON_AF_STAT;
typedef struct dirent  ETYMON_AF_DIRENT;
#endif

#ifndef ftruncate
int ftruncate(int fd, off_t length);
#endif

#ifndef strcasecmp
int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef snprintf
int snprintf(char *str, size_t n, const char *format, ...);
#endif

#ifndef strdup
char *strdup(const char *s);
#endif

typedef struct {
	ETYMON_LOG log;
	char* dbname; /* database name */
	int memory; /* maximum amount of memory to use during operations (MB) */
	int phrase;
} ETYMON_DB_OPTIONS;

/* right after uint4 magic number in db info file */
typedef struct {
	char version_stamp[ETYMON_MAX_VSTAMP_SIZE];
	int doc_n; /* total number of (non-deleted) documents in
		      database */
	uint4 udict_root; /* root of the udict tree */
	int optimized; /* flag: if database is optimized */
	int phrase; /* flag: if database supports phrase searching */
	int word_proximity; /* flag: if database supports word proximity */
} ETYMON_DB_INFO;

typedef struct {
	ETYMON_LOG log;
	char* dbname; /* database name */
	char** query; /* query to search on */
} ETYMON_SEARCH_OPTIONS;

typedef struct {
        unsigned char key[ETYMON_MAX_KEY_SIZE]; /* document key */
        char filename[ETYMON_MAX_PATH_SIZE]; /* document source file name */
	etymon_af_off_t begin; /* starting offset of document within the file */
        etymon_af_off_t end; /* ending offset of document within the file (one byte past end) */
	uint4 parent;  /* doc_id of parent document */
        uint1 dclass_id; /* unique id associated with dclass */
} ETYMON_DOCTABLE;

typedef struct {
	ETYMON_LOG log;
	char* dbname; /* database name */
	int memory; /* maximum amount of memory to use during indexing (MB) */
	int dlevel; /* maximum number of levels to descend (nested documents) */
	char* dclass; /* document class */
	char** files; /* list of files to index */
	int files_stdin; /* boolean: read files to index from standard input */
	int phrase; /* boolean: enable phrase search */
	int word_proximity; /* boolean: enable word proximity operator */
	char* split; /* delimiter of multiple documents (if any)
                        within a file */
	int verbose; /* boolean: verbose output */
	char* dc_options; /* document class options */
} ETYMON_INDEX_OPTIONS;

typedef struct {
	char** fn;
} ETYMON_RSET;

/* NEW DATA STRUCTURES */

#define EL_INFO 0
#define EL_WARNING 1
#define EL_ERROR 2
#define EL_CRITICAL 3

#define EX_MEMORY 1
#define EX_IO 2
#define EX_CREATE_READ_ONLY 10
#define EX_DB_OPEN_LIMIT 11
#define EX_DB_NAME_NULL 12
#define EX_DB_OPEN 13
#define EX_DB_CREATE 14
#define EX_DB_INCOMPATIBLE 15
#define EX_DB_ID_INVALID 16
#define EX_FIELD_UNKNOWN 17
#define EX_QUERY_TERM_TOO_LONG 18
#define EX_QUERY_TOO_COMPLEX 19
#define EX_QUERY_SYNTAX_ERROR 20
#define EX_DB_NOT_READY 21

/* maximum char[] size for a diagnostic message */
#define ETYMON_AF_MAX_MSG_SIZE (1024)

typedef struct {
	int level; /* 0=informational, 1=warning, 2=error, 3=critical error (memory) */
	int code; /* specific error code */
	char msg[ETYMON_AF_MAX_MSG_SIZE]; /* diagnostic message */
	char where[ETYMON_AF_MAX_MSG_SIZE]; /* function where the exception occurred */
} ETYMON_AF_EXCEPTION;

typedef struct {
	void (*write)(const ETYMON_AF_EXCEPTION*);
	ETYMON_AF_EXCEPTION ex;
} ETYMON_AF_LOG;

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

typedef struct {
	char name[ETYMON_AF_MAX_FIELDNAME_SIZE];
} ETYMON_AF_FIELD_NAME;

typedef struct {
	int db_id;
	int list_fields;
	ETYMON_AF_FIELD_NAME* fields;
	int fields_n;
	ETYMON_AF_LOG* log;
} ETYMON_AF_EXPLAIN;

typedef struct {
	int db_id;
	uint4 doc_id;
	uint2 score;
} ETYMON_AF_RESULT;

enum etymon_af_scoring_methods { ETYMON_AF_UNSCORED = 0,
				 /* traditional vector space/IDF
				    weighting method */
				 ETYMON_AF_SCORE_DEFAULT };

enum etymon_af_sorting_methods { ETYMON_AF_UNSORTED = 0,
				 /* sort by score */
				 ETYMON_AF_SORT_SCORE };

typedef struct {
	int* db_id; /* array of databases to search (0-terminated) */
	unsigned char* query;
	int score_results; /* see etymon_af_scoring_methods */
	int sort_results; /* see etymon_af_sorting_methods */
	ETYMON_AF_LOG* log;
	ETYMON_AF_RESULT* results;
	int results_n;
} ETYMON_AF_SEARCH;

typedef struct {
	uint4 parent;  /* doc_id of parent document */
        char* filename; /* document source file name */
	etymon_af_off_t begin; /* starting offset of document within the file */
        etymon_af_off_t end; /* ending offset of document within the file (one byte past end) */
} ETYMON_AF_ERESULT;

#ifdef af__cplusplus
extern "C" {
#endif

	/* public functions here */
	int etymon_db_create(ETYMON_DB_OPTIONS* opt);
	int etymon_index_add_files(ETYMON_INDEX_OPTIONS* opt);
	int etymon_index_optimize(ETYMON_INDEX_OPTIONS* opt);
	int etymon_index_optimize_new(ETYMON_INDEX_OPTIONS* opt);
	char*** etymon_split_options(int argc, char *argv[]);
/*	int etymon_search_word_list(ETYMON_SEARCH_OPTIONS* opt, ETYMON_RSET* rset_out, int oper);
 */
	void etymon_db_list(ETYMON_DB_OPTIONS* opt);

	/* NEW PUBLIC FUNCTIONS */
	int etymon_af_open(ETYMON_AF_OPEN* opt);
	int etymon_af_close(ETYMON_AF_CLOSE* opt);
	int etymon_af_search(ETYMON_AF_SEARCH* opt);
	int etymon_af_explain(ETYMON_AF_EXPLAIN* opt);
	int etymon_af_resolve_results(ETYMON_AF_RESULT* results, int results_n, ETYMON_AF_ERESULT* resolved_results, ETYMON_AF_LOG* log);

	int etymon_af_search_term_compare(const void* a, const void* b);

#ifdef af__cplusplus
}
#endif

#endif
