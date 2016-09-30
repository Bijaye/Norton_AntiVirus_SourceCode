#####################################################################
## TimerSRV.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/TIMER/VCS/TAPSSRV.MAv   1.0   09 May 1997 17:58:34   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## server side of the TAPS application
##
#####################################################################
##
## $Log:   S:/TIMER/VCS/TAPSSRV.MAv  $
#  
#     Rev 1.0   09 May 1997 17:58:34   ksackin
#  Initial revision.
#  
#     Rev 1.0   06 Feb 1997 21:09:20   RFULLER
#  Initial revision
#  
#     Rev 1.0   18 Jul 1996 18:50:18   DSACKIN
#  Initial revision.
#
#     Rev 1.0   13 Jun 1996 00:32:44   DSACKIN
#  Initial revision.
##
#####################################################################

SERVER=tapssrv
RPCFILE=taps

PROGRAM_NAME=$(SERVER).lib
TARGET_TYPE=LIB

MIDLOPTS=/ms_ext

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

DEF+= -DUNICODE -D_UNICODE

$(SERVER).lib: $(SERVER).obj       \
               $(RPCFILE).obj      \
               $(RPCFILE)_s.obj    \
               memstub.obj         \
               rpcrt4.lib          \
               rpcndr.lib          \

# Header file dependencies

$(SERVER).obj: $(SERVER).cpp $(SERVER).h $(RPCFILE).h

$(RPCFILE).obj: $(RPCFILE).cpp $(RPCFILE).h

$(RPCFILE)_s.obj: $(RPCFILE)_s.c $(RPCFILE).h

$(RPCFILE).h $(RPCFILE)_c.c $(RPCFILE)_s.c: $(RPCFILE).idl $(RPCFILE).acf
