#ifndef BLOCK_H
#define BLOCK_H

typedef struct s_block {
    unsigned int id;
    unsigned int timestamp;
    unsigned int checksum;
    struct s_block *next;
} Block;

#endif
