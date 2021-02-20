#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "node.h"
#include <stdbool.h>
#include <stddef.h>

Node *get_nodes();
Node *get_node_from_id(unsigned int nid);
void add_node(Node *node);
void rmv_node(Node *node);
size_t get_num_nodes();
bool blockchain_is_synced();
void synchronize();
void free_blockchain();

#endif
