#include "stddef.h"
#include "_stdlib.h"

int _isdigit(int d) 
{
	if (d < '0' || d > '9')
		return 0;
	return 1;
}

static int getValueOfDigit(char d, int base) 
{
	if (base > 35 || base < 1)
		return -1;
	else {
		if (_isdigit((int) d))
			return d - '0';
		if ((d >= 'a' && d < 'a' + base - 10))
			return 10 + d - 'a';
		if ((d >= 'A' && d < 'A' + base - 10))
			return 10 + d - 'A';
		return -1;
	}
		
}

long _strtol(const char *restrict str, char **restrict endptr, int base)
{
	if (base > 35)
		return 0;
	int result = 0;
	int sign = 1;
	if (*str == '-') {
		sign = -1;
		str++;
	}
	for (int i = 0; *(str + i); i++) {
		char d = *(str + i);
		int n;
		if ((n = getValueOfDigit(d, base)) == -1) {
			if (endptr != NULL)
				*endptr = (char *) str + i;
			return result * sign;
		}
		result *= base;
		result += n;
	}
	return result * sign;
}

