#ifndef _OPT_H
#define _OPT_H

typedef struct {
	int help;
	int interactive;
	int port;
	int nargc;
	char **nargv;
} Opt;

int evalopt(int argc, char *argv[], Opt *opt);
void dumpopt(Opt *opt);

#endif
