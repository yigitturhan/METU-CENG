# Makefile for Extended Shell Project

# Compiler settings - Can change to clang++ if preferred
CXX = g++
CC = gcc

# Compiler flags
CXXFLAGS = -Wall -std=c++11
CFLAGS = -Wall

# Linker flags
LDFLAGS = 

# Name of the executable to be created
TARGET = eshell

# Object files - add more to this list as you create new .cpp files
OBJ = main.o parser.o

# Header files
HEADERS = parser.h

# Rule to create .o files from .cpp files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create .o file from .c file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the program
$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $(TARGET)

# Clean the build directory
clean:
	rm -f $(OBJ) $(TARGET)

# Make the all target the default
.PHONY: all
all: $(TARGET)

# Target to print a helpful message
.PHONY: help
help:
	@echo "Makefile for the Extended Shell Project"
	@echo "Available commands:"
	@echo "  make       - Build the project"
	@echo "  make clean - Remove all build files"
	@echo "  make help  - Display this help"

