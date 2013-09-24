# makefile

IDIR = include
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR)

SDIR = src
ODIR = bin
LIBS=-lm

_DEPS = state.h simdata.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = state.o simdata.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/khanna

dir:
	mkdir -p $(ODIR)

$(ODIR)/khanna: $(OBJ)
	$(CC) $(LIBS) -I$(IDIR) -o $@ $^ $(SDIR)/khanna.cpp

clean:
	rm -f $(ODIR)/*.o *~ $(INCDIR)/*~ 

.PHONY: clean
