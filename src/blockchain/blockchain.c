#include "blockchain_private.h"
#include "node/node_private.h"
#include "node/block/block_private.h"
#include <stdlib.h>

typedef struct s_blockchain {
    Node *head;
    Node *tail;
    size_t num_nodes;
} Blockchain;

static Blockchain blockchain;
static Node dummy_head;
static Node dummy_tail;

static bool is_empty();
static void add_first_node(Node *node);
static void attach_dummy_head_and_tail();
static void detach_dummy_head_and_tail();
static bool all_nodes_are_empty();
static void desync();
static int fill_dummy_sync_node();
static int put_node_content_in_dummy_sync_node(Node *node, Node *dummy_sync_node);
static int put_block_in_dummy_sync_node(Block *block, Node *dummy_sync_node);
static int sync_nodes(const Node *dummy_sync_node);

Node *get_nodes()
{
    return blockchain.head;
}

bool has_node_with_id(unsigned int nid)
{
    return get_node_from_id(nid) != NULL;
}

Node *get_node_from_id(unsigned int nid)
{
    Node *node = blockchain.head;
    while (node && node->id != nid) {
        node = node->next;
    }
    return node;
}

void add_node(Node *node)
{
    if (is_empty()) {
        add_first_node(node);
        return;
    }
    desync();
    node->prev = blockchain.tail;
    blockchain.tail = blockchain.tail->next = node;
    blockchain.num_nodes++;
}

bool is_empty()
{
    return blockchain.num_nodes == 0;
}

void add_first_node(Node *node)
{
    blockchain.head = blockchain.tail = node;
    blockchain.num_nodes = 1;
}

void rmv_node(Node *node)
{
    if (is_empty() || blockchain.num_nodes == 1) {
        free_node(node);
        blockchain.head = blockchain.tail = NULL;
        blockchain.num_nodes = 0;
        return;
    }
    attach_dummy_head_and_tail();
    node->prev->next = node->next;
    node->next->prev = node->prev;
    detach_dummy_head_and_tail();
    free_node(node);
    blockchain.num_nodes--;
}

void attach_dummy_head_and_tail()
{
    blockchain.head->prev = &dummy_head;
    dummy_head.next = blockchain.head;

    blockchain.tail->next = &dummy_tail;
    dummy_tail.prev = blockchain.tail;
}

void detach_dummy_head_and_tail()
{
    blockchain.head = dummy_head.next;
    blockchain.head->prev = NULL;

    blockchain.tail = dummy_tail.prev;
    blockchain.tail->next = NULL;
}

size_t get_num_nodes()
{
    return blockchain.num_nodes;
}

bool blockchain_is_synced()
{
    if (all_nodes_are_empty() || blockchain.num_nodes == 1) {
        return true;
    }
    Node *node = blockchain.head;
    while (node) {
        if (node_is_empty(node) || !node_is_synced(node)) {
            return false;
        }
        node = node->next;
    }
    return true;
}

bool all_nodes_are_empty()
{
    Node *node = blockchain.head;
    while (node) {
        if (!node_is_empty(node)) {
            return false;
        }
        node = node->next;
    }
    return true;
}

void desync()
{
    Node *node = blockchain.head;
    while (node) {
        node->sync_tail = NULL;
        node = node->next;
    }
}

int synchronize()
{
    Node dummy_sync_node = create_node(0);
    int status = fill_dummy_sync_node(&dummy_sync_node) || sync_nodes(&dummy_sync_node);
    free_chain(dummy_sync_node.head);
    return status;
}

int fill_dummy_sync_node(Node *dummy_sync_node)
{
    Node *node = blockchain.head;
    while (node) {
        if (put_node_content_in_dummy_sync_node(node, dummy_sync_node)) {
            return EXIT_FAILURE;
        }
        node = node->next;
    }
    return EXIT_SUCCESS;
}

int put_node_content_in_dummy_sync_node(Node *node, Node *dummy_sync_node)
{
    Block *post_sync_block = get_post_sync_chain(node);
    while (post_sync_block) {
        Block *current = post_sync_block;
        post_sync_block = current->next;
        if (put_block_in_dummy_sync_node(current, dummy_sync_node)) {
            return EXIT_FAILURE;
        }
        rmv_block(current, node);
    }
    return EXIT_SUCCESS;
}

int put_block_in_dummy_sync_node(Block *block, Node *dummy_sync_node)
{
    if (!has_block_with_id(block->id, dummy_sync_node)) {
        Block *clone = new_block(block->id);
        if (!clone) return EXIT_FAILURE;
        add_block(clone, dummy_sync_node);
    }
    return EXIT_SUCCESS;
}

int sync_nodes(const Node *dummy_sync_node)
{
    if (node_is_empty(dummy_sync_node)) return EXIT_SUCCESS;
    Node *node = blockchain.head;
    while (node) {
        Block* clone = clone_chain(dummy_sync_node->head);
        if (!clone) return EXIT_FAILURE;
        add_chain(clone, node);
        declare_node_synced(node);
        node = node->next;
    }
    return EXIT_SUCCESS;
}

void free_blockchain()
{
    free_node_chain(blockchain.head);
}
