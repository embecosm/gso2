
SOURCES=src/algorithms/*.cpp
MAINSOURCE=src/main.cpp
TESTSOURCES=tests/canonical_slots.cpp tests/avr_instruction.cpp tests/bruteforce.cpp tests/test.cpp
TESTS=$(TESTSOURCES:.cpp=.test)
GENERATEDSOURCES=src/frontends/avr_gen.hpp
CFLAGS= -std=c++11 -Wall -g -O3
CC=g++-4.9

.PHONY=test all

all:
	make -j omega buildtest

omega: $(SOURCES) $(GENERATEDSOURCES) $(MAINSOURCE)
	$(CC) $(CFLAGS) $(SOURCES) $(MAINSOURCE) -o omega

%_gen.hpp: %.yml src/generate.py
	python src/generate.py $< $@

test:
	make buildtest
	cd tests && time ./test

buildtest: tests/canonical_speed $(TESTS)

tests/canonical_speed: tests/canonical_speed.cpp $(SOURCES) Makefile
	$(CC) $(CFLAGS) tests/canonical_speed.cpp $(SOURCES) -o tests/canonical_speed -I src

%.test: %.cpp Makefile
	$(CC) $(CFLAGS) $< $(SOURCES) -o $@ -I src -lboost_unit_test_framework -O0

clean:
	rm -f omega tests/tests tests/canonical_speed
	rm -f $(GENERATEDSOURCES)
