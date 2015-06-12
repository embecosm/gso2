
SOURCES=src/algorithms/*.cpp
MAINSOURCE=src/main.cpp
TESTSOURCES=tests/canonical_slots.cpp tests/avr_instruction.cpp tests/bruteforce.cpp
TESTS=$(TESTSOURCES:.cpp=.test)
GENERATEDSOURCES=src/frontends/avr_gen.hpp
CFLAGS= -std=c++11 -Wall -g -O3

.PHONY=test all

all:
	make -j omega buildtest

omega: $(SOURCES) $(GENERATEDSOURCES) $(MAINSOURCE)
	g++-4.9 $(CFLAGS) $(SOURCES) $(MAINSOURCE) -o omega

%_gen.hpp: %.yml src/generate.py
	python src/generate.py $< $@

test:
	make buildtest
	cd tests && time ./test

buildtest: tests/canonical_speed $(TESTS)

# tests/test: $(TESTSOURCES) $(SOURCES)  Makefile
# 	g++-4.9 -std=c++11 $(TESTSOURCES) $(SOURCES) -o tests/test -g -Wall -I src -lboost_unit_test_framework -O0

# tests/avr_test: $(AVRTESTSOURCES) $(SOURCES)  Makefile
# 	g++-4.9 -std=c++11 $(AVRTESTSOURCES) $(SOURCES) -o tests/avr_test -g -Wall -I src -lboost_unit_test_framework -O0


tests/canonical_speed: tests/canonical_speed.cpp $(SOURCES) Makefile
	g++-4.9 $(CFLAGS) tests/canonical_speed.cpp $(SOURCES) -o tests/canonical_speed -I src

%.test: %.cpp Makefile
	g++-4.9 $(CFLAGS) $< $(SOURCES) -o $@ -I src -lboost_unit_test_framework -O0

clean:
	rm -f omega tests/tests tests/canonical_speed
	rm -f $(GENERATEDSOURCES)
