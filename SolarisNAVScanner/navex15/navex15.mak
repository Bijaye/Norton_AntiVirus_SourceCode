#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/navex15.mav   1.61   24 Dec 1998 23:06:50   DCHI  $
#
#  Description:
#       This is a make file for NAVEX.
#
#  See Also:
#       VIRSCAN and AVAPI projects.
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/navex15.mav  $
#  
#     Rev 1.61   24 Dec 1998 23:06:50   DCHI
#  Added remotex.obj and necessary modifications to support it.
#  
#     Rev 1.60   09 Dec 1998 17:46:00   DCHI
#  Removed /DMSX, /DMACROHEU, and /DMACRODAT.
#  
#     Rev 1.59   08 Dec 1998 13:28:10   DCHI
#  Changes for MSX.
#  
#     Rev 1.58   12 Oct 1998 10:59:44   CNACHEN
#  Added support for AOL pws heuristics.
#  
#  
#     Rev 1.57   09 Sep 1998 12:45:26   DCHI
#  Changes for password-protected Excel 97 document repair.
#  
#     Rev 1.56   11 Aug 1998 13:46:00   MKEATIN
#  Removed /ALfw on the WIN platform.  We're not build navex16a.dll in
#  the large memory model.
#  
#     Rev 1.55   11 Aug 1998 13:41:48   MKEATIN
#  Added the /ALfw switch so we don't assume DS == SS.
#  
#     Rev 1.54   06 Aug 1998 13:35:30   MKEATIN
#  Added RCBINDOPTS to the WIN section.
#  
#     Rev 1.53   06 Aug 1998 13:18:50   MKEATIN
#  Fixed the WIN (Win16) section to build NAVEX16a.DLL.
#  
#     Rev 1.52   04 Aug 1998 14:05:50   DCHI
#  Changes for Excel heuristics.
#  
#     Rev 1.51   27 Jul 1998 13:04:28   DKESSNE
#  added pespscan.obj
#  
#     Rev 1.50   02 Jul 1998 18:20:08   DCHI
#  Implemented NOMACRO env var checking for building without macro engine.
#  
#     Rev 1.49   10 Jun 1998 13:15:14   DCHI
#  Added W7H2
#  
#     Rev 1.48   09 Jun 1998 23:11:42   JWILBER
#  Added handling for ZOODEF environment variable.
#
#     Rev 1.47   11 May 1998 18:23:18   DCHI
#  Added Access 2.0 (A2) and Access 97 (A97) sections.
#
#     Rev 1.46   06 May 1998 15:42:28   DCHI
#  Removed copy of mkdecsig.exe.
#
#     Rev 1.45   04 May 1998 12:14:36   AOONWAL
#  Using new dvx file "vavex15.dvx" for Rad
#
#     Rev 1.44   17 Apr 1998 13:45:18   DCHI
#  Added acsapi.lib, acsos.obj, and a97scn.obj.
#
#     Rev 1.43   15 Apr 1998 17:42:44   DCHI
#
#     Rev 1.42   15 Apr 1998 17:40:30   DCHI
#  Oops!  Forgot the libs.
#
#     Rev 1.41   15 Apr 1998 17:24:00   DCHI
#  Modifications for new XL95 and XL5 engines.
#
#     Rev 1.40   03 Mar 1998 12:20:58   CFORMUL
#  Added a -DSARCBOT to XOPTS if this is a SARCBOT build
#
#     Rev 1.39   09 Feb 1998 19:57:12   JWILBER
#  Made changes due to additions of *.wdf and *.h files for foreign
#  offices.
#
#     Rev 1.38   29 Jan 1998 19:03:24   DCHI
#  Added xl4mscan.obj and xl4mrep.obj
#
#     Rev 1.37   13 Jan 1998 17:04:10   FBARAJA
#  No change.
#
#     Rev 1.36   09 Jan 1998 03:52:54   RELNITI
#  SET trojan scanning
#
#     Rev 1.35   29 Dec 1997 17:59:46   JWILBER
#  Modified during Jan98 build.
#
#     Rev 1.34   22 Dec 1997 18:38:12   JWILBER
#  Rolled back to 1.32 to pull out trojan stuff.
#
#     Rev 1.32   26 Nov 1997 18:56:48   DCHI
#  Added files for PowerPoint scanning/repair and commented out define
#  for enabling it.
#
#     Rev 1.31   10 Nov 1997 16:48:14   CNACHEN
#  Properly added bloodhound boot source files as dependencies.
#
#     Rev 1.30   07 Nov 1997 10:56:26   DCHI
#  Added xl5api.lib.
#
#     Rev 1.29   05 Nov 1997 12:05:08   DCHI
#  Added mcrheuen.obj.
#
#     Rev 1.28   16 Oct 1997 13:26:40   DCHI
#  Removed crc.obj from object list, because crc32.lib is used instead.
#
#     Rev 1.27   15 Oct 1997 18:15:24   DCHI
#  Added Word 97 heuristics files.
#
#     Rev 1.26   15 Oct 1997 10:21:12   CNACHEN
#  Commented out trojscan for this month.
#
#     Rev 1.25   09 Oct 1997 17:13:58   CNACHEN
#  Added support for TROJSCAN...
#
#     Rev 1.24   05 Sep 1997 20:43:34   CNACHEN
#  Removed comment lines # from targets
#
#     Rev 1.23   05 Sep 1997 20:35:28   CNACHEN
#  Now supports boot heuristics.
#
#     Rev 1.22   07 Aug 1997 18:24:06   DCHI
#  Added inifile.
#
#     Rev 1.21   07 Aug 1997 09:46:14   DDREW
#  Added export file for NLM
#
#     Rev 1.20   05 Aug 1997 10:07:54   DDREW
#  Made NLM startup file navexnlm.cpp
#
#     Rev 1.19   05 Aug 1997 09:35:54   DDREW
#  Mod to NLM build platform (NAVEX15.DEN)
#
#     Rev 1.18   28 Jul 1997 16:16:48   DCHI
#  Added datadir.obj.
#
#     Rev 1.17   16 Jul 1997 18:45:26   DCHI
#  Readded symkrnl.lib to ARUK after enabling MVP.
#
#     Rev 1.16   09 Jul 1997 16:59:56   DCHI
#  Changes supporting new OLE API.
#
#     Rev 1.15   08 Jul 1997 18:58:40   DDREW
#  NAVEX15.NLM must use navex15.obj
#
#     Rev 1.14   26 Jun 1997 16:11:10   DCHI
#  Synced up with wd7sigs4.wdf from navex.mak.
#
#     Rev 1.13   19 Jun 1997 13:04:50   DCHI
#  Updated ARUK with 15 changes.
#
#     Rev 1.12   05 Jun 1997 16:22:12   MKEATIN
#  updated the ARA/ARU platforms so they build with navex15.obj
#  and navex15.d32 for navex 1.5.
#
#     Rev 1.11   04 Jun 1997 17:24:46   DCHI
#  Added ARU platform.
#
#     Rev 1.10   04 Jun 1997 13:49:44   DCHI
#  Added ARUK section.
#
#     Rev 1.9   04 Jun 1997 12:06:08   DCHI
#  Added NAVEXDA.DLL for DEC ALPHA ARA platform.
#
#     Rev 1.8   27 May 1997 18:50:10   MKEATIN
#  Fixed NTK platform for 1.5
#
#     Rev 1.7   27 May 1997 18:09:52   DCHI
#  Ported changes from navex.mak to navex15.mak.
#
#     Rev 1.6   27 May 1997 18:03:14   MKEATIN
#  Added 1.5 technology
#
#     Rev 1.5   21 May 1997 16:47:36   MKEATIN
#  The VXD platform is now 1.5 enabled.
#
#     Rev 1.4   08 May 1997 16:27:26   MKEATIN
#  Ported changes from navex.mak to navex15.mak.
#
#     Rev 1.3   08 May 1997 14:30:26   CNACHEN
#  Added support for NAVEX15.  Set "NAVEX15" environment variable to something
#  before using BUILD.BTM/UPDATE.BTM.
#
#     Rev 1.2   08 May 1997 13:35:08   MKEATIN
#
#     Rev 1.0   02 May 1997 13:30:14   CNACHEN
#  Initial revision.
#
#     Rev 1.36   30 Apr 1997 10:22:40   DCHI
#  Removed linking of symevent.lib from NTK platform.
#
#     Rev 1.35   18 Apr 1997 18:03:30   AOONWAL
#  Modified during DEV1 malfunction
#
#     Rev ABID   14 Apr 1997 09:23:12   RELNITI
#  MODIFY to include WDVARSIG in XL97 (for FullSet in XL97)
#
#     Rev 1.34   07 Apr 1997 18:11:06   DCHI
#  Added MVP support.
#
#     Rev 1.33   07 Apr 1997 16:39:20   CFORMUL
#  Added building of multiple .wdf files
#
#     Rev 1.32   14 Mar 1997 16:35:30   DCHI
#  Added support for Office 97 repair.
#
#     Rev 1.31   13 Feb 1997 13:36:08   DCHI
#  Modifications to support VBA 5 scanning.
#
#     Rev 1.30   17 Jan 1997 11:01:54   DCHI
#  Modifications supporting new macro engine.
#
#     Rev 1.29   06 Jan 1997 19:05:22   RSTANEV
#  Oops.
#
#     Rev 1.28   06 Jan 1997 19:01:32   RSTANEV
#
#     Rev 1.27   26 Dec 1996 15:23:18   AOONWAL
#  No change.
#
#     Rev 1.26   02 Dec 1996 14:03:20   AOONWAL
#  No change.
#
#     Rev 1.25   29 Oct 1996 13:00:32   AOONWAL
#  No change.
#
#     Rev 1.24   28 Aug 1996 15:57:00   DCHI
#  Added NVXBTSHR.OBJ to list of objects.
#
#     Rev 1.23   05 Aug 1996 10:43:34   DCHI
#  Added excel repair source.
#
#     Rev 1.22   31 Jul 1996 01:49:06   DALLEE
#  Put back the "XOPTS += -Alfw" for .DX platform that was mistakenly
#  removed when backing out the CPU_TYPE addition.
#  .DX navex MUST compile large model with DS != SS due to the way navex.exp
#  is loaded.
#
#     Rev 1.21   25 Jul 1996 18:25:32   DCHI
#  Added EXCLSCAN.*.
#
#     Rev 1.20   08 Jul 1996 15:28:00   RSTANEV
#  Added a bunch of comments.
#
#     Rev 1.19   25 Jun 1996 16:30:58   JWILBER
#  Changed location of references to danish.cpp so it wouldn't get
#  built by itself, and make the compiler barf.
#
#     Rev 1.18   25 Jun 1996 16:11:30   JWILBER
#  Added DANISH.CPP to dependencies for NAVEXSP.CPP and NAVEXSB.CPP.
#
#     Rev 1.17   04 Jun 1996 12:20:26   CNACHEN
#  Added One Half repair support.
#
#     Rev 1.16   14 May 1996 12:18:18   JWILBER
#  Commented out XOPTS addition for CPU type because it makes the &*#%$
#  DOS command line too long in some cases.  The rule for CPP to COM
#  will have to be changed to make use of this.
#
#     Rev 1.15   13 May 1996 19:06:48   JWILBER
#  Added defines for SYM_CPU_X86, to flag endian dependencies for the
#  future.
#
#     Rev 1.14   24 Apr 1996 16:02:24   RSTANEV
#  No more linking to SYMKRNL.LIB and SYMEVENT.LIB.
#
#     Rev 1.13   19 Apr 1996 14:37:56   RSTANEV
#  SYM_NTK does not need /DNAVEX_DRIVER anymore.
#
#     Rev 1.12   17 Apr 1996 08:32:34   JMILLAR
#  add make for NTK (NT kernel)
#  add navexnt to the dependencies
#
#     Rev 1.11   04 Apr 1996 14:57:14   DCHI
#  Added JAVAOBJS to all sections.
#
#     Rev 1.10   13 Feb 1996 17:55:52   MKEATIN
#  Added DX Support
#
#     Rev 1.9   08 Feb 1996 10:37:00   DCHI
#  Added Windows repair source.
#
#     Rev 1.8   05 Feb 1996 17:21:54   DCHI
#  Added lines for compilation of Windows scanner component.
#
#     Rev 1.7   03 Jan 1996 17:15:24   DCHI
#  Modified for compilation of Word Macro scan/repair functionality.
#
#     Rev 1.6   01 Nov 1995 10:51:20   DCHI
#  Added compilation of navexshr.cpp to all modules.
#
#     Rev 1.5   26 Oct 1995 13:52:52   MLELE
#  Added commands to the .ASM.OBJ area of CHK.ASM
#
#     Rev 1.4   26 Oct 1995 13:23:08   MLELE
#  Added CHK.ASM for NLM platform.
#  This is reqd, since __CHK is not available on NetWare 3.11 CLibs
#
#     Rev 1.3   19 Oct 1995 14:20:08   DCHI
#  Modified DOS portion to generate tiny model modules for NAVEX.DAT.
#
#     Rev 1.2   18 Oct 1995 14:27:54   cnachen
#  Added make of NAVEX.NLM.
#
#     Rev 1.1   16 Oct 1995 12:59:44   DCHI
#  Added dependencies on navexhdr.h.
#
#     Rev 1.0   13 Oct 1995 13:04:12   DCHI
#  Initial revision.
#
#############################################################################

