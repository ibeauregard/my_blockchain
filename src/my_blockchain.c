#include <stdio.h>

#include "command.h"
#include "commands.h"

int main()
{	
	Command *command;
	while ((command = get_cmd())) {
		print_cmd(command);
		switch (command->maincmd) {
		case UNDEFINED:
			printf("bad command....\n");
			break;
		case ADD_NODE:
			cmd_add_node(command);
			printf("adding node....\n");
			break;
		case ADD_BLOCK:
			cmd_add_block(command);
			printf("adding block....\n");
			break;
		case RM_NODE:
			printf("removing node....\n");
			break;
		case RM_BLOCK:
			printf("removing block....\n");
			break;
		case LS:
			printf("listing contents....\n");
			break;
		case SYNC:
			printf("syncing....\n");
			break;
		case QUIT:
			cmd_quit();
			printf("quitting....\n");
			goto quit;
		}
	}
	quit:
	free_cmd(command);
	return 0;
}
