#ifndef _AF_SEARCH_NEW_H
#define _AF_SEARCH_NEW_H

#include "defs.h"
#include "search.h"

typedef unsigned char Afchar;

typedef struct {
	char **db;
	int dbn;
	Afchar *query;
	int boolean;
	int score;
	int sort_score;
} Afsearch;

typedef struct {
        uint4 docid;
        uint2 score;
        uint2 db;
} Afresult;

typedef struct {
        Afresult *result;
        int resultn;
} Afsearch_r;

int afsearch(const Afsearch *rq, Afsearch_r *rs);

#endif
