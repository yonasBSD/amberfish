#ifndef _AF_ERR_H
#define _AF_ERR_H

#include <stdlib.h>
#include "config.h"

extern int aferrno;

/*
 *  The first group of errors are designated as "default errors" and
 *  can be returned by almost any public function.
 */
#define AFEUNKNOWN      (1)   /* Unknown internal error */
#define AFEMEM          (2)   /* Out of memory (failed malloc) */
#define AFEBUFOVER      (3)   /* Buffer overflow (usually char[]) */
#define AFEINVAL        (4)   /* Invalid argument */
#define	AFEDBIO         (5)   /* I/O error */
#define	AFEDBLOCK       (6)   /* Database locked */
#define	AFEVERSION      (7)   /* Incompatible database version */
#define	AFEBADDB        (8)   /* Corrupted database file */
/*
 *  The following errors are only returned by certain functions.
 */
#define	AFELINEAR       (9)   /* Unsupported operation on linearized database */

#define AFMAXDEFERR     (8)
#define AFMAXERR        (9)

static char* aferrmsg[] = {
	"",
	"Unknown error",
	"Out of memory",
	"Buffer overflow",
	"Invalid argument",
	"I/O error",
	"Database locked",
	"Incompatible database version",
	"Corrupted database file",
	"Unsupported operation on linearized database"
};

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

char *afstrerror(int errnum);
int afperror(const char *string);

#endif
