/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "search_new.h"
#include "open.h"
#include "search.h"
#include "stem.h"

extern ETYMON_AF_STATE* etymon_af_state[];

static void _log_error(const ETYMON_AF_EXCEPTION *ex)
{
/* 	if (ex->level == 0) { */
	const char *msg = ex->msg;
	const char *where = ex->where;
	char *s = (char *)malloc(strlen(msg) + strlen(where) + 4);
	strcpy(s, msg);
	strcat(s, " [");
	strcat(s, where);
	strcat(s, "]");
	fprintf(stderr, "%s: %s\n", "af", s);
	free(s);
/* 	}  */
}

static inline int _open_dbs(char **db, int dbn, int db_id[])
{
	int x;
	ETYMON_AF_OPEN op;
	ETYMON_AF_LOG log;

	op.read_only = 1;
	op.create = 0;
	op.keep_open = 0;
	op.log = &log;
	log.write = _log_error;
	for (x = 0; x < dbn; x++) {
		op.dbname = db[x];
		if ((db_id[x] = etymon_af_open(&op)) < 0)
			return -1;
	}
	db_id[dbn] = 0;
	
	return 0;
}

static inline int _close_dbs(int *db_id)
{
	int *p;
	ETYMON_AF_CLOSE c;
	ETYMON_AF_LOG log;

	c.log = &log;
	log.write = _log_error;
	for (p = db_id; *p; p++) {
		c.db_id = *p;
		if (etymon_af_close(&c) < 0)
			return -1;
	}

	return 0;
}

static inline uint2 _reverse_lookup(const uint2 *db_r, int db_id)
{
	return db_r[db_id];
}

static inline void _convert_results(const int *db_id,
				    const uint2 *db_r,
				    const ETYMON_AF_SEARCH *s,
				    Afresult **result, int *resultn)
{
	int x = 0;
	
	*resultn = s->results_n;
	*result = (Afresult *) malloc(s->results_n * sizeof (Afresult));
	for (; x < s->results_n; x++) {
		(*result)[x].docid = s->results[x].doc_id;
		(*result)[x].score = s->results[x].score;
		(*result)[x].db = _reverse_lookup(db_r, s->results[x].db_id);
	}
}

static inline int _search_dbs(int *db_id, const uint2 *db_r, Afchar *query,
			      int score, int sort, Afresult **result,
			      int *resultn)
{
	int x;
	ETYMON_AF_SEARCH s;
	ETYMON_AF_LOG log;

	s.db_id = db_id;
	s.query = query;
	s.score_results = score;
	s.sort_results = sort;
	s.log = &log;
	log.write = _log_error;
	if (etymon_af_search(&s) < 0)
		return -1;

	_convert_results(db_id, db_r, &s, result, resultn);
	return 0;
}

static inline uint2 *_init_reverse_lookup(int *db_id, int db_id_n)
{
	int x;
	int max;
	int y;
	uint2 *table;

	for (x = 0, max = 0; x < db_id_n; x++) {
		if ((y = db_id[x]) > max)
			max = y;
	}
	x = (max + 1) * sizeof (uint2);
	table = (uint2 *) malloc(x);
	memset(table, 0, x);
	for (x = 0; x < db_id_n; x++) {
		y = db_id[x];
		table[y] = x;
	}
	return table;
}

/*
int _afsearch(const Afsearch *rq, Afsearch_r *rs)
{
	int db_id[rq->dbn + 1];
	uint2 *db_r;
	
	if (_open_dbs(rq->db, rq->dbn, db_id) < 0)
		return -1;
	db_r = _init_reverse_lookup(db_id, rq->dbn);
	if (_search_dbs(db_id, db_r, rq->query, rq->score,
			rq->sort_score, &rs->result, &rs->resultn) < 0) {
		free(db_r);
		return -1;
	}
	free(db_r);
	if (_close_dbs(db_id) < 0)
		return -1;

// test
	{
		int x;
		for (x = 0; x < rs->resultn; x++)
			printf("%i %s %i\n",
			       rs->result[x].score,
			       rq->db[rs->result[x].db],
			       rs->result[x].docid);
	}
	
	return 0;
}
*/

/******************************** New interface ********************************/

static inline int open_dbs(char **db, int dbn, int *db_id)
{
	int x;
	ETYMON_AF_OPEN op;
	ETYMON_AF_LOG log;

	op.read_only = 1;
	op.create = 0;
	op.keep_open = 0;
	op.log = &log;
	log.write = _log_error;
	for (x = 0; x < dbn; x++) {
		op.dbname = db[x];
		if ((db_id[x] = etymon_af_open(&op)) < 0)
			return -1;
	}
	
	return 0;
}

static inline int close_dbs(int *db_id)
{
	int *p;
	ETYMON_AF_CLOSE c;
	ETYMON_AF_LOG log;

	c.log = &log;
	log.write = _log_error;
	for (p = db_id; *p; p++) {
		c.db_id = *p;
		if (etymon_af_close(&c) < 0)
			return -1;
	}

	return 0;
}

static inline int validate_dbs(const char **db, const int *db_id, int dbn)
{
	int x;
	
	for (x = 0; x < dbn; x++) {
		if (etymon_af_state[db_id[x]]->info.optimized == 0) {
			fprintf(stderr,
				"af: %s: The current version cannot search a non-linearized database\n",
				db[x]);
			return -1;
		}
		if (etymon_af_state[db_id[x]]->info.stemming && !af_stem_available()) {
			fprintf(stderr, "af: %s: Database requires stemming support\n",
				db[x]);
			return -1;
		}
	}
	return 0;
}

/*
int afsearch(const Afsearch *rq, Afsearch_r *rs)
{
	int x;
	int db_id[rq->dbn];
	ETYMON_AF_SEARCH_STATE state;

	if (open_dbs(rq->db, rq->dbn, db_id) < 0)
		return -1;

	if (validate_dbs(rq->db, db_id) < 0)
		return -1;
	
	if (af_search_db(db_id, db_r, rq->query, rq->score,
			rq->sort_score, &rs->result, &rs->resultn) < 0)
		return -1;

	if (close_dbs(db_id) < 0)
		return -1;
	
	return 0;
}
*/
