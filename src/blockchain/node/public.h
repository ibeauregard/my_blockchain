#ifndef NODE_H
#define NODE_H

#include "type.h"

Node *new_node(unsigned int nid);
Block *get_block_from_id(unsigned int bid, Node *node);
void add_block(Block *block, Node *node);
void rmv_block(Block *block, Node *node);

#endif
