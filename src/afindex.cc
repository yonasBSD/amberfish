#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "af.h"


typedef struct {
	char* dbname; /* database name */
	int memory; /* maximum amount of memory to use during indexing (MB) */
	int dlevel; /* maximum document level (nested documents) */
	char* dclass; /* document class */
	int create;
	int optimize;
	char** files;
	int files_stdin;
	int phrase;
	char* split; /* delimiter of multiple documents (if any)
			within a file */
	int verbose;
	char* dc_options;
} ETYMON_SEI_OPTIONS;


int etymon_sei_log_error(char* s, int e) {
	fprintf(stderr, "afindex: %s\n", s);
	return 0;
}


void etymon_sei_validate_option(char** option, int min_arg_count) {
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


ETYMON_SEI_OPTIONS* etymon_sei_process_options(char*** options) {
	ETYMON_SEI_OPTIONS* sei_options;
	int len;
	int r;
	/* initialize option values */
	sei_options = (ETYMON_SEI_OPTIONS*)(malloc(sizeof(ETYMON_SEI_OPTIONS)));
	sei_options->dbname = "";
	sei_options->memory = 3;
	sei_options->dlevel = 1;
	sei_options->dclass = "text";
	sei_options->create = 0;
	sei_options->optimize = 0;	
	sei_options->files = NULL;
	sei_options->files_stdin = 0;
	sei_options->phrase = 0;
	sei_options->split = "";
	sei_options->verbose = 0;
	sei_options->dc_options = "";
	/* loop through and set option values */
	r = 0;
	while (options[r] != NULL) {
		int ok = 0;
		if (strlen(options[r][0]) == 2) {
			/* then it is a single character option, like "-d" */
			switch (options[r][0][1]) {
			case 'd':
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				sei_options->dbname = options[r][1];
				break;
			case 'm':
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				sei_options->memory = atoi(options[r][1]);
				break;
			case 't':
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				if ( (strcmp(options[r][1], "text") != 0) &&
				     (strcmp(options[r][1], "xml") != 0) &&
				     (strcmp(options[r][1], "xml_test") != 0) &&
				     (strcmp(options[r][1], "syr1") != 0) ) {
					fprintf(stderr, "afindex: %s: Unknown document class\n", options[r][1]);
					exit(1);
				}
				sei_options->dclass = options[r][1];
				break;
			case 'C':
				ok = 1;
				sei_options->create = 1;
				break;
			case 'O':
				ok = 1;
				sei_options->optimize = 1;
				break;
			case 'f':
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				/* compute options[r].length */
				len = 0;
				while (options[r][len] != NULL) {
					len++;
				}
				sei_options->files = (char**)(malloc(sizeof(char*) * len));
				memcpy(sei_options->files, options[r] + 1, sizeof(char*) * len);
				break;
			case 'F':
				ok = 1;
				sei_options->files_stdin = 1;
				break;
			case 'v':
				ok = 1;
				sei_options->verbose = 1;
				break;
			default:
				break;
			}
		} else {
			/* otherwise it is a multi-character option */
			if (strcmp(options[r][0], "-phrase") == 0) {
				ok = 1;
				sei_options->phrase = 1;
			}
			if (strcmp(options[r][0], "-vv") == 0) {
				ok = 1;
				sei_options->verbose = 2;
			}
			if (strcmp(options[r][0], "-dc") == 0) {
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				sei_options->dc_options = options[r][1];
			}
			if (strcmp(options[r][0], "-dlevel") == 0) {
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				sei_options->dlevel =
					atoi(options[r][1]);
				if (sei_options->dlevel < 1) {
					etymon_sei_log_error(
						"resetting dlevel to 1", 2);
					sei_options->dlevel = 1;
				}
			}
			if (strcmp(options[r][0], "-split") == 0) {
				ok = 1;
				etymon_sei_validate_option(options[r], 1);
				sei_options->split = options[r][1];
			}
		}
		if ( ! ok ) {
			printf("Unrecognized option \"%s\".\n", options[r][0]);
			exit(-1);
		}
		r++;
	}
	return sei_options;
}


void etymon_sei_print_options() {
	printf("\n");
	printf(ETYMON_AF_BANNER "\n");
	printf(ETYMON_AF_COPYRIGHT "\n");
	printf("\n");
	printf("Usage:   afindex [option] [...]\n");
	printf("\n");
	printf("Options:\n");
	printf("   -d [db_name]      Specifies the database name.\n");
	printf("   -C                Creates a new database/overwrites an existing one.\n");
	printf("   -f [file] [...]   Specifies the files to index.\n");
	printf("   -F                Reads files to index from standard input.\n");
	printf("   -m [memory]       Specifies the maximum amount of memory in MB to use\n");
	printf("                     (default: 3).\n");
	printf("   -O                Optimizes the database.\n");
	printf("   -t [class_name]   Specifies the document class (default: text).\n");
	printf("                     Supported classes: text\n");
#ifdef ETYMON_AF_XML
	printf("                                        xml\n");
#endif
	printf("                                        syr1\n");
	printf("   -phrase           Enables phrase searching (used only with -C).\n");
	printf("   -split [delim]    Divides input files into multiple documents delimited\n");
	printf("                     by the specified string.\n");
	printf("   -dlevel [level]   Specifies the maximum resolution (levels of descent)\n");
	printf("                     for nested documents (default: 1).  Use this for XML\n");
	printf("                     instead of -split\n");
	printf("   -v                Prints name of files as they are indexed.\n");
	printf("   -vv               More verbose output.\n");
	printf("   -dc [options]     Passes options to the document class.\n");
	printf("\n");
	printf("To use a leading \"-\" in an argument, write it as \"--\".\n");
	printf("\n");
}


void etymon_sei_check_conflicts(ETYMON_SEI_OPTIONS* sei_options) {

	if (sei_options->dbname[0] == '\0') {
		etymon_sei_log_error("no database specified", 1);
		exit(1);
	}
	
	if ( (sei_options->files) && (sei_options->files_stdin) ) {
		etymon_sei_log_error("options `-f' and `-F' conflict", 1);
		exit(1);
	}

	if ( (sei_options->phrase) && (sei_options->create == 0) ) {
		etymon_sei_log_error("`-phrase' can only be used with `-C'", 1);
		exit(1);
	}
	
	if ( (sei_options->split[0] != '\0') &&
	     (strcmp(sei_options->dclass, "xml") == 0) ) {
		etymon_sei_log_error(
			"`-split' not supported by xml", 1);
		exit(1);
	}
	
	if ( (sei_options->split[0] != '\0') &&
	     (strcmp(sei_options->dclass, "xml_test") == 0) ) {
		etymon_sei_log_error(
			"`-split' not supported by xml_test", 1);
		exit(1);
	}
	
}


void etymon_sei_execute(ETYMON_SEI_OPTIONS* sei_options) {
	ETYMON_DB_OPTIONS* db_options;
	ETYMON_INDEX_OPTIONS* index_options;
	
	/* set db options */
	db_options = (ETYMON_DB_OPTIONS*)(malloc(sizeof(ETYMON_DB_OPTIONS)));
	db_options->log.error = etymon_sei_log_error;
	db_options->dbname = sei_options->dbname;
	db_options->memory = sei_options->memory;
	db_options->phrase = sei_options->phrase;

	/* set indexing options */
	index_options = (ETYMON_INDEX_OPTIONS*)(malloc(sizeof(ETYMON_INDEX_OPTIONS)));
	index_options->log.error = etymon_sei_log_error;
	index_options->dbname = sei_options->dbname;
	index_options->memory = sei_options->memory;
	index_options->dlevel = sei_options->dlevel;
	index_options->dclass = sei_options->dclass;
	index_options->files = sei_options->files;
	index_options->files_stdin = sei_options->files_stdin;
	index_options->phrase = sei_options->phrase;
	index_options->word_proximity = 0;
	index_options->split = sei_options->split;
	index_options->verbose = sei_options->verbose;
	index_options->dc_options = sei_options->dc_options;
	
	/* first check if we are to create a new database */
	if (sei_options->create) {
		etymon_db_create(db_options);
	}

	/* index input files */
	if ( (sei_options->files != NULL) || (sei_options->files_stdin) ) {
		if (etymon_index_add_files(index_options) == -1) {
			free(db_options);
			free(index_options);
			return;
		}
	}

	if (sei_options->optimize) {
		if (etymon_index_optimize(index_options) == -1) {
			free(db_options);
			free(index_options);
			return;
		}
	}
	
	free(db_options);
	free(index_options);
	
}


int main(int argc, char *argv[]) {
	char*** options;
	ETYMON_SEI_OPTIONS* sei_options;

	if (argc < 2) {
		etymon_sei_print_options();
		return -1;
	}

	options = etymon_split_options(argc, argv);

	sei_options = etymon_sei_process_options(options);

	etymon_sei_check_conflicts(sei_options);
	
	etymon_sei_execute(sei_options);

	if (sei_options->files) {
		free(sei_options->files);
	}
	free(sei_options);
	
	return 0;
}
