
LATEXSRC=doc/design.tex
LATEXDOC=$(LATEXSRC:.tex=.pdf)
SOURCES=src/algorithms/*.cpp src/utility.cpp
SOURCES_HPP=src/*.hpp src/*/*.hpp
MAINSOURCE=src/main.cpp
TESTSOURCES=tests/canonical_slots.cpp tests/avr_instruction.cpp tests/bruteforce.cpp tests/test.cpp tests/slots.cpp tests/utility.cpp tests/frontend.cpp
TESTS=$(TESTSOURCES:.cpp=.test)
GENERATEDSOURCES=src/frontends/avr_gen.hpp
CFLAGS= -std=c++11 -Wall -g -O3 -I libraries
CC=g++-4.9

.PHONY=test all

all:
	make -j omega buildtest omega_slow doc

omega: $(SOURCES) $(GENERATEDSOURCES) $(MAINSOURCE) $(SOURCES_HPP)
	$(CC) $(CFLAGS) $(SOURCES) $(MAINSOURCE) -o omega

omega_slow: $(SOURCES) $(GENERATEDSOURCES) $(MAINSOURCE) $(SOURCES_HPP)
	$(CC) $(CFLAGS) $(SOURCES) $(MAINSOURCE) -o omega_slow -O0

%_gen.hpp: %.yml src/generate.py
	python src/generate.py $< $@

test:
	make buildtest -j
	cd tests && for BIN in ${TESTS}; do ../$$BIN -p $(TESTFLAGS); done

buildtest: tests/canonical_speed $(TESTS)

tests/canonical_speed: tests/canonical_speed.cpp $(SOURCES) Makefile $(SOURCES_HPP)
	$(CC) $(CFLAGS) tests/canonical_speed.cpp $(SOURCES) -o tests/canonical_speed -I src

%.test: %.cpp Makefile $(SOURCES_HPP) $(SOURCES) $(GENERATEDSOURCES)
	$(CC) $(CFLAGS) $< $(SOURCES) -o $@ -I src -lboost_unit_test_framework

clean:
	rm -f omega $(TESTS) tests/canonical_speed
	rm -f $(GENERATEDSOURCES)
	rm -f $(LATEXDOC)

doc: $(LATEXDOC)

%.pdf: %.tex
	cd $(dir $<); pdflatex -shell-escape -halt-on-error $(notdir $<) && rm -f *.aux *.log
