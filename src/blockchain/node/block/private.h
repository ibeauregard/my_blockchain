#ifndef BLOCK_PRIVATE_H
#define BLOCK_PRIVATE_H

#include "type.h"

Block *clone_chain(const Block *head);
void free_block(Block *block);
void free_chain(Block *head);

#endif
