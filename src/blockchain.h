#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "node.h"

typedef struct s_blockchain {
    Node *head;
    size_t length;
} Blockchain;

void add_node(Node *node);
Node *get_node_from_id(unsigned int nid);
void rmv_node(Node *node);

#endif
