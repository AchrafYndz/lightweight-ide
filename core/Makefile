# =========================
# 		Base Config
# =========================

SHELL := /bin/sh

CPP_SRC_EXTENSION := cc
CPP_HEADER_EXTENSION := h

BUILDDIR := ./build
SRCDIR := ./src
TESTDIR := ./test
LIBDIR := ./lib
BIN := lightweight-ide
TEST := lightweight-ide_test

MAIN := $(SRCDIR)/main.$(CPP_SRC_EXTENSION)

CC := clang
CXX := clang++
RM := rm -r

# =========================
# 	   Compile Flags
# =========================

# CPPFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -Wno-deprecated
CPPFLAGS ?= -std=c++17
CPPFLAGS_DEBUG := -g -O0 -fdebug-default-version=4 -fno-limit-debug-info

SRCS := $(shell find $(SRCDIR) -name "*.$(CPP_SRC_EXTENSION)")
OBJS := $(patsubst %.$(CPP_SRC_EXTENSION),$(BUILDDIR)/%.o,$(SRCS))

TEST_SRCS := $(shell find $(SRCDIR) $(TESTDIR) -name "*.$(CPP_SRC_EXTENSION)" -not \( -wholename "$(MAIN)" \) )
TEST_OBJS := $(patsubst %.$(CPP_SRC_EXTENSION),$(BUILDDIR)/%.o,$(TEST_SRCS))

# include all header files
INCFLAGS += $(addprefix -I,$(shell find $(SRCDIR) -type d))

# =========================
#      Library Linking
# =========================

INCFLAGS += -I$(LIBDIR)/nlohmann/include
INCFLAGS += -I$(LIBDIR)/crow/include
INCFLAGS += -I$(LIBDIR)/asio/include

TEST_INCFLAGS += -I$(LIBDIR)/doctest/include

# =========================
#		Miscellaneous
# =========================

CPPFLAGS += $(CPPFLAGS_DEBUG)
CPPFLAGS += $(INCFLAGS)

# =========================
# 	    Make Targets
# =========================

.PHONY: all run build-test test clean flor

all: build build-test

build: $(BUILDDIR)/$(BIN)

build-test: CPPFLAGS += $(TEST_INCFLAGS) -DTEST
build-test: $(BUILDDIR)/$(TEST)

$(BUILDDIR)/%.o: %.$(CPP_SRC_EXTENSION)
	mkdir -p $(dir $@)
	$(CXX) -MMD $(CPPFLAGS) -c $< -o $@

$(BUILDDIR)/$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(BUILDDIR)/$(TEST): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $@

run: build
	$(BUILDDIR)/$(BIN)

test: build-test
	$(BUILDDIR)/$(TEST)

clean:
	$(RM) $(BUILDDIR)

# header file dependency checking
-include $(shell find $(BUILDDIR) -name "*.d")

# command for debugging variables
print-%  : ; @echo $* = $($*)
