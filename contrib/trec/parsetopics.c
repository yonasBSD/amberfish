#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define STRSIZE (1024)

static const char *afcmd = "time af-release -dGX00 -dGX01 -dGX02 -dGX03 -dGX04 -dGX05 -dGX06 -dGX07 -dGX08 -dGX09 -dGX10 -dGX11 -dGX12 -dGX13 -dGX14 -dGX15 -dGX16 -dGX17 -dGX18 -dGX19 -dGX20 -dGX21 -dGX22 -dGX23 -dGX24 -dGX25 -dGX26 -dGX27 -n10000 --style=trec --trec-tag nn04base ";

static const char *nextword(const char *pos, char *word)
{
	const char *p;
	int x;

	p = pos;
	x = 0;

	while (*p && !isalnum(*p))
		p++;
	while (isalnum(*p) || *p == '.')
		word[x++] = tolower(*(p++));

	if (x > 0 && word[x - 1] == '.')
		x--;
	
	word[x] = '\0';
	return p;
}

static int goodword(const char *word)
{
	return strlen(word) > 2 ? 1 : 0;
}

static void finish()
{
	printf("' > results\n");
}

int main(int argc, char *argv[])
{
	int x;
	char line[STRSIZE];
	char word[STRSIZE];
	char num[STRSIZE];
	const char *pos;
	int started;
	int multi;
	int firstterm;
	
	printf("#!/bin/sh\n");

	started = 0;
	multi = 0;
	firstterm = 0;
	
	while (fgets(line, STRSIZE, stdin)) {

		/* remove '\n' at end */
		x = strlen(line);
		if (x > 0 && line[x - 1] == '\n')
			line[x - 1] = '\0';
		while (strlen(line) > 0 && line[strlen(line) - 1] == ' ')
			line[strlen(line) - 1] = '\0';
		if (*line == '\0')
			continue;

/*		fprintf(stderr, "-> %s\n", line);*/

		pos = line;

		if (multi) {
			if (*pos == '<') {
				multi = 0;
				continue;
			}
			do {
				pos = nextword(pos, word);
				if (goodword(word)) {
					if (firstterm)
						firstterm = 0;
					else
						printf(" | ");
					printf("%s", word);
				}
			} while (*pos);
		}
		
		if (!strncmp(pos, "<num>", 5)) {
			if (started)
				finish();
			pos = strstr(pos, "Number:") + 7;
			pos = nextword(pos, num);
			fprintf(stderr, "Found num = [%s]\n", num);
			printf("### TOPIC %s ###\n", num);
			printf("echo \"\"\n");
			printf("echo \"Topic %s\"\n", num);
			printf("%s", afcmd);
			printf("--trec-topic %s -sQ '", num);
			firstterm = 1;
			started = 1;
			continue;
		}

		if (!strncmp(pos, "<title>", 7)) {
			multi = 1;
			continue;
		}

	}

	if (started)
		finish();
	
	return 0;
}
