#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int begin, end, dsize;
	char* filename;
	char* buffer;
        int fd;

	if (argc < 4) {
		printf("Usage:   affetch [file_name] [begin] [end]\n");
		return -1;
	}

	filename = argv[1];
	begin = atoi(argv[2]);
	end = atoi(argv[3]);
	dsize = end - begin;

	if (dsize < 0) {
		printf("Invalid begin/end offsets\n");
		return -2;
	}
	
	fd = open(argv[1], 0);
	if (fd == -1) {
		perror(filename);
		return -3;
	}
		
	buffer = (char*)(malloc(dsize));

	if (buffer) {
		if (lseek(fd, begin, SEEK_SET) == -1) {
			perror(filename);
			return -3;
		}
		
		if (read(fd, buffer, dsize) == -1) {
			perror(filename);
			return -3;
		}

		close(fd);
		
		buffer[dsize] = '\0';
		printf("%s\n", buffer);
		
		free(buffer);
	} else {
		printf("Unable to allocate memory\n");
	}

	return 0;
}
