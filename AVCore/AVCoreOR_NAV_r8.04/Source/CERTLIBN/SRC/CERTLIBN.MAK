# Copyright 1992-1993 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/CERTLIBN/VCS/certlibn.mav   1.60   29 Apr 1997 14:20:14   JBRENNA  $
#
# Description:
#       This is a generic make file
#
#########################################################################
#
# $Log:   S:/CERTLIBN/VCS/certlibn.mav  $
#  
#     Rev 1.60   29 Apr 1997 14:20:14   JBRENNA
#  Change .WIN platform checks to use CORE_WIN16.
#  
#     Rev 1.59   28 Apr 1997 15:49:30   JBRENNA
#  Add CORE_* platform support.
#  
#     Rev 1.58   10 May 1996 20:45:46   YVANG
#  Added dbcsrtns.obj.
#  
#     Rev 1.57   16 Apr 1996 10:32:52   TCASHIN
#  Removed references to NAVOEM.
#  
#     Rev 1.56   15 Apr 1996 09:53:48   TCASHIN
#  Build CERTLIBO if target is NAVOEM.
#  
#     Rev 1.55   10 Apr 1996 20:47:34   JMILLAR
#  add NTK (NT kernel) build
#  
#     Rev 1.54   10 Apr 1996 20:45:08   JMILLAR
#  fix inadvertent regression to Quake 8, nuking our last few changes -
#  Tim is watching do this, and concurs
#  
#     Rev 1.53   10 Apr 1996 18:13:22   TCASHIN
#  Added NAVOEM target.
#  
#     Rev 1.52   25 Jan 1996 14:59:10   GDZIECI
#  Put STRCP, STRL and ONEHALF back in for NLM (lost in previous change).
#  
#     Rev 1.51   11 Jan 1996 18:58:40   JWORDEN
#  Port changes from Quake 6
#  
#     Rev 1.50   29 Nov 1995 17:52:52   GDZIECI
#  STRCP and STRL are now used on NLM platform.
#  
#     Rev 1.49   10 Nov 1995 16:05:58   GDZIECI
#  Removed unused modules for the NLM platform.
#  
#     Rev 1.48   06 Jun 1995 18:35:34   SZIADEH
#  added mkinocd.cpp
#  
#     Rev 1.47   31 May 1995 11:49:02   SZIADEH
#  added onehalfm.cpp.
#  
#     Rev 1.46   11 May 1995 18:52:00   SZIADEH
#  added dint13.obj                
#  
#     Rev 1.45   11 May 1995 16:29:12   SZIADEH
#  uncommented getindata.asm
#  
#     Rev 1.44   11 Apr 1995 12:32:48   RSTANEV
#  Added selftest.obj.
#
#     Rev 1.43   02 Feb 1995 16:58:22   SZIADEH
#  added inscan.cpp
#
#     Rev 1.42   19 Jan 1995 15:16:56   SZIADEH
#  added new files.
#
#     Rev 1.41   20 Dec 1994 17:12:44   BRAD
#  Added string functions which were in CERTLIB4
#
#     Rev 1.40   20 Dec 1994 14:30:46   JMILLARD
#
#  pick up some FIL4 routines for NLM
#
#     Rev 1.39   17 Dec 1994 01:17:58   SZIADEH
#  sync with 3.05
#
#     Rev 1.38   14 Dec 1994 13:56:48   BRAD
#  Added FKILLC.OBJ
#
#     Rev 1.37   13 Dec 1994 19:08:14   JMILLARD
#  add Sami's new functions for NLM, except for rep4_fil
#
#     Rev 1.36   13 Dec 1994 16:13:00   BRAD
#  Added FSTATC.OBJ
#
#     Rev 1.35   12 Dec 1994 20:45:50   SZIADEH
#  added files/functions to sync with certlib4.
#
#     Rev 1.34   01 Nov 1994 13:49:58   JMILLARD
#
#  add NLM platform (not all files are built)
#
#     Rev 1.33   24 Oct 1994 12:00:50   MFALLEN
#  CERTLIBN
#
#     Rev 1.32   21 Oct 1994 13:36:20   keith
#  Cause the C optimization to be for .DLL use.
#
#     Rev 1.31   20 Oct 1994 11:27:14   KEITH
#  Set /PAGE:64 for LIB for .WIN compiles.
#
#     Rev 1.29   15 Sep 1994 17:31:02   CAREY
#  added global avglob.cpp file to certlibn project.
#
#     Rev 1.28   23 Aug 1994 20:41:38   RSTANEV
#  Updated dependencies.
#
#     Rev 1.27   22 Aug 1994 11:03:32   CAREY
#  removed fcdir.asm from the makefiel
#
#     Rev 1.26   17 Aug 1994 13:26:46   CAREY
#  No change.
#
#     Rev 1.25   17 Aug 1994 12:11:58   CAREY
#  removed ffwild.asm and ffindall.asm
#
#     Rev 1.24   27 Jul 1994 16:04:00   CAREY
#  added avapi.inc for appropriate files
#
#     Rev 1.23   21 Jul 1994 17:17:26   MFALLEN
#  Added scn4_del function in scn_del.asm
#
#     Rev 1.22   01 Jul 1994 13:36:18   CAREY
#  Added scn_mte, scn_v2px and scn_malt to the makefile & lib.
#
#     Rev 1.21   24 Jun 1994 14:31:40   RSTANEV
#  Cleaned up rev. log.
#  Added x1.obj
#
#
########################################################################
TARGET_NAME=CERTLIBN
PROGRAM_NAME=$(TARGET_NAME).LIB

