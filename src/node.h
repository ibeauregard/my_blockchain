#ifndef NODE_H
#define NODE_H

#include "block.h"

typedef struct s_node {
    unsigned int id;
    Block *head;
    Block *tail;
    struct s_node *next;
} Node;

Node *new_node(unsigned int nid);
Node *get_node_from_id(unsigned int nid);
Block *get_block_from_id(unsigned int bid, Node *node);
void add_block(Block *block, Node *node);
void remove_block(Block *block, Node *node);
void destruct_node(Node *node);

#endif
