#####################################################################
## APSCli.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/CLINK/VCS/apscli.mav   1.0   09 May 1997 18:35:24   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## client side of the CNAVAPSVC RPC pair
##
#####################################################################
##
## $Log:   S:/CLINK/VCS/apscli.mav  $
#  
#     Rev 1.0   09 May 1997 18:35:24   ksackin
#  Initial revision.
#  
#     Rev 1.0   18 Apr 1997 13:46:32   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:08:58   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 14:54:46   DSACKIN
#  Initial revision.
#  
#     Rev 1.0   07 Nov 1996 19:00:18   DSACKIN
#  Initial revision.
#  
#     Rev 1.0   19 Jul 1996 00:44:04   DSACKIN
#  Initial revision.
##
#####################################################################

CLIENT=APSCli

PROGRAM_NAME=$(CLIENT).lib
TARGET_TYPE=LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

midlopts=/ms_ext
DEF+= -DUNICODE -D_UNICODE

$(CLIENT).lib: $(CLIENT).obj

# Header file dependencies

$(CLIENT).obj: $(CLIENT).cpp $(CLIENT).h

