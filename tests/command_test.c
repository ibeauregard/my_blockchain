#include <assert.h>
#include <fcntl.h>

#include "../src/commands.h"

#define INPUT_FILENAME "tests/command_input.txt"

void test_get_cmd() 
{
	int fd = open(INPUT_FILENAME, O_RDONLY);
	assert(fd != -1);
	Command *command = get_cmd(fd);
	assert(command->maincmd == UNDEFINED);
	free_cmd(command);
}

void test_command()
{
	test_get_cmd();
}

