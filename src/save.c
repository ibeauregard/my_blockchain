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
	// We have this conditional to avoid the error type error with unsigned
	// int (node->sync_tail->id) and signed int (-1). Ideally we have a
	// signed type that could guarantee that it would hold unsigned int.
	if (node->sync_tail) {
		print_count += dprintf(fildes, "%d:%d:", node->id
		                                       , node->sync_tail->id);
	} else {
		print_count += dprintf(fildes, "%d:%d:", node->id, -1);
	}
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

/* save: Will serialize blockchain in the following format:
 *            [nid]:[sync_tail]:[bid],[bid],...
 *            [nid]:[sync_tail]:[bid],[bid],...
 *            ...
 * Returns the number of bytes printed, otherwise -1 if error.
 */
int save(const char *filename, Node *head_node)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC );
	if (fd == -1) return fd;
	// XXX: May want to remove this before production. Not supposed to 
	// use fchmod. Give file 764 righs (rwxrw-r--).
	fchmod(fd, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWOTH | S_IROTH);
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

	// Second token: sync_tail
	token = _strsep(&nidline, &delim);
	long int sync_tail_as_uint = _strtol(token, NULL, 10);

	// Remaining tokens: bids. Also set sync_tail to block if required.
	delim = ',';
	while ((token = _strsep(&nidline, &delim)) != NULL) {
		unsigned int bid = _strtol(token, NULL, 10);
		Block *curr_block;
		if ((curr_block = load_block(bid, node)) == NULL) return NULL;
		if (bid == sync_tail_as_uint) node->sync_tail = curr_block;
	}
	
	// Verify that sync_tail was found in list of blocks
	if (!node->sync_tail && (sync_tail_as_uint != -1)) return NULL;

	return node;
}

// load will fail if duplicate blocks or duplicate nodes or bad sync_tail
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
	return EXIT_SUCCESS;
}

int load(char *filename)
{
	int fd = open(filename, O_RDONLY);
	// Node *head_node = get_nodes();
	if (fd == -1) return EXIT_FAILURE;
	return load_blockchain(fd);
}
