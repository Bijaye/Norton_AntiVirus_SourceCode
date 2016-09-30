# Copyright 1999 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:
#
# Description: Makefile for Dwrite and Dwrite32
#
#************************************************************************

QUAKE_CUSTOM=

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# WIN32

%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=dwrite32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    LIBS += s32krnlo.lib
    TARGET_TYPE=exe
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX
    OBJS += dwrite.obj
    $(PROGRAM_NAME): $(OBJS) $(LIBS) dwrite.def


# DOS
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DOS")
    TARGET_NAME=dwrite
    MODEL = L
    LIBS += fakevmm.lib
    LIBS += dosio.lib
    LIBS += dosui.lib
    LIBS += dostool.lib
    LIBS += strings.lib
    LIBS += symkrnlo.lib
    TARGET_TYPE=exe
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE) 
    CLOPTS += /GX 
    OBJS += dwrite.obj
$(PROGRAM_NAME): $(OBJS) $(LIBS)
%Endif


#UPDATE#
dwrite.obj:         dwrite.cpp  dwrite.h
#ENDUPDATE#
