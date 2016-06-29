# NOTE! You must have rose installed somewhere useful for this
# makefile to work. On atlantis do module load rose/dev
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))

BIN = bin
SRC = src
INCLUDE = include

THIRD_PARTY = third-party
TP_SRC = $(THIRD_PARTY)/source
TP_BUILD = $(THIRD_PARTY)/build
TP_INSTALL = $(THIRD_PARTY)/install
TP_INCLUDE = $(TP_INSTALL)/include
TP_LIBRARY = $(TP_INSTALL)/lib
INITED_FILE=$(THIRD_PARTY)/initialized

TEST = tests
TEST_BIN = $(TEST)/bin
TEST_SRC = $(TEST)/src

INC_FLGS = -I./$(INCLUDE) -I./$(SRC) -I./$(TP_INCLUDE)
LIB_FLGS = -lboost_system -lboost_iostreams -L./$(TP_LIBRARY) -lisl -lrose

CXX = g++
COPTS = -ggdb --std=c++11
CFLGS = $(COPTS) $(INC_FLGS) $(LIB_FLGS)

# SHORT_TESTS = print_types

# TESTS = $(addprefix $(TEST_BIN)/,$(SHORT_TESTS))

# $(SHORT_TESTS): % : $(TESTS)/%

SHORT_OBJS = ISLWalker \
						 TypeWalker

OBJS = $(addprefix $(BIN)/, $(addsuffix .o, $(SHORT_OBJS)))

all: $(INITED_FILE) $(OBJS)

# Building the Ojbect Files
$(OBJS): $(BIN)/%.o : $(SRC)/%.cpp $(INCLUDE)/%.hpp $(INITED_FILE)
	$(CXX) $(CFLGS) $(COPTS) $< -c -o $@

$(TESTS): $(TEST_BIN)/% : $(TEST_SRC)/%.cpp
	$(CXX) $(CFLGS) $(COPTS) $< -c -o $@

# Initialize the project and install third-party materials
init: initialize
initialize: $(INITED_FILE)

$(INITED_FILE): $(THIRD_PARTY)
	mkdir $(TP_INSTALL) $(TP_BUILD)
	mkdir $(TP_INCLUDE)
	mkdir $(TP_LIBRARY)
	#
	# build ISL
	tar -xzf $(TP_SRC)/isl-0.15.tar.gz -C $(TP_BUILD)/.
	mv $(TP_BUILD)/isl-0.15 $(TP_BUILD)/isl
		 cd $(TP_BUILD)/isl \
	&& export CPPFLAGS=-ggdb \
	&& export CFLAGS=-ggdb \
	&& ./configure --prefix=$(PROJECT_DIR)/$(TP_INSTALL) \
	&& make -j$(MAKE_JOBS) \
	&& make install
	touch $(INITED_FILE)

# Cleaning
neat:
	- rm $(BIN)/*.o

clean-third-party:
	- rm -rf $(TP_INSTALL) $(TP_BUILD)
	- rm $(INITED_FILE)

clean-test:
	- rm -r $(TEST_BIN)/*

clean: clean-test
	- rm $(BIN)/*

clean-all: clean-third-party clean
