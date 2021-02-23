#ifndef COMMANDS_H
#define COMMANDS_H

#include "utils/uint_array.h"
#include "command.h"
#include <stdbool.h>

int cmd_add_node(Command *command);
int cmd_add_block(Command *command);
int cmd_quit();
/*
void rmv_nodes(UintArray *nids);
void rmv_blocks(UintArray *bids);
void ls(bool show_bids);
void sync_blockchain();
*/

#endif
