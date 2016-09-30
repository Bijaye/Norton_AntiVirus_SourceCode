#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/navex.mav   1.102   24 Dec 1998 23:16:06   DCHI  $
#
#  Description:
#       This is a make file for NAVEX.
#
#  See Also:
#       VIRSCAN and AVAPI projects.
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/navex.mav  $
#  
#     Rev 1.102   24 Dec 1998 23:16:06   DCHI
#  Added remotex.obj and necessary modifications to support it.
#  
#     Rev 1.101   24 Dec 1998 07:03:16   relniti
#  dfos.obj into NLM, DX, NTK, and VXD
#  
#     Rev 1.100   15 Dec 1998 12:15:22   DCHI
#  Added P97 sigs and PowerPoint scanning to WIN platforms.
#  
#     Rev 1.99   08 Dec 1998 13:28:16   DCHI
#  Changes for MSX.
#  
#     Rev 1.98   09 Nov 1998 13:58:38   DCHI
#  Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
#  
#     Rev 1.97   26 Oct 1998 13:31:14   DCHI
#  Changes to support MKMCRSIG multiple part data file generation
#  and removal of -K option for WIN (causes problems for DOS TSR).
#  
#     Rev 1.96   22 Oct 1998 10:47:20   DCHI
#  Added -K option to RCDEF variable under WIN platform.
#  
#     Rev 1.95   12 Oct 1998 10:59:42   CNACHEN
#  Added support for AOL pws heuristics.
#  
#  
#     Rev 1.94   09 Oct 1998 12:17:54   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.93   21 Sep 1998 17:36:38   JWILBER
#  Added support for Word macro sig files for foreign offices.
#
#     Rev 1.92   09 Sep 1998 12:45:26   DCHI
#  Changes for password-protected Excel 97 document repair.
#
#     Rev 1.91   04 Aug 1998 14:05:48   DCHI
#  Changes for Excel heuristics.
#
#     Rev 1.90   27 Jul 1998 12:59:18   DKESSNE
#  added pespscan.obj
#
#     Rev 1.89   10 Jun 1998 13:15:06   DCHI
#  Added W7H2
#
#     Rev 1.88   09 Jun 1998 23:03:50   JWILBER
#  Added handling for ZOODEF environment variable.
#
#     Rev 1.87   02 Jun 1998 14:04:12   DCHI
#
#     Rev 1.86   11 May 1998 18:23:04   DCHI
#  Added Access 2.0 (A2) and Access 97 (A97) sections.
#
#     Rev 1.85   17 Apr 1998 13:45:00   DCHI
#  Added acsapi.lib, acsos.obj, and a97scn.obj.
#
#     Rev 1.84   16 Apr 1998 21:14:40   RELNITI
#  CHANGE XLVARSIG.WDF to XL97VAR.WDF for XL97SIGS
#
#     Rev 1.83   15 Apr 1998 17:23:54   DCHI
#  Modifications for new XL95 and XL5 engines.
#
#     Rev 1.82   15 Apr 1998 10:57:04   RELNITI
#  ADD XLNAMSIG.WDF to XL97SIGS build
#  CHANGE WDVARSIG.WDF to XLVARSIG.WDF in XL97SIGS build
#
#     Rev 1.81   30 Mar 1998 13:51:08   AOONWAL
#  Added wd7sigs6.wdf
#
#     Rev 1.80   10 Mar 1998 17:13:52   CNACHEN
#  Added trojan scanning support on all platforms.
#
#     Rev 1.79   03 Mar 1998 12:20:36   CFORMUL
#  Added a -DSARCBOT to XOPTS if this is a SARBOT build
#
#     Rev 1.78   03 Mar 1998 10:45:46   JWILBER
#  Fixed small typo.
#
#     Rev 1.77   02 Mar 1998 21:46:08   JWILBER
#  Removed references for macro definition files for foreign offices.
#
#     Rev 1.76   10 Feb 1998 21:29:20   JWILBER
#  Fixed typo so *.cpy files would be removed correctly.
#
#     Rev 1.75   10 Feb 1998 21:17:50   JWILBER
#  Added name sig files for foreign offices.
#
#     Rev 1.74   09 Feb 1998 19:56:22   JWILBER
#  Made changes due to additions of *.wdf and *.h files for foreign
#  offices.
#
#     Rev 1.73   29 Jan 1998 19:03:12   DCHI
#  Added xl4mscan.obj and xl4mrep.obj
#
#     Rev 1.72   14 Jan 1998 18:19:14   CNACHEN
#  Updated Trojan Scan to only be used for Picasso and NAV 4.0 products
#
#     Rev 1.71   13 Jan 1998 17:07:54   FBARAJA
#  Enabled Picasso File Heuristics uncommented heurdx.lib and heur16.lib
#
#     Rev 1.70   13 Jan 1998 17:04:20   FBARAJA
#  Enabled Picasso File Heuristics " XOPTS +=/DUSE_BLOODHOUD_PICASSO "
#
#     Rev 1.69   09 Jan 1998 03:56:02   RELNITI
#  OOPS.. re-commenting CRC32.OBJ
#
#     Rev 1.68   09 Jan 1998 03:50:00   RELNITI
#  Set Trojan Scanning
#
#     Rev 1.67   09 Jan 1998 02:58:20   RELNITI
#  Set Trojan Scanning
#
#     Rev 1.66   29 Dec 1997 17:59:34   JWILBER
#  Modified during Jan98 build.
#
#     Rev 1.65   22 Dec 1997 18:33:22   JWILBER
#  Rolled this back to 1.63 to remove the trojan sigs.
#
#     Rev 1.63   24 Nov 1997 18:21:44   DCHI
#  Added LINKOPTS += /PACKCODE:49152 for WIN16.
#
#     Rev 1.62   13 Nov 1997 12:50:46   JWILBER
#  Commented out the addition of the -K option to the RCDEF environment
#  variable for Win16 builds.  RC for Win16 won't use this option
#  anymore.
#
#     Rev 1.61   10 Nov 1997 16:48:00   CNACHEN
#  Properly added bloodhound boot source files as dependencies.
#
#     Rev 1.60   07 Nov 1997 10:56:20   DCHI
#  Added xl5api.lib.
#
#     Rev 1.59   05 Nov 1997 12:04:54   DCHI
#  Added mcrheuen.obj.
#
#     Rev 1.58   04 Nov 1997 16:50:10   CNACHEN
#  Fixed build problem.
#
#     Rev 1.57   04 Nov 1997 16:33:48   CNACHEN
#  Commented out bloodhound for the time being...
#
#     Rev 1.56   04 Nov 1997 15:56:32   CNACHEN
#  Added support for bloodhound to picasso.
#
#     Rev 1.55   31 Oct 1997 10:31:06   DCHI
#  Changed mkwrdsig to mkmcrsig.
#
#     Rev 1.54   27 Oct 1997 12:44:46   JWILBER
#  Added -K option to RCDEF environment variable.
#
#     Rev 1.53   16 Oct 1997 13:26:26   DCHI
#  Removed crc.obj from object list, because crc32.lib is used instead.
#
#     Rev 1.52   15 Oct 1997 18:15:06   DCHI
#  Added Word 97 heuristics files.
#
#     Rev 1.51   15 Oct 1997 10:21:02   CNACHEN
#  Commented out trojscan for this month.
#
#     Rev 1.50   09 Oct 1997 17:13:50   CNACHEN
#  Added support for TROJSCAN...
#
#     Rev 1.49   02 Oct 1997 17:08:32   AOONWAL
#  Added WD7SIGS5.WDF support
#
#     Rev 1.48   18 Sep 1997 13:56:46   DDREW
#  One more time (maybe)
#
#     Rev 1.47   18 Sep 1997 13:13:10   DDREW
#  Changed mvp.obj back the way it was in the beginning
#
#     Rev 1.46   18 Sep 1997 11:56:18   JWILBER
#  Made another fix to the NLM build.
#
#     Rev 1.45   18 Sep 1997 11:46:34   JWILBER
#  Took mvp.obj out of NLM build only so we can link OK.
#
#     Rev 1.44   05 Sep 1997 21:08:36   CNACHEN
#  Fixed # comment problem
#
#     Rev 1.43   05 Sep 1997 20:35:18   CNACHEN
#  Now supports boot heuristics.
#
#     Rev 1.42   24 Jul 1997 17:07:18   DCHI
#  Added -qaHEU to compilation of WD7 signatures.
#
#     Rev 1.41   16 Jul 1997 17:31:30   DCHI
#  Added XOPTS += /DMACROHEU for WIN and DX.
#
#     Rev 1.40   09 Jul 1997 16:54:30   DCHI
#  Changes supporting new OLE API.
#
#     Rev 1.39   23 Jun 1997 14:58:20   AOONWAL
#  Modified during JULY97 build
#
#     Rev JULY97  16 Jun 1997 17:09:52   ECHIEN
#  Added wd7sigs4
#
#     Rev 1.38   27 May 1997 16:51:12   DCHI
#  Modifications to support dynamic allocation of hit memory and new
#  Office 97 CRC.
#
#     Rev 1.37   08 May 1997 13:32:58   DCHI
#  Added wdvarsig.wdf to WD8 sig set.
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

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

