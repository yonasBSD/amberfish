/*
 *  Copyright (C) 1999-2004 Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "open.h"
#include "search.h"
#include "util.h"

typedef struct {
	char** dbname; /* list of database name */
	char** query;
	int sort_results;
	int score_results;
	int style;  /* reporting style */
} ETYMON_SES_OPTIONS;


void etymon_ses_validate_option(char** option, int min_arg_count) {
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


ETYMON_SES_OPTIONS* etymon_ses_process_options(char*** options) {
	ETYMON_SES_OPTIONS* ses_options;
	int len;
	int r;
	/* initialize option values */
	ses_options = (ETYMON_SES_OPTIONS*)(malloc(sizeof(ETYMON_SES_OPTIONS)));
	ses_options->dbname = (char**)(malloc(sizeof(char*) * 2));
	ses_options->dbname[0] = NULL;
	ses_options->query = NULL;
	ses_options->sort_results = ETYMON_AF_UNSORTED;
	ses_options->score_results = ETYMON_AF_UNSCORED;
	ses_options->style = 0;
	/* loop through and set option values */
	r = 0;
	while (options[r] != NULL) {
		int ok = 0;
		if (strlen(options[r][0]) == 2) {
			/* then it is a single character option, like "-d" */
			switch (options[r][0][1]) {
			case 'd':
				ok = 1;
				etymon_ses_validate_option(options[r], 1);
				/* compute options[r].length */
				len = 0;
				while (options[r][len] != NULL) {
					len++;
				}
				free(ses_options->dbname);
				ses_options->dbname = (char**)(malloc(sizeof(char*) * len));
				memcpy(ses_options->dbname, options[r] + 1, sizeof(char*) * len);
				break;
			case 'q':
				ok = 1;
				etymon_ses_validate_option(options[r], 1);
				/* compute options[r].length */
				len = 0;
				while (options[r][len] != NULL) {
					len++;
				}
				ses_options->query = (char**)(malloc(sizeof(char*) * (len - 1)));
				memcpy(ses_options->query, options[r] + 1, sizeof(char*) * (len - 1));
				break;
			default:
				break;
			}
		} else {
			/* otherwise it is a multi-character option */
			if (strcmp(options[r][0], "-os") == 0) {
				ok = 1;
				ses_options->score_results = ETYMON_AF_SCORE_DEFAULT;
				ses_options->sort_results = ETYMON_AF_SORT_SCORE;
			}
			if (strcmp(options[r][0], "-score") == 0) {
				ok = 1;
				ses_options->score_results = ETYMON_AF_SCORE_DEFAULT;
			}
			if (strcmp(options[r][0], "-style") == 0) {
				ok = 1;
				etymon_ses_validate_option(options[r], 1);
				if ( (strcmp(options[r][1], "list") != 0) &&
				     (strcmp(options[r][1], "lineage") != 0) ) {
					fprintf(stderr, "afsearch: %s: Unknown reporting style\n", options[r][1]);
					exit(1);
				}
				if (strcmp(options[r][1], "lineage") == 0) {
					ses_options->style = 1;
				} else {
					ses_options->style = 0;
				}
			}
		}
		if ( ! ok ) {
			printf("Unrecognized option \"%s\".\n", options[r][0]);
			exit(-1);
		}
		r++;
	}
	return ses_options;
}


void etymon_ses_print_options() {
	printf("\n");
	printf(ETYMON_AF_BANNER "\n");
	printf(ETYMON_AF_COPYRIGHT "\n");
	printf("\n");
	printf("Usage:   afsearch [option] [...]\n");
	printf("\n");
	printf("Options:\n");
	printf("   -d [db_name] [...]   Specifies the list of database names.\n");
	printf("   -q [query]           Specifies the search query.\n");
	printf("   -score               Computes relevance scores.\n");
	printf("   -os                  Computes relevance scores and sorts results by score.\n");
	printf("   -style [style_name]  Specifies the style for reporting results (default:\n");
	printf("                        list).  Supported styles: list\n");
#ifdef ETYMON_AF_XML
	printf("                                                  lineage (XML only)\n");
#endif
	printf("\n");
	printf("Query examples:\n");
	printf("   $ afsearch -d mydb -q '\"ice cream\"'\n");
	printf("   $ afsearch -d mydb -q 'title/.../\"tempest\"'\n");
	printf("   $ afsearch -d mydb -q 'cat* & (snow* | hat*)'\n");
	printf("\n");
	printf("To use a leading \"-\" in an argument, write it as \"--\".\n");
	printf("\n");
}


int etymon_ses_log_error(char* s, int e) {
	fprintf(stderr, "afsearch: %s\n", s);
	return 0;
}


void ses_log(const ETYMON_AF_EXCEPTION* ex) {
/* 	if (ex->level == 0) { */
		fprintf(stderr, "afsearch: %s [%s]\n", ex->msg, ex->where);
/* 	}  */
}


void etymon_ses_check_conflicts(ETYMON_SES_OPTIONS* ses_options) {

	if (ses_options->dbname[0] == NULL) {
		etymon_ses_log_error("no database specified", 1);
		exit(1);
	}
	
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


void ses_presult(AFSEARCH_RESULT* res) {
	printf("%i %s %i %i %s %ld %ld\n",
	       res->score,
	       res->dbname,
	       res->doc_id,
	       res->parent,
	       res->filename,
	       (long)(res->begin),
	       (long)(res->end));
}


void etymon_ses_execute(ETYMON_SES_OPTIONS* ses_options) {
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
	
	log.write = ses_log;

	x = 0;
	for (p = ses_options->dbname; *p != NULL; p++) {
		ope.dbname = *p;
		if ( (db_id[x++] = etymon_af_open(&ope)) == -1 ) {
			return;
		}
	}
	db_id[x] = 0;

	sea.db_id = db_id;
	sea.query = (unsigned char*)(ses_options->query[0]);
	sea.score_results = ses_options->score_results;
	sea.sort_results = ses_options->sort_results;
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
					       ses_options->dbname[sea.results[x].db_id - 1]);
					res[x].db_id =
						sea.results[x].db_id;
					res[x].doc_id =
						sea.results[x].doc_id;
					res[x].parent =
						eresults[x].parent;
					strcpy(res[x].filename, eresults[x].filename);
					res[x].begin = eresults[x].begin;
					res[x].end = eresults[x].end;

					if (ses_options->style == 1) {
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

		if ( (ses_options->style == 1) ||
		     (ses_options->style == 2) ) {
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
				if (ses_options->style == 1) {
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

}


int main(int argc, char *argv[]) {

	char*** options;
	ETYMON_SES_OPTIONS* ses_options;

	if (argc < 2) {
		etymon_ses_print_options();
		return -1;
	}

	options = etymon_split_options(argc, argv);

	ses_options = etymon_ses_process_options(options);

	etymon_ses_check_conflicts(ses_options);

	etymon_ses_execute(ses_options);
	
	if (ses_options->dbname) {
		free(ses_options->dbname);
	}
	if (ses_options->query) {
		free(ses_options->query);
	}
	free(ses_options);
	
	return 0;
}
