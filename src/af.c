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
#include "linear.h"
/* #include "search_new.h" */

#define MAX_DBS (256)
#define MEMORYMIN (3)

static int cmd_index = 0;
static int index_create = 0;
static int index_phrase = 0;
static int index_stem = 1;
static int index_long_words = 1;
static int index_memory = MEMORYMIN;
static int index_dlevel = 1;
static int index_no_linear_buffer = 0;
static char *index_doctype = "text";
static char *index_split = "";
static int index_files_stdin = 0;

static int cmd_linearize = 0;

static int cmd_search = 0;
static char *search_query_boolean = "";
static int search_style = 0;
static int search_skiphits = 0;
static int search_numhits = -1;
static int search_totalhits = 0;

static int cmd_list = 0;

static int cmd_fetch = 0;

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
	if (!strcmp(opt, "skiphits")) {
		search_skiphits = atoi(arg);
		if (search_skiphits < 0)
			search_skiphits = 0;
		return 0;
	}
	if (!strcmp(opt, "totalhits")) {
		search_totalhits = 1;
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
	if (!strcmp(opt, "no-stem")) {
		index_stem = 0;
		return 0;
	}
	if (!strcmp(opt, "fetch")) {
		cmd_fetch = 1;
		return 0;
	}
	if (!strcmp(opt, "version")) {
		cmd_version = 1;
		return 0;
	}
	if (!strcmp(opt, "no-linear-buffer")) {
		index_no_linear_buffer = 1;
		return 0;
	}
/*
	if (!strcmp(opt, "long-words")) {
		index_long_words = 1;
		return 0;
	}
*/
	return 0;
}

