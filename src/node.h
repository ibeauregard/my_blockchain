#ifndef NODE_H
#define NODE_H

#include "block.h"
#include <stdbool.h>

typedef struct s_node {
    unsigned int id;
    Block *head;
    Block *sync_tail;
    Block *tail;
    struct s_node *prev;
    struct s_node *next;
} Node;

Node *new_node(unsigned int nid);
Block *get_block_from_id(unsigned int bid, Node *node);
void add_block(Block *block, Node *node);
void rmv_block(Block *block, Node *node);
bool node_is_synced(Node *node);
bool node_is_empty(Node *node);
void free_node(Node *node);

#endif
