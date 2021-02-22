#include <stdio.h>
#include <stdlib.h>                // For free

#include "command.h"

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
			printf("adding node....\n");
			break;
		case ADD_BLOCK:
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
			printf("quitting....\n");
			goto quit;
		}
	}
	quit:
	free_cmd(command);
	return 0;
}
