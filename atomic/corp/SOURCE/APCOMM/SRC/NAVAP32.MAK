#-------------------------------------------------------------------------
#
# $Header:   S:/APCOMM/VCS/NAVAP32.MAv   1.0   16 Apr 1997 10:25:10   RStanev  $
#
# Description:
#       NAVAP32.DLL makefile
#
#-------------------------------------------------------------------------
#
# $Log:   S:/APCOMM/VCS/NAVAP32.MAv  $
#  
#     Rev 1.0   16 Apr 1997 10:25:10   RStanev
#  Initial revision.
#
#-------------------------------------------------------------------------

TARGET_NAME=NAVAP32
TARGET_TYPE=DLL
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): $(STUB)                \
                 navap32.obj            \
                 apcomm.lib             \
                 drvcomm.lib            \
                 $(Q_SYMDEBUG).LIB      \
                 navap32.res

#UPDATE#
navap32.obj:    platform.h apcomm.h
#ENDUPDATE#
