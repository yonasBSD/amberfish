/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "open.h"
#include "util.h"
#include "lock.h"

static int etymon_af_init_flag = 0;

ETYMON_AF_STATE* etymon_af_state[ETYMON_AF_MAX_OPEN];

void etymon_af_init() {
	memset(etymon_af_state, 0, sizeof(ETYMON_AF_STATE*) * ETYMON_AF_MAX_OPEN);
	etymon_af_init_flag = 1;
}


/* possible errors:
   EX_IO
*/
/* assumes that db_id is valid */
int etymon_af_open_files(char* where, ETYMON_AF_LOG* log, int db_id, int flags) {
	int x_fn, x;
	
	/* open all database files */
	for (x_fn = 0; x_fn < ETYMON_AF_MAX_DB_FILES; x_fn++) {
		etymon_af_state[db_id]->fd[x_fn] = open(etymon_af_state[db_id]->fn[x_fn], flags | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
		if (etymon_af_state[db_id]->fd[x_fn] == -1) {
			etymon_af_log(log, EL_ERROR, EX_IO, where, etymon_af_state[db_id]->dbname, 
			       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
			/* before we leave, make an attempt to close all files */
			for (x = 0; x < x_fn; x++) {
				close(etymon_af_state[db_id]->fd[x]);
			}
			return -1;
		}
	}
	return 0;
}


/* possible errors:
   EX_IO
*/
/* assumes that db_id is valid */
int etymon_af_close_files(char* where, ETYMON_AF_LOG* log, int db_id) {
	int x_fn;
	
	/* close all the database files */
	for (x_fn = 0; x_fn < ETYMON_AF_MAX_DB_FILES; x_fn++) {
		if (close(etymon_af_state[db_id]->fd[x_fn]) == -1) {
			if (etymon_af_state[db_id]->read_only) {
				etymon_af_log(log, EL_WARNING, EX_IO, where, etymon_af_state[db_id]->dbname, 
				       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
			} else {
				etymon_af_log(log, EL_ERROR, EX_IO, where, etymon_af_state[db_id]->dbname, 
				       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
				return -1;
			}
		}
	}

	return 0;
}


/* possible errors:
   EX_DB_NAME_NULL
   EX_CREATE_READ_ONLY
   EX_DB_OPEN_LIMIT
   EX_DB_CREATE
   EX_DB_OPEN
   EX_DB_INCOMPATIBLE
   EX_DB_NOT_READY
*/
/* returns a database identifier in the range 1..255 */
int etymon_af_open(ETYMON_AF_OPEN* opt) {
	int db_id;
	uint4 magic;
	ssize_t nbytes;
	int x_fn;
	int flags;
	int x;
	ETYMON_AF_STAT st;
	ETYMON_LOG etymon_log; /* temporary - just for old locking functions */

	/* check for option errors */
	if (opt->dbname == NULL) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_NAME_NULL, "etymon_af_open()", NULL, NULL, NULL);
		return -1;
	}
	if ( (opt->read_only) && (opt->create) ) {
		etymon_af_log(opt->log, EL_ERROR, EX_CREATE_READ_ONLY, "etymon_af_open()", opt->dbname, NULL, NULL);
		return -1;
	}
	
	/* make sure we are initialized */
	if (etymon_af_init_flag == 0) {
		etymon_af_init();
	}

	/* find next free id */
	db_id = 1;
	while ( (db_id < ETYMON_AF_MAX_OPEN) && (etymon_af_state[db_id] != NULL) ) {
		db_id++;
	}
	if (db_id >= ETYMON_AF_MAX_OPEN) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_OPEN_LIMIT, "etymon_af_open()", opt->dbname, NULL, NULL);
		return -1;
	}

	/* create new table at the free id */
	etymon_af_state[db_id] = (ETYMON_AF_STATE*)(malloc(sizeof(ETYMON_AF_STATE)));
	if (etymon_af_state[db_id] == NULL) {
		etymon_af_log(opt->log, EL_CRITICAL, EX_MEMORY, "etymon_af_open()", opt->dbname, NULL, NULL);
		return -1;
	}
	etymon_af_state[db_id]->dbname = opt->dbname;
	etymon_af_state[db_id]->keep_open = opt->keep_open;
	etymon_af_state[db_id]->read_only = opt->read_only;
	
	/* construct file names */
	for (x_fn = 0; x_fn < ETYMON_AF_MAX_DB_FILES; x_fn++) {
		etymon_db_construct_path(x_fn, opt->dbname, etymon_af_state[db_id]->fn[x_fn]);
	}
	
	/* create database */
	if (opt->create) {

		/* open/create all database files */
		if (etymon_af_open_files("etymon_af_open()", opt->log, db_id, O_WRONLY | O_CREAT | O_TRUNC) == -1) {
			free(etymon_af_state[db_id]);
			return -1;
		}		

		/* clear any lock and then lock the database */
		etymon_db_unlock(opt->dbname, &etymon_log);
		etymon_db_lock(opt->dbname, &etymon_log);
		
		/* initialize dbinfo */
		magic = ETYMON_INDEX_MAGIC;
		nbytes = write(etymon_af_state[db_id]->fd[ETYMON_DBF_INFO], &magic, sizeof(uint4));
		if (nbytes != sizeof(uint4)) {
			etymon_af_log(opt->log, EL_ERROR, EX_DB_CREATE, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
			       etymon_af_state[db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
			free(etymon_af_state[db_id]);
			return -1;
		}
		sprintf(etymon_af_state[db_id]->info.version_stamp, ETYMON_AF_BANNER_STAMP);
		etymon_af_state[db_id]->info.udict_root = 0;
		etymon_af_state[db_id]->info.doc_n = 0;
		etymon_af_state[db_id]->info.optimized = 0;
		etymon_af_state[db_id]->info.phrase = 0;
		etymon_af_state[db_id]->info.word_proximity = 0;
		etymon_af_state[db_id]->info.stemming = 0;
		/* write db info */
		nbytes = write(etymon_af_state[db_id]->fd[ETYMON_DBF_INFO], &(etymon_af_state[db_id]->info), sizeof(ETYMON_DB_INFO));
		if (nbytes != sizeof(ETYMON_DB_INFO)) {
			etymon_af_log(opt->log, EL_ERROR, EX_DB_CREATE, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
			       etymon_af_state[db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
			free(etymon_af_state[db_id]);
			return -1;
		}
	    
		/* close all the database files */
		if (etymon_af_close_files("etymon_af_open()", opt->log, db_id) == -1) {
			return -1;
		}

		/* unlock the database */
		etymon_db_unlock(opt->dbname, &etymon_log);

	} /* create */ else {

		if (etymon_db_ready(opt->dbname, &etymon_log) == 0) {
			etymon_af_log(opt->log, EL_ERROR, EX_DB_NOT_READY, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
			       NULL, NULL);
			free(etymon_af_state[db_id]);
			return -1;
		}

	}

	/* open all database files */
	if (opt->read_only) {
		flags = O_RDONLY;
	} else {
		flags = O_RDWR;
	}
	if (etymon_af_open_files("etymon_af_open()", opt->log, db_id, flags) == -1) {
		free(etymon_af_state[db_id]);
		return -1;
	}		
	
	/* cache database information */
        nbytes = read(etymon_af_state[db_id]->fd[ETYMON_DBF_INFO], &magic, sizeof(uint4));
        if (nbytes != sizeof(uint4)) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_OPEN, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
		       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
		/* before we leave, make an attempt to close all files and free table */
		for (x = 0; x < x_fn; x++) {
			close(etymon_af_state[db_id]->fd[x]);
		}
		free(etymon_af_state[db_id]);
		return -1;
        }
	if (magic != ETYMON_INDEX_MAGIC) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_INCOMPATIBLE, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
		       NULL, NULL);
		/* before we leave, make an attempt to close all files and free table */
		for (x = 0; x < x_fn; x++) {
			close(etymon_af_state[db_id]->fd[x]);
		}
		free(etymon_af_state[db_id]);
		return -1;
	}
	nbytes = read(etymon_af_state[db_id]->fd[ETYMON_DBF_INFO], &(etymon_af_state[db_id]->info), sizeof(ETYMON_DB_INFO));
	if (nbytes != sizeof(ETYMON_DB_INFO)) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_OPEN, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
		       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
		/* before we leave, make an attempt to close all files and free table */
		for (x = 0; x < x_fn; x++) {
			close(etymon_af_state[db_id]->fd[x]);
		}
		free(etymon_af_state[db_id]);
		return -1;
	}

	/* cache field definitions */
	if (etymon_af_fstat(etymon_af_state[db_id]->fd[ETYMON_DBF_FDEF], &st) == -1) {
		perror("etymon_af_open:fstat()");
	}
	if (st.st_size > 0) {
		/* read fdef table into array */
		etymon_af_state[db_id]->fdef = (ETYMON_AF_FDEF_DISK*)(malloc(st.st_size));
		if (etymon_af_state[db_id]->fdef == NULL) {
			etymon_af_log(opt->log, EL_CRITICAL, EX_MEMORY, "etymon_af_open()", opt->dbname, NULL, NULL);
			return -1;
		}
		if (read(etymon_af_state[db_id]->fd[ETYMON_DBF_FDEF], etymon_af_state[db_id]->fdef, st.st_size) == -1) {
			etymon_af_log(opt->log, EL_ERROR, EX_DB_OPEN, "etymon_af_open()", etymon_af_state[db_id]->dbname, 
			       etymon_af_state[db_id]->fn[x_fn], strerror(errno));
			/* before we leave, make an attempt to close all files and free table */
			for (x = 0; x < x_fn; x++) {
				close(etymon_af_state[db_id]->fd[x]);
			}
			free(etymon_af_state[db_id]);
			return -1;
		}
		etymon_af_state[db_id]->fdef_count = st.st_size / sizeof(ETYMON_AF_FDEF_DISK);
	} else {
		etymon_af_state[db_id]->fdef = NULL;
		etymon_af_state[db_id]->fdef_count = 0;
	}
	
	/* close files */
	if (opt->keep_open == 0) {
		/* close all the database files */
		if (etymon_af_close_files("etymon_af_open()", opt->log, db_id) == -1) {
			return -1;
		}
	}

	return db_id;
}


