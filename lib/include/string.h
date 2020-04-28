#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);
char *strtrim(char *s);

#endif // STRING_H_