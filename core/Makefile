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

CPPFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS_DEBUG := -g -Og

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
TEST_INCFLAGS += -I$(LIBDIR)/doctest/include

# =========================
#		Miscellaneous
# =========================

CPPFLAGS += $(INCFLAGS)
CPPFLAGS += $(CPPFLAGS_DEBUG)

# =========================
# 	    Make Targets
# =========================

.PHONY: all run build_test test clean

build: $(BUILDDIR)/$(BIN)

build_test: CPPFLAGS += $(TEST_INCFLAGS) -DTEST
build_test: $(BUILDDIR)/$(TEST)

$(BUILDDIR)/%.o: %.$(CPP_SRC_EXTENSION)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILDDIR)/$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(BUILDDIR)/$(TEST): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $@

run: build
	$(BUILDDIR)/$(BIN)

test: build_test
	$(BUILDDIR)/$(TEST)

clean:
	$(RM) $(BUILDDIR)

# command for debugging variables
print-%  : ; @echo $* = $($*)