OBJS =  navexsf.obj     \
        navexsb.obj     \
        navexsp.obj     \
        navexsm.obj     \
        navexrf.obj     \
        navexrb.obj     \
        navexrp.obj     \
        navexrh.obj     \
        navexshr.obj    \
        nvxbtshr.obj    \
        booscan.obj     \
        bootype.obj     \
        trojscan.obj    \
        pespscan.obj    \
        aolpw.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

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

MACROSIGS = \
        wd7sigs0.obj \
        wd7sigs1.obj \
        wd7sigs2.obj \
        wd7sigs3.obj \
        wd8sigs0.obj \
        wd8sigs1.obj \
        wd8sigs2.obj \
        wd8sigs3.obj \
        x97sigs0.obj \
        x97sigs1.obj \
        x97sigs2.obj \
        x97sigs3.obj \
        x95sigs0.obj \
        x95sigs1.obj \
        x95sigs2.obj \
        x95sigs3.obj \
        xl4sigs0.obj \
        xl4sigs1.obj \
        xl4sigs2.obj \
        xl4sigs3.obj \
        wd7xclsg.obj \
        x95xclsg.obj \
        wd8xclsg.obj \
        x97xclsg.obj \
        a2sigs0.obj  \
        a2sigs1.obj  \
        a2sigs2.obj  \
        a2sigs3.obj  \
