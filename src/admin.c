/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "admin.h"
#include "lock.h"
#include "util.h"
#include "stem.h"
#include "info.h"

void etymon_db_list(ETYMON_DB_OPTIONS* opt) {
	char fn[ETYMON_MAX_PATH_SIZE];
	int doctable_fd, dbinfo_fd;
	ssize_t nbytes;
	ETYMON_AF_STAT st;
	etymon_af_off_t doctable_isize;
	int x, y;
	ETYMON_DOCTABLE doctable;
	uint4 magic;

	/* make sure database is ready */
	if (etymon_db_ready(opt->dbname) == 0) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Database not ready", opt->dbname);
		e = opt->log.error(s, 1);
		if (e != 0) {
			exit(e);
		}
		return;
	}

	/* open db info file for read - only need to read magic number */
	etymon_db_construct_path(ETYMON_DBF_INFO, opt->dbname, fn);
	dbinfo_fd = open(fn, O_RDONLY | ETYMON_AF_O_LARGEFILE);
	if (dbinfo_fd == -1) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Unable to open database", opt->dbname);
		e = opt->log.error(s, 1);
		if (e != 0) {
			exit(e);
		}
		return;
	}
	nbytes = read(dbinfo_fd, &magic, sizeof(uint4));
	if (nbytes != sizeof(uint4)) {
		/* ERROR */
		printf("unable to read %s\n", fn);
		exit(1);
	}
	if (magic != ETYMON_INDEX_MAGIC) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Database created by incompatible version", opt->dbname);
		e = opt->log.error(s, 1);
		close(dbinfo_fd);
		if (e != 0) {
			exit(e);
		}
		return;
	}
	close(dbinfo_fd);

	/* open doctable for read */
	etymon_db_construct_path(ETYMON_DBF_DOCTABLE, opt->dbname, fn);
	doctable_fd = open(fn, O_RDONLY | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
	if (doctable_fd == -1) {
		int e;
		char s[ETYMON_MAX_MSG_SIZE];
		sprintf(s, "%s: Unable to open database", opt->dbname);
		e = opt->log.error(s, 1);
		if (e != 0) {
			exit(e);
		}
		return;
	}
	/* stat doctable to get size */
	if (etymon_af_fstat(doctable_fd, &st) == -1) {
		perror("db_list():fstat()");
	}
	doctable_isize = st.st_size / sizeof(ETYMON_DOCTABLE);

	y = doctable_isize;
	for (x = 0; x < y; x++) {
		nbytes = read(doctable_fd, &doctable, sizeof(ETYMON_DOCTABLE));
		if (nbytes != sizeof(ETYMON_DOCTABLE)) {
			int e;
			char s[ETYMON_MAX_MSG_SIZE];
			sprintf(s, "%s: Database is corrupted", opt->dbname);
			e = opt->log.error(s, 1);
			close(doctable_fd);
			if (e != 0) {
				exit(e);
			}
			return;
		}
		printf("%i %i %s %ld %ld ",
		       x + 1,
		       doctable.parent,
		       doctable.filename,
		       (long)(doctable.begin),
		       (long)(doctable.end));
		switch (doctable.dclass_id) {
		case 1:
			printf("xml\n");
			break;
		case 2:
			printf("xml_test\n");
			break;
		case 100:
			printf("syr1\n");
			break;
		default:
			printf("text\n");
		}
	}
	close(doctable_fd);

}


int etymon_db_delete(ETYMON_DB_OPTIONS* opt) {
	int leftover;
	
	/* need to read the db info file to determine which files to delete */
	/* for now we just roughly construct the file names to delete */
	char path[ETYMON_MAX_PATH_SIZE];
	/* info file */
	etymon_db_construct_path(ETYMON_DBF_INFO, opt->dbname, path);
	if (unlink(path) != 0) {
		return -1;
	}

	/* the following is temporary code to clean up a file from the
	   xml document class; this needs to be integrated into the
	   above */
#ifndef BELIST_USEMEM
	/* temporary file name construction */
	strncpy(path, opt->dbname, ETYMON_MAX_PATH_SIZE - 1);
	path[ETYMON_MAX_PATH_SIZE - 1] = '\0';
	leftover = ETYMON_MAX_PATH_SIZE - strlen(path) - 1;
	strncat(path, ".xm", leftover);
	unlink(path);
#endif
	
	/* otherwise return no error */
	return 0;
}


int etymon_db_init_empty(char* dbname, int ftype) {
	int fd;
	char path[ETYMON_MAX_PATH_SIZE];
	etymon_db_construct_path(ftype, dbname, path);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
	if (fd == -1) {
		/* ERROR */
		printf("error creating file %s in etymon_db_init_empty\n", path);
		exit(1);
	}
	return fd;
}


int etymon_db_init(ETYMON_DB_OPTIONS* opt) {
	int fd;
	ETYMON_DB_INFO dbinfo;
	uint4 magic;
	ssize_t nbytes;

	etymon_db_unlock(opt->dbname);
	etymon_db_lock(opt->dbname, &(opt->log));

	/* create empty db files */
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_DOCTABLE));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_UDICT));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_UPOST));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_UFIELD));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_LPOST));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_LFIELD));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_FDEF));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_UWORD));
	close(etymon_db_init_empty(opt->dbname, ETYMON_DBF_LWORD));

	/* initialize db info */
	fd = etymon_db_init_empty(opt->dbname, ETYMON_DBF_INFO);
	magic = ETYMON_INDEX_MAGIC;
	nbytes = write(fd, &magic, sizeof(uint4));
	if (nbytes != sizeof(uint4)) {
		/* ERROR */
		printf("error writing MN during db creation\n");
		exit(1);
	}
	sprintf(dbinfo.version_stamp, ETYMON_AF_BANNER_STAMP);
	dbinfo.udict_root = 0;
	dbinfo.doc_n = 0;
	dbinfo.optimized = 0;
	dbinfo.phrase = opt->phrase;
	dbinfo.word_proximity = 0;
	dbinfo.stemming = af_stem_available() ? opt->stemming : 0;
	/* write db info */
	nbytes = write(fd, &dbinfo, sizeof(ETYMON_DB_INFO));
	if (nbytes != sizeof(ETYMON_DB_INFO)) {
		/* ERROR */
		printf("error writing DBI during db creation\n");
		exit(1);
	}
	close(fd);

	etymon_db_unlock(opt->dbname);
	
	return 0;
}


int etymon_db_create(ETYMON_DB_OPTIONS* opt) {
	etymon_db_delete(opt);
	return etymon_db_init(opt);
}
