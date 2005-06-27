//gsoapopt c
//gsoap ns1 service name: af
//gsoap ns1 service namespace: http://www.etymon.com/af.wsdl
//gsoap ns1 service location: http://www.etymon.com/
//gsoap ns1 service executable: afd
//gsoap ns1 schema namespace: urn:af

#include "search.h"

ns1__test(char **s);

struct ns1__searchRetrieveRequest {
        char *query;
};

struct ns1__searchRetrieveResponse {
        int numberOfRecords;
};

int ns1__search(struct ns1__searchRetrieveRequest *srq, struct ns1__searchRetrieveResponse *srs);
