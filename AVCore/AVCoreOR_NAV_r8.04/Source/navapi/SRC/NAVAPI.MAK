#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAPI/VCS/navapi.mav   1.21   21 Oct 1998 13:11:10   MKEATIN  $
#
# Description: NAVAPI makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVAPI/VCS/navapi.mav  $
#
#     Rev 1.21   21 Oct 1998 13:11:10   MKEATIN
#  Added the -ALfw switch for DX
#
#     Rev 1.20   26 Aug 1998 16:06:36   DWILSON
#  Fixed a typo.
#
#     Rev 1.19   26 Aug 1998 13:48:46   NMILLER
#  Added vxdload.obj so we disable the message box if we are missing our VxD
#
#     Rev 1.18   11 Aug 1998 14:17:36   MKEATIN
#
#     Rev 1.17   11 Aug 1998 13:32:48   MKEATIN
#  Added the /ALfw switch so we don't assume DS == SS.
#
#     Rev 1.16   05 Aug 1998 15:26:48   MKEATIN
#  Added Win16 platform
#
#     Rev 1.15   04 Aug 1998 15:28:56   DHERTEL
#  Added info to DX build
#
#     Rev 1.14   30 Jul 1998 21:52:42   DHERTEL
#  Changes for NLM packaging of NAVAPI.
#
#     Rev 1.13   29 Jul 1998 18:33:46   DHERTEL
#  DX and NLM changes
#
#     Rev 1.12   22 Jun 1998 22:28:40   DALLEE
#  Added APENABLE.OBJ
#
#     Rev 1.11   22 Jun 1998 21:27:22   ksackin
#  Added navapi.res to the build dependencies for Win32 and Win16.  This
#  resource includes version information for the DLL's we produce.
#
#     Rev 1.10   17 Jun 1998 20:29:06   ksackin
#  Added the /DQCUSTNOX switch to disable exporting of QUAKE API's.  This will
#  cut down our exports to only the exports we want in this API.
#
#     Rev 1.9   11 Jun 1998 17:36:30   DALLEE
#  Added file.obj (NAVScanFile) and virtcb.obj (virutal file IO covers).
#
#     Rev 1.8   01 Jun 1998 22:16:12   ksackin
#  Added info.obj for virus information API's.
#
#     Rev 1.7   29 May 1998 13:30:16   MKEATIN
#  Added memory.obj
#
#     Rev 1.6   27 May 1998 20:38:46   MKEATIN
#  Updated dependancies
#
#     Rev 1.5   27 May 1998 19:40:24   MKEATIN
#  Defining _NAVAPIDLL_ for WIN32
#
#     Rev 1.4   27 May 1998 16:26:26   MKEATIN
#  Added boot.obj (a new NAVAPI boot.obj)
#
#     Rev 1.3   27 May 1998 13:41:40   MKEATIN
#  removed AVAPI1's boot.obj
#
#     Rev 1.2   26 May 1998 19:13:40   MKEATIN
#  Added init.cpp
#
#     Rev 1.1   22 May 1998 20:36:40   MKEATIN
#  Added navcb.obj
#
#     Rev 1.0   21 May 1998 17:10:40   MKEATIN
#  Initial revision.
#-------------------------------------------------------------------------

# WIN16
# -----

%If ("$(CORE_WIN16)" != "")
    MODEL=L
    TARGET_NAME=NAVAPI16
    TARGET_TYPE=DLL
    DEF += /D_NAVAPIDLL_ /DQCUSTNOX
    XOPTS += /ALfw
    RCBINDOPTS = -k -t -31

