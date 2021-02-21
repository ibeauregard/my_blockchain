#ifndef NODE_PUBLIC_H
#define NODE_PUBLIC_H

#include "block/block_public.h"

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

#endif
