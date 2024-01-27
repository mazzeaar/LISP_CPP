CXX = g++
CXXFLAGS = -Wall -std=c++20 -O3
INCLUDEDIR = include
SRCDIR = src
BINDIR = bin
OBJDIR = obj

.PHONY: all clean test step0_repl step1_read_print 

all: step0_repl step1_read_print

step0_repl: $(OBJDIR)/step0_repl.o | $(BINDIR)
	$(CXX) $^ -o $(BINDIR)/$@

step1_read_print: $(OBJDIR)/step1_read_print.o $(OBJDIR)/reader.o | $(BINDIR)
	$(CXX) $^ -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

test:
	(cd ../../ && make "test^cpp^step1")

clean:
	rm -rf $(OBJDIR) $(BINDIR)
