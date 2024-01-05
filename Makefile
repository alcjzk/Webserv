NAME	= webserv

TEST    = test

OBJ_DIR	= ./obj
SRC_DIR = ./src/

CC 		= c++

OPT = 0
WARN = all extra error no-unused-variable no-unused-parameter
EXTRA = -g -MP -MMD

CFLAGS 	= -I$(SRC_DIR) $(EXTRA) $(WARN:%=-W%) -O$(OPT) -std=c++11 -D LOG_ENABLE -D LOGLEVEL_INFO

SHELL	= /bin/sh

SRCS	= main.cpp Config.cpp Server.cpp Runtime.cpp Task.cpp Reader.cpp Method.cpp HTTPVersion.cpp URI.cpp RequestLine.cpp Error.cpp Status.cpp HTTPError.cpp Response.cpp Log.cpp Request.cpp Header.cpp

SRCS_TEST = test.cpp Reader.cpp HTTPVersion.cpp Method.cpp RequestLine.cpp URI.cpp

all: $(NAME)

run: all
	./$(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

$(NAME): $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST): $(SRCS_TEST:%.cpp=$(OBJ_DIR)/%.o)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

.PHONY: all run clean fclean re

vpath %.cpp $(SRC_DIR)
-include $(SRCS:%.cpp=$(OBJ_DIR)/%.d)
