#ifndef _PARSE_H
#define _PARSE_H

#include <stdbool.h>               // For bool

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

Command *get_cmd();
void free_cmd(Command *command);
void print_cmd(Command *command);

#endif // _PARSE_H
