#ifndef BLOCK_H
#define BLOCK_H

typedef struct s_block {
    unsigned int id;
    struct s_block *prev;
    struct s_block *next;
} Block;

Block *new_block(unsigned int bid);
Block *clone_chain(Block *head);
void free_block(Block *block);

#endif
