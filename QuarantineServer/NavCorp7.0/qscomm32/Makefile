# Makefile for QsComm (Quarantine Server Communications)

CC= gcc

INCDIRS = \
-I../shared/include \
-I../include

SYMBOLS = \
-DUNIX \
-DSYM_UNIX \
-DSOLARIS \
-DSYM_SOLARIS \
-DBIG_ENDIAN

CCFLAGS = $(INCDIRS) $(SYMBOLS)

OBJS = \
sockets.o 

.SUFFIXES: .cpp .in

.cpp.o: $<
	$(CC) $(CCFLAGS) -c -g $<

all: libqscomm.a 

clean:
	rm -f *.o libqscomm.a

libqscomm.a: $(OBJS)
	ar $(ARFLAGS) $@ $(OBJS)