# Module objects

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

OBJS =  query.obj   \
        navexsf.obj \
        navexsb.obj \
        navexsp.obj \
        navexsm.obj \
        navexrf.obj \
        navexrb.obj \
        navexrp.obj \
        navexrh.obj \
        navexshr.obj\
        nvxbtshr.obj\
        navexent.obj\
        booscan.obj \
        bootype.obj \
        datadir.obj \
        inifile.obj \
        trojscan.obj \
        pespscan.obj \
        aolpw.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

%if ("$[u,$(NOMACRO)]" == "")

MACROLIBS = \
        crc32.lib    \
        olessapi.lib \
        wd7api.lib   \
        o97api.lib   \
        xl5api.lib   \
        macroheu.lib \
        acsapi.lib   \
        ppapi.lib    \
        df.lib

MSXOBJS = \
        msx.obj    \
        msxadd.obj \
        msxcrc.obj \
        msxdb.obj  \
        msxl1.obj  \
        msxl2.obj

MACROOBJS = \
        wddecsig.obj \
        ldmcrsig.obj \
        mcrscn15.obj \
        mcrrpr15.obj \
        offcsc15.obj \
        wdapsg15.obj \
        wdavsg15.obj \
        wdscan15.obj \
        wdrepr15.obj \
        wdsigutl.obj \
        mcrsgutl.obj \
        wd7scn15.obj \
        o97scn15.obj \
        xl97rpr.obj  \
        o97rpr15.obj \
        olessos.obj  \
        mcrheusc.obj \
        wd7heusc.obj \
        mvp.obj      \
        copy.obj     \
        builtin.obj  \
        mcrxclud.obj \
        w97heusc.obj \
        w97obj.obj   \
        w97str.obj   \
        xl5scn.obj   \
        xl5rpr.obj   \
        xl4scn.obj   \
        xl4rpr.obj   \
        xlrprsht.obj \
        a2scn.obj    \
        a97scn.obj   \
        acsos.obj    \
        w7h2sght.obj \
        w7h2scan.obj \
        excelheu.obj \
        xlheusc.obj  \
        dfos.obj     \
        ppscan.obj   \
        nvx15inf.obj \
        $(MSXOBJS)   \
        $(MACROLIBS)

