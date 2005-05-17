//gsoapopt c
//gsoap ns1 service name: af
//gsoap ns1 service namespace: http://www.etymon.com/af.wsdl
//gsoap ns1 service location: http://www.etymon.com/
//gsoap ns1 service executable: afd
//gsoap ns1 schema namespace: urn:af

#include "search.h"

ns1__test(char **s);

struct ns1__search_rq {
	char *db;
        char *query;
};

struct ns1__result {
	int docid;
	int score;
	int dbid;
};

struct ns1__search_rs {
	struct ns1__result *result;
        int resultn;
};

int ns1__search(struct ns1__search_rq *srq, struct ns1__search_rs *srs);
