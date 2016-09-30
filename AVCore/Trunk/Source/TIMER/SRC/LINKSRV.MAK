#####################################################################
## LinkSrv.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/TIMER/VCS/linksrv.mav   1.0   09 May 1997 17:58:48   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## Server side of the CLink pair
##
#####################################################################
##
## $Log:   S:/TIMER/VCS/linksrv.mav  $
#  
#     Rev 1.0   09 May 1997 17:58:48   ksackin
#  Initial revision.
#  
#     Rev 1.1   18 Apr 1997 14:09:18   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:19:20   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:46:30   DKEEFER
#  Initial revision
#  
#     Rev 1.0   18 Jul 1996 23:42:16   DSACKIN
#  Initial revision.
##
#####################################################################

SERVER=LinkSrv
RPCFILE=Link

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
