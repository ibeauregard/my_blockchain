#include <unistd.h>                // For STDIN

#include "utils/_readline.h"

int parse_arg()
{
	_readline(STDIN_FILENO);
	return 0;
}
