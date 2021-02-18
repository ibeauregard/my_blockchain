#include "parse.h"

int main()
{	
	int parse;
	while ((parse = parse_arg()) != 0) {
		if (parse == -1) {
			// error message
		} else {
			// do something
		}
	}
    return 0;
}
