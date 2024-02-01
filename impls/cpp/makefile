CXX = g++
CXXFLAGS = -Wall -march=native -std=c++20 -O3
INCLUDEDIR = include
SRCDIR = src
BINDIR = bin
OBJDIR = obj

.PHONY: all clean test all_test

STEP1_OBJS := $(OBJDIR)/utils.o $(OBJDIR)/tokenizer.o $(OBJDIR)/types.o $(OBJDIR)/parser.o
STEP2_OBJS := $(STEP1_OBJS) $(OBJDIR)/environment.o

all: clean step2_eval step1_read_print step0_repl

step0_repl: $(OBJDIR)/step0_repl.o | $(BINDIR)
	@echo "linking $@"
	$(CXX) $^ -o $(BINDIR)/$@

step1_read_print:  $(OBJDIR)/step1_read_print.o $(STEP2_OBJS) | $(BINDIR)
	@echo "linking $@"
	$(CXX) $^ -o $(BINDIR)/$@

step2_eval: $(OBJDIR)/step2_eval.o $(STEP2_OBJS) | $(BINDIR)
	@echo "linking $@"
	$(CXX) $^ -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "compiling $<"
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

test: clean 
	@(cd ../../ && make "test^cpp^step2")

run: step2_eval
	@bin/$^

all_test: clean 
	@(cd ../../ && make "test^cpp")

clean:
	@echo "cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)