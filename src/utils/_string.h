#ifndef _MY_STRING_H
#define _MY_STRING_H

#include <stdbool.h>
#include <stddef.h>

typedef unsigned int uint;

bool starts_with(const char* str, char c);
size_t _strlen(const char* str);
char *_strchr(const char *s, int c);
int _strcmp (const char* str1, const char* str2);
int _strncmp (const char* str1, const char* str2, int n);
char *_strcpy(char* dest, const char* source);
char *_strncpy(char* dest, const char* source, size_t num);
char *_strcat(char* dest, const char* source);
char *_strsep(char **stringp, const char *delim);
#endif
