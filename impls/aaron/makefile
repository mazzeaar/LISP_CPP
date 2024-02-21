CXX = g++
CXXFLAGS = -Wall -march=native -std=c++20 -O3

INCLUDEDIR = include
SRCDIR = src
BINDIR = bin
OBJDIR = obj
STEPDIR = steps

MAINS = $(wildcard $(STEPDIR)/step*.cpp)
TARGETS = $(MAINS:%.cpp=%)

all: $(TARGETS)

$(TARGETS): %: %.o libmal.a
	$(CXX) $^ -o $@ $(CXXFLAGS)






$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

test: clean 
	@(cd ../../ && make "test^cpp^step8")

run: step2_eval
	@bin/$^

all_test: clean 
	@(cd ../../ && make "test^cpp")

clean:
	@echo "cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)