# Makefile for QsPak (Quarantine Server Packager)

CC= gcc

INCDIRS = \
-I../shared/include \
-I../include 

SYMBOLS = \
-DUNIX \
-DSYM_UNIX \
-DSYM_SOLARIS \
-DBIG_ENDIAN 

CCFLAGS = $(INCDIRS) $(SYMBOLS)

OBJS = \
package.o 

.SUFFIXES: .cpp .in

.cpp.o: $<
	$(CC) $(CCFLAGS) -c -g $<

all: libqspak.a 

clean:
	rm -f *.o libqspak.a

libqspak.a: $(OBJS)
	ar $(ARFLAGS) $@ $(OBJS)
