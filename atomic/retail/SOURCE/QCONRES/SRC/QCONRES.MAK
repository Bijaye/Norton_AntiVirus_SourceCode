##########################################################################
# $Header:   S:/QCONRES/VCS/qconres.mav   1.1   27 Feb 1998 15:21:38   DBuches  $
#
#
##########################################################################
# $Log:   S:/QCONRES/VCS/qconres.mav  $
#  
#     Rev 1.1   27 Feb 1998 15:21:38   DBuches
#  Removed unneccessary complier switch.
#
#     Rev 1.0   27 Feb 1998 14:54:28   DBuches
#  Initial revision.
#
##########################################################################
BUILD_MFC_EXT=TRUE
TARGET_TYPE=DLL
USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

CLOPTS+= /GX

PROGRAM_NAME=QCONRES.DLL

$(PROGRAM_NAME):        qconres.obj \
                        stdafx.obj \
                        qconres.res
#UPDATE#
#ENDUPDATE#





