# unix.mak
# Unix makefile for the navapi library and apitest program.

# Target project
TARGETS = fcdir	


# Object files
OBJS = \
fcdir.o \
fileops.o \
shared.o 


fcdir: $(OBJS) 
	$(CCC) $(LDFLAGS) $(OBJS) -o $@
	

# Set the C++ compiler.
CCC = gcc

# Prepend .cpp to the suffix list.
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)

# Add rule for building .o files from .cpp files.
.cpp.o: 
	$(CCC) $(CCFLAGS) -c $*.cpp

# Set the compiler flags specific to NAV.
CCFLAGS += -DSYM_UNIX -DBIG_ENDIAN -DSYM_SOLARIS

debug := CCFLAGS += -DSYM_DEBUG

# Set the linker library flags for dynamic linking.
SYSLIBS += -ldl -lrt

# Set the linker library flags for for Posix thread support.
# This library must be LAST on the link line, after the .o's and other libs.
SYSLIBS += -lpthread

# Set libraries
LDLIB += 

#Set Include files
INCLUDES += -I../../inc.avcore -I../../inc.core

# Set the compiler flags for Posix thread support.
CCFLAGS += -D_REENTRANT -D_POSIX_PTHREAD_SOURCE=199506L

# Set the compiler flags that are specific to the 'debug' and 'release' targets.
debug := CCFLAGS += -g
release := CCFLAGS += -O2

# Rules for the standard targets

debug: $(TARGETS)
	cp $? debug

release: $(TARGETS)
	cp $? release

clean:
	rm *.o $(TARGETS)



	
	