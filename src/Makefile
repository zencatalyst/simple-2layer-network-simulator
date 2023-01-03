
#
# Compiler flags
#
CXX = g++
CXXFLAGS = -c -fpie -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow

#
# Linker flags
#
LDFLAGS = -pie

#
# Project files
#
DEPS = BidirectionalMultimessageSimulation.hpp
SRCS = Launch.cpp BidirectionalMultimessageSimulation.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = Simple-2Layer-Network-Simulator

#
# Release build settings
#
RELDIR = ../build/release
RELTARGET = $(RELDIR)/$(TARGET)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCXXFLAGS = -O3 -march=native -mtune=generic -flto
RELLDFLAGS = -O3 -march=native -mtune=generic -flto -s

.PHONY: all clean prep release remake

# Default build
all: prep release

#
# Release rules
#
release: $(RELTARGET)

$(RELTARGET): $(RELOBJS)
	$(CXX) $(LDFLAGS) $(RELLDFLAGS) $^ -o $@

$(RELDIR)/Launch.o: Launch.cpp BidirectionalMultimessageSimulation.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

$(RELDIR)/BidirectionalMultimessageSimulation.o: BidirectionalMultimessageSimulation.cpp BidirectionalMultimessageSimulation.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

#
# Other rules
#
prep:
	@mkdir -p $(RELDIR)

remake: clean all

clean:
	rm -f $(RELTARGET) $(RELOBJS)
