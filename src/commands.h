#ifndef COMMANDS_H
#define COMMANDS_H

#include "utils/uint_array.h"
#include <stdbool.h>

typedef enum e_cmd { UNDEFINED, ADD_NODE, ADD_BLOCK, RM_NODE, 
             RM_BLOCK, LS, SYNC, QUIT } MainCmd;

typedef struct s_command {
	MainCmd maincmd;
	bool lflag;
	bool all;
	int *nidlist;
	int nidcount;
	int *bidlist;
	int bidcount;
} Command;

void free_cmd(Command *command);
void print_cmd(Command *command);
Command *get_cmd();

int cmd_add_node(Command *command);
int cmd_add_block(Command *command);
int cmd_rm_node(Command *command);
int cmd_rm_block(Command *command);
void cmd_ls(Command *command);
int cmd_sync();
int cmd_quit();

#endif
