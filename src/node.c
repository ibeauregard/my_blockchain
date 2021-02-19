#include "node.h"
#include <stdlib.h>

static void add_first_block(Block *block, Node *node);
static void attach_dummy_head_and_tail(Node *node);
static void detach_dummy_head_and_tail(Node *node);

Node *new_node(unsigned int nid)
{
    Node *node = malloc(sizeof (Node));
    node->id = nid;
    node->head = node->sync_tail = node->tail = NULL;
    node->prev = node->next = NULL;
    return node;
}

Block *get_block_from_id(unsigned int bid, Node *node)
{
    Block *block = node->head;
    while (block && block->id != bid) {
        block = block->next;
    }
    return block;
}

void add_block(Block *block, Node *node)
{
    if (node_is_empty(node)) {
        add_first_block(block, node);
        return;
    }
    block->prev = node->tail;
    node->tail->next = block;
    node->tail = block;
}

void add_first_block(Block *block, Node *node)
{
    node->head = node->tail = block;
}

void rmv_block(Block *block, Node *node)
{
    if (node_is_empty(node)) {
        return;
    }
    attach_dummy_head_and_tail(node);
    block->prev->next = block->next;
    block->next->prev = block->prev;
    if (node->sync_tail == block) {
        node->sync_tail = block->prev;
    }
    detach_dummy_head_and_tail(node);
    free_block(block);
}

void attach_dummy_head_and_tail(Node *node)
{
    Block *dummy_head = new_block(0);
    node->head->prev = dummy_head;
    dummy_head->next = node->head;

    Block *dummy_tail = new_block(0);
    node->tail->next = dummy_tail;
    dummy_tail->prev = node->tail;
}

void detach_dummy_head_and_tail(Node *node)
{
    Block *dummy_head = node->head;
    node->head = dummy_head->next;
    node->head->prev = NULL;
    free_block(dummy_head);

    Block  *dummy_tail = node->tail;
    node->tail = dummy_tail->prev;
    node->tail->next = NULL;
    free_block(dummy_tail);
}

bool node_is_synced(const Node *node)
{
    return node->sync_tail == node->tail;
}

bool node_is_empty(const Node *node)
{
    return !node->head;
}

void free_node(Node *node)
{
    free_chain(node->head);
    free(node);
}

void free_node_chain(Node *head)
{
    while (head) {
        Node *current = head;
        head = current->next;
        free_node(current);
    }
}
