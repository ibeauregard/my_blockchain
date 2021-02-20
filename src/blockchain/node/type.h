#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include "block/type.h"

typedef struct s_node {
    unsigned int id;
    Block *head;
    Block *sync_tail;
    Block *tail;
    struct s_node *prev;
    struct s_node *next;
} Node;

#endif
