# Copyright 1992-1995 Symantec Corporation                              #
#########################################################################
#                                                                       #
# $Header:   S:/SYMKRNL/VCS/SYMKRNL.MAv   1.30   30 Jul 1998 20:38:02   dhertel  $ #
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMKRNL/VCS/SYMKRNL.MAv  $ #
#  
#     Rev 1.30   30 Jul 1998 20:38:02   dhertel
#  
#     Rev 1.29   18 Feb 1998 10:02:28   mdunn
#  Ported ROSWELL and ROSNEC changes from the K branch.
#  
#     Rev 1.28   26 Aug 1997 10:52:22   DDREW
#  Added prm_str.obj to NLM platform make suite
#  
#     Rev 1.27   10 Jul 1997 23:38:24   RStanev
#  Adding /DUSE_NEW_STACK_SWAPPING when building DEC Alpha drivers.
#
#     Rev 1.26   29 May 1997 02:26:36   RStanev
#  Added fop_dir.obj to SYM_NTK compilations.
#
#     Rev 1.25   14 May 1997 11:55:38   MBROWN
#  Added dskfat32.obj to WIN platform.
#
#     Rev 1.24   08 Apr 1997 14:11:36   bsobel
#  Build system changes for vxd
#
#     Rev 1.23   25 Feb 1997 01:38:14   RSTANEV
#  Added thdata.obj and stack.obj to SYM_NTK and SYM_VXD compiles.
#
#     Rev 1.22   06 Jan 1997 16:03:34   RSTANEV
#  Changed the obj files used in SYM_VXD platform.
#
#     Rev 1.21   12 Dec 1996 12:37:46   SKURTZ
#  Added necabsop.asm for NEC DOS (NCS) platform
#
#     Rev 1.20   04 Dec 1996 17:28:46   BMCCORK
#  Changed to statically link CRT to solve OS bug on FreeLibrary() from dllmain
#
#     Rev 1.19   02 Dec 1996 16:45:28   BMCCORK
#  Added test platform support
#
#     Rev 1.18   10 Oct 1996 10:59:16   GWESTER
#  Added a comment that NO_JAPANESE_RESOURCES should be defined if you
#  want to keep the Japanese dialogs out of your DLL.
#
#     Rev 1.17   08 Oct 1996 16:35:46   THOFF
#  Added DOSGetCodePage support for OS/2.
#
#     Rev 1.16   25 Sep 1996 14:20:50   BMCCORK
#  Added getcp.obj, which was moved here from dosui
#
#     Rev 1.15   25 Sep 1996 10:15:56   JALLEE
#  Backed out Quake 9 port of NIMMCreate() and NIMMDestroy().  Was directly
#  linking, IMM32 not available under NT 3.51.  These functions should return
#  with LoadLibrary()/GetProcAddress() at some point in the near future.
#
#     Rev 1.14   20 Sep 1996 18:01:20   JALLEE
#  DBCS Port from Quake 9, added imm32.lib for SYM_WIN32 for new Symintl.c functions
#  NIMMCreate() and NIMMDestroy().
#
#     Rev 1.13   19 Sep 1996 21:13:36   SPASNIK
#  Removed /Zm and put option m510 into drvparam.asm
#
#     Rev 1.12   19 Sep 1996 14:55:46   SPAGLIA
#  Added DOS_OEM/ANSI functions to OS/2 platform
#
#     Rev 1.11   17 Sep 1996 18:39:18   BMCCORK
#  Added msgwiat.obj
#
#     Rev 1.10   16 Sep 1996 17:24:14   JOUYANG
#  Ansi to Oem and Oem to Ansi conversion added for Dos Agent
#
#     Rev 1.9   13 Sep 1996 17:45:00   MBARNES
#  Added /Zm option for MASM5 compatibility for DRVPARAM.ASM
#
#     Rev 1.8   21 Aug 1996 15:10:08   MLELE
#  Added NLM_STR.OBJ
#
#     Rev 1.7   18 Jul 1996 18:00:06   BMCCORK
#  Moved to CRT DLL
#
#     Rev 1.6   06 May 1996 14:26:34   RSTANEV
#  Added wildcard.obj to SYM_NTK compilation.
#
#     Rev 1.5   11 Apr 1996 19:15:04   THOFF
#  Added the FOP_DISK module to the OS/2 library.
#
#     Rev 1.4   09 Apr 1996 11:09:06   JMILLAR
#  add NTK support
#
#     Rev 1.3   04 Apr 1996 17:53:12   SPASNIK
#  dscmisc errors out for vxd platform. Rad
#  said it does not need build build for vxd side.
#  and then modified the build.bat. ( buildy dont do
#  that....  )    I removed it
#  from the vxd depend line ( in the make file ).
#  Got better results too...
#  Shall we spin the big wheel again?? Yesiree...
#
#     Rev 1.2   10 Feb 1996 16:18:48   ESALEM
#  No change.
#
#     Rev 1.1   30 Jan 1996 18:45:20   RSTANEV
#  VxD platform now uses DRVMISC.CPP instead of VXDSTAT.CPP.
#
#     Rev 1.0   26 Jan 1996 20:22:48   JREARDON
#  Initial revision.
#
#     Rev 1.163   10 Jan 1996 14:53:58   HENRI
#  Added DSKFAT32.OBJ to DX and W32 platform builds
#
#
#########################################################################

