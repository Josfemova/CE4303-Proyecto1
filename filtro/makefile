CC=gcc
CFLAGS=-g -I$(IDIR) -Wall -Wextra

IDIR=include
ODIR=obj
SRCDIR=src

LIBS=-lfreeimage

# Headers dentro de include/
_DEPS = util.h parse.h memory.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Object files (.o) que haya que compilar 
_OBJ = main.o parse.o memory.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

verificar_filtro: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o core *~ $(INCDIR)/*~ 
	rmdir $(ODIR)
