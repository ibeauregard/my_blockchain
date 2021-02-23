/* save.c: Contains logic for saving (or serializing) blockchain object
 * onto disk and loading (or de-serializing) that same information.
 *
 * A few "design" decisions: 
 *
 * - save() will serialize blockchain in the following format:
 *          [nid]:[bid],[bid],...
 *          [nid]:[bid],[bid],...
 *          ...
 *
 * - load() will fail on 3 conditions: 1) duplicate blocks, 2) duplicate
 *   nodes, 3) failure to open file. The first two conditions indicates
 *   that the file is corrupted while the latter indicates that no file 
 *   exists. Either way a new blockchain should be created.
 *
 */

#include <stdio.h>                 // For dprintf
#include <stdlib.h>                // For EXIT_[X], free
#include <fcntl.h>                 // For open
#include <unistd.h>                // For STDIN
#include <sys/stat.h>              // For fchmod

#include "blockchain/blockchain_public.h"
#include "utils/_string.h"         // For _strsep
#include "utils/_stdlib.h"         // For _strtol
#include "utils/_readline.h"

static int save_block(int fildes, Block *block) 
{
	return dprintf(fildes, "%d", block->id);
}

static int save_node(int fildes, Node *node)
{
	int print_count = 0;
	print_count += dprintf(fildes, "%d:", node->id);
	Block *current_block = node->head;
	while (current_block) {
		print_count += save_block(fildes, current_block);
		print_count += dprintf(fildes, ",");
		current_block = current_block->next;
	}
	return print_count;
}

static int save_blockchain(int fildes, Node *head_node) 
{
	int print_count = 0;
	Node *current_node = head_node;
	while (current_node) {
		print_count += save_node(fildes, current_node);
		print_count += dprintf(fildes, "\n");
		current_node = current_node->next;
	}
	return print_count;
}

int save(const char *filename, Node *head_node)
{
	// Give file 744 righs (rwxr--r--).
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
	                        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH);
	if (fd == -1) return fd;
	return save_blockchain(fd, head_node);
}

static Block *load_block(unsigned int bid, Node *node)
{
	if (has_block_with_id(bid, node)) return NULL;
	Block *block = new_block(bid);
	add_block(block, node);
	return block;
}

static Node *load_node(char *nidline)
{
	// First token: nid.
	char delim = ':';
	char *token = _strsep(&nidline, &delim);
	unsigned int nid = _strtol(token, NULL, 10);
	if (has_node_with_id(nid)) return NULL;
	Node *node = new_node(nid);

	// Remaining tokens: bids.
	delim = ',';
	while ((token = _strsep(&nidline, &delim)) != NULL) {
		unsigned int bid = _strtol(token, NULL, 10);
		Block *curr_block;
		if ((curr_block = load_block(bid, node)) == NULL) return NULL;
	}
	
	return node;
}

static int load_blockchain(int fildes)
{
	char *line;
	Node *node;
	while ((line = _readline(fildes)) != NULL) {
		if ((node = load_node(line)) == NULL) return EXIT_FAILURE;
		add_node(node);
		free(line);
		line = NULL;
	}
	update_sync_state();
	return EXIT_SUCCESS;
}

int load(char *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1) return EXIT_FAILURE;
	return load_blockchain(fd);
}
