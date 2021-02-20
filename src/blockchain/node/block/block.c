#include "public.h"
#include "private.h"
#include <stdlib.h>

static Block create_block(unsigned int bid);
static Block *clone_block(const Block *block, Block *prev);

Block *new_block(unsigned int bid)
{
    Block *block = malloc(sizeof (Block));
    block->id = bid;
    block->prev = NULL;
    block->next = NULL;
    return block;
}

Block *clone_chain(const Block *head)
{
    Block clone_dummy_head = create_block(0);
    Block *clone = &clone_dummy_head;
    while (head) {
        clone = clone_block(head, clone);
        head = head->next;
    }
    clone = clone_dummy_head.next;
    clone->prev = NULL;
    return clone;
}

Block create_block(unsigned int bid)
{
    Block block = {
            .id = bid,
            .prev = NULL,
            .next = NULL
    };
    return block;
}

Block *clone_block(const Block *block, Block *prev)
{
    Block *clone = new_block(block->id);
    prev->next = clone;
    clone->prev = prev;
    return clone;
}

void free_block(Block *block)
{
    free(block);
}

void free_chain(Block *head)
{
    while (head) {
        Block *current = head;
        head = current->next;
        free_block(current);
    }
}
