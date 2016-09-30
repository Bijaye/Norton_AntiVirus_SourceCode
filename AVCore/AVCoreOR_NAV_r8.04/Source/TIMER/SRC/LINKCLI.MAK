#####################################################################
## LinkCli.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/TIMER/VCS/linkcli.mav   1.0   09 May 1997 17:58:44   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## client side of the CLink pair
##
#####################################################################
##
## $Log:   S:/TIMER/VCS/linkcli.mav  $
#  
#     Rev 1.0   09 May 1997 17:58:44   ksackin
#  Initial revision.
#  
#     Rev 1.1   18 Apr 1997 14:09:16   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:19:18   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:46:26   DKEEFER
#  Initial revision
#  
#     Rev 1.0   18 Jul 1996 23:41:36   DSACKIN
#  Initial revision.
##
#####################################################################

CLIENT=LinkCli
RPCFILE=Link

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