/* possible errors:
   EX_IO
   EX_DB_ID_INVALID
*/
int etymon_af_close(ETYMON_AF_CLOSE* opt) {
	ssize_t nbytes;
	
	/* make sure we have a valid pointer in the table */
	if ( (opt->db_id < 1) || (etymon_af_state[opt->db_id] == NULL) ) {
		etymon_af_log(opt->log, EL_ERROR, EX_DB_ID_INVALID, "etymon_af_close()", NULL, NULL, NULL);
		return -1;
	}

	/* write out cached database info */
	if (etymon_af_state[opt->db_id]->read_only == 0) {
		/* first we may have to open the file */
		if (etymon_af_state[opt->db_id]->keep_open == 0) {
			etymon_af_state[opt->db_id]->fd[ETYMON_DBF_INFO] = open(etymon_af_state[opt->db_id]->fn[ETYMON_DBF_INFO],
									 O_RDWR | ETYMON_AF_O_LARGEFILE, ETYMON_DB_PERM);
			if (etymon_af_state[opt->db_id]->fd[ETYMON_DBF_INFO] == -1) {
				etymon_af_log(opt->log, EL_ERROR, EX_IO, "etymon_af_close()", etymon_af_state[opt->db_id]->dbname, 
				       etymon_af_state[opt->db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
				return -1;
			}
		}
		/* now write out dbinfo */
		if (etymon_af_lseek(etymon_af_state[opt->db_id]->fd[ETYMON_DBF_INFO], (etymon_af_off_t)4, SEEK_SET) == -1) {
			etymon_af_log(opt->log, EL_ERROR, EX_IO, "etymon_af_close()", etymon_af_state[opt->db_id]->dbname, 
			       etymon_af_state[opt->db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
			return -1;
		}
		nbytes = write(etymon_af_state[opt->db_id]->fd[ETYMON_DBF_INFO], &(etymon_af_state[opt->db_id]->info),
			       sizeof(ETYMON_DB_INFO));
		if (nbytes != sizeof(ETYMON_DB_INFO)) {
			etymon_af_log(opt->log, EL_ERROR, EX_IO, "etymon_af_close()", etymon_af_state[opt->db_id]->dbname, 
			       etymon_af_state[opt->db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
			return -1;
		}
		/* close the file if we opened it */
		if (etymon_af_state[opt->db_id]->keep_open == 0) {
			if (close(etymon_af_state[opt->db_id]->fd[ETYMON_DBF_INFO]) == -1) {
				etymon_af_log(opt->log, EL_ERROR, EX_IO, "etymon_af_close()", etymon_af_state[opt->db_id]->dbname, 
				       etymon_af_state[opt->db_id]->fn[ETYMON_DBF_INFO], strerror(errno));
			}
		}
	}
	
	/* close database files if they are open */
	if (etymon_af_state[opt->db_id]->keep_open) {
		if (etymon_af_close_files("etymon_af_close()", opt->log, opt->db_id) == -1) {
			return -1;
		}
	}

	/* free the table */
	if (etymon_af_state[opt->db_id]->fdef) {
		free(etymon_af_state[opt->db_id]->fdef);
	}
	free(etymon_af_state[opt->db_id]);
	etymon_af_state[opt->db_id] = NULL;
	
	return 0;
}
