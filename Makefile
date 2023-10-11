NAME	= webserv

OBJ_DIR	= ./obj
SRC_DIR = ./src/

CC 		= c++

OPT = 0
WARN = all extra error no-unused-variable no-unused-parameter
EXTRA = -g -MP -MMD

CFLAGS 	= -I$(SRC_DIR) $(EXTRA) $(WARN:%=-W%) -O$(OPT) -std=c++98
SHELL	= /bin/sh

SRCS	= main.cpp Config.cpp

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

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

.PHONY: all run clean fclean re

vpath %.cpp $(SRC_DIR)
-include $(SRCS:%.cpp=$(OBJ_DIR)/%.d)
