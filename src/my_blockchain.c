#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>                 // For STDIN

#include "commands.h"

int my_blockchain()
{
	load_blockchain();
	Command *command;
	while ((command = get_cmd(STDIN_FILENO))) {
		switch (command->maincmd) {
		case UNDEFINED:
			cmd_not_found();
			break;
		case ADD_NODE:
			cmd_add_node(command);
			break;
		case ADD_BLOCK:
			cmd_add_block(command);
			break;
		case RM_NODE:
			cmd_rm_node(command);
			break;
		case RM_BLOCK:
			cmd_rm_block(command);
			break;
		case LS:
			cmd_ls(command);
			break;
		case SYNC:
			cmd_sync();
			break;
		case QUIT:
			cmd_quit();
			goto quit;
		}
	}
	quit:
	free_cmd(command);
	return EXIT_SUCCESS;
}

int main()
{	
	return my_blockchain();
}
