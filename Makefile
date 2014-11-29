.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS = -ggdb
INCLUDES += -I/usr/local/include
LDFLAGS += -L/usr/local/lib -lpkg

COMPILE = $(CC) -std=gnu99 $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS)
INSTALL = /usr/bin/install
PREFIX = /usr/local
BINPATH = $(PREFIX)/bin
DEPS =
OBJ = main.o

%.o: %.c $(DEPS)
	$(COMPILE) -c -o $@ $<

all: pkgdep

pkgdep: $(OBJ)
	$(LINK) -o $@ $^ $(LDFLAGS)

install: pkgdep
	/usr/bin/install -m 755 pkgdep $(BINPATH)

delete:
	rm -f $(BINPATH)/pkgdep

clean:
	/bin/rm -f pkgdep *.o
