IDIR =include
SRC=src
CC=mpicxx
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR=lib

LIBS=-L/usr/lib/mpich2/lib

_DEPS = Definitions.h ImageManipulator.h Color_Space_Conversions.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ImageManipulator.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sobel-mpi: $(OBJ)
	$(CC) -L$(LDIR) -o $@ $^ $(CFLAGS) $(LIBS) -Wall

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~.
