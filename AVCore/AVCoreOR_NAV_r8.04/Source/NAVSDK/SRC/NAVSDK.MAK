# Copyright 1997 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVSDK/VCS/NAVSDK.MAv   1.8   31 Aug 1998 18:42:10   jsulton  $
#
# Description:
#    Make file for NAVSDK
#
#************************************************************************
# $Log:   S:/NAVSDK/VCS/NAVSDK.MAv  $
#  
#     Rev 1.8   31 Aug 1998 18:42:10   jsulton
#  We no longer run EXECRC for the ARA and ADA platforms.
#  
#     Rev 1.7   09 Dec 1997 18:44:06   DALLEE
#  Changed name of AVAPI lib from N32CORES.LIB to SDKAVAPI.LIB which
#  is a locally compiled version which does not export the VLxxx() functions.
#  
#     Rev 1.6   08 Dec 1997 16:21:04   DALLEE
#  Added SELFTEST.OBJ for local "C" self test code.
#  Updated some dependencies for new modules.
#  
#     Rev 1.5   04 Dec 1997 16:15:42   DALLEE
#  Added context.obj.
#  
#     Rev 1.4   04 Dec 1997 08:35:36   tcashin
#  Changed the DLL name to NAVSDK32.
#  
#     Rev 1.3   02 Dec 1997 15:13:16   DALLEE
#  Slight change to .CRC name for consistency with DLL name.
#  
#     Rev 1.2   02 Dec 1997 14:39:54   DALLEE
#  Fat finger on last checkin. Had too many _'s on /D_NAVOEM_.
#  
#     Rev 1.1   02 Dec 1997 14:36:20   DALLEE
#  Changed all NAVOEMs to NAVSDKs.
#  
#     Rev 1.0   02 Dec 1997 14:29:34   DALLEE
#  Initial revision.
#************************************************************************

TARGET_TYPE=DLL
QUAKE_CUSTOM=
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

KRNL_LANG=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
KRNL_SRC=$(QUAKEPRE)\SYMKRNL\SRC$(QUAKE)

RCOPTS+=/i$(KRNL) /DQCUST
DEF+= /D_NAVSDK_

%if ( "$(CORE_WIN32)" != "" ) && ( "$(CORE_SOFT)" == "" )

    TARGET_NAME=NAVSDK32
    PROGRAM_NAME=$(TARGET_NAME).DLL
    DEFFILE=NAVSDK.D32
    DEF+= /DQCUSTNOX /DQCUST_NOVXD_EMSG /DQCUST

%else

BAD_PLATFORM:
    beep
    echo error: The "$(PLATFORM)" platform is not valid for the $(PROJNAME) project. >> make.err
    echo error: The "$(PLATFORM)" platform is not valid for the $(PROJNAME) project.

%endif

%if ("$(PLATFORM)" != ".ARA") && ("$(PLATFORM)" != ".ADA")

$(TARGET_NAME).CRC: $(PROGRAM_NAME)
    iff not exist S:\TOOLS\EXECRC.EXE then
        beep
        echo error: EXECRC.EXE not found. >> make.err
        echo error: EXECRC.EXE not found.
    else
        iff exist $(PROGRAM_NAME) then
            echo execrc $(PROGRAM_NAME) >> make.err
            echo execrc $(PROGRAM_NAME)
            execrc $(PROGRAM_NAME)
            echo $(PROGRAM_NAME) stamped %_time %_date > $(TARGET_NAME).CRC
        endiff
    endiff

%endif

$(PROGRAM_NAME): \
    navsdk.obj   \
    selftest.obj \
    device.obj   \
    engcbk.obj   \
    context.obj  \
    winmisc.obj  \
    navsdk.res   \
    $(DEFFILE)   \
    \
    SDKAVAPI.LIB \
    NAVDEF32.LIB \
    DRVCOMM.LIB  \
     symkrnl.obj  \
     symcfg.obj   \
     cfg_bloc.obj \
     cfg_ini.obj  \
     cfg_reg.obj  \
     cfg_swit.obj \
     cfg_text.obj \
     dbcs.obj     \
     prm_date.obj \
     prm_dos.obj  \
     prm_mem.obj  \
     prm_misc.obj \
     prm_str.obj  \
     prm_strd.obj \
     prm_szz.obj  \
     prm_crpt.obj \
     dsk_accs.obj \
     dsk_bpb.obj  \
     dsk_cach.obj \
     dsk_misc.obj \
     dsk_type.obj \
     bufferio.obj \
     fop_dir.obj  \
     fop_dir2.obj \
     fop_disk.obj \
     fop_file.obj \
     fop_win.obj  \
     findfirs.obj \
     fop_dos.obj  \
     symintl.obj  \
     name.obj     \
     name_win.obj \
     wildcard.obj \
     hstring.obj  \
     version.obj  \
     prm_hard.obj \
     dsk_abs.obj  \
     dsk_bloc.obj \
     dsk_int.obj  \
     dsk_gen.obj  \
     filetime.obj \
     nec.obj      \
     dsk_drvr.obj \
     dsk_stac.obj \
     vxdload.obj  \
     prm_win.obj  \
     badchar.obj  \
     hugeoem.obj  \
     symasser.obj


