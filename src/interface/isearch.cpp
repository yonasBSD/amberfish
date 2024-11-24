#include <cstdio>
#include <string>
#include <vector>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "thump.h"
#include "isearch.h"

using namespace std;
namespace io = boost::iostreams;

int isearch_query(ostream& out, const Thumprq* thrq, const string& datadir)
{
	FILE *fpipe;
	string command;
	command = "cd " + datadir + "/" + thrq->in_db + " && ../../isearch2/bin/Isearch -d " + thrq->in_db + " -q -infix '" + thrq->find + "'";
	
        printf("[%i] executing: \"%s\"\n", getpid(), command.c_str());
	if ( !(fpipe = (FILE*) popen(command.c_str(),"r")) ) {
		perror("Problems with pipe");
		exit(1);
	}

#ifdef OLD_BOOST
	io::file_descriptor_source fds(fileno(fpipe));
#else
	io::file_descriptor_source fds(fileno(fpipe), io::never_close_handle);
#endif
	io::stream_buffer<io::file_descriptor_source> inb(fds);
	istream in(&inb);

	string line;
	for (int i = 0; i < 7; i++) {  // Skip first several lines.
		if (in)
			getline(in, line);
	}
        int count = 0;
	while (in)
	{
		getline(in, line);

		vector<string> line_split;
		stringstream line_stream(line);
		string token;
		while (getline(line_stream, token, ' ')) {
			trim(&token);
			if (token != "")
				line_split.push_back(token);
		}
		if (line_split.size() != 3)
			continue;
		string filename = line_split[2];

		out << "erc: (:unav) | (:unav) | (:unav) | " << filename << endl << endl;
                count++;
	}
        printf("[%i] returned %i records\n", getpid(), count);

	pclose(fpipe);

	return 0;
}
