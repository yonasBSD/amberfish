#ifndef _STRTOL_H
#define _STRTOL_H

#ifdef __cplusplus
extern "C" {
#endif

int str_to_long(const char *s, long *val, int base);
int str_to_int(const char *s, int *val, int base);

#ifdef __cplusplus
}
#endif

#endif
