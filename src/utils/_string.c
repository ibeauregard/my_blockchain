#include "_string.h"

bool starts_with(const char* str, char c)
{
    return str && str[0] == c;
}

size_t _strlen(const char* str)
{
    size_t len;
    for (len = 0; str[len]; len++);
    return len;
}

int _strcmp (const char* str1, const char* str2)
{
    for (; *str1 && *str2 && *str1 == *str2; str1++, str2++);
    return *str1 - *str2;
}

int _strncmp (const char* str1, const char* str2, int n)
{
    int i = 0;
    for (; i < n && *str1 && *str2 && *str1 == *str2; str1++, str2++, i++);
    if (i >= n) {
        str1--;
        str2--;
    }
    return *str1 - *str2;
}

char* _strcpy(char* dest, const char* source)
{
    size_t i;
    for (i = 0; source[i]; i++)
    {
        dest[i] = source[i];
    }
    dest[i] = 0;
    return dest;
}

char* _strncpy(char* dest, const char* source, size_t num)
{
    size_t i;
    for (i = 0; source[i] && i < num; i++)
    {
        dest[i] = source[i];
    }
    for (; i < num; i++)
    {
        dest[i] = 0;
    }
    return dest;
}

char* _strcat(char* dest, const char* source)
{
    uint i, j;
    for (i = 0; dest[i]; i++);
    for (j = 0; source[j]; j++)
    {
        dest[i + j] = source[j];
    }
    dest[i + j] = 0;
    return dest;
}

// char *_strtok(char *str, const char *delim)
// {
// 	static char *lastStr;
// 	if (str != NULL)
// 		lastStr = str;
//
// 	int letterCount = 0;
// 	char *loopStr = lastStr;
// 	while (*loopStr++ != *delim) {
// 		letterCount++;
// 	}
// 	char *token = malloc(sizeof(char) * letterCount);
//
// 	while (*lastStr != *delim) {
// 		*token++ = *lastStr++;
// 	}
//
// 	while (*lastStr == delim) {
// 		lastStr++;
// 	}
//
// 	return token;
// }
