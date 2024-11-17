#include <cstdio>
#include <string>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "thump.h"
#include "af.h"

using namespace std;
namespace io = boost::iostreams;

int af_query(ostream& out, const Thumprq* thrq)
{
	FILE *fpipe;
	string command;
	command = "cd ../../../data/" + thrq->in_db + " && ../../amberfish/bin/af -s -d " + thrq->in_db + " -Q '" + thrq->find + "'";
	
#ifdef DEBUG
	cout << command << endl;
#endif
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
	while (in)
	{
		getline(in, line);
		out << line << endl;
	}

	pclose(fpipe);

	return 0;
}
