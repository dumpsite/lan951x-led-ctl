ECHO	= /bin/echo -e
SHELL	= /bin/sh
RM	= /bin/rm -f
CC	= gcc
STRIP	= strip

GIT_VERSION := $(shell git --no-pager describe --tags --dirty |sed 's/\([^-]*-g\)/r\1/;s/-/./g')

CFLAGS	= -g -Os -std=c11 -I./include -Wall -Wstrict-prototypes -Wconversion
CFLAGS	+= -Wmissing-prototypes -Wshadow -Wextra -Wunused
CFLAGS	+= -DVERSION=\"$(GIT_VERSION)\"
LDFLAGS	= -lusb-1.0

PROGS = lan951x-led-ctl

all:	$(PROGS)

clean:
	$(RM) src/*.o
tidy:
	$(RM) src/*.o $(PROGS)

install: $(PROGS)
	@$(ECHO) "\t==> Installing programs to $(DESTDIR)/bin"
	@install -m 0755 -d $(DESTDIR)/bin
	@install -m 0755 -t $(DESTDIR)/bin $(PROGS)

# Generic instructions
src/%.o: src/%.c
	@$(ECHO) "\t--> Compiling `basename $<`"
	@$(CC) $(CFLAGS) -c $< -o $@

# Specific programs
lan951x-led-ctl:	src/lan951x-led-ctl.o
	@$(ECHO) "\t==> Linking objects to output file $@"
	$(CC) $(CFLAGS) $+ $(LDFLAGS) -o $@
	@$(STRIP) $@
