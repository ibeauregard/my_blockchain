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
void destruct_node(Node *node);

#endif
