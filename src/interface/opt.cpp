#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "opt.h"
#include "err.h"
#include "strtol.h"

void initopt(Opt *opt)
{
	opt->help = 0;
	opt->interactive = 0;
	opt->port = 0;
	opt->nargc = 0;
	opt->nargv = NULL;
}

static void optionerr(const char *msg, int g, char *optarg)
{
	fprintf(stderr, "%s: %s `-%c %s'\n", prgname, msg, g, optarg);
}

static int check_conflicts(Opt *opt)
{
	if (opt->interactive && opt->port) {
		fprintf(stderr, "%s: port ignored in interactive mode\n",
			prgname);
		return 0;
	}
	return 0;
}

static void evaloptlong(char *name, char *arg, Opt *opt)
{
	if (!strcmp(name, "help")) {
		opt->help = 1;
		return;
	}
}

int evalopt(int argc, char *argv[], Opt *opt)
{
	static struct option longopts[] = {
		{ "help", 0, 0, 0 },
		{ "interactive", 0, 0, 'I' },
		{ "port", 1, 0, 'p' },
		{ 0, 0, 0, 0 }
	};
	int g, x;

	initopt(opt);
	while (1) {
		int longindex = 0;
		g = getopt_long(argc, argv,
				"Ip:",
				longopts, &longindex);
		if (g == -1)
			break;
		if (g == '?' || g == ':')
			return -1;
		switch (g) {
		case 0:
			evaloptlong( (char *) longopts[longindex].name,
				     optarg, opt);
			break;
		case 'I':
			opt->interactive = 1;
			break;
		case 'p':
			if (str_to_int(optarg, &opt->port, 10) < 0
				|| opt->port < 1) {
				optionerr("invalid port", g, optarg);
				return -1;
			}
			break;
		}
	}
	if (optind < argc) {
		opt->nargv = argv + optind;
		opt->nargc = argc - optind;
	}
	if (opt->nargc > 0) {
		fprintf(stderr, "%s: unrecognized extra arguments `", prgname);
		for (x = 0; x < opt->nargc; x++) {
			if (x > 0)
				fprintf(stderr, " ");
			fprintf(stderr, "%s", opt->nargv[x]);
		}
		fprintf(stderr, "'\n");
		return -1;
	}
	if (check_conflicts(opt) < 0)
		return -1;
	return 0;
}
