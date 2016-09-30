#####################################################################
## LinkSrv.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/CLINK/VCS/linksrv.mav   1.0   09 May 1997 18:35:14   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## Server side of the CLink pair
##
#####################################################################
##
## $Log:   S:/CLINK/VCS/linksrv.mav  $
#  
#     Rev 1.0   09 May 1997 18:35:14   ksackin
#  Initial revision.
#  
#     Rev 1.0   18 Apr 1997 13:46:18   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:08:56   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 14:54:38   DSACKIN
#  Initial revision.
#  
#     Rev 1.0   07 Nov 1996 14:52:56   DSACKIN
#  Initial revision.
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
