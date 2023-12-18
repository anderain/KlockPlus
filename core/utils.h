#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>

#define     STR_EQUAL(s1, s2) (((s1) != NULL) && ((s2) != NULL) && strcmp((s1), (s2)) == 0)
int         STR_COPY(char *dest, int max, const char *src);
char *      STR_DUMP(const char *str);

#define DEFAUL_FTOA_PRECISION   8

double  k_atof(const char *s);
char *  k_ftoa(double f, char * buf, int precision);
char*   k_itoa(int num, char* str, int base);
long    k_atoi(const char* S);

#endif