#include "af.h"
#include "engine.h"
#include "defs.h"
#include "text.h"
#include "xml.h"
#include "xml_test.h"
#include "syr1.h"

static ETYMON_AF_STATE* etymon_af_state[ETYMON_AF_MAX_OPEN];

#include "util.cc"
#include "docbuf.cc"
#include "fdef.cc"
#include "lock.cc"
#include "index.cc"
#include "admin.cc"

/* new interface */
#include "log.cc"
#include "open.cc"
#include "search.cc"
#include "explain.cc"
