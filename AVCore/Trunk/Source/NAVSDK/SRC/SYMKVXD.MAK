# Copyright 1996 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVSDK/VCS/symkvxd.mav   1.0   02 Dec 1997 14:58:16   DALLEE  $
#
# Description:
#    Make file for NAVOEM.VXD, the privatized version of SYMKVxD.
#
#************************************************************************
# $Log:   S:/NAVSDK/VCS/symkvxd.mav  $
#  
#     Rev 1.0   02 Dec 1997 14:58:16   DALLEE
#  Initial revision.
#  
#     Rev 1.0   31 May 1996 10:19:38   TCASHIN
#  Initial revision.
#************************************************************************
VXD_NAME=NAVSDK
PROGRAM_NAME=$(VXD_NAME).VXD
MODULE_NAME=SYMKVxD
TARGET_TYPE=VXD
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

ASMOPTS+= /DMASM6

DEF+= /DSYMKRNL_MODULENAME=$(VXD_NAME) 
RCOPTS=/DQ_VER=\"$(QUAKE_VER)\"

LINKER=LINK386

%if "$(CV)" != ""
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512 /CO
%else
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512
%endif

SYSLIBS=
MAP=MAPSYM

RESFILE = symkvxd.res

NAVSDK.CRC : $(VXD_NAME).VXD
    iff not exist S:\TOOLS\EXECRC.EXE then
        beep
        echo error: EXECRC.EXE not found. >> make.err
        echo error: EXECRC.EXE not found.
    else
        iff exist $(PROGRAM_NAME) then
            echo execrc $(PROGRAM_NAME) >> make.err
            echo execrc $(PROGRAM_NAME)
            execrc $(PROGRAM_NAME)
            echo $(PROGRAM_NAME) stamped %_time %_date > NAVSDK.CRC
        endiff
    endiff

$(PROGRAM_NAME): $(VXD_NAME).386
        COPY $(VXD_NAME).386 $(VXD_NAME).VXD
        DEL  $(VXD_NAME).386

$(VXD_NAME).386: $(MODULE_NAME).OBJ IOSSrv.OBJ $(RESFILE) $(MODULE_NAME).DEF

$(RESFILE):    symkvxd.rc

#UPDATE#
#ENDUPDATE#