static int process_opt(int argc, char *argv[])
{
	static struct option longopts[] = {
		{ "create", 0, 0, 'C' },
		{ "db", 1, 0, 'd' },
		{ "debug", 0, 0, 'D' },
		{ "dlevel", 1, 0, 0 },
		{ "doctype", 1, 0, 't' },
		{ "fetch", 0, 0, 0 },
		{ "index", 0, 0, 'i' },
		{ "linearize", 0, 0, 'L' },
		{ "list", 0, 0, 'l' },
/*		{ "long-words", 0, 0, 0 },*/
		{ "memory", 1, 0, 'm' },
		{ "no-linear-buffer", 0, 0, 0 },
		{ "no-stem", 0, 0, 0 },
		{ "numhits", 1, 0, 'n' },
		{ "phrase", 0, 0, 0 },
		{ "query-boolean", 1, 0, 'Q' },
		{ "search", 0, 0, 's' },
		{ "skiphits", 1, 0, 0 },
		{ "split", 1, 0, 0 },
		{ "style", 1, 0, 0 },
		{ "totalhits", 0, 0, 0 },
		{ "verbose", 0, 0, 'v' },
		{ "version", 0, 0, 0 },
		{ 0, 0, 0, 0 }
	};
	int g;

	while (1) {
		int longindex = 0;
		g = getopt_long(argc, argv,
				"CDFLQ:d:ilm:n:st:v",
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
		case 'L':
			cmd_linearize = 1;
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
		case 'D':
			if (verbose)
				verbose++;
			else
				verbose = 5;
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
			if (index_memory < MEMORYMIN)
				index_memory = MEMORYMIN;
			break;
		case 'n':
			search_numhits = atoi(optarg);
			if (search_numhits < 0)
				search_numhits = 0;
			break;
		case 't':
			index_doctype = optarg;
			break;
		case 'Q':
			search_query_boolean = optarg;
			break;
		case '?':
			return -1;
		default:
			printf("getopt error: %o\n", g);
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
	printf("index_stem = %i\n", index_stem);
	printf("index_memory = %i\n", index_memory);
	printf("index_dlevel = %i\n", index_dlevel);
	printf("index_doctype = %s\n", index_doctype);
	printf("index_split = %s\n", index_split);
	printf("index_files_stdin = %i\n", index_files_stdin);
	printf("cmd_search = %i\n", cmd_search);
	printf("search_query_boolean = %s\n", search_query_boolean);
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
	char dbname[AFPATHSIZE];
	int db_id;
	int doc_id;
	int parent;  /* doc_id of parent document */
        char filename[AFPATHSIZE]; /* document source file name */
	etymon_af_off_t begin; /* starting offset of document within the file */
        etymon_af_off_t end; /* ending offset of document within the file (one byte past end) */
} AFSEARCH_RESULT;

typedef struct AFSEARCH_RLIST_STRUCT {
	AFSEARCH_RESULT r;
	struct AFSEARCH_RLIST_STRUCT* next;
} AFSEARCH_RLIST;

typedef struct AFSEARCH_RTREE_STRUCT {
	AFSEARCH_RESULT r;
	struct AFSEARCH_RTREE_STRUCT* next;
	struct AFSEARCH_RTREE_STRUCT* child;
} AFSEARCH_RTREE;


void ses_presult(AFSEARCH_RESULT *res)
{
	printf("%i %s %i %i %s %ld %ld\n",
	       res->score / 100,
	       res->dbname,
	       res->doc_id,
	       res->parent,
	       res->filename,
	       (long)(res->begin),
	       (long)(res->end));
}

static void printerr(const char *s)
{
	fprintf(stderr, "%s: %s\n", argv0, s);
}

static int searcherr()
{
	switch (aferrno) {
	case AFEDBIO:
		printerr("Unable to read database");
		break;
	default:
		afperror(argv0);
	}
	return -1;
}

static int exec_search()
{
/*	ETYMON_AF_OPEN ope; */
	Afopen op;
	Afopen_r opr;
	Afsearch se;
	Afsearch_r ser;
	Afclose cl;
	Afclose_r clr;
/*	ETYMON_AF_SEARCH sea;*/
/*	ETYMON_AF_CLOSE clo;*/
	ETYMON_AF_LOG log;
	Uint2 dbid[ETYMON_AF_MAX_OPEN];
	int dbidn;
	int x;
	Afresultmd* resultmd;
	char** p;
	AFSEARCH_RESULT* res;
	int res_n;
	int r;
/*
//	AFSEARCH_RLIST* rlist_head = 0;
//	AFSEARCH_RLIST* rlist_tail = 0;
//	AFSEARCH_RLIST* rlist_p;
*/

	op.mode = "r";

	x = 0;
	for (x = 0; x < dbname_n; x++) {
		op.dbpath = dbname[x];
		if (afopen(&op, &opr) < 0)
			return searcherr();
/*			return afperror(argv0);*/
		dbid[x] = opr.dbid;
/*		if ( (db_id[x] = etymon_af_open(&ope)) == -1 ) {
			return -1;
		} */
	}
	dbidn = x;
	
	se.dbid = dbid;
	se.dbidn = dbidn;
	se.query = (Afchar *) search_query_boolean;
	se.qtype = AFQUERYBOOLEAN;
	/*
	sea.score_results = ses_options->score_results;
	sea.sort_results = ses_options->sort_results;
	*/
	se.score = AFSCOREDEFAULT;
	
	r = afsearch(&se, &ser);
	afsortscore(ser.result, ser.resultn);
	
	if (r != -1) {
		if (search_totalhits)
			printf("%i\n", ser.resultn);
	}
	
	if (r != -1 && search_numhits) {

		if (search_skiphits) {
			if (search_skiphits < ser.resultn) {
				size_t movebytes = (ser.resultn - search_skiphits) *
					sizeof (Afresult);
				memmove(ser.result,
					ser.result + search_skiphits,
					movebytes);
				ser.result = (Afresult *) realloc(
					ser.result,
					movebytes);
				ser.resultn = ser.resultn - search_skiphits;
			} else {
				ser.result = (Afresult *) realloc(
					ser.result, sizeof (Afresult));
				ser.resultn = 0;
			}
		}
		
		if (search_numhits > 0 && ser.resultn > search_numhits) {
			ser.resultn = search_numhits;
			ser.result = (Afresult *) realloc(
				ser.result,
				search_numhits * sizeof (Afresult));
		}
		
		res_n = ser.resultn;
		resultmd = (Afresultmd*)(malloc((res_n + 1) * sizeof(Afresultmd)));
		res = (AFSEARCH_RESULT*)(malloc((res_n + 1) * sizeof(AFSEARCH_RESULT)));
		if ( (!resultmd) || (!res) ) {
			fprintf(stderr, "af: unable to allocate memory for search results\n");
		} else {
			if (afgetresultmd(ser.result,
						      res_n,
						      resultmd) != -1) {
				for (x = 0; x < res_n; x++) {
					res[x].score = ser.result[x].score;
					strcpy(res[x].dbname,
					       dbname[ser.result[x].dbid - 1]);
					res[x].db_id =
						ser.result[x].dbid;
					res[x].doc_id =
						ser.result[x].docid;
					res[x].parent =
						resultmd[x].parent;
					memcpy(res[x].filename, resultmd[x].docpath, AFPATHSIZE);
					res[x].begin = resultmd[x].begin;
					res[x].end = resultmd[x].end;

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
					
/*					free(resultmd[x].docpath);*/
				}
			}

			free(resultmd);
		}

		if (ser.result) {
			free(ser.result);
		}

		if ( (search_style == 1) ||
		     (search_style == 2) ) {
			/* style == (lineage || tree) */
			int y, z;
/*			AFSEARCH_RTREE* rtrees[res_n]; */
			AFSEARCH_RTREE* rtree_head;
			AFSEARCH_RTREE* rtree_p;
			/* AFSEARCH_RTREE* rtree_p_new; */
			Afresult* results =
				(Afresult*)(malloc(2 * sizeof(Afresult)));
			resultmd = (Afresultmd*)(malloc(2 * sizeof(Afresultmd)));
			
			for (x = 0; x < res_n; x++) {
				rtree_head =
 					(AFSEARCH_RTREE*)(malloc(sizeof(AFSEARCH_RTREE)));
				memcpy(&(rtree_head->r), &(res[x]),
				       sizeof(AFSEARCH_RESULT));
				rtree_head->next = 0;
				rtree_head->child = 0;
				while (rtree_head->r.parent) {
					/*
					printf("Back... (db_id=%i)\n",
					       rtree_head->r.db_id);
					*/
					results->dbid = rtree_head->r.db_id;
					results->docid = rtree_head->r.parent;
					results->score = rtree_head->r.score;
					afgetresultmd(results, 1, resultmd);
					rtree_p =
						(AFSEARCH_RTREE*)(malloc(sizeof(AFSEARCH_RTREE)));
					rtree_p->r.score = rtree_head->r.score;
					rtree_p->r.doc_id = results->docid;
					rtree_p->r.db_id = results->dbid;
					rtree_p->r.parent = resultmd->parent;
					rtree_p->r.begin = resultmd->begin;
					rtree_p->r.end = resultmd->end;
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
			free(resultmd);
		}
		
		free(res);
	}
	
	for (x = 0; x < dbidn; x++) {
		cl.dbid = dbid[x];
		afclose(&cl, &clr);
	}

	return 0;
}

static int exec_index()
{
	/*ETYMON_DB_OPTIONS db_options;*/
	Afindex index_options;
	Afopen op;
	Afopen_r opr;
	Afclose cl;
	Afclose_r clr;

	op.dbpath = *dbname;
	op.mode = index_create ? "w+" : "r+";
	op.phrase = index_phrase;
	op.stem = index_stem;
	if (afopen(&op, &opr) < 0)
		return searcherr();

	/* set db options */
	/*
	memset(&db_options, 0, sizeof(ETYMON_DB_OPTIONS));
	db_options.log.error = log_error;
	db_options.dbname = *dbname;
	db_options.memory = index_memory;
	db_options.phrase = index_phrase;
	db_options.stem = index_stem;
	*/
	/* set indexing options */
	memset(&index_options, 0, sizeof index_options);
	index_options.dbid = opr.dbid;
	index_options.memory = index_memory;
	index_options.dlevel = index_dlevel;
	index_options.doctype = index_doctype;
	index_options.source = nonopt_argv;
	index_options.sourcen = nonopt_argv_n;
	index_options._stdin = index_files_stdin;
/*	index_options.word_proximity = 0; */
	index_options.split = (Afchar *) index_split;
	index_options.verbose = verbose;
	index_options._longwords = index_long_words;
	
	/* first check if we are to create a new database */
/*
	if (index_create) {
		etymon_db_create(&db_options);
	}
*/

	/* index input files */
	if ( (nonopt_argv_n != 0) || (index_files_stdin) ) {
		if (etymon_index_add_files(&index_options) == -1)
			return -1;
	}

	cl.dbid = opr.dbid;
	afclose(&cl, &clr);

	return 0;
}

static int exec_linearize()
{
/*	ETYMON_INDEX_OPTIONS index_options;*/

	/* set indexing options */
/*
	memset(&index_options, 0, sizeof(ETYMON_INDEX_OPTIONS));
	index_options.log.error = log_error;
	index_options.dbname = *dbname;
	index_options.memory = index_memory;
	index_options.dlevel = index_dlevel;
	index_options.dclass = index_doctype;
	index_options.files = nonopt_argv;
	index_options.files_n = nonopt_argv_n;
	index_options.files_stdin = index_files_stdin;
	index_options.split = index_split;
	index_options.verbose = verbose;
	index_options.dc_options = "";
	index_options.long_words = index_long_words;
*/
	
	Aflinear rq;
	rq.db = *dbname;
	rq.verbose = verbose;
	rq.memory = index_memory;
	rq.nobuffer = index_no_linear_buffer;
	if (_aflinear(&rq) < 0) {
		if (aferrno == AFELINEAR)
			aferror("Database is already linearized");
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

static int exec_fetch()
{
	int begin, end, dsize;
	char* filename;
	char* buffer;
        int fd;

	filename = nonopt_argv[0];
	begin = atoi(nonopt_argv[1]);
	end = atoi(nonopt_argv[2]);
	dsize = end - begin;

	if (dsize < 0)
		return aferror("Invalid begin/end offsets");
	
	fd = open(filename, 0);
	if (fd == -1)
		return aferror("Error opening file");
		
	buffer = (char*)(malloc(dsize));

	if (buffer) {
		if (lseek(fd, begin, SEEK_SET) == -1)
			return aferror("File seek error");
		
		if (read(fd, buffer, dsize) == -1)
			return aferror("File read error");

		close(fd);
		
		buffer[dsize] = '\0';
		printf("%s\n", buffer);
		
		free(buffer);
	} else {
		return aferror("Unable to allocate memory");
	}

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
	     (!cmd_linearize) &&
	     (!cmd_search) &&
	     (!cmd_list) &&
	     (!cmd_fetch) &&
	     (!cmd_version) )
		return aferror("No command option specified");
	if ((cmd_index +
	     cmd_linearize +
	     cmd_search +
	     cmd_list +
	     cmd_fetch +
	     cmd_version) > 1)
		return aferror("Too many command options specified");
	return 0;
}

static int validate_opt_index()
{
	if (!cmd_index)
		return 0;
	if (nonopt_argv_n == 0 && !index_create && !index_files_stdin)
		return aferror("No files specified for indexing");
	if (index_phrase && !index_create)
		return aferror("Option --phrase can only be used with -C");
	if ( strcmp(index_doctype, "text")
#ifdef ETYMON_AF_XML
	      && strcmp(index_doctype, "xml")
#endif
		)
		return aferror("Unsupported document type");
	     
	return 0;
}

static int validate_opt_search()
{
	if (!cmd_search)
		return 0;
	if (*search_query_boolean == '\0')
		return aferror("No query specified");
	return 0;
}

static int validate_opt_fetch()
{
	if (!cmd_fetch)
		return 0;
	if (nonopt_argv_n < 3)
		return aferror("Not enough arguments for fetch");
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
	if (validate_opt_fetch() < 0)
		return -1;
	if (dbname_n == 0) {
		if (cmd_index || cmd_linearize || cmd_search || cmd_list)
			return aferror("No database name specified");
	}
	return 0;
}

int afmain(int argc, char *argv[])
{
	argv0 = argv[0];
	if (process_opt(argc, argv) < 0)
		exit(-1);
	/* dump_opt(); */

	if (validate_opt() < 0)
		exit(-1);
	
	if (cmd_index)
		return exec_index();
	if (cmd_linearize)
		return exec_linearize();
	if (cmd_search)
		return exec_search();
	if (cmd_list)
		return exec_list();
	if (cmd_fetch)
		return exec_fetch();
	if (cmd_version)
		return exec_version();
		
	return -1;
}
