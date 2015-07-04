CC=g++

CFLAGS=-O3 -ffast-math -funroll-loops
FINAL=-lboost_program_options -lboost_regex
SRCDIR=src
BINDIR=bin
LIBS=
INCS=

.PHONY: clean

all: ${BINDIR}/stackDecoder

${BINDIR}/stackDecoder: ${SRCDIR}/models.h
	mkdir -p $(BINDIR)
	g++ -std=c++11 $(CFLAGS) $(LIBS) $(INCS) $(SRCDIR)/stackDecoder.cc -o $(BINDIR)/stackDecoder $(FINAL)

clean:
	rm ${BINDIR}/*
