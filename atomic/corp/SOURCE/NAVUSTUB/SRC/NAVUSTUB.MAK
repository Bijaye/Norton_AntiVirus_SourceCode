##########################################################################
# $Header:   S:/NAVUSTUB/VCS/navustub.mav   1.0   15 Oct 1998 18:03:26   mdunn  $
#
# MAKEFILE for NavUStub - stub for launching the uninstaller.
#
##########################################################################
# $Log:   S:/NAVUSTUB/VCS/navustub.mav  $
#  
#     Rev 1.0   15 Oct 1998 18:03:26   mdunn
#  Initial revision.
##########################################################################

TARGET_TYPE=EXE
USE_CRT_LIB=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

navustub.exe: navustub.obj navustub.res

#UPDATE#
#ENDUPDATE#

