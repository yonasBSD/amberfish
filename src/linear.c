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

/*
int afgetfsize_nonansi(FILE *f, off_t *size)
{
	struct stat st;
	
	if (fstat(fileno(f), &st) < 0)
		return aferr(AFEDBIO);
	*size = st.st_size;
	return 0;
}
*/

int afgetfsize(FILE *f, off_t *size)
{
	off_t save;

	if ((save = ftello(f)) < 0)
		return aferr(AFEDBIO);
	if (fseeko(f, 0, SEEK_END) < 0)
		return aferr(AFEDBIO);
	if ((*size = ftello(f)) < 0)
		return aferr(AFEDBIO);
	if (fseeko(f, save, SEEK_SET) < 0)
		return aferr(AFEDBIO);
	return 0;
}

typedef struct {
	const Aflinear *rq;
	Affile f;
	Dbinfo info;
	off_t udictp;
	off_t udictnb;
	off_t upostn;
	off_t ufieldn;
	off_t uwordn;
	off_t lpostn;
	off_t lfieldn;
	off_t lwordn;
	ETYMON_INDEX_PAGE_L pagel;
} Aflinst;

int getfsizes(Aflinst *t)
{
	off_t nb;

	if (afgetfsize(t->f.udict, &t->udictnb) < 0)
		return -1;
	
	if (afgetfsize(t->f.upost, &nb) < 0)
		return -1;
	t->upostn = nb / sizeof (ETYMON_INDEX_UPOST);
	if (afgetfsize(t->f.ufield, &nb) < 0)
		return -1;
	t->ufieldn = nb / sizeof (ETYMON_INDEX_UFIELD);
	if (afgetfsize(t->f.uword, &nb) < 0)
		return -1;
	t->uwordn = nb / sizeof (ETYMON_INDEX_UWORD);
	if (afgetfsize(t->f.lpost, &nb) < 0)
		return -1;
	t->lpostn = nb / sizeof (ETYMON_INDEX_LPOST);
	if (afgetfsize(t->f.lfield, &nb) < 0)
		return -1;
	t->lfieldn = nb / sizeof (ETYMON_INDEX_LFIELD);
	if (afgetfsize(t->f.lword, &nb) < 0)
		return -1;
	t->lwordn = nb / sizeof (ETYMON_INDEX_LWORD);

	return 0;
}

int seekleftleaf(Aflinst *t)
{
	Uint1 leaf;
	ETYMON_INDEX_PAGE_NL pagenl;
	
	t->udictp = t->info.udict_root;
	while (1) {
		if (fseeko(t->f.udict, t->udictp, SEEK_SET) < 0)
			return aferr(AFEDBIO);
		if (fread(&leaf, 1, 1, t->f.udict) < 0)
			return aferr(AFEDBIO);
		if (leaf)
			return 0;
		if (fread(&pagenl, 1, sizeof pagenl, t->f.udict) < sizeof pagenl)
			return aferr(AFEDBIO);
		t->udictp = pagenl.p[0];
	}
}

int readleaf(Aflinst *t)
{
	if (fseeko(t->f.udict, 
}

int linearize(Aflinst *t)
{
	
	if (seekleftleaf(t) < 0)
		return -1;
	if (readleaf(t) < 0)
		return -1;
	

	
	return 0;
}

int aflinear(const Aflinear *rq)
{
	Aflinst t;

	t.rq = rq;
	if (rq->verbose >= 2)
		printf("Linearizing\n");

	if (rq->verbose >= 4)
		printf("Getting database lock\n");
	if (getlock(rq->db) < 0)
		return -1;

	if (rq->verbose >= 4)
		printf("Opening database\n");
	if (openfiles(rq->db, &t.f) < 0)
		return -1;
	if (afdbver(t.f.info) < 0)
		return -1;
	if (afreadinfo(t.f.info, &t.info) < 0)
		return -1;

	/* exit if db is already linearized */
	if (t.info.optimized)
		return afclosedbf(&t.f);

	if (getfsizes(&t) < 0)
		return -1;
	
	if (linearize(&t) < 0)
		return -1;
	
	return 0;
}
