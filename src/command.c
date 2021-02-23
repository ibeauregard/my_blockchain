/* command.c contains logic for reading input, parsing it, and returning a
 * struct Command. The most important function is get_cmd(), which parses
 * the first token / word and then passes off the remaining parsing to one of
 * the set_[X]_cmd() functions. set_add_cmd() and set_rm_cmd() are the only
 * ones that call set_id_list(). The most complicated set_[X]() function
 * is set_id_list(), which combines various functionality into one function.
 * It's currently quite messy, but I made a judgement to keep it all here
 * for ease of reference vs modularizing and spreading it out over various 
 * functions, especially because I don't expect we need to reuse it.
 *
 * get_cmd()  ->  set_add_cmd()  ->  set_id_list()
 *            ->  set_rm_cmd()   ->  set_id_list()
 *            ->  set_ls_cmd()
 *            ->  set_sync_cmd()
 *            ->  set_quit_cmd()
 *
 * A few "design" decisions: 
 *
 * - The struct Command contains a field maincmp that differentiates ADD_NODE
 *   from ADD_BLOCK and RM_NODE from RM_BLOCK. We could consider them subsets
 *   of the same cmd, but their logic is entirely different. So for all intents
 *   and purposes they are separate instructions.
 *
 * - Additionally, I opted to include 4 members bidlist, bidcount, nidlist,
 *   and nidcount in Command rather than create another struct to hold them.
 *
 * - new_cmd() uses a static instance of struct Command instead of malloc.
 *   The reason for doing this is to minimize free() calls in the parent fxn.
 *   Additionally, because only one instance of struct Command is ever required
 *   at any given time, this decision seemed appropriate. Note that free() 
 *   cannot be avoided entirely, as it is still required for [x]idlist members.
 *
 * - get_cmd() parses just the first token of each command and then passes
 *   the remaining tokens to a set_X_cmd() function. This way, for e.g., only 
 *   the set_add_cmd() function needs to parse the second token, expecting 
 *   either a "block" or "node" token (and the other set_X_cmd() functions 
 *   don't have to worry about it). 
 */

#include <stdio.h>                 // For printf
#include <stdlib.h>                // For free
#include <unistd.h>                // For STDIN

#include "command.h"
#include "utils/_readline.h"
#include "utils/_string.h"         // For _strcmp, _strsep
#include "utils/_stdlib.h"         // For _strtol, _isnumeric

/* print_cmd: Used for debugging - prints struct Command.
 */
void print_cmd(Command *command)
{
	printf("maincmd: %d\n", command->maincmd);
	printf("lflag: %d\n", command->lflag);
	printf("all: %d\n", command->all);
	// Print nidlist
	printf("nidcount: %d\n", command->nidcount);
	printf("nid: ");
	for (int i = 0; i < command->nidcount; i++) {
		printf("%d, ", *(command->nidlist+i));
	}
	printf("\n");
	// Print bidlist
	printf("bidcount: %d\n", command->bidcount);
	printf("bid: ");
	for (int i = 0; i < command->bidcount; i++) {
		printf("%d, ", *(command->bidlist+i));
	}
	printf("\n");
}

/* new_cmd(): Uses a static instance of struct Command instead of malloc.
 * The reason for doing this is to minimize free() calls in the parent fxn.
 * Additionally, because only one instance of struct Command is ever required
 * at any given time, this decision seemed appropriate.
 *
 * With that being said, still need to malloc memory for .nidlist member.
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

/* free_cmd: Needed for freeing just the memory allocated in set_id_list().
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

/* Declare set_id_list() here so set_add_cmd() and set_rm_cmd() can use it */
static void set_id_list(Command *command, char **line, int n, char type);

/* set_add_cmd: Accounts for:
 * add node nid...       
 * add block bid nid...    ...where * can be used.
 */
static void set_add_cmd(Command *command, char **line)
{
	char delim = ' ';
	char *token = _strsep(line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = ADD_NODE;
		set_id_list(command, line, 0, 'n');
		if (!command->nidcount) {
			command->maincmd = UNDEFINED;
		}
	} else if (!_strcmp("block", token)) {
		command->maincmd = ADD_BLOCK;
		set_id_list(command, line, 1, 'b');
		set_id_list(command, line, 0, 'n');
		if ((!command->nidcount && !command->all) || !command->bidcount) {
			command->maincmd = UNDEFINED;
		}
	} 
	return;
}

