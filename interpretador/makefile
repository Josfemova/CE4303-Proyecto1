CC=gcc
CFLAGS=-g -Wall -Wextra

ODIR=obj
SRCDIR=src

LIBS=-lfreeimage

# Object files (.o) que haya que compilar 
_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

interpretar: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o core *~ $(INCDIR)/*~ 
	rmdir $(ODIR)
