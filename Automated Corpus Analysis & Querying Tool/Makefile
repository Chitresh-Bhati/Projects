CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRCS_COMMON = Node.cpp dict.cpp qna_tool.cpp
SRCS_SEARCH = search.cpp

all: corpus_tool test_runner

corpus_tool: $(SRCS_COMMON) $(SRCS_SEARCH) tester.cpp
	$(CXX) $(CXXFLAGS) $(SRCS_COMMON) $(SRCS_SEARCH) tester.cpp -o corpus_tool

test_runner: $(SRCS_COMMON) $(SRCS_SEARCH) test.cpp
	$(CXX) $(CXXFLAGS) $(SRCS_COMMON) $(SRCS_SEARCH) test.cpp -o test_runner

test: test_runner
	./test_runner

clean:
	rm -f corpus_tool test_runner paragraph_*.txt query.txt

.PHONY: all test clean
