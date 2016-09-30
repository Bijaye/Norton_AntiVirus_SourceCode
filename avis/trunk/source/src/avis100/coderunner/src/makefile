OPTS = -O2 -s -DCR_TSR
#OPTS = -g -DCR_TSR

CFLAGS = $(OPTS) -DW95_32 -DW_32 -DCR_MOREFEATURES -DCR_MOREFEATURES2 -DCR_FULLDECRYPTION -D__UNIX__ -DNEW1

CC = g++

OBJS = cr_runr2.o cr_runr1.o cr_runr3.o cr_misc.o \
	cr_fscan.o mystuff.o myutils.o cr_glob.o sigcmp.o algscan.o

codo: $(OBJS)
	$(CC) $(OPTS) $(CFLAGS) $(OBJS) -o codo

algscan.o: algscan.c
cr_runr1.o: cr_runr1.c
cr_runr2.o: cr_runr2.c
cr_runr3.o: cr_runr3.c
cr_fscan.o: cr_fscan.c
cr_misc.o: cr_misc.c
sigcmp.o: sigcmp.c
cr_glob.o: cr_glob.c
mystuff.o: mystuff.c
myutils.o: myutils.c

clean:
	rm -f *.o
