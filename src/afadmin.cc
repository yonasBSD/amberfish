/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "admin.h"
#include "open.h"
#include "explain.h"
#include "util.h"

typedef struct {
	char* dbname; /* database name */
	int list; /* list documents */
	int list_fields; /* list fields */
	int status; /* database status */
} ETYMON_SEA_OPTIONS;


void etymon_sea_validate_option(char** option, int min_arg_count) {
	/* compute option length */
	int len = 0;
	while (option[len] != NULL) {
		len++;
	}
	if (len < (min_arg_count + 1)) {
		printf("Missing arguments for option \"%s\".\n", option[0]);
		exit(-1);
	}
}


ETYMON_SEA_OPTIONS* etymon_sea_process_options(char*** options) {
	ETYMON_SEA_OPTIONS* sea_options;
	int r;
	/* initialize option values */
	sea_options = (ETYMON_SEA_OPTIONS*)(malloc(sizeof(ETYMON_SEA_OPTIONS)));
	sea_options->dbname = "";
	sea_options->list = 0;
	sea_options->list_fields = 0;
	sea_options->status = 0;
	/* loop through and set option values */
	r = 0;
	while (options[r] != NULL) {
		int ok = 0;
		if (strlen(options[r][0]) == 2) {
			/* then it is a single character option, like "-d" */
			switch (options[r][0][1]) {
			case 'd':
				ok = 1;
				etymon_sea_validate_option(options[r], 1);
				sea_options->dbname = options[r][1];
				break;
			case 'l':
				ok = 1;
				sea_options->list = 1;
				break;
			default:
				break;
			}
		} else {
			/* otherwise it is a multi-character option */
			if (strcmp(options[r][0], "-lf") == 0) {
				ok = 1;
				sea_options->list_fields = 1;
			}
			if (strcmp(options[r][0], "-status") == 0) {
				ok = 1;
				sea_options->status = 1;
			}
		}
		if ( ! ok ) {
			printf("Unrecognized option \"%s\".\n", options[r][0]);
			exit(-1);
		}
		r++;
	}
	return sea_options;
}


void etymon_sea_print_options() {
	printf("\n");
	printf(ETYMON_AF_BANNER "\n");
	printf(ETYMON_AF_COPYRIGHT "\n");
	printf("\n");
	printf("Usage:   afadmin [option] [...]\n");
	printf("\n");
	printf("Options:\n");
	printf("   -d [db_name]   Specifies the database name.\n");
	printf("   -l             Lists the documents in the database.\n");
	printf("   -lf            Lists the known fields in the database.\n");
	printf("   -status        Prints the status of the database.\n");
	printf("\n");
	printf("To use a leading \"-\" in an argument, write it as \"--\".\n");
	printf("\n");
}


int etymon_sea_log_error(char* s, int e) {
	fprintf(stderr, "afadmin: %s\n", s);
	return 0;
}


void afadmin_log(const ETYMON_AF_EXCEPTION* ex) {
/* 	if (ex->level == 0) { */
		fprintf(stderr, "afadmin: %s [%s]\n", ex->msg, ex->where);
/* 	}  */
}


void afadmin_log_status(const ETYMON_AF_EXCEPTION* ex) {
	/* filter out database not ready exception - to be used when
           explicitly testing status with -status flag. */
	if (ex->code == EX_DB_NOT_READY) {
		printf("status=not_ready\n");
	} else {
		afadmin_log(ex);
	}
}


void etymon_sea_check_conflicts(ETYMON_SEA_OPTIONS* sea_options) {

	if (sea_options->dbname[0] == '\0') {
		etymon_sea_log_error("no database specified", 1);
		exit(1);
	}
	
}


void etymon_sea_execute(ETYMON_SEA_OPTIONS* sea_options) {
	ETYMON_DB_OPTIONS* db_options;
	ETYMON_AF_OPEN open_opt;
	ETYMON_AF_CLOSE close_opt;
	ETYMON_AF_EXPLAIN explain_opt;
	ETYMON_AF_LOG log;
	int db_id;
	int x;
	
	/* set db options */
	db_options = (ETYMON_DB_OPTIONS*)(malloc(sizeof(ETYMON_DB_OPTIONS)));
	db_options->log.error = etymon_sea_log_error;
	db_options->dbname = sea_options->dbname;

	if (sea_options->status == 1) {
		open_opt.dbname = sea_options->dbname;
		open_opt.read_only = 1;
		open_opt.create = 0;
		open_opt.keep_open = 0;
		open_opt.log = &log;
		log.write = afadmin_log_status;
		db_id = etymon_af_open(&open_opt);
		if (db_id != -1) {
			printf("status=ready\n");
			close_opt.log = &log;
			log.write = afadmin_log;
			close_opt.db_id = db_id;
			etymon_af_close(&close_opt);
		}
	}
	
	if (sea_options->list == 1) {
		etymon_db_list(db_options);
	}

	if (sea_options->list_fields == 1) {
		open_opt.dbname = sea_options->dbname;
		open_opt.read_only = 1;
		open_opt.create = 0;
		open_opt.keep_open = 0;
		open_opt.log = &log;
		log.write = afadmin_log;
		db_id = etymon_af_open(&open_opt);
		if (db_id != -1) {
			explain_opt.db_id = db_id;
			explain_opt.list_fields = 1;
			explain_opt.log = &log;
			if (etymon_af_explain(&explain_opt) != -1) {
				for (x = 0; x < explain_opt.fields_n; x++) {
					printf("%s\n", explain_opt.fields[x].name);
				}
			}
			close_opt.db_id = db_id;
			close_opt.log = &log;
			etymon_af_close(&close_opt);
		}
	}

	free(db_options);
	
}


int main(int argc, char *argv[]) {
	char*** options;
	ETYMON_SEA_OPTIONS* sea_options;

	if (argc < 2) {
		etymon_sea_print_options();
		return -1;
	}

	options = etymon_split_options(argc, argv);

	sea_options = etymon_sea_process_options(options);

	etymon_sea_check_conflicts(sea_options);

	etymon_sea_execute(sea_options);

	free(sea_options);
	
	return 0;
}
