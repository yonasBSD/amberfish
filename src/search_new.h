#ifndef _AF_SEARCH_NEW_H
#define _AF_SEARCH_NEW_H

#include "defs.h"
#include "search.h"

typedef struct {
	char *dbname;
	unsigned char* query;
	int boolean;
	int score_results;
	int sort_results;
} AF_SEARCH;

#define AF_RESULT ETYMON_AF_RESULT

typedef struct {
        AF_RESULT *results;
        int results_n;
} AF_SEARCH_R;

int af_search(const AF_SEARCH *request, AF_SEARCH_R *response);

#endif