%If ("$[u,$(PLATFORM)]" == ".WIN" || "$[u,$(PLATFORM)]" == ".W32")
        p97sigs0.obj \
        p97sigs1.obj \
        p97sigs2.obj \
        p97sigs3.obj \
%EndIf
        a97sigs0.obj \
        a97sigs1.obj \
        a97sigs2.obj \
        a97sigs3.obj

MACROOBJS = \
        wddecsig.obj \
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
        w7h2sigs.obj \
        excelheu.obj \
        xlheusc.obj  \
        dfos.obj     \
        $(MACROSIGS) \
        $(MACROLIBS)

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

NEPEOBJS = \
        nepescan.obj \
        nepeshr.obj \
        neperep.obj \
        remotex.obj \
        winsig.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

JAVAOBJS = \
        javascan.obj \
        javashr.obj \
        javasig.obj

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
#                     D O S                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".DOS")

    MODEL=T
    TARGET_NAME=NAVEX
    TARGET_TYPE=DAT
    PROGRAM_NAME=$(TARGET_NAME).DAT
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

navex.dat:      navexsf.com \
                navexsb.com \
                navexsp.com \
                navexsm.com \
                navexrf.com \
                navexrb.com \
                navexrp.com

navexsf.com:    navexsf.cpp \
                navexshr.cpp \
                docscan.cpp \
                winscan.cpp \
                java.cpp

navexsb.com:    navexsb.cpp \
                navexshr.cpp \
                nvxbtshr.cpp

navexsp.com:    navexsp.cpp \
                navexshr.cpp \
                nvxbtshr.cpp

navexsm.com:    navexsm.cpp \
                navexshr.cpp

navexrf.com:    navexrf.cpp \
                navexshr.cpp \
                docrepar.cpp \
                winrep.cpp

navexrb.com:    navexrb.cpp \
                navexshr.cpp \
                nvxbtshr.cpp

navexrp.com:    navexrp.cpp \
                navexshr.cpp \
                navexrh.cpp \
                nvxbtshr.cpp


####################################################
#                      DX                          #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")

    TARGET_NAME=NAVEX
    TARGET_TYPE=EXE
    PROGRAM_NAME=$(TARGET_NAME).EXE
#   XOPTS += -DSYM_CPU_X86
    XOPTS += -Alfw
    XOPTS += /DMACROHEU
    XOPTS += /DUSE_BLOODHOUND_PICASSO

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navexdx.obj    \
                 inifile.obj    \
                 nvx15inf.obj   \
                 heur.obj       \
                 callfake.obj   \
                 heurdx.lib     \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)



