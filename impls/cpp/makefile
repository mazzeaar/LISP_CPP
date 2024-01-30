CXX = g++
CXXFLAGS = -Wall -std=c++20 -O3
INCLUDEDIR = include
SRCDIR = src
BINDIR = bin
OBJDIR = obj

.PHONY: all clean test

STEP1_OBJS := $(OBJDIR)/utils.o $(OBJDIR)/reader.o $(OBJDIR)/types.o
STEP2_OBJS := $(STEP1_OBJS)

all: clean step0_repl step1_read_print step2_eval

step0_repl: $(OBJDIR)/step0_repl.o | $(BINDIR)
	@echo "linking $@"
	@$(CXX) $^ -o $(BINDIR)/$@

step1_read_print:  $(OBJDIR)/step1_read_print.o $(STEP1_OBJS) | $(BINDIR)
	@echo "linking $@"
	@$(CXX) $^ -o $(BINDIR)/$@

step2_eval: $(OBJDIR)/step2_eval.o $(STEP2_OBJS) | $(BINDIR)
	@echo "linking $@"
	@$(CXX) $^ -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "compiling $<"
	@$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

test: clean 
	@(cd ../../ && make "test^cpp^step2")

clean:
	@echo "cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)