#ifndef NODE_PRIVATE_H
#define NODE_PRIVATE_H

#include "type.h"
#include <stdbool.h>

bool node_is_synced(const Node *node);
bool node_is_empty(const Node *node);
void free_node(Node *node);
void free_node_chain(Node *node);

#endif
