#ifndef BLOCK_H
#define BLOCK_H

typedef struct s_block {
    unsigned int id;
    unsigned int timestamp;
    unsigned int checksum;
    struct s_block *next;
} Block;

Block *new_block(unsigned int bid);
void destruct_block(Block *block);

#endif
