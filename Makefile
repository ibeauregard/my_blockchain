CC = gcc
CFLAGS += -Wall -Wextra -Wpedantic -Werror -g3
SANITIZE = -fsanitize=address
LINKERFLAG = -lm

MAIN = my_blockchain
SRC_DIR = src
SRC = $(SRC_DIR)/$(MAIN).c
SRCS = $(filter-out $(SRC), $(wildcard $(SRC_DIR)/*.c)            \
                            $(wildcard $(SRC_DIR)/*/*.c)          \
                            $(wildcard $(SRC_DIR)/*/*/*.c)        \
                            $(wildcard $(SRC_DIR)/*/*/*/*.c))
SRC_OBJS = $(SRCS:.c=.o)

TEST_MAIN = my_blockchain_test
TEST_DIR = tests
TEST = $(TEST_DIR)/$(TEST_MAIN).c
TESTS = $(filter-out $(TEST), $(wildcard $(TEST_DIR)/*.c)         \
                              $(wildcard $(TEST_DIR)/*/*.c))
TESTS_OBJS = $(TESTS:.c=.o)

.PHONY = all test clean fclean re

all: $(MAIN)

$(MAIN): $(SRC_OBJS)
	$(CC) $(CFLAGS) $(SANITIZE) $(SRC) -o $@ $(LINKERFLAG) $^

test: $(TEST_MAIN)

$(TEST_MAIN): $(SRC_OBJS) $(TESTS_OBJS)
	$(CC) $(CFLAGS) $(SANITIZE) $(TEST) -o $@ $(LINKERFLAG) $^
	./$(TEST_MAIN)

clean:
	$(RM) $(SRC_OBJS) $(TESTS_OBJS)

fclean: clean
	$(RM) $(MAIN) $(TEST_MAIN)

re: fclean all
