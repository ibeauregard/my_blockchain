#ifndef NODE_H
#define NODE_H

#include "node_public.h"
#include <stdbool.h>

void add_chain(Block *head, Node *node);
bool node_is_synced(const Node *node);
void declare_node_synced(Node *node);
bool node_is_empty(const Node *node);
void free_node(Node *node);
void free_node_chain(Node *node);

#endif