%Else

XOPTS += /DNOMACRO

MACROOBJS = \
        ldmcrsig.obj \
        dfos.obj     \
        crc32.lib    \
        df.lib

%EndIf

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

NEPEOBJS = \
        nepescan.obj \
        nepeshr.obj \
        neperep.obj \
        remotex.obj \
        winsig.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

JAVAOBJS = \
        javascan.obj \
        javashr.obj \
        javasig.obj

XOPTS += /DNAVEX15

XOPTS += /DNDEBUG

# For SARCBOT builds only and not for weekly defs

%If ("$[u,$(SARCBOT)]" == "1")
    XOPTS += -DSARCBOT
%Endif

# For Zoo Def builds

%If ("$[u,$(ZOODEF)]" == "1")
    XOPTS += -DZOODEF
%Endif

####################################################
#                      DX                          #
####################################################

%If ("$[u,$(PLATFORM)]" == ".DX")

    TARGET_NAME=NAVEX15
    TARGET_TYPE=EXE
    PROGRAM_NAME=$(TARGET_NAME).EXE
#   XOPTS += -DSYM_CPU_X86
    XOPTS += -Alfw

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex15.obj    \
                 navxdx15.obj   \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     W 3 2                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".W32")

    TARGET_NAME=NAVEX32a
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex15.obj    \
                 naventry.obj   \
                 navex15.d32    \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     V X D                        #
