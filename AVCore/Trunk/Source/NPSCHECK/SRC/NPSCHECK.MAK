#########################################################################
# $Header:   S:/NPSCHECK/VCS/npscheck.mav   1.1   19 May 1998 10:10:32   mdunn  $
#
# Makefile for the NPSCheck app.
#
#########################################################################
# $Log:   S:/NPSCHECK/VCS/npscheck.mav  $
#  
#     Rev 1.1   19 May 1998 10:10:32   mdunn
#  Added npscheck.res.
#  
#     Rev 1.0   17 Apr 1998 11:36:40   mdunn
#  Initial revision.
#########################################################################

PROGRAM_NAME=NPSCheck.EXE
TARGET_TYPE=EXE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

NPSCheck.exe: npscheck.obj npscheck.res

#UPDATE#
#ENDUPDATE#

