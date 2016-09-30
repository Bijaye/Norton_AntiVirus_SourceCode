#####################################################################
## TimerCLI.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/TIMER/VCS/TAPSCLI.MAv   1.0   09 May 1997 17:58:32   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## client side of the TAPS application
##
#####################################################################
##
## $Log:   S:/TIMER/VCS/TAPSCLI.MAv  $
#  
#     Rev 1.0   09 May 1997 17:58:32   ksackin
#  Initial revision.
#  
#     Rev 1.0   06 Feb 1997 21:09:20   RFULLER
#  Initial revision
#  
#     Rev 1.0   18 Jul 1996 18:49:16   DSACKIN
#  Initial revision.
#
#     Rev 1.0   13 Jun 1996 00:32:54   DSACKIN
#  Initial revision.
##
#####################################################################

CLIENT=tapscli
RPCFILE=taps

PROGRAM_NAME=$(CLIENT).lib
TARGET_TYPE=LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

midlopts=/ms_ext
DEF+= -DUNICODE -D_UNICODE

$(CLIENT).lib: $(CLIENT).obj       \
               $(RPCFILE)_c.obj    \
               memstub.obj         \
               rpcrt4.lib          \
               rpcndr.lib          \


# Header file dependencies

$(CLIENT).obj: $(CLIENT).cpp $(CLIENT).h $(RPCFILE).h

$(RPCFILE).obj: $(RPCFILE).cpp $(RPCFILE).h

$(RPCFILE)_c.obj: $(RPCFILE)_c.c $(RPCFILE).h

$(RPCFILE)_s.obj: $(RPCFILE)_s.c $(RPCFILE).h

$(RPCFILE).h $(RPCFILE)_c.c $(RPCFILE)_s.c: $(RPCFILE).idl $(RPCFILE).acf
   MIDL $(midlopts) $(RPCFILE).idl
