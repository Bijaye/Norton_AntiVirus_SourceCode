#######################################################################
# $Header:   S:/NAVBIN32/VCS/NAVBIN32.MAv   1.0   20 May 1997 17:44:30   DBUCHES  $
#######################################################################
# MAKEFILE for NAVBIN32.EXE
#
#
#
#######################################################################
# $Log:   S:/NAVBIN32/VCS/NAVBIN32.MAv  $
#  
#     Rev 1.0   20 May 1997 17:44:30   DBUCHES
#  Initial revision.
#######################################################################
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LINKOPTS += -SUBSYSTEM:console

LIBS += kernel32.lib
LIBS += user32.lib
LIBS += gdi32.lib
LIBS += winspool.lib
LIBS += comdlg32.lib
LIBS += advapi32.lib
LIBS += shell32.lib
LIBS += ole32.lib
LIBS += oleaut32.lib
LIBS += uuid.lib
LIBS += odbc32.lib
LIBS += odbccp32.lib

PROGRAM_NAME=NAVBIN32.EXE

$(PROGRAM_NAME): main.obj


