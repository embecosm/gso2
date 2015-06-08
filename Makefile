
SOURCES=src/algorithms/*.cpp
MAINSOURCE=src/main.cpp
TESTSOURCES=tests/*.cpp

.PHONY=test all

all:
	g++-4.9 -std=c++11 $(SOURCES) -o omega -g -Wall

test: $(TESTSOURCES) $(SOURCES)
	g++-4.9 -std=c++11 $(TESTSOURCES) $(SOURCES) -o tests/test -g -Wall -I src -lboost_unit_test_framework
	cd tests && time ./test
