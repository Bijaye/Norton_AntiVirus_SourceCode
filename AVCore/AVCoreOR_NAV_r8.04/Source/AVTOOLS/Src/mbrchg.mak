# Copyright 1999 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:
#
# Description: Makefile for MBRCHG and MBRCHG32
#
#************************************************************************

QUAKE_CUSTOM=

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# WIN32

%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=MBRCHG32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    LIBS += s32krnlo.lib
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX
    OBJS += mbrchg.obj
    $(PROGRAM_NAME): $(OBJS) $(LIBS) mbrchg.def


# DOS
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DOS")
    TARGET_NAME=MBRCHG
    MODEL = L
    LIBS += fakevmm.lib
    LIBS += dosio.lib
    LIBS += dosui.lib
    LIBS += dostool.lib
    LIBS += strings.lib
    LIBS += symkrnlo.lib
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE) 
    CLOPTS += /GX 
    OBJS += mbrchg.obj
$(PROGRAM_NAME): $(OBJS) $(LIBS)
%Endif


#UPDATE#
mbrchg.obj:         mbrchg.cpp  mbrchg.h
#ENDUPDATE#
