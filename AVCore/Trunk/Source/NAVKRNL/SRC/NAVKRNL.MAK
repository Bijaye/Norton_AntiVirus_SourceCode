#
# $Header:   S:/NAVKRNL/VCS/navkrnl.mav   1.3   29 Apr 1997 11:29:46   JBRENNA  $
#
#=========================================================================
# $Log:   S:/NAVKRNL/VCS/navkrnl.mav  $
#  
#     Rev 1.3   29 Apr 1997 11:29:46   JBRENNA
#  Put TARGET_TYPE=VXD back in. We are getting build problems. Will see if
#  anything breaks with it in.
#  
#     Rev 1.2   28 Apr 1997 17:39:50   JBRENNA
#  Remove the TARGET_TYPE. It is not needed, hopefully.
#  
#     Rev 1.1   05 Aug 1996 11:55:30   JBRENNA
#  LuigiPlus Port: DALLEE: Added NAVKVXD.RES for version info.
#  
#=========================================================================

MODULE_NAME=NAVKVxD
TARGET_TYPE=VXD
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

Q_NAVKRNLVXD=NAVKRNL$(QUAKE_VER)

ASMOPTS+= /DMASM6

DEF+= /DSYMKRNL_MODULENAME=$(Q_NAVKRNLVXD)

LINKER=LINK386

%if "$(CV)" != ""
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512 /CO
%else
  LINKOPTS=/MAP/NOD/NOE/B/F/SEGMENTS:512
%endif

SYSLIBS=
MAP=MAPSYM

$(Q_NAVKRNLVXD).VXD: SYMKVxD.386
        COPY SYMKVxD.386 $(Q_NAVKRNLVXD).VXD
        DEL  SYMKVxD.386

SYMKVxD.386: SYMKVxD.OBJ IOSSrv.OBJ NAVKRNL.DEF NAVKVXD.RES

NAVKVXD.RES: NAVKVXD.RC

#UPDATE#
#ENDUPDATE#
