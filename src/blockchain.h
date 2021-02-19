#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "node.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct s_blockchain {
    Node *head;
    Node *tail;
    size_t num_nodes;
} Blockchain;

void load_blockchain(Node *nodes);
Node *get_nodes();
Node *get_node_from_id(unsigned int nid);
void add_node(Node *node);
void rmv_node(Node *node);
size_t get_num_nodes();
bool blockchain_is_synced();
void sync_blockchain();

#endif
