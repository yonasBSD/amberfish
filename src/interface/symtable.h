#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define SYMTSIZE 1024

typedef struct {
	char *name;
	int isnew;
} Symbol;

extern Symbol symt[];
extern int symtn;

int symadd(char *sym);
int symaddstr(char *sym);
void cleanup_symt();

#ifdef __cplusplus
}
#endif

#endif
