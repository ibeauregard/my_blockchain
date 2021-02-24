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
 * deserializing the data for saving and loading. commands.c also pulls
 * some logic from error.c but it is small in comparison.
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

#include "commands.h"
#include "blockchain/blockchain_public.h"
#include "save.h"
#include "parse.h"
#include "error.h"
#include "utils/_string.h"
#include "utils/_readline.h"

#define SAVE_PATHNAME "my_blockchain.save"
#define MAX_PROMPT_SIZE 64

/* print_cmd: Used for debugging - prints struct Command.
 */
void print_cmd(Command *command)
{
	printf("maincmd: %d\n", command->maincmd);
	printf("lflag: %d\n", command->lflag);
	printf("all: %d\n", command->all);
	printf("nidcount: %ld\n", command->nidcount);
	printf("nid: ");
	for (size_t i = 0; i < command->nidcount; i++) {
		printf("%d, ", *(command->nidlist+i));
	}
	printf("\n");
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

/* print_prompt: prints either [sX] or [-X] where X is the number of 
 * nodes in chain, 's' indicates nodes are synced, and '-' indictes 
 * nodes are not synced.
 */
void print_prompt() 
{
	char sync_state = blockchain_is_synced() ? 's' : '-';
	size_t n_nodes = get_num_nodes();
	char buffer[MAX_PROMPT_SIZE];
	// If try and use printf, may not always print before the
	// _readline command due to buffering / compiler optimization.
	sprintf(buffer, "[%c%ld]> ", sync_state, n_nodes);
	write(STDIN_FILENO, buffer, _strlen(buffer));
}

/* get_cmd: Really just a wrapper function that combines printing the
 * prompt, reading from STDIN, then parsing the string to get Command.
 */
Command *get_cmd()
{
	print_prompt();
	Command *command = new_cmd();
	char *line = _readline(STDIN_FILENO);
	parse_cmd(command, line);
	free(line);
	return command;
}

int load_blockchain()
{
	return load(SAVE_PATHNAME);
}

int cmd_add_node(Command *command)
{
	unsigned int nid = *(command->nidlist);
	if (has_node_with_id(nid)) {
		print_error(ERROR_ID_NODE_EXISTS);
		return EXIT_FAILURE;
	}
	Node *node = new_node(nid);
	if (!node) {
		print_error(ERROR_ID_NO_RESOURCES);
		return EXIT_FAILURE;
	}
	add_node(node);
	return EXIT_SUCCESS;
}

int cmd_add_block(Command *command)
{
	unsigned int bid = *(command->bidlist);
	int blocks_added = 0;

	// If all nodes to be impacted
	if (command->all) {
		Node *node = get_nodes();
		while (node) {
			if (has_block_with_id(bid, node)) {
				node = node->next;
				continue;
			}
			Block *block = new_block(bid);
			if (!block) {
				print_error(ERROR_ID_NO_RESOURCES);
				return EXIT_FAILURE;
			}
			add_block(block, node);
			blocks_added++;
			node = node->next;
		}
	}

	// If all flag not specified
	else {
		unsigned int *nidlist = command->nidlist;
		size_t nidcount = command->nidcount;
		for (size_t i = 0; i < nidcount; i++) {
			unsigned int nid = *(nidlist + i);
			if (!has_node_with_id(nid)) continue;
			Node *node = get_node_from_id(nid);
			if (has_block_with_id(bid, node)) continue;
			Block *block = new_block(bid);
			if (!block) {
				print_error(ERROR_ID_NO_RESOURCES);
				return EXIT_FAILURE;
			}
			add_block(block, node);
			blocks_added++;
		}
	}

	update_sync_state();
	// We only print error if no blocks were found throughout all nodes.
	// If one node has block but the rest don't, shouldn't show error.
	// Also if multiple blocks provided and some never appear, so long 
	// as one appears once, error will not be generated.
	if (!blocks_added) {
		print_error(ERROR_ID_BLOCK_EXISTS);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int cmd_rm_node(Command *command)
{
	int nodes_removed = 0;

	// If all nodes to be deleted
	if (command->all) {
		Node *node = get_nodes();
		while (node) {
			Node *next_node = node->next;
			rmv_node(node);
			nodes_removed++;
			node = next_node;
		}
		return EXIT_SUCCESS;
	}

	// If all flag not specified
	else {
		unsigned int *nidlist = command->nidlist;
		size_t nidcount = command->nidcount;
		for (size_t i = 0; i < nidcount; i++) {
			unsigned int nid = *(nidlist + i);
			if (!has_node_with_id(nid)) continue;
			rmv_node(get_node_from_id(nid));
			nodes_removed++;
		}
	}

	update_sync_state();
	if (!nodes_removed) {
		print_error(ERROR_ID_NODE_NOT_EXISTS);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int cmd_rm_block(Command *command)
{
	int blocks_removed = 0;

	unsigned int *bidlist = command->bidlist;
	size_t bidcount = command->bidcount;
	for (size_t i = 0; i < bidcount; i++) {
		unsigned int bid = *(bidlist + i);
		Node *node = get_nodes();
		while (node) {
			if (!has_block_with_id(bid, node)) {
				node = node->next;
				continue;
			};
			rmv_block(get_block_from_id(bid, node), node);
			blocks_removed++;
			node = node->next;
		}
	}
	update_sync_state();
	if (!blocks_removed) {
		print_error(ERROR_ID_BLOCK_NOT_EXISTS);
		return EXIT_FAILURE;
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
	int sync_result = synchronize();
	if (sync_result == EXIT_FAILURE) {
		print_error(ERROR_ID_NO_RESOURCES);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int cmd_quit()
{
	save(SAVE_PATHNAME, get_nodes());
	return EXIT_SUCCESS;
}

void cmd_not_found()
{
	print_error(ERROR_ID_CMD_NOT_FOUND);
}

