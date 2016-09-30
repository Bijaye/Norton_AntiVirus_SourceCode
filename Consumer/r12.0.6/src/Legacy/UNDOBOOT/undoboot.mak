#************************************************************************
# Copyright 2000 Symantec, Peter Norton Product Group
#************************************************************************
#
#************************************************************************
#************************************************************************

PROGRAM_NAME=UNDOBOOT.EXE

MODEL=L
SYSLIBS=
NDSTRINGS=YES
LINKER=OLINK403.EXE
CLOPTS+=/Zi

.include $(QUAKEINC)\PLATFORM.MAK

TARGET_TYPE=EXE

NAV.EXE: "$(PROGRAM_NAME)"
    navcrc "$(PROGRAM_NAME)" >> MAKE.ERR
    type make.err

$(PROGRAM_NAME): UNDOBOOT.obj    \
                 syminteg.lib    \
                 certlibn.lib    \
                 callback.lib    \
                 DOSUI.LIB       \
                 DOSIO.LIB       \
                 DOSTOOL.LIB     \
                 STRINGS.LIB     \
                 FAKEVMM.LIB     \
                 $(Q_SYMKRNL).LIB 

#UPDATE#
UNDOBOOT.obj:    UNDOBOOT.c platform.h
#ENDUPDATE#
