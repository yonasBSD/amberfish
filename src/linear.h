#ifndef _AF_LINEAR_H
#define _AF_LINEAR_H

#include "index.h"

typedef struct {
	char *db;
	int verbose;
} Aflinear;

int aflinear(const Aflinear *rq);

/*int _etymon_index_optimize(ETYMON_INDEX_OPTIONS *opt);*/

#endif