# Define NO_JAPANESE_RESOURCES if you don't want to compile the Japanese dialogs
# into your DLL.

TARGET_NAME=SYMKRNL
%If "$(PLATFORM)" != ".OS2"
TARGET_TYPE=DLL
%Else
TARGET_TYPE=LIB
%Endif

## Statically link with the CRT DLL to avoid a bug in the Windows DLL processing
## Calling FreeLibrary() on the thunk DLL from Symkrnl can cause the CRT DLL to
## unload while Symkrnl is still using it!!  This can be avoided by statically linking.
## USE_CRT_DLL=TRUE

.INCLUDE <PLATFORM.MAK>

# if NO_SYMEVENT is defined in the environment, build a version of SYMKRNL
# which does not require SYMEVENT
%If "$(NO_SYMEVENT)" != ""
    XOPTS+= /DNO_SYMEVENT
%Endif

%If "$(PLATFORM)" != ".OS2"
.INCLUDE <ML.MAK>
%Endif


%If "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".X86" || "$(PLATFORM)" == ".DX" || "$(PLATFORM)" == ".NCX" || "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" == ".NCS" || "$(PLATFORM)" == ".DVS"

%If "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".X86"
XOPTS+= -Gt1
%Endif

PROGRAM_NAME=$(Q_SYMKRNL).LIB

$(PROGRAM_NAME):                        \
                $(TARGET_NAME).obj      \
                symcfg.obj              \
                cfg_bloc.obj            \
                cfg_ini.obj             \
                cfg_prof.obj            \
                cfg_reg.obj             \
                cfg_swit.obj            \
                cfg_text.obj            \
                dbcs.obj                \
                dsk_abs.obj             \
                dsk_accs.obj            \
                dsk_bloc.obj            \
                dsk_bpb.obj             \
                dsk_cach.obj            \
                dsk_drvr.obj            \
                dskfat32.obj            \
                dsk_gen.obj             \
                dsk_int.obj             \
                dsk_misc.obj            \
                dsk_type.obj            \
                fop_dir.obj             \
                fop_dir2.obj            \
                fop_disk.obj            \
                fop_file.obj            \
                findfirs.obj            \
                filetime.obj            \
                fop_dos.obj             \
                bufferio.obj            \
                name.obj                \
                name_win.obj            \
                wildcard.obj            \
                prm_date.obj            \
                prm_dos.obj             \
                prm_hard.obj            \
                prm_mach.obj            \
                prm_mem.obj             \
                prm_misc.obj            \
                prm_str.obj             \
                prm_strd.obj            \
                prm_szz.obj             \
                prm_win.obj             \
                prm_lstr.obj            \
                prm_err.obj             \
                prm_sft.obj             \
                prm_crpt.obj            \
                romcopy.obj             \
                tsr.obj                 \
                tsr_asm.obj             \
                symintl.obj             \
                int13dos.obj            \
                version.obj             \
                seterror.obj            \
                cvtft.obj               \
                nec.obj                 \
                getcp.obj               \
                badchar.obj             \
                $(Q_SYMDEBUG).LIB       \