# Setting TARGETTYPE=DLL and using a target name with a .LIB extension
# allows .DLL optimization flags and still creates a static lib to
# link other files to create a .DLL.                    KLM 10/21/94

%If "$(CORE_WIN16)" != ""
    TARGET_TYPE=DLL
%Else
    TARGET_TYPE=LIB
%Endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

%If "$(PLATFORM)" == ".TSR"
ASMOPTS+= /DSYM_TSR

%ElseIf "$(CORE_WIN16)" != ""
LIBOPTS+= /PAGE:64

%EndIf

%If "$(PLATFORM)" == ".NLM"
LIBOPTS+= -p=32
$(PROGRAM_NAME):        \
        fstart.obj      \
        mwstos.obj      \
        nmod.obj        \
        mcrc.obj        \
        rep4_dcr.obj    \
        scn_alg.obj     \
        scn_crc.obj     \
        scn_del.obj     \
        scn_me.obj      \
        scn_mem.obj     \
        scn_n20.obj     \
        scn_v2px.obj    \
        scn_mte.obj     \
        scn_malt.obj    \
        scnget.obj      \
        ffall.obj       \
        ffwild.obj      \
        fprot.obj       \
        fcrc.obj        \
        stricomp.obj    \
        strcomp.obj     \
        strcp.obj       \
        strl.obj        \
        sgcpu.obj       \
        avglob.obj      \
        dbcsrtns.obj    \
        fred.obj        \
        fopen.obj       \
        fseek.obj       \
        fclose.obj      \
        fwrit.obj       \
        fattr.obj       \
        fsize.obj       \
        fstatc.obj      \
        inscan.obj      \
        onehalf.obj

%ElseIf "$(CORE_SOFT)" == "K"

