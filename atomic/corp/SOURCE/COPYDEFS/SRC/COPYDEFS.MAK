##########################################################################
# $Header:   S:/COPYDEFS/VCS/copydefs.mav   1.1   17 Mar 1998 10:37:12   tcashin  $
#
# MAKEFILE for CopyDefs - rescue definitions disk maker for Roswell 95 NEC
#
##########################################################################
# $Log:   S:/COPYDEFS/VCS/copydefs.mav  $
#  
#     Rev 1.1   17 Mar 1998 10:37:12   tcashin
#  Added patch32i.lib and chan32i.lib.
#  
#     Rev 1.0   19 Feb 1998 10:11:36   mdunn
#  Initial revision.
#  
#     Rev 1.1   01 Oct 1997 10:24:48   mdunn
#  Added CONSOLE_MODE=YES line since this is, after all, a console mode app.
#  
#     Rev 1.0   25 Sep 1997 19:12:20   mdunn
#  Initial revision.
##########################################################################

TARGET_TYPE=EXE
PROGRAM_NAME=CopyDefs.exe
CONSOLE_MODE=YES
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS += defutils.lib
LIBS += patch32i.lib
LIBS += chan32i.lib

$(PROGRAM_NAME):    utils.obj copydefs.obj copydefs.res

#UPDATE#
#ENDUPDATE#





