#ifndef BLOCK_PUBLIC_H
#define BLOCK_PUBLIC_H

typedef struct s_block {
    unsigned int id;
    struct s_block *prev;
    struct s_block *next;
} Block;

Block *new_block(unsigned int bid);

#endif