OBJS=            \
    memory.obj   \
    info.obj     \
    boot.obj     \
    file.obj     \
    init.obj     \
    navcb.obj    \
    virtcb.obj   \
    avapi.obj    \
    static.obj   \
    cpu.obj      \
    datafile.obj \
    exclude.obj  \
    exec.obj     \
    flags.obj    \
    instr.obj    \
    instr2.obj   \
    page.obj     \
    pamapi.obj   \
    repair.obj   \
    search.obj   \
    sigex.obj    \
    cover.obj    \
    cache.obj    \
    symkrnl.obj  \
    symcfg.obj   \
    cfg_bloc.obj \
    cfg_ini.obj  \
    cfg_prof.obj \
    cfg_reg.obj  \
    cfg_swit.obj \
    cfg_text.obj \
    dbcs.obj     \
    prm_date.obj \
    prm_dos.obj  \
    prm_hard.obj \
    prm_mach.obj \
    prm_mem.obj  \
    prm_misc.obj \
    prm_str.obj  \
    prm_strd.obj \
    prm_szz.obj  \
    prm_win.obj  \
    prm_lstr.obj \
    prm_err.obj  \
    prm_sft.obj  \
    prm_crpt.obj \
    dsk_abs.obj  \
    dsk_accs.obj \
    dsk_bloc.obj \
    dsk_bpb.obj  \
    dsk_cach.obj \
    dsk_drvr.obj \
    dsk_gen.obj  \
    dsk_int.obj  \
    dsk_misc.obj \
    dsk_type.obj \
    dskfat32.obj \
    int13win.obj \
    bufferio.obj \
    fop_win.obj  \
    fop_dir.obj  \
    fop_dir2.obj \
    fop_disk.obj \
    fop_file.obj \
    filetime.obj \
    cvtft.obj    \
    findfirs.obj \
    fop_dos.obj  \
    symintl.obj  \
    name.obj     \
    name_win.obj \
    wildcard.obj \
    tsr.obj      \
    tsr_asm.obj  \
    hstring.obj  \
    romcopy.obj  \
    version.obj  \
    hugeoem.obj  \
    nec.obj      \
    badchar.obj  \
    dsk_stac.obj


LIBS=            \
%if "$(CORE_BMODE)"=="D"
    $(Q_SYMDEBUG).LIB\
%endif
    stackerw.lib \
    AVENGE.LIB

RES =            \
    NAVAPI.RES

DEFFILE =        \
    NAVAPI.DEF


# WIN32
# -----

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=NAVAPI32
    TARGET_TYPE=DLL
    DEF += /D_NAVAPIDLL_ /DQCUSTNOX

# If BOUNDS_CHECKER is set to non-"", we compile with BoundsChecker
# instrumentation. Remember to have NMCL.exe and NMLINK.exe from
# BoundsChecker in the PATH. Also, BCINTERF.LIB must be in the LIB
# path.
%if ("$(BOUNDS_CHECKER)" != "")
    CC=NMCL
    LINK=NMLINK
    BOUNDS_CHECKER_LIB=BCINTERF.LIB
%else
    BOUNDS_CHECKER_LIB=
%endif

OBJS=            \
    memory.obj   \
    info.obj     \
    boot.obj     \
    file.obj     \
    init.obj     \
    navcb.obj    \
    virtcb.obj   \
    apenable.obj \
    avapi.obj    \
    static.obj   \
    cpu.obj      \
    datafile.obj \
    exclude.obj  \
    exec.obj     \
    flags.obj    \
    instr.obj    \
    instr2.obj   \
    page.obj     \
    pamapi.obj   \
    repair.obj   \
    search.obj   \
    sigex.obj    \
    cover.obj    \
    cache.obj    \
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

LIBS=            \
    $(BOUNDS_CHECKER_LIB) \
    AVENGE.LIB   \
    DRVCOMM.LIB  \

RES = NAVAPI.RES


# TENBERRY DX16
# -------------

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    TARGET_NAME=NAVAPI
    TARGET_TYPE=LIB
    XOPTS += -ALfw

OBJS=            	\
    avapi.obj		\
    boot.obj		\
    cache.obj		\
    cover.obj		\
    cpu.obj		\
    datafile.obj	\
    exclude.obj		\
    exec.obj		\
    file.obj		\
    flags.obj		\
    info.obj		\
    init.obj		\
    instr.obj		\
    instr2.obj		\
    memory.obj		\
    navcb.obj		\
    page.obj		\
    pamapi.obj		\
    repair.obj		\
    search.obj		\
    sigex.obj		\
    static.obj		\
    virtcb.obj          \
    dsk_abs.obj

