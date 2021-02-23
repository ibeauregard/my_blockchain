#include <stdio.h>

#include "command.h"
#include "commands.h"

int main()
{	
	Command *command;
	while ((command = get_cmd())) {
		switch (command->maincmd) {
		case UNDEFINED:
			printf("bad command....\n");
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
	return 0;
}
