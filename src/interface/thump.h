#ifndef _THUMP_H
#define _THUMP_H

#include <string>

using namespace std;

void yycompile(FILE *fin, FILE *fout);

typedef struct {
	string append_attr;
	string append_val;
	string as;
	string find;
	int help;
	string in_db;
	string in_table;
	string key;
	string list_len;
	string replace;
	string show;
	string sort;
	int unique;
	string user_name;
	string user_auth;
} Thumprq;

#endif
