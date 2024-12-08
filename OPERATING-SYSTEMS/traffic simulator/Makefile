CC = g++
CFLAGS = -g -Wall -pthread
SRCS = helper.cpp main.cpp narrowBridge.cpp ferry.cpp crossRoad.cpp WriteOutput.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)

