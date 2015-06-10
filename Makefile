
SOURCES=src/algorithms/*.cpp
MAINSOURCE=src/main.cpp
TESTSOURCES=tests/canonical_slots.cpp
GENERATEDSOURCES=src/frontends/avr_gen.hpp

.PHONY=test all

all: omega

omega: $(SOURCES) $(GENERATEDSOURCES)
	g++-4.9 -std=c++11 $(SOURCES) $(MAINSOURCE) -o omega -g -Wall

%_gen.hpp: %.yml
	python src/generate.py $< $@

test:
	make buildtest
	cd tests && time ./test

buildtest: tests/tests tests/canonical_speed

tests/tests: $(TESTSOURCES) $(SOURCES)  Makefile
	g++-4.9 -std=c++11 $(TESTSOURCES) $(SOURCES) -o tests/test -g -Wall -I src -lboost_unit_test_framework -O0

tests/canonical_speed: tests/canonical_speed.cpp $(SOURCES) Makefile
	g++-4.9 -std=c++11 tests/canonical_speed.cpp $(SOURCES) -o tests/canonical_speed -I src -O3 -g