NAVSDK.RES: \
    navsdk.rc \
	sdkver.h \
    $(KRNL_LANG)\symkrnl.rc \
    $(KRNL_LANG)\dialogs.dlg \
    dialogs.h \
    symkrnli.h


#UPDATE#
navsdk.obj:     platform.h xapi.h file.h disk.h avapi.h navsdk.h oemapi.h
selftest.obj:   platform.h navsdk.h
device.obj:     platform.h disk.h navsdk.h oemapi.h
engcbk.obj:     platform.h disk.h avapi.h callbk15.h avcb.h navcb.h oemapi.h
context.obj:    platform.h navsdk.h oemapi.h
vxdwraps.obj:   platform.inc ios.inc ifsmgr.inc
vxdstat.obj:    platform.h xapi.h
vsymkrnl.obj:   platform.inc vsymkrnl.inc
services.obj:   platform.inc ios.inc blockdev.inc vsymkrnl.inc
iossrv.obj:     platform.inc blockdev.inc ior.inc iop.inc aep.inc isp.inc \
                ios.inc drp.inc ilb.inc dcb.inc sgd.inc ida.inc symkvxd.inc
rtl32.obj:      platform.h
bufferio.obj:   platform.h stddos.h ibmpc.h file.h
cfg_bloc.obj:   platform.h file.h xapi.h symcfg.h
cfg_ini.obj:    platform.h file.h xapi.h symcfg.h
cfg_reg.obj:    platform.h file.h xapi.h symcfg.h
cfg_swit.obj:   platform.h file.h xapi.h symcfg.h
cfg_text.obj:   platform.h file.h xapi.h symcfg.h
dbcs.obj:       platform.h file.h xapi.h dbcs.h
dsk_abs.obj:    platform.h disk.h xapi.h thunk.h
dsk_accs.obj:   platform.h disk.h xapi.h file.h dos.h
dsk_bloc.obj:   platform.h disk.h xapi.h symcfg.h thunk.h
dsk_bpb.obj:    platform.h disk.h xapi.h
dsk_cach.obj:   platform.h xapi.h disk.h
dsk_drvr.obj:   platform.h undoc.h file.h disk.h xapi.h
dsk_gen.obj:    platform.h file.h disk.h xapi.h
dsk_int.obj:    platform.h disk.h xapi.h thunk.h
dsk_misc.obj:   platform.h xapi.h thunk.h disk.h file.h
dsk_type.obj:   platform.h xapi.h symcfg.h file.h disk.h dbcs.h
filetime.obj:   platform.h xapi.h file.h symkrnl.h
findfirs.obj:   platform.h xapi.h file.h undoc.h symkrnl.h
fop_dir.obj:    platform.h undoc.h file.h disk.h xapi.h
fop_dir2.obj:   platform.h file.h xapi.h
fop_disk.obj:   platform.h disk.h undoc.h file.h xapi.h
fop_dos.obj:    platform.h file.h xapi.h
fop_file.obj:   platform.h xapi.h file.h disk.h symkrnl.h undoc.h
fop_win.obj:    platform.h file.h
hstring.obj:    platform.h undoc.h hstring.h
hugeoem.obj:    platform.h hugeoem.h
name.obj:       platform.h xapi.h file.h disk.h symkrnl.h dbcs.h
name_win.obj:   platform.h file.h disk.h xapi.h symkrnl.h dbcs.h
prm_crpt.obj:   platform.h xapi.h
prm_date.obj:   platform.h undoc.h xapi.h
prm_dos.obj:    platform.h xapi.h file.h
prm_hard.obj:   platform.h undoc.h disk.h xapi.h symevent.h symevnt.ver \
                version.h quakever.h dialogs.h thunk.h
prm_mach.obj:   platform.h xapi.h stddos.h ibmpc.h hardware.h
prm_mem.obj:    platform.h xapi.h symkrnl.h file.h thunk.h disk.h \
                dosvmm.h
prm_misc.obj:   platform.h undoc.h xapi.h
prm_sft.obj:    platform.h xapi.h disk.h file.h symevent.h symevnt.ver \
                version.h quakever.h stddos.h ibmpc.h
prm_str.obj:    platform.h xapi.h symintl.h symkrnli.h stddos.h ibmpc.h
prm_strd.obj:   platform.h xapi.h symintl.h symkrnli.h stddos.h ibmpc.h
prm_szz.obj:    platform.h xapi.h
prm_win.obj:    platform.h xapi.h disk.h
symcfg.obj:     platform.h file.h xapi.h symcfg.h disk.h undoc.h
symintl.obj:    platform.h xapi.h stddos.h ibmpc.h symkrnli.h symintl.h \
                dbcs.h
symkrnl.obj:    platform.h disk.h xapi.h symkrnl.h file.h thunk.h
tsr.obj:        platform.h tsr.h
version.obj:    platform.h version.h quakever.h dialogs.h symkrnli.h
cvtft.obj:      mprec.inc
int13dos.obj:   struct.inc types.inc
seterror.obj:   mixed.inc
tsr_asm.obj:    mixed.inc
badchar.obj:    platform.h xapi.h
#ENDUPDATE#
