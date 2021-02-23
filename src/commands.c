/* commands.c contains all the high-level logic for the my_blockchain program.
 * The code divides roughly into two parts: The first contains functions 
 * new_cmd, free_cmd, and get_cmd which are primariy used for instantiating
 * a struct Command using user input, and the second contains functions for 
 * each of the different blockchain commands (e.g. add_node, add_block, etc.)
 *
 * commands.c pulls logic primarily from three main locations. The first is
 * "blockchain_public.h" which contains the logic for manipulating the main
 * struct Blockchain (and its sub-structures like struct Node and struct
 * Block). The second is parse.h which contains the logic for parsing input.
 * And the third is save.h which contains the logic for serializing and 
 * deserializing the data for saving and loading.
 *
 * A few "design" decisions: 
 *
 * - The struct Command contains a field maincmp that differentiates ADD_NODE
 *   from ADD_BLOCK and RM_NODE from RM_BLOCK. We could consider them subsets
 *   of the same cmd, but their logic is entirely different. So for all intents
 *   and purposes they are separate instructions.
 *
 * - Additionally, we opted to include 4 members bidlist, bidcount, nidlist,
 *   and nidcount in Command rather than create another struct to hold them.
 *
 * - new_cmd() uses a static instance of struct Command instead of malloc.
 *   The reason for doing this is to minimize free() calls in the parent fxn.
 *   Additionally, because only one instance of struct Command is ever required
 *   at any given time, this decision seemed appropriate. Note that free() 
 *   cannot be avoided entirely, as it is still required for [x]idlist members.
 */

#include <stdio.h>                           // For printf
#include <stdlib.h>                          // For EXIT_[X]
#include <unistd.h>                          // For STDIN

#include "blockchain/blockchain_public.h"
#include "commands.h"
#include "save.h"
#include "parse.h"
#include "utils/_readline.h"

#define SAVE_PATHNAME "my_blockchain.save"
#define SAVE_PATHNAME_TEST "my_blockchain_test.save"

/* print_cmd: Used for debugging - prints struct Command.
 */
void print_cmd(Command *command)
{
	printf("maincmd: %d\n", command->maincmd);
	printf("lflag: %d\n", command->lflag);
	printf("all: %d\n", command->all);
	// Print nidlist
	printf("nidcount: %ld\n", command->nidcount);
	printf("nid: ");
	for (size_t i = 0; i < command->nidcount; i++) {
		printf("%d, ", *(command->nidlist+i));
	}
	printf("\n");
	// Print bidlist
	printf("bidcount: %ld\n", command->bidcount);
	printf("bid: ");
	for (size_t i = 0; i < command->bidcount; i++) {
		printf("%d, ", *(command->bidlist+i));
	}
	printf("\n");
}

/* new_cmd(): Uses a static instance of struct Command instead of malloc.
 * The reason for doing this is to minimize free() calls in the parent fxn.
 * Additionally, because only one instance of struct Command is ever required
 * at any given time, this decision seemed appropriate.
 *
 * With that being said, still need to malloc memory for .xidlist member.
 * Therefore, before each new call to new_cmd(), we need to free this memory.
 */
static Command *new_cmd()
{
	static Command command;
	free_cmd(&command);
	Command resetcommand = {
		.maincmd = UNDEFINED,
		.lflag = false,
		.all = false,
		.nidlist = NULL,
		.nidcount = 0,
		.bidlist = NULL,
		.bidcount = 0,
	};
	command = resetcommand;
	return &command;
}

/* free_cmd: Needed for freeing just the memory allocated in parse_id_list().
 * While struct Command is a static instance, the .[x]idlist needs to point to 
 * malloc'd memory. Thus this memory needs to be freed at the very end of the
 * program AND right before each new call of new_cmd(). If not, new_cmd() will
 * reset .[x]idlist = NULL and memory will leak.
 */
void free_cmd(Command *command)
{
	if (command->nidlist) {
		free(command->nidlist);
		command->nidlist = NULL;
	}
	if (command->bidlist) {
		free(command->bidlist);
		command->bidlist = NULL;
	}
}

Command *get_cmd()
{
	Command *command = new_cmd();
	char *line = _readline(STDIN_FILENO);
	parse_cmd(command, line);
	free(line);
	return command;
}

int load_cmd()
{
	return load(SAVE_PATHNAME_TEST);
}

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
	unsigned int *nidlist = command->nidlist;
	size_t nidcount = command->nidcount;
	for (size_t i = 0; i < nidcount; i++) {
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
	unsigned int *bidlist = command->bidlist;
	size_t bidcount = command->bidcount;
	for (size_t i = 0; i < bidcount; i++) {
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
