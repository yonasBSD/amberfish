/*
 *  Copyright (C) 2005  Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "symtable.h"
#include "strtol.h"

Symbol symt[SYMTSIZE];
int symtn = 0;

extern FILE *yyout;
extern void comp_error();

char *decode_url(char *url)
{
	char *ns, *nsp;
	char *p;
	char hex[3];
	long hexc;

	hex[2] = '\0';
	ns = malloc(strlen(url) + 1);
	nsp = ns;
	p = url;
	while (1) {
		switch (*p) {
		case '\0':
			*nsp = '\0';
			return ns;
		case '%':
			hex[0] = *(++p);
			if (!hex[0]) {
				comp_error();
				fprintf(yyout, "Invalid URL encoding: %%\r\n");
				exit(EXIT_FAILURE);
			}
			hex[1] = *(++p);
			if (!hex[1]) {
				comp_error();
				fprintf(yyout, "Invalid URL encoding: %%%c\r\n", hex[0]);
				exit(EXIT_FAILURE);
			}
			if (str_to_long(hex, &hexc, 16) < 0) {
				comp_error();
				fprintf(yyout, "Invalid URL encoding: %%%c%c\r\n", hex[0], hex[1]);
				exit(EXIT_FAILURE);
			}
			*(nsp++) = (char) hexc;
			break;
		case '+':
			*(nsp++) = ' ';
			break;
		default:
			*(nsp++) = *p;
		}
		p++;
	}
}

/*
void str_to_sql(char **str)
{
	char *ns, *nsp;
	char *p;

	ns = malloc(strlen(*str) * 2);
	strcpy(ns, " '");
	nsp = ns + 2;
	p = *str + 1;
	while (1) {
		switch (*p) {
		case '"':
			*(nsp++) = '\'';
			*nsp = '\0';
			free(*str);
			*str = ns;
			return;
		case '\'':
			*(nsp++) = '\'';
			*(nsp++) = '\'';
			break;
		case '\\':
			p++;
			switch (*p) {
			case '"':
				*(nsp++) = '"';
				break;
			default:
				*(nsp++) = '\\';
				*(nsp++) = *p;
			}
			break;
		default:
			*(nsp++) = *p;
		}
		p++;
	}
}
*/

/**
 * Look up and return the index of a symbol in the symbol table,
 * adding it if the table does not contain it.
 */
static int addsym(char *sym, int string)
{
	if (symtn == SYMTSIZE) {
		printf("[%d] symbol table overflow\n", getpid());
		exit(EXIT_FAILURE);
	}
	for (int x = 0; x < symtn; x++) {
		if (strcmp(symt[x].name, sym) == 0) {
			symt[x].isnew = 0;
			return x;
		}
	}
	symt[symtn].name = decode_url(sym);
	if (string) {
		/*str_to_sql(&(symt[symtn].name));*/
		printf("[%d] string type unsupported in symbol: %s\n", getpid(), sym);
		exit(EXIT_FAILURE);
	}
	symt[symtn].isnew = 1;
	return symtn++;
}

int symadd(char *sym)
{
	return addsym(sym, 0);
}

int symaddstr(char *sym)
{
	return addsym(sym, 1);
}

void cleanup_symt() {
	int x;

	for (x = 0; x < symtn; x++)
		free(symt[x].name);
}

/*
void dumpsymt()
{
	int x;

	fprintf(yyout, "\n;  Symbol table\n");
	for (x = 0; x < symtn; x++) {
		fprintf(yyout, ";  (%d) \"%s\"\tV=%d\n", x, symt[x].name, symt[x].var);
	}
}
*/
