#include <stdio.h>                           // For printf
#include <stdlib.h>                          // For EXIT_[X]

#include "save.h"
#include "command.h"
#include "blockchain/blockchain_public.h"

#define SAVE_PATHNAME "my_blockchain.save"

int cmd_add_node(Command *command)
{
	unsigned int nid = *(command->nidlist);
	if (has_node_with_id(nid)) return EXIT_FAILURE;
	add_node(new_node(nid));
	return EXIT_SUCCESS;
}

int cmd_add_block(Command *command)
{
	unsigned int bid = *(command->bidlist);
	int *nidlist = command->nidlist;
	int nidcount = command->nidcount;
	for (int i = 0; i < nidcount; i++) {
		unsigned int nid = *(nidlist + i);
		if (!has_node_with_id(nid)) continue;
		Node *node = get_node_from_id(nid);
		if (has_block_with_id(bid, node)) continue;
		Block *block = new_block(bid);
		add_block(block, node);
	}
	return EXIT_SUCCESS;
}

int cmd_rm_node(Command *command)
{
	unsigned int nid = *(command->nidlist);
	if (!has_node_with_id(nid)) return EXIT_FAILURE;
	rmv_node(get_node_from_id(nid));
	return EXIT_SUCCESS;
}

int cmd_rm_block(Command *command)
{
	int *bidlist = command->bidlist;
	int bidcount = command->bidcount;
	for (int i = 0; i < bidcount; i++) {
		unsigned int bid = *(bidlist + i);
		Node *node = get_nodes();
		while (node) {
			if (!has_block_with_id(bid, node)) continue;
			rmv_block(get_block_from_id(bid, node), node);
			node = node->next;
		}
	}
	return EXIT_SUCCESS;
}

void cmd_ls(Command *command)
{
	Node *node = get_nodes();
	while (node) {
		printf("%d: ", node->id);
		Block *block = node->head;
		while (command->lflag && block) {
			printf("%d, ", block->id);
			block = block->next;
		}
		printf("\n");
		node = node->next;
	}
}

int cmd_sync() 
{
	return synchronize();
}

int cmd_quit()
{
	save(SAVE_PATHNAME, get_nodes());
	return EXIT_SUCCESS;
}
