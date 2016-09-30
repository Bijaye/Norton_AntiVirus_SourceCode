# unix.mak
# Unix makefile for the avenge library.

TARGETS = libavenge.a
include ../unixmake.inc

OBJS = \
algload.o \
algscan.o \
avcache.o \
avenge.o \
booload.o \
bootrep.o \
crcload.o \
crcscan.o \
delentry.o \
engcopyf.o \
filerep.o \
fstart.o \
getbulk.o \
getinfo.o \
infc.o \
infoaux.o \
memload.o \
memscan.o \
namec.o \
reparaux.o \
vdatfile.o 

libavenge.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