####################################################


%ElseIf ("$[u,$(PLATFORM)]" == ".VXD")

    TARGET_NAME=NAVEX15
    TARGET_TYPE=VXD
    PROGRAM_NAME=$(TARGET_NAME).VXD
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex15.obj    \
                 vavex15.obj    \
                 vavex15.dvx    \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     A R A                        #
#                     A R U                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".ARA") || ("$[u,$(PLATFORM)]" == ".ARU")

    TARGET_NAME=NAVEXda
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex15.obj    \
                 naventry.obj   \
                 navex15.d32    \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     N T K                        #
####################################################


%ElseIf ("$[u,$(PLATFORM)]" == ".NTK")

    TARGET_NAME=NAVEX15
    TARGET_TYPE=SYS
    PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

XOPTS+= /Gs10000

LINKOPTSExt1=-MERGE:_PAGE=PAGE -MERGE:_TEXT=.text
LINKOPTSExt2=-MERGE:.rdata=.text
LINKOPTSExt3=-OPT:REF
LINKOPTSExt4=-RELEASE -INCREMENTAL:NO -FULLBUILD -FORCE:MULTIPLE -debug:NONE
LINKOPTSExt5=-version:3.51 -osversion:3.51 -align:0x20 -subsystem:native,3.51
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry@8

