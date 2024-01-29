CXX = g++
CXXFLAGS = -Wall -std=c++20 -O3
INCLUDEDIR = include
SRCDIR = src
BINDIR = bin
OBJDIR = obj

.PHONY: all clean test step0_repl step1_read_print 

all: clean step0_repl step1_read_print

step0_repl: $(OBJDIR)/step0_repl.o | $(BINDIR)
	@echo "linking step0_repl"
	@$(CXX) $^ -o $(BINDIR)/$@

step1_read_print: $(OBJDIR)/step1_read_print.o $(OBJDIR)/reader.o | $(BINDIR)
	@echo "linking step1_read_print"
	@$(CXX) $^ -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "compiling $<"
	@$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

test: clean 
	@(cd ../../ && make "test^cpp^step1")

clean:
	@echo "cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)
