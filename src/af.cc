/*
 *  Copyright (C) 2004  Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

/*
-i, --index
-C, --create
-l, --linearize
-F (deprecated)
nonopt_arg are files to index

-d, --db dbname (dup)
-v, --verbose (dup)
*/

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "index.h"
#include "admin.h"
#include "util.h"
#include "af_auto.h"

#define MAX_DBS (256)

static int action_index = 0;
static int index_create = 0;
static int index_linearize = 0;
static int index_files_stdin = 0; /* deprecated */

static int action_list = 0;

static char *dbname[MAX_DBS];
static int dbname_n = 0;
static int verbose = 0;

static char **nonopt_argv = NULL;
static int nonopt_argv_n = 0;

static int process_opt_long(char *opt, char *arg)
{
	if (!strcmp(opt, "list")) {
		action_list = 1;
		goto process_opt_long_break;
	}
 process_opt_long_break:
	return 0;
}

static int process_opt(int argc, char *argv[])
{
	static struct option longopts[] = {
		{"index", 0, 0, 'i'},
		{"create", 0, 0, 'C'},
		{"verbose", 0, 0, 'v'},
		{"linearize", 0, 0, 'l'},
		{"list", 0, 0, 0},
		{"db", 1, 0, 'd'},
		{0, 0, 0, 0}
	};
	int g;
	
	while (1) {
		int longindex = 0;
		g = getopt_long (argc, argv, "CFd:ilv",
				 longopts, &longindex);
		if (g == -1)
			break;
		switch (g) {
		case 0:
			process_opt_long(
				(char *)longopts[longindex].name,
				optarg);
			break;
		case 'i':
			action_index = 1;
			break;
		case 'C':
			index_create = 1;
			break;
		case 'v':
			verbose++;
			break;
		case 'l':
			index_linearize = 1;
			break;
		case 'F':
			index_files_stdin = 1;
			break;
		case 'd':
			if (dbname_n == MAX_DBS) {
				printf("Too many databases\n");
				return -1;
			}
			dbname[dbname_n++] = optarg;
			break;
		case '?':
			break;
		default:
			printf("getopt: %o\n", g);
		}
	}
	if (optind < argc) {
		nonopt_argv = argv + optind;
		nonopt_argv_n = argc - optind;
	}
	return 0;
}

static void dump_opt()
{
	int x;
	printf("action_index = %i\n", action_index);
	printf("action_list = %i\n", action_list);
	printf("index_create = %i\n", index_create);
	printf("index_linearize = %i\n", index_linearize);
	printf("index_files_stdin = %i (deprecated)\n", index_files_stdin);
	for (x = 0; x < dbname_n; x++)
		printf("db: %s\n", dbname[x]);
	printf("verbose = %i\n", verbose);
	if (nonopt_argv) {
	for (x = 0; x < nonopt_argv_n; x++)
		printf("nonopt_argv: %s\n", nonopt_argv[x]);
	}
}

static int exec_index()
{
	ETYMON_DB_OPTIONS db_options;
	ETYMON_INDEX_OPTIONS index_options;

	/* set db options */
	memset(&db_options, 0, sizeof(ETYMON_DB_OPTIONS));
	db_options.log.error = NULL; /*etymon_sei_log_error;*/
	db_options.dbname = *dbname;
	db_options.memory = 32; /*sei_options->memory;*/
	db_options.phrase = 0; /*sei_options->phrase;*/
	/* set indexing options */
	memset(&index_options, 0, sizeof(ETYMON_INDEX_OPTIONS));
	index_options.log.error = NULL; /*etymon_sei_log_error;*/
	index_options.dbname = *dbname;
	index_options.memory = 32; /*sei_options->memory;*/
	index_options.dlevel = 0; /*sei_options->dlevel;*/
	index_options.dclass = "text"; /*sei_options->dclass;*/
	index_options.files = nonopt_argv;
	index_options.files_n = nonopt_argv_n;
	index_options.files_stdin = index_files_stdin; /* deprecated */
	index_options.phrase = 0; /*sei_options->phrase;*/
	index_options.word_proximity = 0;
	index_options.split = ""; /*sei_options->split;*/
	index_options.verbose = verbose;
	index_options.dc_options = ""; /*sei_options->dc_options;*/
	
	/* first check if we are to create a new database */
	if (index_create) {
		etymon_db_create(&db_options);
	}

	/* index input files */
	if ( (nonopt_argv_n != 0) || (index_files_stdin) ) {
		if (etymon_index_add_files(&index_options) == -1)
			return -1;
	}

	if (index_linearize) {
		if (etymon_index_optimize(&index_options) == -1)
			return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	process_opt(argc, argv);
	dump_opt();
	/* need to validate and check consistency here */

	if (action_index)
		exec_index();
	
	return 0;
}