LIBS=            	\
    AVENGE.LIB   	\
    $(Q_SYMKRNL).LIB 	\
    $(DOSVMM) 		\
    dosio.lib 		\
    dosui.lib 		\
    dostool.lib 	\
    strings.lib


# NLM
# ---

%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")
    TARGET_NAME=NAVAPI
    TARGET_TYPE=LIB
    DEF += /DNLM_NAVAPI

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS=            	\
    avapi.obj		\
    boot.obj		\
    cache.obj		\
    cover.obj		\
    cpu.obj		\
    datafile.obj	\
    exclude.obj		\
    exec.obj		\
    file.obj		\
    flags.obj		\
    info.obj		\
    init.obj		\
    instr.obj		\
    instr2.obj		\
    memory.obj		\
    navcb.obj		\
    nlmstuff.obj        \
    page.obj		\
    pamapi.obj		\
    repair.obj		\
    search.obj		\
    sigex.obj		\
    static.obj		\
    virtcb.obj

#   apenable.obj	\

LIBS=            	\
    AVENGE.LIB


# VXD
# ---

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")
    TARGET_NAME=NAVAPI
    TARGET_TYPE=LIB

OBJS=            \
    navapi.obj   \
    navexapi.obj \
    avapistr.obj

LIBS=            \
    AVENGE.LIB


# NTK
# ---

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")
    TARGET_NAME=NAVAPI
    TARGET_TYPE=LIB

XOPTS += /Gs10000

OBJS=            \
    navapi.obj   \
    navexapi.obj

LIBS=            \
    AVENGE.LIB


# UNSUPPORTED
# -----------

%Else
BAD_PLATFORM:
    beep
    Echo '$(PLATFORM)' is not yet supported.

%EndIf


# ALL PLATFORMS
# -------------

%If ("$[u,$(PLATFORM)]" != ".NLM")
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
%Endif

%If "$(PLATFORM)" != ".OS2"
.INCLUDE <ML.MAK>
%Endif

# Win32 only
%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    LINKOPTS += /LINK50COMPAT
%endif

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

KRNL_LANG=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
KRNL_SRC=$(QUAKEPRE)\SYMKRNL\SRC$(QUAKE)

$(PROGRAM_NAME): $(OBJS) $(LIBS) $(RES) $(DEFFILE)

#UPDATE#
navcb.obj:      platform.h disk.h file.h navapi.h avapi_l.h virtcb.h
memory.obj:     platform.h navapi.h avapi_l.h apenable.h
file.obj:       platform.h navapi.h avapi_l.h virtcb.h apenable.h
virtcb.obj:     platform.h navapi.h avapi_l.h virtcb.h
info.obj:       platform.h navapi.h avapi_l.h napiver.h
init.obj:       platform.h dbcs_str.h xapi.h avapi_l.h navapi.h navcb_l.h apenable.h
boot.obj:       platform.h dbcs_str.h xapi.h disk.h avapi_l.h navapi.h apenable.h
apenable.obj:   platform.h xapi.h drvcomm.h apenable.h
avapi.obj:      platform.h dbcs_str.h ctsn.h xapi.h virscan.h cts4.h \
                virusinf.h disk.h file.h repair.h avapi_l.h callbk15.h navcb_l.h
static.obj:     platform.h xapi.h avapi_l.h navex15.h
scanmem.obj:    platform.h dbcs_str.h xapi.h avapi.h ctsn.h callback.h
nlmload.obj:    platform.h dbcs_str.h ctsn.h xapi.h virscan.h cts4.h \
                virusinf.h disk.h file.h repair.h avapi.h callback.h
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
vxdload.obj:    platform.h quakever.h
#ENDUPDATE#
