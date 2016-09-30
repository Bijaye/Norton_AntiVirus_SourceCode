# Copyright 1996 Symantec Corp. / Peter Norton Product Group
#************************************************************************
#
# Description:
#    Make file for sample application.
#
#************************************************************************

PROGRAM_NAME=N32DEMO.EXE

TARGET_TYPE=EXE
QUAKE_CUSTOM=
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS= NAVSDK32.LIB NAVDEF32.LIB

NAVDEMO.CRC:     $(PROGRAM_NAME)
                 if exist $(PROGRAM_NAME) execrc $(PROGRAM_NAME)
                        
$(PROGRAM_NAME):\
    navdemo.obj \
    navdwnd.obj \
    navdemo.def \
    navdemo.res \


NAVDEMO.RES :   \
    navdemo.rc  \
    resource.h   \
    navdemo.ico


#UPDATE#
#ENDUPDATE#