####################################################
#                     W 3 2                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".W32")

    TARGET_NAME=NAVEX32
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex.obj      \
                 naventry.obj   \
                 navex32.d32    \
                 inifile.obj    \
                 nvx15inf.obj   \
                 ppscan.obj     \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     V X D                        #
####################################################


%ElseIf ("$[u,$(PLATFORM)]" == ".VXD")

    TARGET_NAME=NAVEX
    TARGET_TYPE=VXD
    PROGRAM_NAME=$(TARGET_NAME).VXD
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex.obj      \
                 vavex.obj      \
                 vavex.dvx      \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)


####################################################
#                     N T K                        #
####################################################


%ElseIf ("$[u,$(PLATFORM)]" == ".NTK")

    TARGET_NAME=NAVEX
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

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

$(PROGRAM_NAME): \
                 NAVEXNT.obj    \
                 NAVEX.obj      \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)    \
                 ntoskrnl.lib   \
                 hal.lib        \
                 int64.lib      \
                 symkrnl.lib


####################################################
#                     W I N                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")

    TARGET_NAME=NAVEX16
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
#   XOPTS += /DSYM_CPU_X86
    XOPTS += /DMACROHEU
    XOPTS += /DUSE_BLOODHOUND_PICASSO
#   RCDEF += -K


%If ("$[u,$(QUAKE)]" == "QAK2")
.INCLUDE $(QUAKEINC)\WINDLLC7.MAK
%Else
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK
%EndIf

LINKOPTS += /PACKCODE:49152

$(PROGRAM_NAME): navex.obj      \
                 naventry.obj   \
                 navex16.def    \
                 inifile.obj    \
                 nvx15inf.obj   \
                 ppscan.obj     \
                 heur16.lib     \
                 heur.obj       \
                 callfake.obj   \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)



####################################################
#                     N L M                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")

    TARGET_NAME=NAVEX
    TARGET_TYPE=NLM
    PROGRAM_NAME=$(TARGET_NAME).NLM
#   XOPTS += /DSYM_CPU_X86

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
#.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex.obj      \
                 nechild.obj    \
                 chk.obj        \
                 $(OBJS)        \
                 $(MACROOBJS)   \
                 $(NEPEOBJS)    \
                 $(JAVAOBJS)    \
                 NAV.IMP        \
                 NAVEX.DEN

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

# Word document virus signatures

wd7sigs0.cpp: wd7sigs.lst  \
              wdnamsig.wdf \
              wdausnam.wdf \
              wdeurnam.wdf \
              wdjapnam.wdf \
              wd7sigs2.wdf \
              wd7sigs3.wdf \
              wd7sigs4.wdf \
              wd7sigs5.wdf \
              wd7sigs6.wdf \
              wd7sigs7.wdf \
              wdaussig.wdf \
              wdeursig.wdf \
              wdjapsig.wdf \
              wdvarsig.wdf \
              wd7sigs.wdf
              if exist wd7sigsf.err del wd7sigsf.err
              if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
              if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
              if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
              if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
              if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
              if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
              if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
              if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
              if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigs8.cpy
              if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd7sigs9.cpy
              if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd7sigsa.cpy
              if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd7sigsb.cpy
              if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd7sigsc.cpy
              if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd7sigsd.cpy
              if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd7sigse.cpy
              if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsF.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
              if errorlevel ne 0 type wd7sigsf.err
              if errorlevel eq 0 del wd7sigsf.err > nul
              if exist wd7sigs0.cpy del wd7sigs.lst wd7sigs0.cpy
              if exist wd7sigs1.cpy del wdnamsig.wdf wd7sigs1.cpy
              if exist wd7sigs2.cpy del wd7sigs2.wdf wd7sigs2.cpy
              if exist wd7sigs3.cpy del wd7sigs3.wdf wd7sigs3.cpy
              if exist wd7sigs4.cpy del wdvarsig.wdf wd7sigs4.cpy
              if exist wd7sigs5.cpy del wd7sigs.wdf wd7sigs5.cpy
              if exist wd7sigs6.cpy del wd7sigs4.wdf wd7sigs6.cpy
              if exist wd7sigs7.cpy del wd7sigs5.wdf wd7sigs7.cpy
              if exist wd7sigs8.cpy del wd7sigs6.wdf wd7sigs8.cpy
              if exist wd7sigs9.cpy del wdaussig.wdf wd7sigs9.cpy
              if exist wd7sigsa.cpy del wdeursig.wdf wd7sigsa.cpy
              if exist wd7sigsb.cpy del wdjapsig.wdf wd7sigsb.cpy
              if exist wd7sigsc.cpy del wdausnam.wdf wd7sigsc.cpy
              if exist wd7sigsd.cpy del wdeurnam.wdf wd7sigsd.cpy
              if exist wd7sigse.cpy del wdjapnam.wdf wd7sigse.cpy
              if exist wd7sigsF.cpy del wd7sigs7.wdf wd7sigsF.cpy