SYSLIBS=

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

$(PROGRAM_NAME): \
                 NAVXNT15.obj   \
                 NAVEX15.obj    \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)    \
                 ntoskrnl.lib   \
                 hal.lib        \
                 int64.lib      \
                 symkrnl.lib


####################################################
#                    A R U K                       #
####################################################


%ElseIf ("$[u,$(PLATFORM)]" == ".ARUK")

    TARGET_NAME=NAVEXDA
    TARGET_TYPE=SYS
    PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

XOPTS+= /Gs10000

LINKOPTSExt1=-MERGE:_PAGE=PAGE -MERGE:_TEXT=.text
LINKOPTSExt2=-MERGE:.rdata=.text
LINKOPTSExt3=-OPT:REF
LINKOPTSExt4=-RELEASE -INCREMENTAL:NO -FULLBUILD -FORCE:MULTIPLE -debug:NONE
LINKOPTSExt5=-version:3.51 -osversion:3.51 -align:0x20 -subsystem:native,3.51
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry

SYSLIBS=

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX15.NTK !!!

$(PROGRAM_NAME): \
                 NAVXNT15.obj   \
                 NAVEX15.obj    \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)    \
                 ntoskrnl.lib   \
                 hal.lib        \
                 symkrnl.lib


####################################################
#                     W I N                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")

    TARGET_NAME =NAVEX16a
    TARGET_TYPE =DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
    RCBINDOPTS  =-K -T -31

%If ("$[u,$(QUAKE)]" == "QAK2")
.INCLUDE $(QUAKEINC)\WINDLLC7.MAK
%Else
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK
%EndIf

$(PROGRAM_NAME): navex15.obj    \
                 naventry.obj   \
                 navex16a.def   \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     N L M                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")

    TARGET_NAME=NAVEX15
    TARGET_TYPE=NLM
    PROGRAM_NAME=$(TARGET_NAME).NLM
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
#.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navexnlm.obj   \
                 navex15.obj    \
                 chk.obj        \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)    \
                 NAV.EXP        \
                 NAV.IMP        \
                 NAVEX15.DEN

# Must compile CHK.ASM without debug information regardless of the CV
# environment variable...

chk.obj:         chk.asm
                 %set ERRF=$[r,$*].err
                 %do bCOPYLOCAL
                 %echo   ML $[r,$<].$[e,$<] (/W3 /WX /nologo /c -DSYM_NLM)
                 ML /W3 /WX /nologo /c -DSYM_NLM $[r,$<].$[e,$<] >>$(ERRF) $(OS2ERR)
                 %do bERROUT

%Else
BAD_PLATFORM:
    beep
    Echo '$(PLATFORM)' is not yet supported by the NAVEX makefile.
%EndIf

%If ("$[u,$(QUAKE)]" == "QAK2")

#UPDATE#
vavex.obj:    platform.inc
navex.obj:    platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
naventry.obj: platform.h
navexsf.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexsb.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h danish.cpp
navexsp.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h danish.cpp
navexsm.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexrf.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexrb.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexrp.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexrh.obj:  platform.h cts4.h navex.h navex15.h callbk15.h navexshr.h
navexshr.obj: platform.h navexshr.h
navexent.obj: platform.h navex15.h callbk15.h
#ENDUPDATE#

