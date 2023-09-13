
#
# Compiler flags
#
CXX = g++
CXXFLAGS = -c -fpie -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow \
			-Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
			-Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
			-Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo \
			-Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

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
CXXFLAGS += $(shell pkg-config --cflags fmt)
LDFLAGS += $(shell pkg-config --libs fmt)

#
# spdlog lib specific flags
#
CXXFLAGS += $(shell pkg-config --cflags spdlog)
LDFLAGS += $(shell pkg-config --libs spdlog)

#
# Debug build settings
#
DBGDIR = ../build/debug
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGTARGET = $(DBGDIR)/$(TARGET)
DBGCXXFLAGS = -Og -g -DSNS_DEBUG=1
DBGLDFLAGS = -Og

#
# Release build settings
#
RELDIR = ../build/release
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELTARGET = $(RELDIR)/$(TARGET)
RELCXXFLAGS = -O3 -march=native -mtune=native -flto -DNDEBUG -DSNS_DEBUG=0
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

$(DBGDIR)/Launch.o: Launch.cpp BidirectionalMultimessageSimulation.hpp Util.hpp
	$(CXX) $(CXXFLAGS) $(DBGCXXFLAGS) $< -o $@

$(DBGDIR)/Application.o: Application.cpp Application.hpp Util.hpp
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

$(RELDIR)/Launch.o: Launch.cpp BidirectionalMultimessageSimulation.hpp Util.hpp
	$(CXX) $(CXXFLAGS) $(RELCXXFLAGS) $< -o $@

$(RELDIR)/Application.o: Application.cpp Application.hpp Util.hpp
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