%If ( "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".X86" )
                txtcvrt.obj             \
%Endif
%If ( "$(PLATFORM)" == ".NCX" || "$(PLATFORM)" == ".NCS" )
                drvparam.obj            \
                txtcvrt.obj             \
%Endif
%If ( "$(PLATFORM)" == ".NCS" )
                necabsop.obj            \
%Endif
%If ( "$(PLATFORM)" == ".DX"  || "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" == ".NCX" )
                dsk_stac.obj            \
%Endif
                stacker.lib


%ElseIf ("$(PLATFORM)" == ".NLM")

PROGRAM_NAME=$(Q_SYMKRNL).LIB

$(PROGRAM_NAME):        symcfg.obj              \
                        page_mem.obj            \
                        cfg_bloc.obj            \
                        cfg_swit.obj            \
                        cvtft.obj               \
                        name.obj                \
                        filetime.obj            \
                        dbcs.obj                \
                        fop_file.obj            \
                        findfirs.obj            \
                        prm_crpt.obj            \
                        prm_date.obj            \
                        prm_mem.obj             \
                        prm_str.obj             \
                        prm_dos.obj             \
                        nlm_str.obj

%ElseIf ("$(PLATFORM)" == ".WIN" || "$(CORE_WIN16)" != "" || "$(PLATFORM)" == ".MIR" || "$(PLATFORM)" == ".DSV" || "$(PLATFORM)" == ".NEC" )

PROGRAM_NAME=$(Q_SYMKRNL).DLL

$(PROGRAM_NAME):        $(STUB)                 \
                        $(TARGET_NAME).obj      \
                        symcfg.obj              \
                        cfg_bloc.obj            \
                        cfg_ini.obj             \
                        cfg_prof.obj            \
                        cfg_reg.obj             \
                        cfg_swit.obj            \
                        cfg_text.obj            \
                        dbcs.obj                \
                        prm_date.obj            \
                        prm_dos.obj             \
                        prm_hard.obj            \
                        prm_mach.obj            \
                        prm_mem.obj             \
                        prm_misc.obj            \
                        prm_str.obj             \
                        prm_strd.obj            \
                        prm_szz.obj             \
                        prm_win.obj             \
                        prm_lstr.obj            \
                        prm_err.obj             \
                        prm_sft.obj             \
                        prm_crpt.obj            \
                        dsk_abs.obj             \
                        dsk_accs.obj            \
                        dsk_bloc.obj            \
                        dsk_bpb.obj             \
                        dsk_cach.obj            \
                        dsk_drvr.obj            \
                        dsk_gen.obj             \
                        dsk_int.obj             \
                        dsk_misc.obj            \
                        dsk_type.obj            \
                        dskfat32.obj            \
                        int13win.obj            \
                        bufferio.obj            \
                        fop_win.obj             \
                        fop_dir.obj             \
                        fop_dir2.obj            \
                        fop_disk.obj            \
                        fop_file.obj            \
                        findfirs.obj            \
                        filetime.obj            \
                        fop_dos.obj             \
                        symintl.obj             \
                        name.obj                \
                        name_win.obj            \
                        wildcard.obj            \
                        tsr.obj                 \
                        tsr_asm.obj             \
                        hstring.obj             \
                        romcopy.obj             \
                        version.obj             \
                        hugeoem.obj             \
                        cvtft.obj               \
                        nec.obj                 \
%If ("$(PLATFORM)" == ".WIN" || "$(CORE_WIN16)" != "")
                        $(Q_SYMDEBUG).LIB       \
%Endif
                        $(TARGET_NAME).res      \
%If "$(PLATFORM)" == ".MIR"
                        krnlstub.obj            \
%Endif
                        badchar.obj             \
                        dsk_stac.obj            \
                        symkrnl.def             \
                        stackerw.lib

create_implib: $(Q_SYMKRNL).lib

$(Q_SYMKRNL).lib: $(TARGET_NAME).def

%ElseIf "$(PLATFORM)" == ".VXD" || "$(CORE_SOFT)" == "V"

PROGRAM_NAME=SYMKRNL.LIB

$(PROGRAM_NAME):        vxdkrnl.obj         \
                        iossrv.obj          \
                        drvmisc.obj         \
                        stack.obj           \
                        thdata.obj          \
                        rtl32.obj           \
                        dbcs.obj            \
                        vxd_strs.obj        \
                        dsk_abs.obj         \
                        dsk_type.obj        \
                        name.obj            \
                        findfirs.obj        \
                        prm_date.obj        \
                        prm_str.obj         \
                        prm_strd.obj        \
                        prm_misc.obj        \
                        fop_dos.obj         \
                        fop_disk.obj        \
                        filetime.obj        \
                        fop_file.obj        \
                        wildcard.obj        \
                        name_win.obj        \
                        dsk_misc.obj        \
                        cfg_reg.obj         \
                        prm_win.obj         \
                        fop_dir2.obj        \
                        badchar.obj         \
                        prm_mem.obj         \
                        nec.obj

%ElseIf "$(PLATFORM)" == ".NTK" || "$(CORE_SOFT)" == "K"

%If "$(CORE_PLAT)" == "A"
XOPTS+= /DUSE_NEW_STACK_SWAPPING
%EndIf

PROGRAM_NAME=SYMKRNL.LIB

$(PROGRAM_NAME):        fop_file.obj        \
                        fop_dos.obj         \
                        fop_dir.obj         \
                        dsk_abs.obj         \
                        dsk_misc.obj        \
                        prm_mem.obj         \
                        prm_misc.obj        \
                        prm_str.obj         \
                        prm_dos.obj         \
                        prm_date.obj        \
                        filetime.obj        \
                        name.obj            \
                        cfg_reg.obj         \
                        rtl32.obj           \
                        drvmisc.obj         \
                        stack.obj           \
                        thdata.obj          \
                        badchar.obj         \
                        wildcard.obj        \
                        nec.obj

%ElseIf "$(PLATFORM)" == ".OS2"

PROGRAM_NAME=$(Q_SYMKRNL).LIB

$(PROGRAM_NAME):        $(TARGET_NAME).obj      \
                        badchar.obj             \
                        cfg_prof.obj            \
                        cfg_swit.obj            \
                        codepage.obj            \
                        dbcs.obj                \
                        dsk_misc.obj            \
                        dsk_type.obj            \
                        fop_dir2.obj            \
                        fop_disk.obj            \
                        fop_file.obj            \
                        findfirs.obj            \
                        hugeoem.obj             \
                        name.obj                \
                        prm_date.obj            \
                        prm_str.obj             \
                        prm_strd.obj            \
                        prm_mem.obj             \
                        nec.obj                 \
                        prm_dos.obj             \
                        txtcvrt.obj

%ElseIf "$(PLATFORM)" == ".W32" || ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")

PROGRAM_NAME=$(Q_SYMKRNL).DLL

$(PROGRAM_NAME):        $(STUB)                 \
                        $(TARGET_NAME).obj      \
                        symcfg.obj              \
                        cfg_bloc.obj            \
                        cfg_ini.obj             \
                        cfg_prof.obj            \
                        cfg_reg.obj             \
                        cfg_swit.obj            \
                        cfg_text.obj            \
                        dbcs.obj                \
                        prm_date.obj            \
                        prm_dos.obj             \
                        prm_mem.obj             \
                        prm_misc.obj            \
                        prm_str.obj             \
                        prm_strd.obj            \
                        prm_szz.obj             \
                        prm_crpt.obj            \
                        dsk_accs.obj            \
                        dsk_bpb.obj             \
                        dsk_cach.obj            \
                        dskfat32.obj            \
                        dsk_misc.obj            \
                        dsk_type.obj            \
                        bufferio.obj            \
                        fop_win.obj             \
                        fop_dir.obj             \
                        fop_dir2.obj            \
                        fop_disk.obj            \
                        fop_file.obj            \
                        findfirs.obj            \
                        fop_dos.obj             \
                        symintl.obj             \
                        name.obj                \
                        name_win.obj            \
                        wildcard.obj            \
                        hstring.obj             \
                        version.obj             \
                        prm_hard.obj            \
                        dsk_abs.obj             \
                        dsk_bloc.obj            \
                        dsk_int.obj             \
                        dsk_gen.obj             \
                        filetime.obj            \
                        nec.obj                 \
                        dsk_drvr.obj            \
                        dsk_stac.obj            \
                        vxdload.obj             \
                        $(Q_SYMDEBUG).LIB       \
                        $(TARGET_NAME).res      \
                        prm_win.obj             \
                        msgwait.obj             \
                        badchar.obj             \
                        hugeoem.obj

# W32 is same as WIN, except for these modules.
#                        prm_mach.obj            \
#                        prm_sft.obj             \
#                        dsk_drvr.obj            \
#                        tsr.obj                 \
#                        imm32.lib               \
# These modules are ASM
#                        int13win.obj            \
#                        prm_lstr.obj            \
#                        prm_err.obj             \
#                        tsr_asm.obj             \
#                        romcopy.obj             \


%Endif

#UPDATE#
vxdkrnl.obj:    platform.inc ios.inc blockdev.inc ifsmgr.inc
iossrv.obj:     platform.inc blockdev.inc ior.inc iop.inc aep.inc isp.inc \
                ios.inc drp.inc ilb.inc dcb.inc sgd.inc ida.inc symkvxd.inc
drvmisc.obj:    platform.h xapi.h
rtl32.obj:      platform.h
bufferio.obj:   platform.h stddos.h ibmpc.h file.h
cfg_bloc.obj:   platform.h file.h xapi.h symcfg.h
cfg_ini.obj:    platform.h file.h xapi.h symcfg.h
cfg_prof.obj:   platform.h xapi.h
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
fop_win.obj:    platform.h file.h
nlm_str.obj:    platform.h dbcs_str.h dosvmm.h
txtcvrt.obj:    platform.h xapi.h
#ENDUPDATE#
