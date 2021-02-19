#include <stdio.h>                 // For printf
#include <stdlib.h>                // For free
#include <unistd.h>                // For STDIN

#include "command.h"
#include "utils/_readline.h"
#include "utils/_string.h"         // For _strcmp

/* _strsep: Our implemenation of stdlib strsep() from string.h library
 * -------------------------------------------------------------------
 * Function manipulates stringp so that no allocation of memory is required.
 * Returns pointer to the next token (ie. adjacent chars leading up to 
 * delim or '\0'). 
 */
char *_strsep(char **stringp, const char *delim)
{
	if (!**stringp)
		return NULL;
	char *og_stringp = *stringp;
	while (**stringp != *delim && **stringp != '\0') {
		(*stringp)++;
	}
	if (**stringp == '\0') {
		return og_stringp;
	} 
	**stringp = '\0';
	(*stringp)++;
	while (**stringp == *delim) {
		(*stringp)++;
	}
	return og_stringp;
}

Command *new_cmd()
{
	static Command command = {
		.maincmd = UNDEFINED,
		.lflag = 0,
		.nidlist = NULL,
		.bidlist = NULL,
	};
	return &command;
}

void reset_cmd(Command *command) 
{
	command->maincmd = UNDEFINED;
	command->lflag = 0;
	command->nidlist = NULL;
	command->bidlist = NULL;
}

void set_add_cmd(Command *command, char *line)
{
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = ADD_NODE;
		// set_nid_list(command, line);
	} else if (!_strcmp("block", token)) {
		command->maincmd = ADD_BLOCK;
		// set_bid_list(command, line);
	} 
	return;
}

void set_rm_cmd(Command *command, char *line)
{
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = RM_NODE;
		// set_nid_list(command, line);
	} else if (!_strcmp("block", token)) {
		command->maincmd = RM_BLOCK;
		// set_bid_list(command, line);
	} 
	return;
}

void set_ls_cmd(Command *command, char *line)
{
	(void) line;
	command->maincmd = LS;
}

void set_sync_cmd(Command *command)
{
	command->maincmd = SYNC;
}

void set_quit_cmd(Command *command)
{
	command->maincmd = QUIT;
}

/* 
 */
Command *get_cmd()
{
	Command *command = new_cmd();
	reset_cmd(command);
	char *line = _readline(STDIN_FILENO);
	char *line_to_free = line;
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!_strcmp("add", token)) {
		set_add_cmd(command, line);
	} else if (!_strcmp("rm", token)) {
		set_rm_cmd(command, line);
	} else if (!_strcmp("ls", token)) {
		set_ls_cmd(command, line);
	} else if (!_strcmp("sync", token)) {
		set_sync_cmd(command);
	} else if (!_strcmp("quit", token)) {
		set_quit_cmd(command);
	} 	
	free(line_to_free);
	return command;
}
