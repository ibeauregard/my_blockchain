#ifndef NODE_H
#define NODE_H

#include "node_public.h"
#include <stdbool.h>

Node create_node(unsigned int nid);
Block *get_post_sync_chain(const Node *node);
void add_chain(Block *head, Node *node);
bool node_is_synced(const Node *node);
void declare_node_synced(Node *node);
bool node_is_empty(const Node *node);
void free_node(Node *node);
void free_node_chain(Node *node);

#endif
