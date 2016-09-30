# Copyright 1996 Symantec Corp. / Peter Norton Product Group
#************************************************************************
#
# Description:
#    Make file for definst application.
#
#************************************************************************

PROGRAM_NAME=DEFINST.EXE

TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS= NAVDEF32.LIB
LINKOPTS += -SUBSYSTEM:CONSOLE

DEFINST.CRC:     $(PROGRAM_NAME)
                 if exist $(PROGRAM_NAME) execrc $(PROGRAM_NAME)
                        
$(PROGRAM_NAME):\
    definst.obj 

#UPDATE#
#ENDUPDATE#
