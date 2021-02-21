#include "blockchain_private.h"
#include "node/node_private.h"
#include <stddef.h>

typedef struct s_blockchain {
    Node *head;
    Node *tail;
    size_t num_nodes;
} Blockchain;

static Blockchain blockchain = {
        .head = NULL,
        .tail = NULL,
        .num_nodes = 0
};

static bool is_empty();
static void add_first_node(Node *node);
static void attach_dummy_head_and_tail();
static void detach_dummy_head_and_tail();
static bool all_nodes_are_empty();

Node *get_nodes()
{
    return blockchain.head;
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
    if (is_empty() || blockchain.head == blockchain.tail) {
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
    Node *dummy_head = new_node(0);
    blockchain.head->prev = dummy_head;
    dummy_head->next = blockchain.head;

    Node *dummy_tail = new_node(0);
    blockchain.tail->next = dummy_tail;
    dummy_tail->prev = blockchain.tail;
}

void detach_dummy_head_and_tail()
{
    Node *dummy_head = blockchain.head->prev;
    blockchain.head = dummy_head->next;
    blockchain.head->prev = NULL;
    free_node(dummy_head);

    Node  *dummy_tail = blockchain.tail->next;
    blockchain.tail = dummy_tail->prev;
    blockchain.tail->next = NULL;
    free_node(dummy_tail);
}

size_t get_num_nodes()
{
    return blockchain.num_nodes;
}

bool blockchain_is_synced()
{
    if (all_nodes_are_empty()) {
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

void synchronize()
{

}

void free_blockchain()
{
    free_node_chain(blockchain.head);
}
