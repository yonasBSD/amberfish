/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdio.h>
#include <string.h>
#include "linear.h"
#include "lock.h"
#include "util.h"
#include "info.h"

/*
static int logerr(char *s, int e) {
	fprintf(stderr, "%s\n", s);
	return 0;
}
*/

static inline int getlock(const char *db)
{
	if (!etymon_db_ready(db, NULL))
		return aferr(AFEDBLOCK);
	if (etymon_db_lock(db, NULL) < 0)
		return -1;
	return 0;
}

static inline int openfiles(const char *db, Affile *f)
{
	memset(f, 0, sizeof f);
	if (!(f->info = afopendbf(db, AFFTINFO, "r+b")))
		return -1;
	if (!(f->udict = afopendbf(db, AFFTUDICT, "r+b")))
		return -1;
	if (!(f->upost = afopendbf(db, AFFTUPOST, "rb")))
		return -1;
	if (!(f->ufield = afopendbf(db, AFFTUFIELD, "rb")))
		return -1;
	if (!(f->uword = afopendbf(db, AFFTUWORD, "rb")))
		return -1;
	if (!(f->lpost = afopendbf(db, AFFTLPOST, "ab")))
		return -1;
	if (!(f->lfield = afopendbf(db, AFFTLFIELD, "ab")))
		return -1;
	if (!(f->lword = afopendbf(db, AFFTLWORD, "ab")))
		return -1;
	return 0;
}

int linearize(Affile *f, Dbinfo *info, int verbose)
{
	
}

int aflinear(const Aflinear *rq)
{
	Affile f;
	Dbinfo info;

	if (rq->verbose >= 2)
		printf("Linearizing\n");

	if (rq->verbose >= 4)
		printf("Getting database lock\n");
	if (getlock(rq->db) < 0)
		return -1;

	if (rq->verbose >= 4)
		printf("Opening database\n");
	if (openfiles(rq->db, &f) < 0)
		return -1;
	if (afdbver(f.info) < 0)
		return -1;
	if (afreadinfo(f.info, &info) < 0)
		return -1;

	/* exit if db is already linearized */
	if (info.optimized)
		return afclosedbf(&f);

	if (linearize(&f, &info) < 0)
		return -1;
	
	return 0;
}
