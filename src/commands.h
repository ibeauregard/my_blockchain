#ifndef COMMANDS_H
#define COMMANDS_H

#include "utils/uint_array.h"
#include "command.h"
#include <stdbool.h>

int cmd_add_node(Command *command);
int cmd_add_block(Command *command);
int cmd_rm_node(Command *command);
int cmd_rm_block(Command *command);
void cmd_ls(Command *command);
int cmd_sync();
int cmd_quit();

#endif
