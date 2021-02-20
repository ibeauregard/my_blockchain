#ifndef COMMANDS_H
#define COMMANDS_H

#include "utils/uint_array.h"
#include <stdbool.h>

void add_node(unsigned int nid);
void rmv_nodes(UintArray *nids);
void add_block(unsigned int bid, UintArray *nids);
void rmv_blocks(UintArray *bids);
void ls(bool show_bids);
void sync_blockchain();
void quit();

#endif