$(PROGRAM_NAME):        \
        fkillc.obj      \
        avglob.obj      \
        inscan.obj      \
        stricomp.obj    \
        onehalfm.obj    \
        mkinocd.obj     \
        fstatc.obj      \
        selftest.obj    \
        strupc.obj      \
        dbclose.obj     \
        dbcreat.obj     \
        dbcsrtns.obj    \
        dbdel.obj       \
        dbgetent.obj    \
        dbinst.obj      \
        dblquick.obj    \
        dbopcre.obj     \
        dbopen.obj      \
        dbsrch.obj      \
        dbsrq.obj       \
        dis4_typ.obj    \
        dis4vmbr.obj    \
        disbpb.obj      \
        dislcbt.obj     \
        disrbs.obj      \
        disrmbr.obj     \
        dosunv.obj      \
        dosvec.obj      \
        drph.obj        \
        dwph.obj        \
        fattr.obj       \
        fcdta.obj       \
        fclose.obj      \
        fdel.obj        \
        fdir.obj        \
        fcdir.obj       \
        fchdir.obj      \
        ffall.obj       \
        ffwild.obj      \
        fprot.obj       \
        fdtime.obj      \
        ffindc.obj      \
        ffindf.obj      \
        fft2dost.obj    \
        fgdta.obj       \
        fisat.obj       \
        filst.obj       \
        fmedia.obj      \
        fnext.obj       \
        fopen.obj       \
        fred.obj        \
        fseek.obj       \
        fsize.obj       \
        fstart.obj      \
        fstat.obj       \
        fwrit.obj       \
        fcrc.obj        \
        ir_cmp35.obj    \
        ir_cpbak.obj    \
        ir_detmn.obj    \
        ir_fnd35.obj    \
        ir_main.obj     \
        mcrc.obj        \
        memset.obj      \
        musbeep.obj     \
        mwstos.obj      \
        nmod.obj        \
        rep4_bge.obj    \
        rep4_bnd.obj    \
        rep4_boo.obj    \
        rep4_bpg.obj    \
        rep4_bun.obj    \
        rep4_cal.obj    \
        rep4_com.obj    \
        rep4_dcr.obj    \
        rep4_exe.obj    \
        rep4_fil.obj    \
        rep4_gen.obj    \
        rep4_phy.obj    \
        rep4_sph.obj    \
        rep4_sys.obj    \
        scn_alg.obj     \
        scn_boo.obj     \
        scn_crc.obj     \
        scn_del.obj     \
        scn_me.obj      \
        scn_mem.obj     \
        scn_n20.obj     \
        scn_v2px.obj    \
        scn_mte.obj     \
        scn_malt.obj    \
        scnget.obj      \
        strcp.obj       \
        strl.obj        \
        strll.obj       \
        syshalt.obj     \
        strcomp.obj     \
        sgcpu.obj       \
        x1.obj          \
        dispart.obj     \
        getdosen.obj    \
        memsc.obj       \
        memss.obj       \
        stsrch.obj      \
        onehalf.obj     \
        sysgn.obj       \
        disbtnm.obj     \
        dishdmbr.obj    \
        disisbt.obj     \
        filcrcfl.obj    \
        memcopy.obj     \
        disbtst.obj     \
        getindat.obj    \
        dint13.obj      \

%Else