wd7sigs1.cpp: wd7sigs0.cpp
wd7sigs2.cpp: wd7sigs0.cpp
wd7sigs3.cpp: wd7sigs0.cpp

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdjapsig.wdf:
wdnamsig.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapnam.wdf:
wdvarsig.wdf:

wd8sigs0.cpp: wd8sigs.lst  \
              wdnamsig.wdf \
              wdausnam.wdf \
              wdeurnam.wdf \
              wdjapnam.wdf \
              wd7sigs.wdf  \
              wd7sigs2.wdf \
              wd7sigs3.wdf \
              wd7sigs4.wdf \
              wd7sigs5.wdf \
              wd7sigs6.wdf \
              wd7sigs7.wdf \
              wdaussig.wdf \
              wdeursig.wdf \
              wdjapsig.wdf \
              wd8sigs.wdf  \
              wdvarsig.wdf
              if exist wd8sigsf.err del wd8sigsf.err
              if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
              if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd8sigs1.cpy
              if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd8sigs2.cpy
              if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd8sigs3.cpy
              if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd8sigs4.cpy
              if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
              if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd8sigs6.cpy
              if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd8sigs7.cpy
              if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd8sigs8.cpy
              if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd8sigs9.cpy
              if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd8sigsa.cpy
              if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd8sigsb.cpy
              if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd8sigsc.cpy
              if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd8sigsd.cpy
              if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd8sigse.cpy
              if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd8sigsf.cpy
              if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sig10.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs 컴컴> wd8sigsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs > wd8sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
              if errorlevel ne 0 type wd8sigsf.err
              if errorlevel eq 0 del wd8sigsf.err > nul
              if exist wd8sigs0.cpy del wd8sigs.lst wd8sigs0.cpy
              if exist wd8sigs1.cpy del wdnamsig.wdf wd8sigs1.cpy
              if exist wd8sigs2.cpy del wd7sigs.wdf wd8sigs2.cpy
              if exist wd8sigs3.cpy del wd7sigs2.wdf wd8sigs3.cpy
              if exist wd8sigs4.cpy del wd7sigs3.wdf wd8sigs4.cpy
              if exist wd8sigs5.cpy del wd8sigs.wdf wd8sigs5.cpy
              if exist wd8sigs6.cpy del wdvarsig.wdf wd8sigs6.cpy
              if exist wd8sigs7.cpy del wd7sigs4.wdf wd8sigs7.cpy
              if exist wd8sigs8.cpy del wd7sigs5.wdf wd8sigs8.cpy
              if exist wd8sigs9.cpy del wd7sigs6.wdf wd8sigs9.cpy
              if exist wd8sigsa.cpy del wdaussig.wdf wd8sigsa.cpy
              if exist wd8sigsb.cpy del wdeursig.wdf wd8sigsb.cpy
              if exist wd8sigsc.cpy del wdjapsig.wdf wd8sigsc.cpy
              if exist wd8sigsd.cpy del wdausnam.wdf wd8sigsd.cpy
              if exist wd8sigse.cpy del wdeurnam.wdf wd8sigse.cpy
              if exist wd8sigsf.cpy del wdjapnam.wdf wd8sigsf.cpy
              if exist wd8sig10.cpy del wd7sigs7.wdf wd8sig10.cpy

wd8sigs1.cpp: wd8sigs0.cpp
wd8sigs2.cpp: wd8sigs0.cpp
wd8sigs3.cpp: wd8sigs0.cpp

wdnamsig.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapnam.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdjapsig.wdf:
wd8sigs.wdf:
wdvarsig.wdf:

