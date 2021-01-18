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

RAIN_INCLUDES := $(shell find $(RAIN_INCLUDE_DIR) -name *.hpp)

EMILIA_WEB_INCLUDES := $(filter-out $(VERSION_BUILD_INCLUDE), \
	$(wildcard $(INCLUDE_DIR)*.hpp))
EMILIA_WEB_SRCS := $(wildcard $(SRC_DIR)*.cpp)
EMILIA_WEB_NAMES := $(basename $(notdir $(EMILIA_WEB_SRCS)))
EMILIA_WEB_OBJS := $(addsuffix .o, \
	$(addprefix $(OBJ_DIR), $(EMILIA_WEB_NAMES)))
EMILIA_WEB_BIN := $(BIN_DIR)$(BIN_NAME)

# Flags for build. Release by default.
CXX := g++
CXXFLAGS := -I$(INCLUDE_DIR) -I$(RAIN_INCLUDE_DIR) -std=c++17 -pthread -Wall
CXXFLAGS_DEBUG := -O0
CXXFLAGS_RELEASE := -O3
LDLIBS := -lresolv

# OS-specific behavior.
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S), Darwin)
	LDLIBS := $(LDLIBS) -lstdc++fs
endif

# Check if DEBUG=1 is set.
ifeq ($(RELEASE), 1)
  CXXFLAGS := $(CXXFLAGS) $(CXXFLAGS_RELEASE)
else
  CXXFLAGS := $(CXXFLAGS) $(CXXFLAGS_DEBUG)
endif

# Object files should not be deleted to not recompile under no changes.
.SECONDARY: $(EMILIA_WEB_OBJS) $(EMILIA_WEB_BIN)

# Mark .PHONY for rules to avoid compiling files named the same as rules.

# Compile without running.
.PHONY: $(BIN_NAME)
$(BIN_NAME): $(VERSION_BUILD_INCLUDE) $(EMILIA_WEB_BIN)

# Run.
.PHONY: run
run: $(BIN_NAME)
	@cd $(BIN_DIR) && ./$(BIN_NAME)

# Compile without incrementing build.
.PHONY: noinc
noinc: $(EMILIA_WEB_BIN)

# Rules for emilia-web binaries and objects.
$(BIN_DIR)%: $(OBJ_DIR)%.o $(EMILIA_WEB_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $< -o $@
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(EMILIA_WEB_INCLUDES) $(RAIN_INCLUDES)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Delete compile-time files.
.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

# Increment build number.
$(VERSION_BUILD_INCLUDE): $(EMILIA_WEB_INCLUDES) $(EMILIA_WEB_SRCS) $(RAIN_INCLUDES)
	@read line < $(VERSION_BUILD_INCLUDE); \
	TOKENS=($$line); \
	VERSION_BUILD=$$(($${TOKENS[2]} + 1)); \
	echo -e "$${TOKENS[0]} $${TOKENS[1]} $$VERSION_BUILD\c" > \
		$(VERSION_BUILD_INCLUDE); \
	echo build $$VERSION_BUILD