$(PROGRAM_NAME):        \
        dbclose.obj     \
        dbcreat.obj     \
        dbcsrtns.obj    \
        dbdel.obj       \
        dbgetent.obj    \
        dbinst.obj      \
        dblquick.obj    \
        dbopcre.obj     \
        dbopen.obj      \
        dbsrch.obj      \
        dbsrq.obj       \
        dis4_typ.obj    \
        dis4vmbr.obj    \
        disbpb.obj      \
        dislcbt.obj     \
        disrbs.obj      \
        disrmbr.obj     \
        dosunv.obj      \
        dosvec.obj      \
        drph.obj        \
        dwph.obj        \
        fattr.obj       \
        fcdta.obj       \
        fclose.obj      \
        fdel.obj        \
        fdir.obj        \
        fcdir.obj       \
        fkillc.obj      \
        fchdir.obj      \
        ffall.obj       \
        ffwild.obj      \
        fprot.obj       \
        fdtime.obj      \
        ffindc.obj      \
        ffindf.obj      \
        fft2dost.obj    \
        fgdta.obj       \
        fisat.obj       \
        filst.obj       \
        fmedia.obj      \
        fnext.obj       \
        fopen.obj       \
        fred.obj        \
        fseek.obj       \
        fsize.obj       \
        fstart.obj      \
        fstat.obj       \
        fstatc.obj      \
        fwrit.obj       \
        fcrc.obj        \
        ir_cmp35.obj    \
        ir_cpbak.obj    \
        ir_detmn.obj    \
        ir_fnd35.obj    \
        ir_main.obj     \
        mcrc.obj        \
        memset.obj      \
        musbeep.obj     \
        mwstos.obj      \
        nmod.obj        \
        rep4_bge.obj    \
        rep4_bnd.obj    \
        rep4_boo.obj    \
        rep4_bpg.obj    \
        rep4_bun.obj    \
        rep4_cal.obj    \
        rep4_com.obj    \
        rep4_dcr.obj    \
        rep4_exe.obj    \
        rep4_fil.obj    \
        rep4_gen.obj    \
        rep4_phy.obj    \
        rep4_sph.obj    \
        rep4_sys.obj    \
        scn_alg.obj     \
        scn_boo.obj     \
        scn_crc.obj     \
        scn_del.obj     \
        scn_me.obj      \
        scn_mem.obj     \
        scn_n20.obj     \
        scn_v2px.obj    \
        scn_mte.obj     \
        scn_malt.obj    \
        scnget.obj      \
        selftest.obj    \
        strcp.obj       \
        strupc.obj      \
        strl.obj        \
        strll.obj       \
        syshalt.obj     \
        avglob.obj      \
        inscan.obj      \
        stricomp.obj    \
        strcomp.obj     \
        sgcpu.obj       \
        x1.obj          \
        dispart.obj     \
        getdosen.obj    \
        memsc.obj       \
        memss.obj       \
        stsrch.obj      \
        onehalf.obj     \
        onehalfm.obj    \
        sysgn.obj       \
        disbtnm.obj     \
        dishdmbr.obj    \
        disisbt.obj     \
        filcrcfl.obj    \
        memcopy.obj     \
        disbtst.obj     \
        getindat.obj    \
%If "$(CORE_SOFT)" == "V"
        mkinocd.obj     \
%Endif
        dint13.obj
        
%Endif

#UPDATE#

