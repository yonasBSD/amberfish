/*
 *  This is a temporary holding place for definitions that don't yet
 *  have a home.
 */

#ifndef _AF_DEFS_H
#define _AF_DEFS_H

#include <fcntl.h>
#include "config.h"

/* I'd like to move this to where it is used and reduce the number of
   times it is used. */
#define ETYMON_AF_BANNER "Amberfish, Version " AF_VERSION
#define ETYMON_AF_COPYRIGHT "Copyright (C) 1999-2004 Etymon Systems, Inc.  All Rights Reserved."
#define ETYMON_AF_BANNER_STAMP ETYMON_AF_BANNER ".  " ETYMON_AF_COPYRIGHT

/* uint4 at beginning of db info file to indicate index version; used to determine compatibility */
#define ETYMON_INDEX_MAGIC (4)

/* maximum char[] size for an absolute path */
#define ETYMON_MAX_PATH_SIZE (1024)

/* maximum char[] key size */
/* 11 is big enough to hold the default uint4 keys */
#define ETYMON_MAX_KEY_SIZE (11)

/* size of buffer to store version stamp in db info file */
#define ETYMON_MAX_VSTAMP_SIZE (256)

/* maximum char[] size for a field name (not an entire field path) */
#define ETYMON_AF_MAX_FIELDNAME_SIZE (32)

/* maximum char[] size for an error message */
#define ETYMON_MAX_MSG_SIZE (1024)

/* Maybe this should be moved to open.h eventually. */
#define ETYMON_DBF_INFO (0)
#define ETYMON_DBF_INFO_EXT ".db"
#define ETYMON_DBF_DOCTABLE (1)
#define ETYMON_DBF_DOCTABLE_EXT ".dt"
#define ETYMON_DBF_UDICT (2)
#define ETYMON_DBF_UDICT_EXT ".x0"
#define ETYMON_DBF_UPOST (3)
#define ETYMON_DBF_UPOST_EXT ".y0"
#define ETYMON_DBF_UFIELD (4)
#define ETYMON_DBF_UFIELD_EXT ".z0"
#define ETYMON_DBF_LPOST (5)
#define ETYMON_DBF_LPOST_EXT ".y1"
#define ETYMON_DBF_LFIELD (6)
#define ETYMON_DBF_LFIELD_EXT ".z1"
#define ETYMON_DBF_FDEF (7)
#define ETYMON_DBF_FDEF_EXT ".fd"
#define ETYMON_DBF_UWORD (8)
#define ETYMON_DBF_UWORD_EXT ".w0"
#define ETYMON_DBF_LWORD (9)
#define ETYMON_DBF_LWORD_EXT ".w1"
#define ETYMON_DBF_LOCK (10)
#define ETYMON_DBF_LOCK_EXT ".lk"

#define ETYMON_DB_PERM (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

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
        unsigned char key[ETYMON_MAX_KEY_SIZE]; /* document key */
        char filename[ETYMON_MAX_PATH_SIZE]; /* document source file name */
	etymon_af_off_t begin; /* starting offset of document within the file */
        etymon_af_off_t end; /* ending offset of document within the file (one byte past end) */
	uint4 parent;  /* doc_id of parent document */
        uint1 dclass_id; /* unique id associated with dclass */
} ETYMON_DOCTABLE;

typedef struct {
	unsigned char name[ETYMON_AF_MAX_FIELDNAME_SIZE];
	uint2 left;
	uint2 right;
} ETYMON_AF_FDEF_DISK;

typedef struct {
	int (*error)(char*, int);
} ETYMON_LOG;

/* maximum number of keys in a page */
#define ETYMON_MAX_KEYS_L (5000)
#define ETYMON_MAX_KEYS_NL (8000)

/* maximum char[] size for a parsed token */
/* this must be set to the larger of ETYMON_MAX_WORD_SIZE and ETYMON_MAX_FIELDNAME_SIZE */
#define ETYMON_MAX_TOKEN_SIZE (32)

/* maximum char[] size for an indexable word */
#define ETYMON_MAX_WORD_SIZE (32)

/* maximum char[] size for a single term within a query */
#define ETYMON_MAX_QUERY_TERM_SIZE (1024)

/* maximum level of nesting in structured fields */
#define ETYMON_MAX_FIELD_NEST (64)

/* maxmium depth (levels) of tree allowed */
#define ETYMON_MAX_PAGE_DEPTH (4)

/* average key length */
#define ETYMON_MEAN_KEY_LEN_L (8)
#define ETYMON_MEAN_KEY_LEN_NL (5)

/* size of key buffer in pages */
#define ETYMON_MAX_KEY_AREA_L (ETYMON_MAX_KEYS_L * ETYMON_MEAN_KEY_LEN_L)
#define ETYMON_MAX_KEY_AREA_NL (ETYMON_MAX_KEYS_NL * ETYMON_MEAN_KEY_LEN_NL)

#define ETYMON_AF_MAX_OP_STACK_DEPTH (256)
#define ETYMON_AF_MAX_R_STACK_DEPTH (256)

#define ETYMON_AF_OP_OR (1)
#define ETYMON_AF_OP_AND (2)
#define ETYMON_AF_OP_GROUP_OPEN (3)
#define ETYMON_AF_OP_GROUP_CLOSE (4)

typedef struct {
	uint2 n; /* number of keys */
	uint4 p[ETYMON_MAX_KEYS_NL + 1]; /* pointers to other pages (offset) */
	uint2 offset[ETYMON_MAX_KEYS_NL + 1]; /* offsets to keys */
	unsigned char keys[ETYMON_MAX_KEY_AREA_NL]; /* key buffer */
} ETYMON_INDEX_PAGE_NL;