x97sigs0.cpp: xl97sigs.lst \
              xlnamsig.wdf \
              xl97var.wdf \
              xl97sigs.wdf
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist XL97VAR.WDF copy %proj%\src\XL97VAR.WDF > xl97sgs2.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs 컴컴> xl97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del XL97VAR.WDF xl97sgs2.cpy
              if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy

x97sigs1.cpp: x97sigs0.cpp
x97sigs2.cpp: x97sigs0.cpp
x97sigs3.cpp: x97sigs0.cpp

xlnamsig.wdf:
xl97sigs.wdf:
XL97VAR.WDF:

x95sigs0.cpp: xl95sigs.lst \
              xlnamsig.wdf \
              xl95sigs.wdf \
              xlvarsig.wdf
              if exist xl95sgsf.err del xl95sgsf.err
              if not exist xl95sigs.lst copy %proj%\src\xl95sigs.lst > xl95sgs0.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xlnamsig.cpy
              if not exist xl95sigs.wdf copy %proj%\src\xl95sigs.wdf > xl95sgs1.cpy
              if not exist xlvarsig.wdf copy %proj%\src\xlvarsig.wdf > xl95sgs2.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs 컴컴> xl95sgsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs > xl95sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl95sgsf.err
              if errorlevel ne 0 type xl95sgsf.err
              if errorlevel eq 0 del xl95sgsf.err > nul
              if exist xl95sgs0.cpy del xl95sigs.lst xl95sgs0.cpy
              if exist xlnamsig.cpy del xlnamsig.wdf xlnamsig.cpy
              if exist xl95sgs1.cpy del xl95sigs.wdf xl95sgs1.cpy
              if exist xl95sgs2.cpy del xlvarsig.wdf xl95sgs2.cpy

x95sigs1.cpp: x95sigs0.cpp
x95sigs2.cpp: x95sigs0.cpp
x95sigs3.cpp: x95sigs0.cpp

xlnamsig.wdf:
xl95sigs.wdf:
xlvarsig.wdf:

xl4sigs0.cpp: xl4sigs.lst \
              xl4sigs.wdf
              if exist xl4sgsf.err del xl4sgsf.err
              if not exist xl4sigs.lst copy %proj%\src\xl4sigs.lst > xl4sgs0.cpy
              if not exist xl4sigs.wdf copy %proj%\src\xl4sigs.wdf > xl4sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs 컴컴> xl4sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs
              mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs > xl4sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl4sgsf.err
              if errorlevel ne 0 type xl4sgsf.err
              if errorlevel eq 0 del xl4sgsf.err > nul
              if exist xl4sgs0.cpy del xl4sigs.lst xl4sgs0.cpy
              if exist xl4sgs1.cpy del xl4sigs.wdf xl4sgs1.cpy

xl4sigs1.cpp: xl4sigs0.cpp
xl4sigs2.cpp: xl4sigs0.cpp
xl4sigs3.cpp: xl4sigs0.cpp

xl4sigs.wdf:

wd7xclsg.cpp: wd7xclsg.xdf
              if exist wd7xclsg.err del wd7xclsf.err
              if not exist wd7xclsg.xdf copy %proj%\src\wd7xclsg.xdf > wd7xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg 컴컴> wd7xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg
              mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg > wd7xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7xclsf.err
              if errorlevel ne 0 type wd7xclsf.err
              if errorlevel eq 0 del wd7xclsf.err > nul
              if exist wd7xclsg.cpy del wd7xclsg.xdf wd7xclsg.cpy

wd7xclsg.xdf:

x95xclsg.cpp: x95xclsg.xdf
              if exist x95xclsg.err del x95xclsf.err
              if not exist x95xclsg.xdf copy %proj%\src\x95xclsg.xdf > x95xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyXL95 x95xclsg.xdf -sFAR_DATA_SIGS x95xclsg 컴컴> x95xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyXL95 x95xclsg.xdf -sFAR_DATA_SIGS x95xclsg
              mkxclsig.exe -qaPASSED -tyXL95 x95xclsg.xdf -sFAR_DATA_SIGS x95xclsg > x95xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>x95xclsf.err
              if errorlevel ne 0 type x95xclsf.err
              if errorlevel eq 0 del x95xclsf.err > nul
              if exist x95xclsg.cpy del x95xclsg.xdf x95xclsg.cpy

x95xclsg.xdf:

