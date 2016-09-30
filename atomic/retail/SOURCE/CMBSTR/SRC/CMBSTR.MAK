########################################################################
# Copyright 1993 Symantec Corporation                                   
########################################################################
#                                                                       
# $Header:   S:/cmbstr/VCS/cmbstr.mav   1.0   06 Feb 1997 20:49:54   RFULLER  $
#                                                                       
# Description:                                                          
#   NAV Windows string routines.
#
########################################################################
# $Log:   S:/cmbstr/VCS/cmbstr.mav  $
#  
#     Rev 1.0   06 Feb 1997 20:49:54   RFULLER
#  Initial revision
#  
#     Rev 1.0   30 Jan 1996 15:48:44   BARRY
#  Initial revision.
#  
#     Rev 1.0   30 Jan 1996 09:55:22   BARRY
#  Initial revision.
#  
#     Rev 1.10   25 Apr 1994 12:16:22   DALLEE
#  Compile objects using the DLL flags since this static lib will be included
#  in dll's.
########################################################################

TARGET_NAME=CMBSTR
TARGET_TYPE=DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB


$(PROGRAM_NAME):    cmblib.obj

#UPDATE#
cmblib.obj:	platform.h xapi.h file.h cts4.h cmbstr.h navutil.h \
		options.h navopts.h exclude.h netobj.h symnet.h register.h \
		symcfg.h symalog.h 
#ENDUPDATE#
