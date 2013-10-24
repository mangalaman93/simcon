# makefile

IDIR = include
CC = g++
DEBUG = -g
PROFILE =
CFLAGS = -Wall -c $(DEBUG) -I$(IDIR) $(PROFILE)

SDIR = src
ODIR = bin
LIBS = -lm
RES  = results

_DEPS = simdata.h utils.h algo.h stateIterator.h state.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = simdata.o utils.o algo.o stateIterator.o state.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

all: dir $(ODIR)/mdp $(ODIR)/khanna

dir:
	mkdir -p $(ODIR)

$(ODIR)/mdp: $(OBJ)
	$(CC) $(LIBS) -I$(IDIR) -o $@ $^ $(SDIR)/mdp.cpp $(PROFILE)

$(ODIR)/khanna: $(OBJ)
	$(CC) $(LIBS) -I$(IDIR) -o $@ $^ $(SDIR)/khanna.cpp $(PROFILE)

clean:
	rm -rf $(ODIR) *~ $(INCDIR)/*~
	rm -rf $(RES)/*.pdf
	rm -rf $(RES)/*.txt

plot:
	cd $(RES) && gnuplot workload.p cum_profits.p

.PHONY: clean $(ODIR)/mdp $(ODIR)/khanna
