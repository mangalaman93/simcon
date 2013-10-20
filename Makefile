# makefile

IDIR = include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) $(PROFILE)

SDIR = src
ODIR = bin
LIBS = -lm

_DEPS = simdata.h utils.h algo.h stateIterator.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = simdata.o utils.o algo.o stateIterator.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/mdp

dir:
	mkdir -p $(ODIR)

$(ODIR)/mdp: $(OBJ)
	$(CC) $(LIBS) -I$(IDIR) -o $@ $^ $(SDIR)/mdp.cpp $(PROFILE)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~

.PHONY: clean
