# Copyright 1995 Symantec, Peter Norton Product Group
########################################################################
#									
# $Header:   S:/BVZIP/VCS/BVZIP.MAv   1.1   22 May 1997 13:33:14   BILL  $
#									
# Description:								
#       This is the make file for the Beaverton File Services DLL.
#       It supports ZIP and other decompression engines for NAV.
########################################################################
# $Log:   S:/BVZIP/VCS/BVZIP.MAv  $
#  
#     Rev 1.1   22 May 1997 13:33:14   BILL
#  Created stubs for alpha
#  
#     Rev 1.0   06 Feb 1997 20:49:30   RFULLER
#  Initial revision
#  
#     Rev 1.0   30 Jan 1996 15:48:00   BARRY
#  Initial revision.
#  
#     Rev 1.0   30 Jan 1996 09:54:38   BARRY
#  Initial revision.
#  
#     Rev 1.4   22 May 1995 17:11:04   BARRY
#  Removed comment
#  
#     Rev 1.3   22 May 1995 15:26:18   BARRY
#  Removed obsolete DLL target reference
#  
#     Rev 1.2   22 May 1995 10:03:22   BARRY
#  Now builds a 32-bit LIB (don't really need a DLL for this)
#  
#     Rev 1.1   16 May 1995 15:02:20   BARRY
#  Removed resource from dependencies
#  
#     Rev 1.0   08 May 1995 22:45:36   BARRY
#  Initial revision.
########################################################################

TARGET_NAME=BVZIP
TARGET_TYPE=LIB
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

DEF += /DNAV95 /DSYM_BUILD_SM

FSVOBJS= \
 pmemlist.obj

STUBOBJS = \
 navstub.obj

ZIPOBJS= \
 compress.obj \
 crc32.obj \
 dclzh45.obj \
 enlzh45.obj \
 expand.obj \
 explode.obj \
 fatten.obj \
 lzhutil.obj \
 unarc.obj \
 unshrink.obj \
 utility.obj \
 wnzip.obj \
 zip1xexp.obj \
 zip2xcod.obj \
 zip2xcom.obj \
 zip2xexp.obj \
 zip2xio.obj \
 zip2xshr.obj \
 z_shrink.obj 

NOBUILDS = \
 dilute.obj \
 distill.obj \
 implode.obj


%If "$(CORE_PLAT)" == "I"
$(PROGRAM_NAME): \
 $(ZIPOBJS) \
 $(FSVOBJS) \
 $(STUBOBJS) \
 $(Q_SYMKRNL).LIB
%Else
$(PROGRAM_NAME): \
 wnzip.obj \
%Endif

#UPDATE#
#ENDUPDATE#
