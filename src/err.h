#ifndef _AF_ERR_H
#define _AF_ERR_H

#include <stdlib.h>
#include "config.h"

extern int aferrno;

#define AFEMEM          (1)     /* Out of memory (failed malloc) */
#define AFEBUFOVER      (2)     /* Buffer overflow (usually char[]) */
#define AFEUNDEF        (3)     /* Undefined/unknown value */
#define	AFEDBIO        (10)     /* I/O error while accessing a database file */
#define	AFEDBLOCK      (11)     /* Database locked */
#define	AFEVERSION     (12)     /* Incompatible database version */
#define	AFEBADDB       (13)     /* Corrupted database file */

static inline int aferr(int err)
{
	aferrno = err;
	return -1;
}

static inline void *aferrn(int err)
{
	aferrno = err;
	return NULL;
}

#endif
