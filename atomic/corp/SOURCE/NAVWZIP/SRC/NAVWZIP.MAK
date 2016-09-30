#-------------------------------------------------------------------------
# $Header:   S:/NAVWZIP/VCS/navwzip.mav   1.2   11 Aug 1997 00:51:54   KSACKIN  $
#                                                                       
#-------------------------------------------------------------------------
# $Log:   S:/NAVWZIP/VCS/navwzip.mav  $
#  
#     Rev 1.2   11 Aug 1997 00:51:54   KSACKIN
#  Removed the structured exception handling from this DLL.  We are now going to
#  handle this one level higher in NavWork so that we will be able to completely
#  reload the NavZip dll when a crash occurs.  This is so that we can have a
#  little more confidence in the state of the Zip scanning engine after it
#  crashes.
#  
#     Rev 1.0   06 Feb 1997 21:04:48   RFULLER
#  Initial revision
#  
#     Rev 1.0   30 Jan 1996 15:56:08   BARRY
#  Initial revision.
#  
#     Rev 1.0   30 Jan 1996 14:51:08   BARRY
#  Initial revision.
#  
#     Rev 1.7   13 Jun 1995 14:24:00   MARKL
#  Added resource file.
#  
#     Rev 1.6   24 May 1995 14:30:04   BARRY
#  Now use BVZIP.LIB instead of N32FSV.LIB
#  
#     Rev 1.5   25 Apr 1995 20:59:22   DALLEE
#  Use the REAL name for SYMKRNL -- $(Q_SYMKRNL) goes to our custom DLL.
#  
#     Rev 1.4   15 Mar 1995 10:54:54   SZIADEH
#  Changed the zip engine name from NAVFSV to n32fsv.
#  
#     Rev 1.3   13 Mar 1995 22:03:52   SZIADEH
#  use the correct name for ZIP engine and SYMKRNL.
#
#     Rev 1.2   07 Mar 1995 22:21:24   MFALLEN
#  Temporarely commented out DLL's to solve symkrnl dependency issues.
#  
#     Rev 1.1   28 Feb 1995 16:55:08   SZIADEH
#  added the def file.
#  
#     Rev 1.0   24 Feb 1995 17:00:04   SZIADEH
#  Initial revision.
#-------------------------------------------------------------------------

TARGET_NAME=n32zip
TARGET_TYPE=DLL

PROGRAM_NAME=$(TARGET_NAME).DLL

DEF_FILE=NAVWZIP.DEF

ZIPENGINE=BVZIP

OBJS = navwzip.obj

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):   \
 $(OBJS)           \
 navwzip.res       \
 $(DEF_FILE)       \
 $(ZIPENGINE).LIB  \
 $(Q_SYMKRNL).LIB  \
 $(Q_SYMDEBUG).LIB

create_implib: $(TARGET_NAME).lib

$(TARGET_NAME).lib: $(DEF_FILE)

#UPDATE#

navwzip.obj : navwzip.cpp wnzip.h symzip.h navwzip.h wnfsv.h

#ENDUPDATE#

