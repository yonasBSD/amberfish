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

static int _open_db(const char *dbname)
{
	ETYMON_AF_OPEN op;
	ETYMON_AF_LOG log;
	
	op.read_only = 1;
	op.create = 0;
	op.keep_open = 0;
	op.log = &log;
	log.write = _log_error;
	return etymon_af_open(&op);
}

static int _close_db(const int id)
{
	ETYMON_AF_CLOSE c;
	ETYMON_AF_LOG log;

	c.db_id = id;
	c.log = &log;
	log.write = _log_error;
	return etymon_af_close(&c);
}

static int _search_db(int db, const unsigned char *query, const int score,
		      const int sort)
{
	ETYMON_AF_SEARCH se;
	ETYMON_AF_LOG log;

	se.db_id = db;
	se.query = query;
	se.score_results = score;
	se.sort_results = sort;
	se.log = &log;
	log.write = _log_error;
	return etymon_af_search(&se);
}

int af_search(const AF_SEARCH *request, AF_SEARCH_R *response)
{
	int db = _open_db(request->dbname);
	if (_search_db(db, request->query, request->score_results,
			request->sort_results) < 0)
		return -1;
	_close_db(db);

	
	
	return 0;
}
