#########################################################################
# $Header:   S:/CTASKAPI/VCS/CTaskAPI.mav   1.0   06 Apr 1998 17:36:14   mdunn  $
#
# Makefile for the CTaskAPI project.  This makes a static LIB.
#
#########################################################################
# $Log:   S:/CTASKAPI/VCS/CTaskAPI.mav  $
#  
#     Rev 1.0   06 Apr 1998 17:36:14   mdunn
#  Initial revision.
#########################################################################

TARGET_TYPE=LIB
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

TARGET_NAME=CTASKAPI

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

OBJS     = ctask.obj npsstuff.obj mtsstuff.obj

LIBS     =

XOPTS    += /D_MBCS

$(PROGRAM_NAME): $(OBJS) \
                 $(LIBS)

#UPDATE#
#ENDUPDATE#
