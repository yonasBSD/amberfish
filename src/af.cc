/*
 *  Copyright (C) 2004  Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "config.h"
#include "open.h"
#include "index.h"
#include "search.h"
#include "admin.h"
#include "explain.h"
#include "util.h"

#define MAX_DBS (256)

static int cmd_index = 0;
static int index_create = 0;
static int index_phrase = 0;
static int index_memory = 3;
static int index_dlevel = 1;
static int index_no_linear = 0;
static char *index_doctype = "text";
static char *index_split = "";
static int index_files_stdin = 0; /* deprecated */

static int cmd_search = 0;
static char *search_query = "";
static int search_style = 0;

static int cmd_list = 0;

static int cmd_version = 0;

static char *dbname[MAX_DBS];
static int dbname_n = 0;
static int verbose = 0;

static char **nonopt_argv = NULL;
static int nonopt_argv_n = 0;

static char *argv0 = NULL;

static int process_opt_long(char *opt, char *arg)
{
	if (!strcmp(opt, "split")) {
		index_split = arg;
		return 0;
	}
	if (!strcmp(opt, "dlevel")) {
		index_dlevel = atoi(arg);
		return 0;
	}
	if (!strcmp(opt, "style")) {
		search_style = strcmp(arg, "lineage") ? 0 : 1;
		return 0;
	}
	if (!strcmp(opt, "phrase")) {
		index_phrase = 1;
		return 0;
	}
	if (!strcmp(opt, "version")) {
		cmd_version = 1;
		return 0;
	}
	if (!strcmp(opt, "no-linear")) {
		index_no_linear = 1;
		return 0;
	}
	return 0;
}

