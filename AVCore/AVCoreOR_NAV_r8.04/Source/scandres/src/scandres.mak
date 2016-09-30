##########################################################################
# $Header:   S:/SCANDRES/VCS/scandres.mav   1.0   02 Jun 1998 18:51:14   jtaylor  $
#
#
##########################################################################
# $Log:   S:/SCANDRES/VCS/scandres.mav  $
#  
#     Rev 1.0   02 Jun 1998 18:51:14   jtaylor
#  Initial revision.
#
##########################################################################
BUILD_MFC_EXT=TRUE
TARGET_TYPE=DLL
USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

CLOPTS+= /GX

PROGRAM_NAME=SCANDRES.DLL

$(PROGRAM_NAME): stdafx.obj              \
                 ScanDres.res            \
                 ScanDres.obj            \
                 ScanDres.d32
#UPDATE#
#ENDUPDATE#

