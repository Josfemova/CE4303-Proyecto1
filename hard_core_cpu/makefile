CC=arm-linux-gnueabihf-gcc
CFLAGS=-g -I$(IDIR) -Wall -Wextra -Werror -static

IDIR=include
ODIR=obj
SRCDIR=src

LIBS=-lpthread -lm

# Headers dentro de include/
_DEPS = misc.h types.h filter.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Object files (.o) que haya que compilar 
_OBJ = main.o filter.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

hps_executable: $(OBJ)
	$(CC) -o $(ODIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean install

clean:
	rm -rf $(ODIR)

