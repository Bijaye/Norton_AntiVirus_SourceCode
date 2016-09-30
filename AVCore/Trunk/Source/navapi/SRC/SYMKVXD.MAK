# Copyright 1996 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVAPI/VCS/SYMKVXD.MAv   1.3   23 Jun 1998 02:53:38   ksackin  $
#
# Description:
#    Make file for NAVOEM.VXD, the privatized version of SYMKVxD.
#
#************************************************************************
# $Log:   S:/NAVAPI/VCS/SYMKVXD.MAv  $
#  
#     Rev 1.3   23 Jun 1998 02:53:38   ksackin
#  Reworked some of the defines to make a little more sense.  Also, juggled some
#  of the defines so that the linker would work correcly.
#
#     Rev 1.2   15 Jun 1998 20:50:50   MKEATIN
#  remove the resource guy for now.
#
#     Rev 1.1   11 Jun 1998 00:09:26   DALLEE
#
#     Rev 1.0   26 May 1998 19:03:56   MKEATIN
#  Initial revision.
#
#     Rev 1.0   02 Dec 1997 14:58:16   DALLEE
#  Initial revision.
#
#     Rev 1.0   31 May 1996 10:19:38   TCASHIN
#  Initial revision.
#************************************************************************
MODULE_NAME=NAVAPI
TARGET_TYPE=VXD
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

ASMOPTS+= /DMASM6

DEF+= /DSYMKRNL_MODULENAME=$(MODULE_NAME)

LINKER=LINK386

%if "$(CV)" != ""
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512 /CO
%else
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512
%endif

SYSLIBS=
MAP=MAPSYM

RESFILE = napivxd.res

NAVAPI.CRC : $(MODULE_NAME).VXD
    iff not exist S:\TOOLS\EXECRC.EXE then
        beep
        echo error: EXECRC.EXE not found. >> make.err
        echo error: EXECRC.EXE not found.
    else
        iff exist $(MODULE_NAME).VXD then
            echo execrc $(MODULE_NAME).VXD >> make.err
            echo execrc $(MODULE_NAME).VXD
            execrc $(MODULE_NAME).VXD
            echo $(MODULE_NAME).VXD stamped %_time %_date > NAVAPI.CRC
        endiff
    endiff

$(MODULE_NAME).VXD: $(MODULE_NAME).386
        COPY $(MODULE_NAME).386 $(MODULE_NAME).VXD
        DEL $(MODULE_NAME).386


$(MODULE_NAME).386: \
    symkvxd.obj \
    IOSSrv.OBJ \
    $(RESFILE) \
    symkvxd.def

$(RESFILE):    napivxd.rc

#UPDATE#
#ENDUPDATE#
