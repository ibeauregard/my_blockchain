#ifndef NODE_LIST_H
#define NODE_LIST_H

#include "node.h"

typedef struct s_node_list {
    Node *head;
    size_t length;
} NodeList;

void add_node(Node *node);
void rmv_node(Node *node);

#endif
