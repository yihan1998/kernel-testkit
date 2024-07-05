# Compiler
CC := gcc
# Compiler flags
CFLAGS := -std=gnu11 -Wall -Wextra
# Directories
SRCDIR := src
INCDIR := include
CFLAGS += -I$(INCDIR) 
BUILDDIR := build
# Source files
SRCS :=  $(shell find ./$(SRCDIR) -name '*.c' -print)
# Object files
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
# Target executable
TARGET := $(BUILDDIR)/your_c_program

# Default target
all: $(TARGET)

# Linking the executable from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compiling each source file into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up intermediate files and executable
clean:
	rm -rf $(BUILDDIR)

# Phony targets
.PHONY: all clean
