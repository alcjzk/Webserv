# main targets:
# - webserv (creates root webserv as a copy of bin/release)
# - d/debug (build debug version)
# - release (build release version)
# - t/test (build and run unit tests)
# - r/run (build and run)
# - f/fuzz (build and run fuzz tests)
# - fmt (run clang-format on sources)
# + fclean, clean, re, all (default)

NAME = webserv

OBJ_DIR = obj
SRC_DIR = $(sort $(dir $(wildcard src/*/))) src
BIN_DIR = bin

SRCS =							\
Config.cpp						\
Server.cpp						\
Runtime.cpp						\
Task.cpp						\
Reader.cpp						\
Method.cpp						\
HTTPVersion.cpp					\
RequestLine.cpp					\
Error.cpp						\
Status.cpp						\
HTTPError.cpp					\
Response.cpp					\
Log.cpp							\
Request.cpp						\
Header.cpp						\
http.cpp						\
TiniNode.cpp					\
TiniTree.cpp					\
TiniUtils.cpp					\
TiniValidator.cpp				\
Route.cpp						\
FileResponse.cpp				\
Routes.cpp						\
Path.cpp						\
HostAttributes.cpp				\
DirectoryResponse.cpp			\
RedirectionResponse.cpp			\
TimeoutResponse.cpp 			\
AcceptTask.cpp					\
ReceiveRequestTask.cpp			\
SendResponseTask.cpp			\
TemplateEngine.cpp              \
HttpUri.cpp						\
File.cpp						\
ReadTask.cpp					\
FileResponseTask.cpp			\
ErrorResponseTask.cpp			\
BasicTask.cpp					\
Buffer.cpp						\
main.cpp

SRCS_TEST = test/testmain.cpp

DEBUG_TARGET = $(BIN_DIR)/debug
RELEASE_TARGET = $(BIN_DIR)/release
TEST_TARGET = $(BIN_DIR)/test

# make run will use this target
RUN_TARGET = $(DEBUG_TARGET)

CC		= c++
STD		= c++17
SHELL	= /bin/sh

$(DEBUG_TARGET) $(TEST_TARGET): CFLAGS = $(STD:%=-std=%) -MP -MMD -Wall -Wextra -Werror -g -Wno-unused-variable -Wno-unused-parameter -O0 -D TEST $(SRC_DIR:%=-I%) -Itest -D LOG_ENABLE -D LOGLEVEL_INFO -fsanitize=address
$(RELEASE_TARGET): CFLAGS = $(STD:%=-std=%) -MP -MMD -Wall -Wextra -Werror -Wpedantic -Wno-unused-variable -Wno-unused-parameter -O3 $(SRC_DIR:%=-I%) -D LOG_ENABLE -D LOGLEVEL_ERR

OBJS_DEBUG = $(SRCS:%.cpp=$(OBJ_DIR)/debug/%.o)
OBJS_RELEASE = $(SRCS:%.cpp=$(OBJ_DIR)/release/%.o)
OBJS_TEST = $(filter-out $(OBJ_DIR)/debug/main.o,$(OBJS_DEBUG)) $(OBJ_DIR)/testmain.o

.PHONY: all
all: $(DEBUG_TARGET) $(TEST_TARGET) $(RELEASE_TARGET) $(NAME)

$(NAME): $(RELEASE_TARGET)
	cp $< $@

$(DEBUG_TARGET): $(OBJS_DEBUG) | $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^
	@echo CC = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo "->" $@
	@echo

$(TEST_TARGET): $(OBJS_TEST) | $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^
	@echo CC = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo "->" $@
	@echo

$(RELEASE_TARGET): $(OBJS_RELEASE) | $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $@ $^
	@echo CC = $(CC)
	@echo CFLAGS = $(CFLAGS)
	@echo "->" $@
	@echo

$(OBJS_DEBUG): $(OBJ_DIR)/debug/%.o: %.cpp | $(OBJ_DIR)/debug
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $< "->" $@

$(OBJS_RELEASE): $(OBJ_DIR)/release/%.o: %.cpp | $(OBJ_DIR)/release
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $< "->" $@

$(OBJ_DIR)/testmain.o: test/testmain.cpp
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $< "->" $@

$(OBJ_DIR) $(OBJ_DIR)/release $(OBJ_DIR)/debug $(BIN_DIR):
	@mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf test/testmain.cpp
	@echo

.PHONY: fclean
fclean: clean
	rm -rf $(BIN_DIR)
	rm -rf $(NAME)
	@echo

.PHONY: re
re: fclean all

HEADERS = $(wildcard src/*.hpp) $(wildcard src/*/*.hpp)

test/testmain.cpp: $(HEADERS)
	@echo $^
	@python3 test/testgen.py test/testmain.cpp $(HEADERS)
	@echo Generated $@

.PHONY: release
release: $(RELEASE_TARGET)

.PHONY: test
test: $(TEST_TARGET)
	./$<
.PHONY: t
t: test

.PHONY: debug
debug: $(DEBUG_TARGET)
.PHONY: d
d: debug

.PHONY: run
run: $(RUN_TARGET)
	./$<
.PHONY: r
r: run

.PHONY: fmt
fmt:
	clang-format -i src/*.cpp src/*.hpp src/*/*.cpp src/*/*.hpp test/*.hpp

.PHONY: fuzz
fuzz: $(DEBUG_TARGET)
	cd test && sh ./fuzz.sh 25
.PHONY: f
f: fuzz

vpath %.cpp $(SRC_DIR) test

-include $(SRCS:%.cpp=$(OBJ_DIR)/debug/%.d)
-include $(SRCS:%.cpp=$(OBJ_DIR)/release/%.d)
-include $(SRCS_TEST:%.cpp=$(OBJ_DIR)/%.d)