typedef struct {
	uint2 n; /* number of keys */
	uint4 prev; /* previous leaf node (offset) */
	uint4 next; /* next left node (offset) */
	uint4 post[ETYMON_MAX_KEYS_L]; /* postings for each key */
	uint4 post_n[ETYMON_MAX_KEYS_L]; /* number of postings for each key */
	uint2 offset[ETYMON_MAX_KEYS_L + 1]; /* offsets to keys */
	unsigned char keys[ETYMON_MAX_KEY_AREA_L]; /* key buffer */
} ETYMON_INDEX_PAGE_L;

typedef struct {
	uint4 pos; /* position of page on disk or 0 if empty slot */
	uint1 is_nl; /* is it a non-leaf (here) or leaf (in the leaf cache) */
	ETYMON_INDEX_PAGE_NL nl;
} ETYMON_INDEX_PCACHE_NODE;

typedef struct {
	uint2 f[ETYMON_MAX_FIELD_NEST];
	int next;
} ETYMON_INDEX_FCACHE_NODE;

typedef struct {
	uint4 wn;
	int next;
} ETYMON_INDEX_WNCACHE_NODE;

typedef struct {
	unsigned char word[ETYMON_MAX_WORD_SIZE];
	int left;
	int right;
	int next; /* circular linked list */
	uint2 freq;
	uint4 doc_id;
	int fields;
	int word_numbers_head;
	int word_numbers_tail;
} ETYMON_INDEX_WCACHE_NODE;

typedef struct {
	uint4 doc_id; /* document id */
	uint2 freq; /* frequency */
	uint4 fields; /* field pointer */
	uint4 fields_n; /* number of fields */
	uint4 word_numbers; /* word numbers pointer */
	uint4 word_numbers_n; /* number of word numbers */
	uint4 next; /* next posting (index) or 0 */
} ETYMON_INDEX_UPOST;

typedef struct {
	uint4 doc_id; /* document id */
	uint2 freq; /* frequency */
	uint4 fields; /* field pointer */
	uint4 fields_n; /* number of fields */
	uint4 word_numbers; /* word numbers pointer */
	uint4 word_numbers_n; /* number of word numbers */
} ETYMON_INDEX_LPOST;

typedef struct {
	uint2 fields[ETYMON_MAX_FIELD_NEST];
	uint4 next; /* next field (index) or 0 */
} ETYMON_INDEX_UFIELD;

typedef struct {
	uint4 wn;
	uint4 next; /* next word number (index) or 0 */
} ETYMON_INDEX_UWORD;

typedef struct {
	uint2 fields[ETYMON_MAX_FIELD_NEST];
} ETYMON_INDEX_LFIELD;

typedef struct {
	uint4 wn;
} ETYMON_INDEX_LWORD;

typedef struct ETYMON_AF_FDEF_MEM_STRUCT {
	uint2 n;
	unsigned char name[ETYMON_AF_MAX_FIELDNAME_SIZE];
	struct ETYMON_AF_FDEF_MEM_STRUCT* left;
	struct ETYMON_AF_FDEF_MEM_STRUCT* right;
	struct ETYMON_AF_FDEF_MEM_STRUCT* next;
} ETYMON_AF_FDEF_MEM;

typedef struct {
	ETYMON_LOG* log;
	char* dbname; /* database name */
	int doctable_fd; /* doctable file descriptor */
	etymon_af_off_t doctable_next_id; /* next available doctable number */
	ETYMON_DOCTABLE doctable; /* doctable buffer to use repeatedly */
	ETYMON_INDEX_WCACHE_NODE* wcache; /* binary tree (array) of word cache nodes */
	int wcache_size;
	int wcache_count;
	int wcache_root;
	ETYMON_INDEX_FCACHE_NODE* fcache; /* linked list (array) of field cache nodes */
	int fcache_size;
	int fcache_count;
	ETYMON_INDEX_WNCACHE_NODE* wncache; /* linked list (array) of word number cache nodes */
	int wncache_size;
	int wncache_count;
	int udict_fd; /* udict file descriptor */
	etymon_af_off_t udict_size; /* current size of udict */
	int upost_fd; /* upost file descriptor */
	etymon_af_off_t upost_isize; /* current size of upost */
	int ufield_fd; /* ufield file descriptor */
	etymon_af_off_t ufield_isize; /* current size of ufield */
	int uword_fd; /* uword file descriptor */
	etymon_af_off_t uword_isize; /* current size of uword */
	uint4 udict_root; /* root of the udict tree (offset) */
	ETYMON_INDEX_PCACHE_NODE* pcache_nl; /* non-leaf page cache */
	ETYMON_INDEX_PAGE_L pcache_l; /* leaf page cache */
	uint4 pcache_l_write; /* offset position for write caching, or 0 if pcache_l has been flushed */
	int pcache_nl_size;
	int pcache_count;
	ETYMON_INDEX_PAGE_NL overflow_nl; /* overflow non-leaf page */
	ETYMON_INDEX_PAGE_L overflow_l; /* overflow leaf page */
	ETYMON_INDEX_PAGE_L extra_l; /* extra leaf page */
	ETYMON_INDEX_UPOST upost;
	ETYMON_INDEX_UFIELD ufield;
	ETYMON_INDEX_UWORD uword;
	ETYMON_AF_FDEF_MEM* fdef_root; /* pointer to root node of fdef binary tree */
	ETYMON_AF_FDEF_MEM* fdef_tail; /* pointer to tail node of fdef threaded list */
	uint2 fdef_count;
	int phrase; /* enable phrase searching */
	int word_proximity; /* enable word proximity operator */
	int number_words; /* enable recordings of word number data */
	int doc_n; /* total number of (non-deleted) documents in
		     database */
} ETYMON_INDEX_INDEXING_STATE;

#endif
