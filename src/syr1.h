#ifndef DC_SYR1_H
#define DC_SYR1_H

#include "af.h"
#include "engine.h"

#ifdef af__cplusplus
extern "C" {
#endif

	int dc_syr1_init(ETYMON_AF_DC_INIT* dc_init);
	int dc_syr1_index(ETYMON_AF_DC_INDEX* dc_index);

#ifdef af__cplusplus
}
#endif
	
#endif
