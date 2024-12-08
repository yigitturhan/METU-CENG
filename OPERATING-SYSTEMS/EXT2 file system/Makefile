CC = g++
CFLAGS = -std=c++14 -Wall

SRCS = ext2fs_print.c recext2fs.cpp
OBJS := $(SRCS:.c =.o)
OBJS := $(OBJS:.cpp =.o)
EXECUTABLE = recext2fs
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
%.O: %.C
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(EXECUTABLE)

