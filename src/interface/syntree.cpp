/*
 *  Copyright (C) 2005  Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include "syntree.h"
#include "symtable.h"

//Syntree *tree;

Syntree *makecon(int id)
{
	Syntree *p;

	p = (Syntree *) malloc(sizeof (Syntree));
	p->type = 0;
	p->u.con.id = id;
	return p;
}

Syntree *makeid(int id)
{
	Syntree *p;

	p = (Syntree *) malloc(sizeof (Syntree));
	p->type = 1;
	p->u.id.id = id;
	return p;
}

Syntree *makeop(int oper, int opn, Syntree *opa0, Syntree *opa1, Syntree *opa2)
{
	Syntree *p;

	p = (Syntree *) malloc(sizeof (Syntree));
	p->type = 2;
	p->u.op.op = oper;
	p->u.op.opn = opn;
	p->u.op.opa[0] = opa0;
	p->u.op.opa[1] = opa1;
	p->u.op.opa[2] = opa2;
	return p;
}

static void indent(int ind)
{
	int x;
	printf("[%d] ", getpid());
	for (x = 0; x < ind; x++) {
		printf(" ");
	}
}

static void dumpsubtree(Syntree *p, int ind)
{
	int x;
	if (!p) {
		indent(ind);
		printf("(NULL)\n");
		return;
	}

	fflush(stdout);
	switch (p->type) {
	case 0:
		indent(ind);
		printf("(C) \"%s\"\n", symt[p->u.con.id].name);
		break;
	case 1:
		indent(ind);
		printf("(I) \"%s\"\n", symt[p->u.id.id].name);
		break;
	case 2:
		indent(ind);
		printf("(O) ");
		switch (p->u.op.op) {
		default:
			if (isprint(p->u.op.op))
				printf("'%c'", p->u.op.op);
			else
				printf("%d", p->u.op.op);
		}
		printf(" [%d] {\n", p->u.op.opn);
		for (x = 0; x < p->u.op.opn; x++) {
			dumpsubtree(p->u.op.opa[x], ind + 4);
		}
		indent(ind);
		printf("}\n");
	}
}

void dumptree(Syntree *p)
{
	dumpsubtree(p, 0);
}

void syntree_free(Syntree *p)
{
	int x;

	/*
	if (p) {
		if (p->type == 2) {
			for (x = 0; x < p->u.op.opn; x++)
				syntree_free(p->u.op.opa[x]);
		}
		free(p);
	}
	*/
}
