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
 * at any given time, this decision seemed appropriate. Note that free() 
 * cannot be avoided entirely, as it is still required for idlist members.
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
#include "utils/_stdlib.h"         // For _strtol

/* new_cmd(): Uses a static instance of struct Command instead of malloc.
 * The reason for doing this is to minimize free() calls in the parent fxn.
 * Additionally, because only one instance of struct Command is ever required
 * at any given time, this decision seemed appropriate.
 *
 * With that being said, still need to malloc memoryu for .idlist member.
 * Therefore, before each new call to new_cmd(), we need to free this memory.
 */
static Command *new_cmd()
{
	static Command command;
	free_cmd(&command);
	Command tmpcommand = {
		.maincmd = UNDEFINED,
		.lflag = 0,
		.all = 0,
		.idlist = NULL,
		.idcount = 0,
	};
	command = tmpcommand;
	return &command;
}

/* print_idlist: Used for debugging
 */
void print_idlist(Command *command)
{
	for (int i = 0; i < command->idcount; i++) {
		printf("%d\n", *(command->idlist+i));
	}
}

/* free_cmd: Needed for freeing just the memory that was to be allocated.
 * While struct Command is a static instance, the .idlist needs to point to 
 * malloc'd memory. Thus this memory needs to be freed at the very end of the
 * program AND right before each new call of new_cmd(). If not, new_cmd() will
 * reset .idlist = NULL and memory will leak.
 */
void free_cmd(Command *command)
{
	if (command->idcount) {
		free(command->idlist);
	}

}

static int set_id_list(int **id, char *line);

static void set_add_cmd(Command *command, char *line)
{
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = ADD_NODE;
		command->idcount = set_id_list(&command->idlist, line);
	} else if (!_strcmp("block", token)) {
		command->maincmd = ADD_BLOCK;
		command->idcount = set_id_list(&command->idlist, line);
	} 
	return;
}

static void set_rm_cmd(Command *command, char *line)
{
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = RM_NODE;
		command->idcount = set_id_list(&command->idlist, line);
	} else if (!_strcmp("block", token)) {
		command->maincmd = RM_BLOCK;
		command->idcount = set_id_list(&command->idlist, line);
	} 
	return;
}

/* set_id_list: This function required for set_add_cmd and set_rm_cmd.
 * It takes the remaining tokens, which are supposed to be lists of
 * bid's or nid's and adds them as an array to struct Command. The
 * function returns the number of tokens added.
 *
 * Because we allocate memory here, we have to free it at the end of
 * the program. This feels somewhat bizarre, because it's only these
 * members of struct Command that need to be freed.
 */
static int set_id_list(int **id, char *line) 
{
	char delim = ' ';
	// Allocate exact amount of memory
	int tokencount = 0;
	char cpyline[_strlen(line)];
	_strcpy(cpyline, line);
	char *cpyptr = cpyline;
	while (_strsep(&cpyptr, &delim)) {
		tokencount++;
	}
	if (!tokencount) return 0;
	*id = malloc(sizeof(int) * tokencount);
	// Get tokens and put into memory
	char *token;
	for (int i = 0; i < tokencount; i++) {
		token = _strsep(&line, &delim);
		*(*id + i) = (int) strtol(token, NULL, 10);
	}
	return tokencount;
}

static void set_ls_cmd(Command *command, char *line)
{
	(void) line;
	command->maincmd = LS;
}

static void set_sync_cmd(Command *command)
{
	command->maincmd = SYNC;
}

static void set_quit_cmd(Command *command)
{
	command->maincmd = QUIT;
}

/* get_cmd: This function parses the first token and then passes off
 * remaining parsing to one of the set_[X]_cmd functions. 
 */
Command *get_cmd()
{
	Command *command = new_cmd();
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
