#include <stdio.h>
#include "../src/blockchain/blockchain_public.h"

static void test_blockchain_sample();
static void print_node(const Node *node);
static void print_blockchain();

void test_blockchain() {
	test_blockchain_sample();
}

void test_blockchain_sample()
{
    printf("%s\n", "Printing empty blockchain; should be empty");
    print_blockchain();

    printf("%s\n", "Sync empty blockchain");
    synchronize();
    print_blockchain();

    printf("%s\n", "Adding one node");
    Node *node = new_node(1);
    add_node(node);
    print_blockchain();

    printf("%s\n", "Sync blockchain with one empty node");
    synchronize();
    print_blockchain();

    printf("%s\n", "Adding one block to that node");
    Block *block = new_block(1);
    add_block(block, node);
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Sync blockchain twice with one node containing one block");
    synchronize();
    print_blockchain();

    printf("%s\n", "Adding several nodes and several blocks");
    block = new_block(2);
    add_block(block, node);
    update_sync_state();
    node = new_node(2);
    add_node(node);
    block = new_block(3);
    add_block(block, node);
    block = new_block(4);
    add_block(block, node);
    block = new_block(5);
    add_block(block, node);
    block = new_block(6);
    add_block(block, node);
    block = new_block(7);
    add_block(block, node);
    block = new_block(8);
    add_block(block, node);
    update_sync_state();
    add_node(new_node(3));
    print_blockchain();

    printf("%s\n", "Remove in middle");
    node = get_node_from_id(2);
    rmv_block(get_block_from_id(5, node), node);
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Remove head");
    rmv_block(get_block_from_id(3, node), node);
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Remove tail");
    rmv_block(get_block_from_id(8, node), node);
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Sync blockchain");
    synchronize();
    print_blockchain();

    printf("%s\n", "Add one block to first node");
    block = new_block(8);
    add_block(block, get_node_from_id(1));
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Sync blockchain");
    synchronize();
    print_blockchain();

    printf("%s\n", "Remove middle node");
    rmv_node(get_node_from_id(2));
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Remove head node");
    rmv_node(get_node_from_id(1));
    update_sync_state();
    print_blockchain();

    printf("%s\n", "Remove head and tail node (only node)");
    rmv_node(get_node_from_id(3));
    update_sync_state();
    print_blockchain();

    free_blockchain();
}

void print_blockchain()
{
    Node *node = get_nodes();
    while (node) {
        printf("Node # %u: ", node->id);
        print_node(node);
        puts("");
        node = node->next;
    }
    puts("");
}

void print_node(const Node *node)
{
    Block *block = node->head;
    while (block) {
        printf("Block # %u, ", block->id);
        block = block->next;
    }
}