/* set_rm_cmd: Accounts for:
 * rm node nid...     ...where * can be used
 * rm block bid...
 */
static void set_rm_cmd(Command *command, char **line)
{
	char delim = ' ';
	char *token = _strsep(line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = RM_NODE;
		set_id_list(command, line, 0, 'n');
		if (!command->nidcount) {
			command->maincmd = UNDEFINED;
		}
	} else if (!_strcmp("block", token)) {
		command->maincmd = RM_BLOCK;
		set_id_list(command, line, 0, 'b');
		if (!command->bidcount) {
			command->maincmd = UNDEFINED;
		}
	} 
	return;
}

/* set_id_list: Currently a messy GOD function. Should probably be
 * refactored into multiple smaller modules. 
 *
 * This function is required for set_add_cmd and set_rm_cmd.
 * It takes the remaining tokens, which are supposed to be one or more
 * bid's or nid's, and adds them as a malloc'd array to Command. The
 * function also updates .[x]idcount and/or .all if necessary.
 *
 * Because we allocate memory here, we have to free it later.
 *
 * Parameters
 * ----------
 * @command: The struct Command we want to add nid's and bid's to.
 * @line: The string of remaining tokens.
 * @n: How many tokens we want to parse and append to [x]idlist.
 * @type: Either 'n' or 'b'. Indicates which of [x]idlist or [x]idcount
 *        to update.
 *
 */
static void set_id_list(Command *command, char **line, int n, char type) 
{
	char delim = ' ';
	int **xidlist = NULL;
	// Determine which list we're populating
	if (type == 'n') {
		xidlist = &command->nidlist;
	} else if (type == 'b') {
		xidlist = &command->bidlist;
	}
	// Allocate exact amount of memory to .xidlist
	int tokencount = 0;
	char cpyline[_strlen(*line) + 1];
	_strcpy(cpyline, *line);
	char *cpyptr = cpyline;
	while (_strsep(&cpyptr, &delim)) {
		tokencount++;
	}
	if (!tokencount) return;
	*xidlist = malloc(sizeof(int) * tokencount);
	// Get tokens and add to .xidlist up to a maximum of n
	// (unless n = 0 in which case no limit)
	char *token;
	int xidcount = 0;
	int max = (n == 0) ? tokencount : n;
	for (int i = 0; i < tokencount && i < max; i++) {
		token = _strsep(line, &delim);
		// If any tokens are *, we set all flag and continue
		if (!_strcmp("*", token)) {
			command->all = true;
			continue;
		}
		// If token isn't a number (and isn't *), bad command
		if (!_isnumeric(token)) {
			command->maincmd = UNDEFINED;
			return;
		}
		*(*xidlist + xidcount) = (int) strtol(token, NULL, 10);
		xidcount++;
	}
	if (type == 'n') {
		command->nidcount = xidcount;
	} else if (type == 'b') {
		command->bidcount = xidcount;
	}
	return;

}

/* set_ls_cmd: Needs to check if there is an '-l' flag. 
 * Right now only checks second token.
 */
static void set_ls_cmd(Command *command, char **line)
{
	command->maincmd = LS;
	char delim = ' ';
	char *token = _strsep(line, &delim);
	// If we don't include this check, _strcmp will error if token is NULL.
	if (!token) return;
	if (!_strcmp("-l", token)) {
		command->lflag = true;
	}
	return;
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
		set_add_cmd(command, &line);
	} else if (!_strcmp("rm", token)) {
		set_rm_cmd(command, &line);
	} else if (!_strcmp("ls", token)) {
		set_ls_cmd(command, &line);
	} else if (!_strcmp("sync", token)) {
		set_sync_cmd(command);
	} else if (!_strcmp("quit", token)) {
		set_quit_cmd(command);
	} 	
	free(line_to_free);
	line_to_free = NULL;
	return command;
}
