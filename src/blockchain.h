#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "node.h"
#include <stdbool.h>

typedef struct s_blockchain {
    Node *head;
    size_t length;
} Blockchain;

Node *get_node_from_id(unsigned int nid);
void add_node(Node *node);
void rmv_node(Node *node);
bool blockchain_is_synced();
void sync_blockchain();

#endif
