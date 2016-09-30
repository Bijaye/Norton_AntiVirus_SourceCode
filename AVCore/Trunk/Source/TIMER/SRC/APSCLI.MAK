#####################################################################
## APSCli.mak
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/TIMER/VCS/apscli.mav   1.0   09 May 1997 17:58:52   ksackin  $
##
## This makefile contains the instructions for building ONLY the
## client side of the CNAVAPSVC RPC pair
##
#####################################################################
##
## $Log:   S:/TIMER/VCS/apscli.mav  $
#  
#     Rev 1.0   09 May 1997 17:58:52   ksackin
#  Initial revision.
#  
#     Rev 1.1   18 Apr 1997 14:09:22   DSACKIN
#  Ported from Tesla
#  
#     Rev 1.0   13 Mar 1997 15:19:20   RFULLER
#  Initial revision
#  
#     Rev 1.0   08 Nov 1996 11:46:34   DKEEFER
#  Initial revision
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

