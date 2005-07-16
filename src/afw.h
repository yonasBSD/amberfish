//gsoapopt c

#include "search.h"

SRW__test(char **s);

struct SRWRecord {
	char *SRW__recordData;
};

struct SRWRecords {
	struct SRWRecord *__ptrSRW__record;
	int __size;
};

struct SRW__searchRetrieveResponse {
        int SRW__numberOfRecords;
	struct SRWRecords SRW__records;
};

int SRW__searchRetrieveRequest(char *SRW__query, 
			       struct SRW__searchRetrieveResponse *srs);
