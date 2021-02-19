/* command.c contains logic for reading input, parsing it, and returning a
 * struct Command. The most important function is get_cmd(), which parses
 * the first token / word and then passes off the remaining parsing to one of
 * the set_[X]_cmd() functions.
 *
 * A couple of "design" decisions: 
 *
 * 0) The struct Command contains a field maincmp that differentiates ADD_NODE
 * and ADD_BLOCK. While one could consider them two subsets of the same cmd,
 * in reality their logic is entirely different. So for all intents and 
 * purposes they are completely different commands.
 *
 * 1) new_cmd() uses a static instance of struct Command instead of malloc.
 * The reason for doing this is to minimize free() calls in the parent fxn.
 * Additionally, because only one instance of struct Command is ever required
 * at any given time, this decision seemed appropriate.
 *
 * 2) get_cmd() creates and returns a new struct Command instead of accepting
 * one as a parameter. While either could have worked, it seemed appropriate
 * that a function named "get_[X]" would "get" and "return" something.
 *
 * 3) get_cmd() parses just the first token of each command and then passes
 * the remaining tokens to a set_X_cmd() function. This way, for e.g., only the 
 * set_add_cmd() function needs to parse the second token, expecting either
 * a "block" or "node" token (and the other set_X_cmd() functions don't
 * have to worry about it). 
 * 
 * 4) While code here could call functions like add_node() or add_block()
 * directly, I believe that confuses how one thinks about the abstraction.
 */

#include <stdio.h>                 // For printf
#include <stdlib.h>                // For free
#include <unistd.h>                // For STDIN

#include "command.h"
#include "utils/_readline.h"
#include "utils/_string.h"         // For _strcmp, _strsep

/* new_cmd(): Uses a static instance of struct Command instead of malloc.
 * The reason for doing this is to minimize free() calls in the parent fxn.
 * Additionally, because only one instance of struct Command is ever required
 * at any given time, this decision seemed appropriate.
 */
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

/* reset_cmd: Because we use a static instance of struct Command, we need 
 * to "reset" the structure each time it is called.
 */
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

/* get_cmd: This function parses the first token and then passes off
 * remaining parsing to one of the set_[X]_cmd functions. 
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
