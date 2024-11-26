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
#include "af.h"

using namespace std;
namespace io = boost::iostreams;

int af_query(ostream& out, const Thumprq* thrq, const string& datadir)
{
	FILE *fpipe;
	string command;
	command = "cd " + datadir + "/" + thrq->in_db + " && af -s -d " + thrq->in_db + " -Q '" + thrq->find + "'";
	
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
		if (line_split.size() != 8)
			continue;
		string filename = line_split[5];
		string begin = line_split[6];
		string end = line_split[7];

		out << "erc: (:unav) | (:unav) | (:unav) | " << filename << endl;
		out << "note: " << begin << " " << end << endl << endl;
                count++;
	}
        printf("[%i] returned %i records\n", getpid(), count);

	pclose(fpipe);

	return 0;
}