static int process_opt(int argc, char *argv[])
{
	static struct option longopts[] = {
		{ "create", 0, 0, 'C' },
		{ "db", 1, 0, 'd' },
		{ "dlevel", 1, 0, 0 },
		{ "doctype", 1, 0, 't' },
		{ "index", 0, 0, 'i' },
		{ "list", 0, 0, 'l' },
		{ "memory", 1, 0, 'm' },
		{ "no-linear", 0, 0, 0 },
		{ "query", 1, 0, 'q' },
		{ "search", 0, 0, 's' },
		{ "split", 1, 0, 0 },
		{ "style", 1, 0, 0 },
		{ "phrase", 0, 0, 0 },
		{ "verbose", 0, 0, 'v' },
		{ "version", 0, 0, 0 },
		{ 0, 0, 0, 0 }
	};
	int g;

	while (1) {
		int longindex = 0;
		g = getopt_long(argc, argv,
				"CFd:ilm:q:st:v",
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
			cmd_index = 1;
			break;
		case 's':
			cmd_search = 1;
			break;
		case 'C':
			index_create = 1;
			break;
		case 'v':
			verbose++;
			break;
		case 'l':
			cmd_list = 1;
			break;
		case 'F':
			index_files_stdin = 1;
			break;
		case 'd':
			if (dbname_n == MAX_DBS) {
				fprintf(stderr,
					"%s: too many databases (maximum is %i)\n",
					argv[0], MAX_DBS);
				return -1;
			}
			dbname[dbname_n++] = optarg;
			break;
		case 'm':
			index_memory = atoi(optarg);
			break;
		case 't':
			index_doctype = optarg;
			break;
		case 'q':
			search_query = optarg;
			break;
		case '?':
			return -1;
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
	printf("cmd_index = %i\n", cmd_index);
	printf("index_create = %i\n", index_create);
	printf("index_phrase = %i\n", index_phrase);
	printf("index_memory = %i\n", index_memory);
	printf("index_dlevel = %i\n", index_dlevel);
	printf("index_doctype = %s\n", index_doctype);
	printf("index_split = %s\n", index_split);
	printf("index_no_linear = %i\n", index_no_linear);
	printf("index_files_stdin = %i (deprecated)\n", index_files_stdin);
	printf("cmd_search = %i\n", cmd_search);
	printf("search_query = %s\n", search_query);
	printf("search_style = %i\n", search_style);
	printf("cmd_list = %i\n", cmd_list);
	printf("cmd_version = %i\n", cmd_version);
	for (x = 0; x < dbname_n; x++)
		printf("db: %s\n", dbname[x]);
	printf("verbose = %i\n", verbose);
	if (nonopt_argv) {
	for (x = 0; x < nonopt_argv_n; x++)
		printf("nonopt_argv: %s\n", nonopt_argv[x]);
	}
}

static int aferror(char *s)
{
	fprintf(stderr, "%s: %s\n", argv0, s);
	return -1;
}

int log_error(char *s, int e)
{
	aferror(s);
	return 0;
}

void log_error_new(const ETYMON_AF_EXCEPTION *ex)
{
/* 	if (ex->level == 0) { */
	const char *msg = ex->msg;
	const char *where = ex->where;
	char *s = (char *)malloc(strlen(msg) + strlen(where) + 4);
	strcpy(s, msg);
	strcat(s, " [");
	strcat(s, where);
	strcat(s, "]");
	aferror(s);
	free(s);
/* 	}  */
}

typedef struct {
	int score;
	char dbname[ETYMON_MAX_PATH_SIZE];
	int db_id;
	int doc_id;
	int parent;  /* doc_id of parent document */
        char filename[ETYMON_MAX_PATH_SIZE]; /* document source file name */
	etymon_af_off_t begin; /* starting offset of document within the file */
        etymon_af_off_t end; /* ending offset of document within the file (one byte past end) */
} AFSEARCH_RESULT;

typedef struct AFSEARCH_RLIST_STRUCT {
	AFSEARCH_RESULT r;
	AFSEARCH_RLIST_STRUCT* next;
} AFSEARCH_RLIST;

typedef struct AFSEARCH_RTREE_STRUCT {
	AFSEARCH_RESULT r;
	AFSEARCH_RTREE_STRUCT* next;
	AFSEARCH_RTREE_STRUCT* child;
} AFSEARCH_RTREE;


void ses_presult(AFSEARCH_RESULT *res)
{
	printf("%i %s %i %i %s %ld %ld\n",
	       res->score,
	       res->dbname,
	       res->doc_id,
	       res->parent,
	       res->filename,
	       (long)(res->begin),
	       (long)(res->end));
}

static int exec_search()
{
	ETYMON_AF_OPEN ope;
	ETYMON_AF_SEARCH sea;
	ETYMON_AF_CLOSE clo;
	ETYMON_AF_LOG log;
	int db_id[256];
	int x;
	ETYMON_AF_ERESULT* eresults;
	char** p;
	AFSEARCH_RESULT* res;
	int res_n;
//	AFSEARCH_RLIST* rlist_head = 0;
//	AFSEARCH_RLIST* rlist_tail = 0;
//	AFSEARCH_RLIST* rlist_p;

	ope.read_only = 1;
	ope.create = 0;
	ope.keep_open = 0;
	ope.log = &log;
	
	log.write = log_error_new;

	x = 0;
	for (x = 0; x < dbname_n; x++) {
		ope.dbname = dbname[x];
		if ( (db_id[x] = etymon_af_open(&ope)) == -1 ) {
			return -1;
		}
	}
	db_id[x] = 0;

	sea.db_id = db_id;
	sea.query = (unsigned char*)(search_query);
	/*
	sea.score_results = ses_options->score_results;
	sea.sort_results = ses_options->sort_results;
	*/
	sea.score_results = ETYMON_AF_SCORE_DEFAULT;
	sea.sort_results = ETYMON_AF_SORT_SCORE;
	sea.log = &log;
	if (etymon_af_search(&sea) != -1) {

		res_n = sea.results_n;
		eresults = (ETYMON_AF_ERESULT*)(malloc((res_n + 1) * sizeof(ETYMON_AF_ERESULT)));
		res = (AFSEARCH_RESULT*)(malloc((res_n + 1) * sizeof(AFSEARCH_RESULT)));
		if ( (!eresults) || (!res) ) {
			fprintf(stderr, "afsearch: unable to allocate memory for search results\n");
		} else {
			if (etymon_af_resolve_results(sea.results, res_n, eresults, &log) != -1) {
				for (x = 0; x < res_n; x++) {
					res[x].score = sea.results[x].score;
					strcpy(res[x].dbname,
					       dbname[sea.results[x].db_id - 1]);
					res[x].db_id =
						sea.results[x].db_id;
					res[x].doc_id =
						sea.results[x].doc_id;
					res[x].parent =
						eresults[x].parent;
					strcpy(res[x].filename, eresults[x].filename);
					res[x].begin = eresults[x].begin;
					res[x].end = eresults[x].end;

					if (search_style == 1) {
						/* add to rlist */
						/*
						rlist_p =
							(AFSEARCH_RLIST*)(malloc(sizeof(AFSEARCH_RLIST)));
						memcpy(&(rlist_p->r),
						       &res,
						       sizeof(AFSEARCH_RESULT));
						rlist_p->next = 0;
						if (rlist_head) {
							rlist_tail->next = rlist_p;
						} else {
							rlist_head = rlist_p;
						}
						rlist_tail = rlist_p;
						*/
					} else {
						ses_presult(res + x);
					}
					
					free(eresults[x].filename);
				}
			}

			free(eresults);
		}

		if (sea.results) {
			free(sea.results);
		}

		if ( (search_style == 1) ||
		     (search_style == 2) ) {
			// style == (lineage || tree)
			int y, z;
//			AFSEARCH_RTREE* rtrees[res_n];
			AFSEARCH_RTREE* rtree_head;
			AFSEARCH_RTREE* rtree_p;
			//AFSEARCH_RTREE* rtree_p_new;
			ETYMON_AF_RESULT* results =
				(ETYMON_AF_RESULT*)(malloc(2 * sizeof(ETYMON_AF_RESULT)));
			eresults = (ETYMON_AF_ERESULT*)(malloc(2 * sizeof(ETYMON_AF_ERESULT)));
			
			for (x = 0; x < res_n; x++) {
				rtree_head =
 					(AFSEARCH_RTREE*)(malloc(sizeof(AFSEARCH_RTREE)));
				memcpy(&(rtree_head->r), &(res[x]),
				       sizeof(AFSEARCH_RESULT));
				rtree_head->next = 0;
				rtree_head->child = 0;
				while (rtree_head->r.parent) {
//					printf("Back... (db_id=%i)\n",
//					       rtree_head->r.db_id);
					results->db_id = rtree_head->r.db_id;
					results->doc_id = rtree_head->r.parent;
					results->score = rtree_head->r.score;
					etymon_af_resolve_results(results, 1, eresults, &log);
					rtree_p =
						(AFSEARCH_RTREE*)(malloc(sizeof(AFSEARCH_RTREE)));
					rtree_p->r.score = rtree_head->r.score;
					rtree_p->r.doc_id = results->doc_id;
					rtree_p->r.db_id = results->db_id;
					rtree_p->r.parent = eresults->parent;
					rtree_p->r.begin = eresults->begin;
					rtree_p->r.end = eresults->end;
					strcpy(rtree_p->r.dbname, rtree_head->r.dbname);
					strcpy(rtree_p->r.filename, rtree_head->r.filename);
					rtree_p->next = 0;
					rtree_p->child = rtree_head;
					rtree_head = rtree_p;
				}
				if (search_style == 1) {
					y = 0;
					while (rtree_head) {
						for (z = 0; z < y;
						     z++) printf(" ");
						printf("+ ");
						ses_presult(&(rtree_head->r));
						rtree_p = rtree_head;
						rtree_head =
							rtree_head->child;
						free(rtree_p);
						y++;
					}
				}
				
			}

			free(results);
			free(eresults);
		}
		
		free(res);
	}
	
	clo.log = &log;
	for (x = 0; db_id[x] != 0; x++) {
		clo.db_id = db_id[x];
		etymon_af_close(&clo);
	}

	return 0;
}

static int exec_index()
{
	ETYMON_DB_OPTIONS db_options;
	ETYMON_INDEX_OPTIONS index_options;

	/* set db options */
	memset(&db_options, 0, sizeof(ETYMON_DB_OPTIONS));
	db_options.log.error = log_error;
	db_options.dbname = *dbname;
	db_options.memory = index_memory;
	db_options.phrase = index_phrase;
	/* set indexing options */
	memset(&index_options, 0, sizeof(ETYMON_INDEX_OPTIONS));
	index_options.log.error = log_error;
	index_options.dbname = *dbname;
	index_options.memory = index_memory;
	index_options.dlevel = index_dlevel;
	index_options.dclass = index_doctype;
	index_options.files = nonopt_argv;
	index_options.files_n = nonopt_argv_n;
	index_options.files_stdin = index_files_stdin; /* deprecated */
	index_options.phrase = index_phrase;
	index_options.word_proximity = 0;
	index_options.split = index_split;
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

	if (!index_no_linear) {
		if (etymon_index_optimize(&index_options) == -1)
			return -1;
	}
	return 0;
}

static int exec_list()
{
	ETYMON_DB_OPTIONS db_options;
	ETYMON_AF_OPEN open_opt;
	ETYMON_AF_CLOSE close_opt;
	ETYMON_AF_EXPLAIN explain_opt;
	ETYMON_AF_LOG log;
	int db_id;
	int x;
	
	/* set db options */
	memset(&db_options, 0, sizeof(ETYMON_DB_OPTIONS));
	db_options.log.error = log_error;
	db_options.dbname = *dbname;

	/*
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
	*/
	
	/* if (sea_options->list == 1) { */
	etymon_db_list(&db_options);
	/* } */

	/*
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
	*/
	
	return 0;
}

static int exec_version()
{
	printf(AF_VERSION);
	printf("\n");
	return 0;
}

static int validate_opt_cmd()
{
	if ( (!cmd_index) &&
	     (!cmd_search) &&
	     (!cmd_list) &&
	     (!cmd_version) )
		return aferror("No command option specified");
	if ((cmd_index + cmd_search + cmd_list + cmd_version) > 1)
		return aferror("Too many command options specified");
	return 0;
}

static int validate_opt_index()
{
	if (!cmd_index)
		return 0;
	if (nonopt_argv_n == 0)
		return aferror("No files specified for indexing");
	return 0;
}

static int validate_opt_search()
{
	if (!cmd_search)
		return 0;
	if (*search_query == '\0')
		return aferror("No query specified");
	return 0;
}

static int validate_opt()
{
	if (validate_opt_cmd() < 0)
		return -1;
	if (validate_opt_index() < 0)
		return -1;
	if (validate_opt_search() < 0)
		return -1;
	if (dbname_n == 0) {
		if (cmd_index || cmd_search || cmd_list)
			return aferror("No database name specified");
	}
	return 0;
}

int main(int argc, char *argv[])
{
	argv0 = argv[0];
	if (process_opt(argc, argv) < 0)
		exit(-1);
	/* dump_opt(); */

	if (validate_opt() < 0)
		exit(-1);
	
	if (cmd_index)
		return exec_index();
	if (cmd_search)
		return exec_search();
	if (cmd_list)
		return exec_list();
	if (cmd_version)
		return exec_version();
		
	return -1;
}
