#ifndef BLOCK_TYPE_H
#define BLOCK_TYPE_H

typedef struct s_block {
    unsigned int id;
    struct s_block *prev;
    struct s_block *next;
} Block;

#endif