wd8xclsg.cpp: wd8xclsg.xdf
              if exist wd8xclsg.err del wd8xclsf.err
              if not exist wd8xclsg.xdf copy %proj%\src\wd8xclsg.xdf > wd8xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg 컴컴> wd8xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg
              mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg > wd8xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8xclsf.err
              if errorlevel ne 0 type wd8xclsf.err
              if errorlevel eq 0 del wd8xclsf.err > nul
              if exist wd8xclsg.cpy del wd8xclsg.xdf wd8xclsg.cpy

wd8xclsg.xdf:

x97xclsg.cpp: x97xclsg.xdf
              if exist x97xclsg.err del x97xclsf.err
              if not exist x97xclsg.xdf copy %proj%\src\x97xclsg.xdf > x97xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyXL97 x97xclsg.xdf -sFAR_DATA_SIGS x97xclsg 컴컴> x97xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyXL97 x97xclsg.xdf -sFAR_DATA_SIGS x97xclsg
              mkxclsig.exe -qaPASSED -tyXL97 x97xclsg.xdf -sFAR_DATA_SIGS x97xclsg > x97xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>x97xclsf.err
              if errorlevel ne 0 type x97xclsf.err
              if errorlevel eq 0 del x97xclsf.err > nul
              if exist x97xclsg.cpy del x97xclsg.xdf x97xclsg.cpy

x97xclsg.xdf:

a2sigs0.cpp:  a2sigs.lst \
              a2sigs.wdf
              if exist a2sgsf.err del a2sgsf.err
              if not exist a2sigs.lst copy %proj%\src\a2sigs.lst > a2sgs0.cpy
              if not exist a2sigs.wdf copy %proj%\src\a2sigs.wdf > a2sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs 컴컴> a2sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs
              mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs > a2sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>a2sgsf.err
              if errorlevel ne 0 type a2sgsf.err
              if errorlevel eq 0 del a2sgsf.err > nul
              if exist a2sgs0.cpy del a2sigs.lst a2sgs0.cpy
              if exist a2sgs1.cpy del a2sigs.wdf a2sgs1.cpy

a2sigs.wdf:

a2sigs1.cpp:  a2sigs0.cpp
a2sigs2.cpp:  a2sigs0.cpp
a2sigs3.cpp:  a2sigs0.cpp

a97sigs0.cpp: a97sigs.lst \
              a97sigs.wdf
              if exist a97sgsf.err del a97sgsf.err
              if not exist a97sigs.lst copy %proj%\src\a97sigs.lst > a97sgs0.cpy
              if not exist a97sigs.wdf copy %proj%\src\a97sigs.wdf > a97sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs 컴컴> a97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs
              mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs > a97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>a97sgsf.err
              if errorlevel ne 0 type a97sgsf.err
              if errorlevel eq 0 del a97sgsf.err > nul
              if exist a97sgs0.cpy del a97sigs.lst a97sgs0.cpy
              if exist a97sgs1.cpy del a97sigs.wdf a97sgs1.cpy

a97sigs1.cpp: a97sigs0.cpp
a97sigs2.cpp: a97sigs0.cpp
a97sigs3.cpp: a97sigs0.cpp

a97sigs.wdf:

%If ("$[u,$(PLATFORM)]" == ".WIN" || "$[u,$(PLATFORM)]" == ".W32")

p97sigs0.cpp: p97sigs.lst \
              p97sigs.wdf
              if exist p97sgsf.err del p97sgsf.err
              if not exist p97sigs.lst copy %proj%\src\p97sigs.lst > p97sgs0.cpy
              if not exist p97sigs.wdf copy %proj%\src\p97sigs.wdf > p97sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyP97 -lp97sigs.lst -sFAR_DATA_SIGS p97sigs 컴컴> p97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyP97 -lp97sigs.lst -sFAR_DATA_SIGS p97sigs
              mkmcrsig.exe -qaPASSED -tyP97 -lp97sigs.lst -sFAR_DATA_SIGS p97sigs > p97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>p97sgsf.err
              if errorlevel ne 0 type p97sgsf.err
              if errorlevel eq 0 del p97sgsf.err > nul
              if exist p97sgs0.cpy del p97sigs.lst p97sgs0.cpy
              if exist p97sgs1.cpy del p97sigs.wdf p97sgs1.cpy

