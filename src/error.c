#include <stdio.h>                           // For printf
#include <unistd.h>                          // For STDIN

#include "error.h"

/* print_error: Prints error message to STDERR. Most are self-explanatory
 * except perhaps ERROR_ID_NO_RESOURCES. Error occurs when add_block, add_node,
 * or synchronise return NULL, indicating that space was not available to
 * be allocated.
 */
void print_error(short error_id)
{
	char *error_msg;
	switch (error_id) {
	case ERROR_ID_NO_RESOURCES:
		error_msg = "no more resources available on the computer";
		break;
	case ERROR_ID_NODE_EXISTS:
		error_msg = "this node already exists";
		break;
	case ERROR_ID_BLOCK_EXISTS:
		error_msg = "this block already exists";
		break;
	case ERROR_ID_NODE_NOT_EXISTS:
		error_msg = "node doesn't exist";
		break;
	case ERROR_ID_BLOCK_NOT_EXISTS:
		error_msg = "block doesn't exist";
		break;
	case ERROR_ID_CMD_NOT_FOUND:
		error_msg = "command not found";
		break;
	}
	dprintf(STDERR_FILENO, "%s\n", error_msg);
}
