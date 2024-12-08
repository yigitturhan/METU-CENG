# Define the compiler
CXX=g++

# Compiler flags
CXXFLAGS=-Wall -g -w -O3

# Output directories for object and dependency files
OBJDIR=obj
DEPDIR=dep

# Create the directories if they do not exist
$(shell mkdir -p $(OBJDIR) $(DEPDIR))

# Define the C++ source files
SRCS=$(wildcard *.cpp)

# Define the C++ object files with directory
OBJS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

# Define dependency files with directory
DEPS=$(patsubst %.cpp,$(DEPDIR)/%.d,$(SRCS))

# Define the executable file 
MAIN=rasterizer

.PHONY: all clean rasterizer

all:    $(MAIN)
	clear
	@echo  Simple compiler named $(MAIN) has been compiled

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# Rule to generate object files and their dependencies
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) -MM $(CXXFLAGS) $< | sed 's|^.*:|$(DEPDIR)/$*.d $(OBJDIR)/$*.o:|' > $(DEPDIR)/$*.d

clean:
	$(RM) -r $(OBJDIR) $(DEPDIR) $(MAIN) *.ppm

# Include the generated dependencies
-include $(DEPS)

# Custom target for building the rasterizer
rasterizer: all
