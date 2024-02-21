CXX = g++
CXXFLAGS = -Wall -march=native -std=c++20 -O3

INCLUDEDIR = include
SRCDIR = src
STEPSDIR = steps
BINDIR = bin
OBJDIR = obj
LIBDIR = lib
LIBNAME = libmal.a

LIB_SRCS := $(wildcard $(SRCDIR)/*.cpp)
LIB_OBJS := $(LIB_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

STEP_SRCS := $(wildcard $(STEPSDIR)/*.cpp)
STEP_OBJS := $(STEP_SRCS:$(STEPSDIR)/%.cpp=$(OBJDIR)/%.o)

STEPS := $(STEP_SRCS:$(STEPSDIR)/%.cpp=%)

.PHONY: all clean test tests

all: clean $(STEPS)

$(OBJDIR) $(BINDIR) $(LIBDIR):
	@mkdir -p $@

$(STEPS): %: $(OBJDIR)/%.o $(LIBDIR)/$(LIBNAME) | $(BINDIR)
	@echo "=> linking $@"
	$(CXX) $(CXXFLAGS) -L$(LIBDIR) $^ -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "=> compiling $@.cpp"
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/%.o: $(STEPSDIR)/%.cpp | $(OBJDIR)
	@echo "=> compiling $@.cpp"
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(LIBDIR)/$(LIBNAME): $(LIB_OBJS) | $(LIBDIR)
	@echo "=> Building library"
	ar rcs $@ $^

test:
	(cd ../../ && make "test^cpp^stepA")

tests:
	(cd ../../ && make "test^cpp")

clean:
	@rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)
