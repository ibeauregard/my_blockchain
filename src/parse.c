/* parse.c contains logic for parsing input and "returning" a filled
 * struct Command. The most important function is parse_cmd(), which parses
 * the first token / word and then passes off the remaining parsing to one of
 * the parse_[X]_cmd() functions. parse_add_cmd() and parse_rm_cmd() are the only
 * ones that call parse_id_list(). The most complicated parse_[X] function
 * is parse_id_list(), which combines various functionality into one function.
 * It's currently quite messy, but I made a judgement to keep it all here
 * for ease of reference vs modularizing and spreading it out over various 
 * functions, especially because I don't expect we need to reuse it.
 *
 * parse_cmd()  ->  parse_add_cmd()  ->  parse_id_list()
 *              ->  parse_rm_cmd()   ->  parse_id_list()
 *              ->  parse_ls_cmd()
 *              ->  parse_sync_cmd()
 *              ->  parse_quit_cmd()
 *
 */

#include <stdio.h>                 // For printf
#include <stdlib.h>                // For free

#include "parse.h"
#include "utils/_string.h"         // For _strcmp, _strsep
#include "utils/_stdlib.h"         // For _strtol, _isnumeric

/* Declare parse_id_list here so parse_add_cmd and parse_rm_cmd can use it */
static void parse_id_list(Command *command, char **line, int n, char type);

/* parse_add_cmd: Accounts for:
 * add node nid...       
 * add block bid nid...    ...where * can be used.
 */
static void parse_add_cmd(Command *command, char **line)
{
	char delim = ' ';
	char *token = _strsep(line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = ADD_NODE;
		parse_id_list(command, line, 0, 'n');
		if (!command->nidcount) {
			command->maincmd = UNDEFINED;
		}
	} else if (!_strcmp("block", token)) {
		command->maincmd = ADD_BLOCK;
		parse_id_list(command, line, 1, 'b');
		parse_id_list(command, line, 0, 'n');
		if ((!command->nidcount && !command->all) || !command->bidcount) {
			command->maincmd = UNDEFINED;
		}
	} 
	return;
}

/* parse_rm_cmd: Accounts for:
 * rm node nid...     ...where * can be used
 * rm block bid...
 */
static void parse_rm_cmd(Command *command, char **line)
{
	char delim = ' ';
	char *token = _strsep(line, &delim);
	if (!token) return;
	if (!_strcmp("node", token)) {
		command->maincmd = RM_NODE;
		parse_id_list(command, line, 0, 'n');
		if (!command->nidcount && !command->all) {
			command->maincmd = UNDEFINED;
		}
	} else if (!_strcmp("block", token)) {
		command->maincmd = RM_BLOCK;
		parse_id_list(command, line, 0, 'b');
		if (!command->bidcount) {
			command->maincmd = UNDEFINED;
		}
	} 
	return;
}

/* parse_id_list: Currently a messy GOD function. Should probably be
 * refactored into multiple smaller modules. 
 *
 * This function is required for parse_add_cmd and parse_rm_cmd.
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
static void parse_id_list(Command *command, char **line, int n, char type) 
{
	char delim = ' ';
	unsigned int **xidlist = NULL;
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
	*xidlist = malloc(sizeof(unsigned int) * tokencount);
	// Get tokens and add to .xidlist up to a maximum of n
	// (unless n = 0 in which case no limit)
	char *token;
	size_t xidcount = 0;
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
		*(*xidlist + xidcount) = strtol(token, NULL, 10);
		xidcount++;
	}
	if (type == 'n') {
		command->nidcount = xidcount;
	} else if (type == 'b') {
		command->bidcount = xidcount;
	}
	return;

}

/* parse_ls_cmd: Needs to check if there is an '-l' flag. 
 * Right now only checks second token.
 */
static void parse_ls_cmd(Command *command, char **line)
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

static void parse_sync_cmd(Command *command)
{
	command->maincmd = SYNC;
}

static void parse_quit_cmd(Command *command)
{
	command->maincmd = QUIT;
}

/* parse_cmd: This function parses the first token and then passes off
 * remaining parsing to one of the parse_[X]_cmd functions. 
 */
void parse_cmd(Command *command, char *line)
{
	char delim = ' ';
	char *token = _strsep(&line, &delim);
	if (!_strcmp("add", token)) {
		parse_add_cmd(command, &line);
	} else if (!_strcmp("rm", token)) {
		parse_rm_cmd(command, &line);
	} else if (!_strcmp("ls", token)) {
		parse_ls_cmd(command, &line);
	} else if (!_strcmp("sync", token)) {
		parse_sync_cmd(command);
	} else if (!_strcmp("quit", token)) {
		parse_quit_cmd(command);
	} 	
}
