#include <stdio.h>                 // For dprintf
#include <fcntl.h>                 // For open
#include <sys/stat.h>              // For fchmod

#include "blockchain/blockchain_public.h"

static int save_block(int fildes, Block *block) 
{
	return dprintf(fildes, "%d", block->id);
}

static int save_node(int fildes, Node *node)
{
	int print_count = 0;
	print_count += dprintf(fildes, "%d:%d:", node->id, node->sync_tail->id);
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
