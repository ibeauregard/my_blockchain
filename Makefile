CC = gcc
CFLAGS += -Wall -Wextra -Wpedantic -Werror -g3
SANITIZE = -fsanitize=address
LINKERFLAG = -lm

SRCS = $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c)
SRC_OBJS = $(SRCS:.c=.o)
TESTS = $(wildcard tests/*.c) $(wildcard tests/*/*.c)
TESTS_OBJS = $(TESTS:.c=.o)

MAIN = my_blockchain
TEST_EXEC = my_blockchain_test

.PHONY = all test clean fclean re

all: $(MAIN)

$(MAIN): $(SRC_OBJS)
	$(CC) $(CFLAGS) $(SANITIZE) -o $@ $(LINKERFLAG) $^

test: $(TEST_EXEC)

$(TEST_EXEC): $(TESTS_OBJS)
	$(CC) $(CFLAGS) $(SANITIZE) -o $@ $(LINKERFLAG) $^
	./$(TEST_EXEC)

clean:
	$(RM) $(SRC_OBJS) $(TESTS_OBJS)

fclean: clean
	$(RM) $(MAIN) $(TEST_EXEC)

re: fclean all
