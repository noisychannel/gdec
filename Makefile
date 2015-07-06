CC=g++

CFLAGS=-std=c++11 -O3 -ffast-math -funroll-loops
DEBUG_CFLAGS=-g
SAFE_CFLAGS=-Wall -Werror
FINAL=-lboost_program_options -lboost_regex
SRCDIR=src
BINDIR=bin
LIBS=
INCS=

.PHONY: clean

all: ${BINDIR}/stackDecoder

debug: ${BINDIR}/stackDecoder.d

${BINDIR}/stackDecoder: ${SRCDIR}/models.h
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $(INCS) $(SRCDIR)/stackDecoder.cc -o $(BINDIR)/stackDecoder $(FINAL)

${BINDIR}/stackDecoder.d: ${SRCDIR}/models.h
	mkdir -p $(BINDIR)
	$(CC) $(DEBUG_CFLAGS) $(SAFE_CFLAGS) $(CFLAGS) $(LIBS) $(INCS) $(SRCDIR)/stackDecoder.cc -o $(BINDIR)/stackDecoder.d $(FINAL)

clean:
	rm ${BINDIR}/*