%Else

#UPDATE#
nechild.obj:  platform.h ctsn.h navex.h navex15.h callbk15.h navexshr.h
vavex.obj:    platform.inc
navex.obj:    platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
naventry.obj: platform.h
navexsf.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexsb.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexsp.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexsm.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexrf.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexrb.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexrp.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexrh.obj:  platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
navexshr.obj: platform.h navexshr.h
navexnt.obj:  platform.h avapi.h ctsn.h callback.h navex.h navex15.h callbk15.h
navexent.obj: platform.h navex15.h callbk15.h
booscan.obj: platform.h bootype.h boodata.h boodefs.h
bootype.obj: platform.h bootype.h
trojscan.obj: trojscan.h trojsigs.h platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
#crc32.obj:    platform.h crc32.h

# Special handling for macro virus scan and repair code

%If ("$[u,$(PLATFORM)]" == ".DOS")

docscan.obj:  macrscan.cpp mcrscndf.cpp storobj.cpp worddoc.cpp exclscan.cpp \
              macrscan.h mcrscndf.h storobj.h worddoc.h exclscan.h \
              excelshr.cpp excelshr.h

docrepar.obj: macrodel.cpp macrorep.cpp mcrscndf.cpp mcrrepdf.cpp \
              olewrite.cpp storobj.cpp worddoc.cpp \
              macrodel.h macrorep.h mcrscndf.h mcrrepdf.h \
              olewrite.h storobj.h worddoc.h
              excelshr.cpp excelshr.h excelrep.cpp excelrep.h

nepescan.obj: nepescan.cpp nepeshr.cpp winsig.cpp \
              platform.h navexshr.h callback.h nepeshr.h \
              nepescan.h winconst.h winsig.h

neperep.obj:  neperep.cpp nepeshr.cpp \
              platform.h navexshr.h callback.h nepeshr.h \
              neperep.h winconst.h winsig.h

%Else

wdapsig.obj : platform.h callback.h storobj.h wdapsig.h
wdapvsig.obj: platform.h callback.h storobj.h wdapvsig.h
wdencdoc.obj: platform.h callback.h storobj.h wdencdoc.h
wdencrd.obj : platform.h callback.h storobj.h wdencrd.h
wdencwr.obj : platform.h callback.h storobj.h wdencwr.h
wdscan.obj  : platform.h callback.h storobj.h wdscan.h
wdrepair.obj: platform.h callback.h storobj.h wdrepair.h
wdsigutl.obj: platform.h callback.h storobj.h wdsigutl.h
wd7scan.obj: platform.h callback.h storobj.h wd7scan.h mcrhitmm.h
offcscan.obj: platform.h callback.h storobj.h offcscan.h
vba5rep.obj: platform.h callback.h storobj.h vba5scan.h mcrhitmm.h
vba5scan.obj: platform.h callback.h storobj.h vba5rep.h mcrhitmm.h
vbalznt.obj: platform.h callback.h storobj.h vbalznt.h
mcrhitmm.obj: mcrhitmm.h

macrodel.obj: platform.h callback.h storobj.h worddoc.h macrodel.h
macrorep.obj: platform.h callback.h storobj.h worddoc.h macrorep.h macrodel.h wd7sigs.h mcrhitmm.h
macrscan.obj: platform.h callback.h storobj.h worddoc.h macrscan.h
storobj.obj:  platform.h callback.h storobj.h navexshr.h
worddoc.obj:  platform.h callback.h storobj.h worddoc.h
exclscan.obj: platform.h callback.h storobj.h exclscan.h excelrep.h
excelshr.obj: excelshr.h
excelrep.obj: excelshr.h excelrep.h

nepescan.obj: platform.h navexshr.h callback.h nepescan.h winconst.h winsig.h
neperep.obj: platform.h navexshr.h callback.h neperep.h winconst.h
winsig.obj:   platform.h navexshr.h winsig.h
pespscan.obj: endutils.h nepescan.h winsig.h winconst.h nepeshr.h pespscan.h navexvid.h

javascan.obj: platform.h navexshr.h callback.h javascan.h javashr.h javasig.h
javasig.obj:  platform.h navexshr.h javasig.h

%EndIf

#ENDUPDATE#

%EndIf
