#ifndef _AF_SEARCH_H
#define _AF_SEARCH_H

#include "misc.h"
#include "log.h"

typedef struct {
	ETYMON_LOG* log;
	ETYMON_DB_INFO dbinfo;
	int doctable_fd;
	int udict_fd;
	int fdef_fd;
	int upost_fd;
	int ufield_fd;
	int lpost_fd;
	int lfield_fd;
	ETYMON_AF_FDEF_DISK* fdef_disk;
} ETYMON_SEARCH_SEARCHING_STATE;

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

int etymon_af_search(ETYMON_AF_SEARCH* opt);

int etymon_af_search_term_compare(const void* a, const void* b);

int etymon_af_resolve_results(ETYMON_AF_RESULT* results, int results_n, ETYMON_AF_ERESULT* resolved_results, ETYMON_AF_LOG* log);

#endif
