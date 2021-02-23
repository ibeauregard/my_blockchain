#include <stdlib.h>                          // For EXIT_[X]

#include "commands.h"
#include "blockchain/blockchain_public.h"

int cmd_add_node(Command *command)
{
	unsigned int nid = *(command->nidlist);
	if (has_node_with_id(nid)) return EXIT_FAILURE;
	add_node(get_node_from_id(nid));
	return EXIT_SUCCESS;
}

int cmd_add_block(Command *command)
{
	unsigned int bid = *(command->bidlist);
	int *nidlist = command->nidlist;
	int nidcount = command->nidcount;
	for (int i = 0; i < nidcount; i++) {
		unsigned int nid = (int) *(nidlist + i);
		if (!has_node_with_id(nid)) continue;
		Node *node = get_node_from_id(nid);
		if (has_block_with_id(bid, node)) continue;
		Block *block = new_block(bid);
		add_block(block, node);
	}
	return EXIT_SUCCESS;
}

int cmd_quit()
{
	return EXIT_SUCCESS;
}