dbclose.obj:    ctsn.inc
dbcreat.obj:    ctsn.inc
dbdel.obj:      ctsn.inc
dbgetent.obj:   ctsn.inc
dbinst.obj:     ctsn.inc
dblquick.obj:   ctsn.inc
dbopcre.obj:    ctsn.inc
dbopen.obj:     ctsn.inc
dbsrch.obj:     ctsn.inc
dbsrq.obj:      ctsn.inc
dis4_typ.obj:   ctsn.inc repairn.inc
dis4vmbr.obj:   ctsn.inc
disbpb.obj:     ctsn.inc
dislcbt.obj:    ctsn.inc
disrbs.obj:     ctsn.inc
disrmbr.obj:    ctsn.inc
dosunv.obj:     ctsn.inc
dosvec.obj:     ctsn.inc
drph.obj:       ctsn.inc avapi.inc
dwph.obj:       ctsn.inc avapi.inc
fattr.obj:      ctsn.inc avapi.inc
fdir.obj:       ctsn.inc
fcdir.obj:      ctsn.inc
fchdir.obj:     ctsn.inc
fcdta.obj:      ctsn.inc
fclose.obj:     ctsn.inc avapi.inc
fdel.obj:       ctsn.inc avapi.inc
fdtime.obj:     ctsn.inc avapi.inc
ffall.obj:      ctsn.inc
ffindc.obj:     ctsn.inc
ffindf.obj:     ctsn.inc
fft2dost.obj:   ctsn.inc
ffwild.obj:     ctsn.inc
fgdta.obj:      ctsn.inc
fisat.obj:      ctsn.inc
filst.obj:      ctsn.inc
fmedia.obj:     ctsn.inc
fnext.obj:      ctsn.inc
fopen.obj:      ctsn.inc avapi.inc
fprot.obj:      ctsn.inc
fred.obj:       ctsn.inc avapi.inc
fseek.obj:      ctsn.inc avapi.inc
fsize.obj:      ctsn.inc
fstart.obj:     ctsn.inc
fwrit.obj:      ctsn.inc avapi.inc
ir_cmp35.obj:   ctsn.inc
ir_cpbak.obj:   ctsn.inc
ir_detmn.obj:   ctsn.inc
ir_fnd35.obj:   ctsn.inc
ir_main.obj:    ctsn.inc
mcrc.obj:       ctsn.inc
memset.obj:     ctsn.inc
musbeep.obj:    ctsn.inc
mwstos.obj:     ctsn.inc
nmod.obj:       ctsn.inc
rep4_bge.obj:   ctsn.inc repairn.inc
rep4_bnd.obj:   ctsn.inc repairn.inc
rep4_boo.obj:   ctsn.inc repairn.inc
rep4_bpg.obj:   ctsn.inc repairn.inc
rep4_bun.obj:   ctsn.inc repairn.inc
rep4_cal.obj:   ctsn.inc repairn.inc
rep4_com.obj:   ctsn.inc repairn.inc
rep4_dcr.obj:   ctsn.inc repairn.inc
rep4_exe.obj:   ctsn.inc repairn.inc
rep4_fil.obj:   ctsn.inc repairn.inc
rep4_gen.obj:   ctsn.inc repairn.inc
rep4_phy.obj:   ctsn.inc repairn.inc
rep4_sph.obj:   ctsn.inc repairn.inc
rep4_sys.obj:   ctsn.inc repairn.inc
scn_alg.obj:    ctsn.inc repairn.inc
scn_boo.obj:    ctsn.inc
scn_crc.obj:    ctsn.inc
scn_del.obj:    ctsn.inc
scn_me.obj:     ctsn.inc
scn_mem.obj:    ctsn.inc
scn_n20.obj:    ctsn.inc
scn_v2px.obj:   ctsn.inc
scn_mte.obj:    ctsn.inc
scn_malt.obj:   ctsn.inc
scnget.obj:     ctsn.inc
selftest:       platform.h file.h ctsn.h
strcomp.obj:    ctsn.inc
strcp.obj:      ctsn.inc
strl.obj:       ctsn.inc
strll.obj:      ctsn.inc
syshalt.obj:    ctsn.inc
x1.obj:         ctsn.inc
sgcpu.obj:      ctsn.inc
dispart.obj:    ctsn.inc
getdosen.obj:   ctsn.inc
memsc.obj:      ctsn.inc
memss.obj:      ctsn.inc
stsrch.obj:     ctsn.inc
onehalf.obj:    ctsn.inc 1half.inc
onehalfm.obj:   ctsn.h   virscan.h
sysgn.obj:      ctsn.inc
getindat.obj:   ctsn.inc 1half.inc
disbtnm.obj:    ctsn.inc
dishdmbr.obj:   ctsn.inc
disisbt.obj:    ctsn.inc
filcrcfl.obj:   ctsn.inc
memcopy.obj:    ctsn.inc
disbtst.obj:    ctsn.inc
mkinocd.obj:    platform.h ctsn.h avapi.h

inscan.obj:     platform.h dbcs_str.h xapi.h file.h ctsn.h callback.h \
                virscan.h virusinf.h platform.h dbcs_str.h xapi.h file.h \
                ctsn.h callback.h virscan.h virusinf.h
avglob.obj:     platform.h dbcs_str.h file.h xapi.h disk.h ctsn.h \
                callback.h
strupc.obj:     platform.h dbcs_str.h ctsn.h callback.h
fstatc.obj:     platform.h dbcs_str.h xapi.h file.h ctsn.h callback.h
fkillc.obj:     platform.h dbcs_str.h ctsn.h callback.h
stricomp.obj:   platform.h dbcs_str.h ctsn.h callback.h
fcrc.obj:       CTSN.INC
memsc.obj:      CTSN.INC
fstat.obj:      CTSN.INC

#ENDUPDATE#
