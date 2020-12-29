SHELL := /bin/bash

# Directories relative paths.
ROOT_DIR := ../
RAIN_INCLUDE_DIR := $(ROOT_DIR)../rain/include/
INCLUDE_DIR := $(ROOT_DIR)include/
SRC_DIR := $(ROOT_DIR)src/
OBJ_DIR := $(ROOT_DIR)obj/
BIN_DIR := $(ROOT_DIR)bin/
BIN_NAME := emilia-web

# Groups of files and filenames.
VERSION_BUILD_INCLUDE := $(INCLUDE_DIR)build.hpp

RAIN_INCLUDE := $(shell find $(RAIN_INCLUDE_DIR) -name *.hpp)

EMILIA_WEB_INCLUDE := $(filter-out $(VERSION_BUILD_INCLUDE), \
	$(wildcard $(INCLUDE_DIR)*.hpp))
EMILIA_WEB_SRC := $(wildcard $(SRC_DIR)*.cpp)
EMILIA_WEB_NAME := $(basename $(notdir $(EMILIA_WEB_SRC)))
EMILIA_WEB_OBJ := $(addsuffix .o, \
	$(addprefix $(OBJ_DIR)$(BIN_NAME)/, $(EMILIA_WEB_NAME)))
EMILIA_WEB_BIN := $(BIN_DIR)$(BIN_NAME)

# Flags for build. Release by default.
CXX := g++
CXXFLAG := -I$(INCLUDE_DIR) -I$(RAIN_INCLUDE_DIR) -std=c++17 -g -Wall -pthread -lresolv
CXXFLAG_DEBUG := -O0
CXXFLAG_RELEASE := -O3 -march=native

# OS-specific behavior.
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S), Darwin)
	CXXFLAG := $(CXXFLAG) -lstdc++fs
endif

# Check if DEBUG=1 is set.
ifeq ($(DEBUG), 1)
  CXXFLAG := $(CXXFLAG) $(CXXFLAG_DEBUG)
else
  CXXFLAG := $(CXXFLAG) $(CXXFLAG_RELEASE)
endif

# Object files should not be deleted to not recompile under no changes.
.SECONDARY: $(EMILIA_WEB_OBJ) $(EMILIA_WEB_BIN)

# Mark .PHONY for rules to avoid compiling files named the same as rules.

# Compile without running.
.PHONY: $(BIN_NAME)
$(BIN_NAME): $(VERSION_BUILD_INCLUDE) $(EMILIA_WEB_BIN)

# Compile and run.
.PHONY: run
run: $(VERSION_BUILD_INCLUDE) $(EMILIA_WEB_BIN)
	./$(EMILIA_WEB_BIN) $(ARGS)

# Compile with incrementing build nor running.
.PHONY: noinc
noinc: $(EMILIA_WEB_BIN)

# Rules for emilia-web binaries and objects.
$(BIN_DIR)%: $(OBJ_DIR)$(BIN_NAME)/%.o $(EMILIA_WEB_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $< $(CXXFLAG)
$(OBJ_DIR)$(BIN_NAME)/%.o: $(SRC_DIR)%.cpp $(EMILIA_WEB_INCLUDE) $(RAIN_INCLUDE)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAG)

# Delete compile-time files.
.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

# Increment build number.
$(VERSION_BUILD_INCLUDE): $(EMILIA_WEB_INCLUDE) $(EMILIA_WEB_SRC) $(RAIN_INCLUDE)
	@read line < $(VERSION_BUILD_INCLUDE); \
	TOKENS=($$line); \
	VERSION_BUILD=$$(($${TOKENS[2]} + 1)); \
	echo -e "$${TOKENS[0]} $${TOKENS[1]} $$VERSION_BUILD\c" > \
		$(VERSION_BUILD_INCLUDE); \
	echo build $$VERSION_BUILD
