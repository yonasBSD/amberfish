#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define PATHSIZE (1024)

int main(int argc, char *argv[])
{
	int x;
	char sfn[PATHSIZE];
	FILE *sfile;
	FILE *ofile;
	off_t ssize;
	char *data;
	int datasize;
	struct stat st;
	char *p;
	char *pos;
	char docno[32], docno1[32], docno2[32], docno3[32];
	char ofn[PATHSIZE];
	mode_t fmode;
	
	datasize = 8;
	data = (char *) malloc(datasize);

	while (fgets(sfn, PATHSIZE, stdin)) {

		/* remove '\n' at end */
		x = strlen(sfn);
		if (x > 0 && sfn[x - 1] == '\n')
			sfn[x - 1] = '\0';
		while (strlen(sfn) > 0 && sfn[strlen(sfn) - 1] == ' ')
			sfn[strlen(sfn) - 1] = '\0';
		if (*sfn == '\0')
			continue;

		sfile = fopen(sfn, "r");
		if (!sfile) {
			printf("Error opening [%s]\n", sfn);
			continue;
		}
		printf("%s\n", sfn);

		/* load the source file */

/*
		fseeko(sfile, 0, SEEK_END);
		ssize = ftello(sfile);
		fseeko(sfile, 0, SEEK_SET);
*/
		fstat(fileno(sfile), &st);
		ssize = st.st_size;
		if ((ssize + 1) > datasize) {
			datasize = ssize + 1;
			printf("Using %lu bytes of memory\n", datasize);
			free(data);
			data = (char *) malloc(datasize);
		}

		fread(data, 1, ssize, sfile);
		data[ssize] = '\0';

		/* check for '\0' */

		for (x = 0; x < ssize; x++) {
			if (data[x] == '\0')
				data[x] = ' ';
		}

		/* parse the source file */

		pos = data;

		while (1) {

			if (!(pos = strstr(pos, "<DOCNO>")))
				break;
			pos += 7;
			strncpy(docno, pos, 24);
			docno[24] = '\0';
			if (!(p = strchr(docno, '<'))) {
				printf("Error parsing DOCNO: [%s] from [", docno);
				for (x = 0; x < 80; x++)
					printf("%c", pos[x]);
				printf("]\n");
				continue;
			}
			*p = '\0';
			printf("Found DOCNO: [%s]\n", docno);
			strcpy(docno1, docno);
			if (!(p = strchr(docno1, '-'))) {
				printf("Error parsing DOCNO components from [%s]\n", docno);
				continue;
			}
			*p = '\0';
			strcpy(docno2, p + 1);
			if (!(p = strchr(docno2, '-'))) {
				printf("Error parsing DOCNO components from [%s]\n", docno);
				continue;
			}
			*p = '\0';
			strcpy(docno3, p + 1);
/*
			printf("Parsed DOCNO components: [%s] [%s] [%s]\n", docno1, docno2, docno3);
*/

			fmode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
			strcpy(ofn, docno1);
/*			printf("Making directory [%s]\n", ofn);*/
			mkdir(ofn, fmode);
			strcat(ofn, "/");
			strcat(ofn, docno2);
/*			printf("Making directory [%s]\n", ofn);*/
			mkdir(ofn, fmode);

			strcat(ofn, "/");
			strcat(ofn, docno);
/*			printf("Writing file [%s]\n", ofn);*/

			ofile = fopen(ofn, "w");
			if (!ofile) {
				printf("Error writing file: [%s]\nHalting\n", ofn);
				break;
			}

			pos = strstr(pos, "</DOCHDR>");
			if (!pos) {
				printf("Error parsing /DOCHDR\n");
				continue;
			}

			pos += 9;
			p = strstr(pos, "</DOC>");
			if (p)
				*p = '\0';
			if (fwrite(pos, 1, x = strlen(pos), ofile) < x) {
				printf("Error writing to file: [%s]\nHalting\n", ofn);
				break;
			}
/*			printf("Wrote %i bytes to file [%s]\n", x, ofn);*/
			if (p)
				pos = p + 1;

			if (fclose(ofile) != 0) {
				printf("Error closing file: [%s]\nHalting\n", ofn);
				break;
			}
			

/*
			for (x = 0; x < 80; x++)
				printf("%c", pos[x]);
			printf("\n");
*/

			
			

			if (!(pos = strstr(pos, "<DOC>")))
				break;
		}

		/* close the source file */

		fclose(sfile);
		
	}

	free(data);

	return 0;
}
