CC := gcc
CFLAGS_ANSI = -ansi -pedantic
CFLAGS := -Wall -Wextra -O2
LDFLAGS :=
LIBS := -lm

all: aseconv acoconv

aseconv: ase.c lab.c gimp.c
	$(CC) $(CFLAGS) $(CFLAGS_ANSI) $? -o $@ $(LDFLAGS) $(LIBS)

acoconv: aco.c gimp.c
	$(CC) $(CFLAGS) $(CFLAGS_ANSI) $? -o $@ $(LDFLAGS) $(LIBS)

clean: aseconv acoconv
	rm -f $?

.PHONY: clean
