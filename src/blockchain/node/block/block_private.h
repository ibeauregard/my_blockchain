#ifndef BLOCK_H
#define BLOCK_H

#include "block_public.h"

Block *clone_chain(const Block *head);
void free_block(Block *block);
void free_chain(Block *head);

#endif
