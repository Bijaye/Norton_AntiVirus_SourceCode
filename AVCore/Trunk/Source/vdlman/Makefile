# unix.mak
# Unix makefile for the navapi library and apitest program.

TARGETS = vdlman
CC = gcc
CCFLAGS = -DSYM_UNIX -DUNIX
LDLIBS = -ldl -lrt libnavvdl.a libdefutils.a libtcs.a
INCLUDES = -I../../avcomp/include -I../../inc.avcore -I../../inc.core 

OBJS = \
vdlman.cpp


vdlman: $(OBJS) libnavvdl.a libdefutils.a libtcs.a
	        $(CC) $(INCLUDES) $(OBJS) $(LDLIBS) $(CCFLAGS) -g -o $@
	        
	        
	        
	        
	        
	        
	        



	
	
	
	
	
	


	