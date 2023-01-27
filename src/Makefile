
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
DEPS = Application.hpp BidirectionalMultimessageSimulation.hpp Util.hpp Formatters.hpp
SRCS = Launch.cpp Application.cpp BidirectionalMultimessageSimulation.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = Simple-2Layer-Network-Simulator

#
# {fmt} lib specific flags
#
CXXFLAGS += `pkg-config --cflags fmt`
LDFLAGS += `pkg-config --libs fmt`

#
# Debug build settings
#
DBGDIR = ../build/debug
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGTARGET = $(DBGDIR)/$(TARGET)
DBGCXXFLAGS = -Og -g
DBGLDFLAGS = -Og

#
# Release build settings
#
RELDIR = ../build/release
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELTARGET = $(RELDIR)/$(TARGET)
RELCXXFLAGS = -O3 -march=native -mtune=native -flto
RELLDFLAGS = -O3 -march=native -mtune=native -flto=auto -s

.PHONY: all prep debug release remake clean

# Default build rules
all: prep release

#
# Debug build rules
#
debug: $(DBGTARGET)

$(DBGTARGET): $(DBGOBJS)
	$(CXX) $(LDFLAGS) $(DBGLDFLAGS) $^ -o $@

$(DBGDIR)/Launch.o: Launch.cpp Application.hpp BidirectionalMultimessageSimulation.hpp Util.hpp
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) $< -o $@

$(DBGDIR)/Application.o: Application.cpp Application.hpp
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) $< -o $@

$(DBGDIR)/BidirectionalMultimessageSimulation.o: BidirectionalMultimessageSimulation.cpp \
												 BidirectionalMultimessageSimulation.hpp \
												 Formatters.hpp
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) $< -o $@

#
# Release build rules
#
release: $(RELTARGET)

$(RELTARGET): $(RELOBJS)
	$(CXX) $(LDFLAGS) $(RELLDFLAGS) $^ -o $@

$(RELDIR)/Launch.o: Launch.cpp Application.hpp BidirectionalMultimessageSimulation.hpp Util.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

$(RELDIR)/Application.o: Application.cpp Application.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

$(RELDIR)/BidirectionalMultimessageSimulation.o: BidirectionalMultimessageSimulation.cpp \
												 BidirectionalMultimessageSimulation.hpp \
												 Formatters.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

#
# Preparation rule
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR)

#
# Remaking rule
#
remake: clean all

#
# Cleaning rule
#
clean:
	rm -f $(DBGOBJS) $(DBGTARGET) $(RELOBJS) $(RELTARGET)
