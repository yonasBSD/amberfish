/*
 *  Copyright (C) 2004  Etymon Systems, Inc.
 *
 *  Authors:  Nassib Nassar
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SERVER_PORT (6145)

extern "C" {
#include "girs.h"
}

int search(GIRS_SEARCH_REQUEST *rq, GIRS_SEARCH_RESPONSE *rs)
{
	/* testing only */
	printf("Received query: [%s]\n", rq->query);
	rs->rset_n = 321;
	/* end test */
	
        return 0;
}

int main(int argc, char *argv[])
{
	GIRS_SERVER_START server_start;

	memset(&server_start, 0, sizeof(GIRS_SERVER_START));
	server_start.port = SERVER_PORT;
	server_start.f_search = search;
        return girs_server_start(&server_start);
}