p97sigs1.cpp: p97sigs0.cpp
p97sigs2.cpp: p97sigs0.cpp
p97sigs3.cpp: p97sigs0.cpp

p97sigs.wdf:

%EndIf

%If ("$[u,$(QUAKE)]" == "QAK2")

#UPDATE#
vavex.obj:    platform.inc
navex.obj:    platform.h cts4.h navex.h navexshr.h
naventry.obj: platform.h
navexsf.obj:  platform.h cts4.h navex.h navexshr.h
navexsb.obj:  platform.h cts4.h navex.h navexshr.h danish.cpp
navexsp.obj:  platform.h cts4.h navex.h navexshr.h danish.cpp
navexsm.obj:  platform.h cts4.h navex.h navexshr.h
navexrf.obj:  platform.h cts4.h navex.h navexshr.h
navexrb.obj:  platform.h cts4.h navex.h navexshr.h
navexrp.obj:  platform.h cts4.h navex.h navexshr.h
navexrh.obj:  platform.h cts4.h navex.h navexshr.h
navexshr.obj: platform.h navexshr.h
#ENDUPDATE#

%Else

#UPDATE#
nechild.obj:  platform.h ctsn.h navex.h navexshr.h
vavex.obj:    platform.inc
navex.obj:    platform.h ctsn.h callback.h navex.h navexshr.h
naventry.obj: platform.h
navexsf.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexsb.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexsp.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexsm.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexrf.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexrb.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexrp.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexrh.obj:  platform.h ctsn.h callback.h navex.h navexshr.h
navexshr.obj: platform.h navexshr.h
navexnt.obj:  platform.h avapi.h ctsn.h callback.h navex.h
booscan.obj: platform.h bootype.h boodata.h boodefs.h
bootype.obj: platform.h bootype.h
trojscan.obj: trojscan.h trojsigs.h platform.h ctsn.h callback.h navex.h navex15.h callbk15.h navexshr.h
#crc32.obj:    platform.h crc32.h

heur.obj: heurapi.h navheur1.h inifile.h callfake.h ctsn.h callback.h navex.h platform.h
inifile.obj: platform.h callback.h inifile.h
callfake.obj: platform.h ctsn.h callback.h navex.h heurapi.h navheur1.h

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

wd7sigs.h   : wd7sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
wd8sigs.h   : wd8sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wd8sigs.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
xl97sigs.h  : xl97sigs.lst xlnamsig.wdf xl97var.wdf xl97sigs.wdf
wd7sigs.cpp : wd7sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
wd8sigs.cpp : wd8sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wd8sigs.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
xl97sigs.cpp: xl97sigs.lst xlnamsig.wdf XL97VAR.WDF xl97sigs.wdf
wd7sigs.obj : wd7sigs.cpp macrovid.h austrvid.h europvid.h japanvid.h
wd8sigs.obj : wd8sigs.cpp macrovid.h austrvid.h europvid.h japanvid.h
xl97sigs.obj: xl97sigs.cpp macrovid.h austrvid.h europvid.h japanvid.h
wd7xclsg.obj: wd7xclsg.cpp
wd8xclsg.obj: wd8xclsg.cpp
wdapsig.obj : platform.h callback.h storobj.h wdapsig.h
wdapvsig.obj: platform.h callback.h storobj.h wdapvsig.h
wdencdoc.obj: platform.h callback.h storobj.h wdencdoc.h
wdencrd.obj : platform.h callback.h storobj.h wdencrd.h
wdencwr.obj : platform.h callback.h storobj.h wdencwr.h
wdscan.obj  : platform.h callback.h storobj.h wdscan.h
wdrepair.obj: platform.h callback.h storobj.h wdrepair.h
wdsigutl.obj: platform.h callback.h storobj.h wdsigutl.h
wd7scan.obj: platform.h callback.h storobj.h wd7scan.h wd7sigs.h mcrhitmm.h
offcscan.obj: platform.h callback.h storobj.h offcscan.h
vba5rep.obj: platform.h callback.h storobj.h vba5scan.h wd8sigs.h xl97sigs.h mcrhitmm.h
vba5scan.obj: platform.h callback.h storobj.h vba5rep.h wd8sigs.h xl97sigs.h mcrhitmm.h
vbalznt.obj: platform.h callback.h storobj.h vbalznt.h
mcrhitmm.obj: mcrhitmm.h wd7sigs.h wd8sigs.h xl97sigs.h

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
