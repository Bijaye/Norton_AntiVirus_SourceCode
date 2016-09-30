#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/navex15.mav   1.5   21 May 1997 16:47:36   MKEATIN  $
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
#  
#     Rev 1.2   08 May 1997 13:35:08   MKEATIN
#  
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

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

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
        navexent.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

MACROOBJS = \
        wddecsig.obj \
        wd7sigs.obj  \
        wd8sigs.obj  \
        xl97sigs.obj \
        macrscan.obj \
        macrodel.obj \
        macrorep.obj \
        offcscan.obj \
        storobj.obj  \
        worddoc.obj  \
        wdapsig.obj  \
        wdapvsig.obj \
        wdencdoc.obj \
        wdencrd.obj  \
        wdencwr.obj  \
        wdscan.obj   \
        wdrepair.obj \
        wdsigutl.obj \
        wd7scan.obj  \
        vba5rep.obj  \
        vba5scan.obj \
        vbalznt.obj  \
        exclscan.obj \
        excelshr.obj \
        excelrep.obj \
        mvp.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

NEPEOBJS = \
        nepescan.obj \
        nepeshr.obj \
        neperep.obj \
        winsig.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

JAVAOBJS = \
        javascan.obj \
        javashr.obj \
        javasig.obj

XOPTS += /DNAVEX15

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
                 vavex.dvx      \
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

    TARGET_NAME=NAVEX16a
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL
#   XOPTS += /DSYM_CPU_X86

%If ("$[u,$(QUAKE)]" == "QAK2")
.INCLUDE $(QUAKEINC)\WINDLLC7.MAK
%Else
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK
%EndIf

$(PROGRAM_NAME): navex.obj      \
                 naventry.obj   \
                 navex16.def    \
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

# Word document decryption and virus signatures

wddecsig.cpp: wddecsig.ddf
             if exist wddecsgf.err del wddecsgf.err
             if not exist wddecsig.ddf copy %proj%\src\wddecsig.ddf > wddecsig.cpy
             Echo mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig 컴컴> wddecsgf.err
             @echo mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig
             mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig > wddecsgf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wddecsgf.err
             if errorlevel ne 0 type wddecsgf.err
             if errorlevel eq 0 del wddecsgf.err > nul
             if exist wddecsig.cpy del wddecsig.ddf wddecsig.cpy

wddecsig.ddf:


wd7sigs.cpp: wd7sigs.lst \
             wdnamsig.wdf \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wdvarsig.wdf \
             wd7sigs.wdf
             if exist wd7sigsf.err del wd7sigsf.err
             if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
             Echo mkwrdsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
             @echo mkwrdsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs
             mkwrdsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
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

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wdnamsig.wdf:
wdvarsig.wdf:

wd8sigs.cpp: wd8sigs.lst \
             wdnamsig.wdf \
             wd7sigs.wdf \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wd8sigs.wdf \
             wdvarsig.wdf
             if exist wd8sigsf.err del wd8sigsf.err
             if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd8sigs1.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd8sigs2.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd8sigs3.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd8sigs4.cpy
             if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd8sigs6.cpy
             Echo mkwrdsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
             @echo mkwrdsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs
             mkwrdsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs > wd8sigsf.err
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

wdnamsig.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd8sigs.wdf:
wdvarsig.wdf:

xl97sigs.cpp: xl97sigs.lst \
              wdvarsig.wdf \
              xl97sigs.wdf
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > xl97sgs2.cpy
              Echo mkwrdsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs 컴컴> xl97sgsf.err
              @echo mkwrdsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs
              mkwrdsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del wdvarsig.wdf xl97sgs2.cpy

xl97sigs.wdf:
wdvarsig.wdf:

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

wddecsig.obj: wddecsig.ddf wddecsig.cpp
wd7sigs.obj : wd7sigs.lst wdnamsig.wdf wd7sigs.wdf wd7sigs.cpp \
              wd7sigs2.wdf wd7sigs3.wdf wdvarsig.wdf
wdapsig.obj : platform.h callback.h storobj.h wdapsig.h
wdapvsig.obj: platform.h callback.h storobj.h wdapvsig.h
wdencdoc.obj: platform.h callback.h storobj.h wdencdoc.h
wdencrd.obj : platform.h callback.h storobj.h wdencrd.h
wdencwr.obj : platform.h callback.h storobj.h wdencwr.h
wdscan.obj  : platform.h callback.h storobj.h wdscan.h
wdrepair.obj: platform.h callback.h storobj.h wdrepair.h
wdsigutl.obj: platform.h callback.h storobj.h wdsigutl.h
wd7scan.obj: platform.h callback.h storobj.h wd7scan.h
offcscan.obj: platform.h callback.h storobj.h offcscan.h
vba5rep.obj: platform.h callback.h storobj.h vba5scan.h
vba5scan.obj: platform.h callback.h storobj.h vba5rep.h
vbalznt.obj: platform.h callback.h storobj.h vbalznt.h

macrodel.obj: platform.h callback.h storobj.h worddoc.h macrodel.h
macrorep.obj: platform.h callback.h storobj.h worddoc.h macrorep.h macrodel.h
macrscan.obj: platform.h callback.h storobj.h worddoc.h macrscan.h
storobj.obj:  platform.h callback.h storobj.h navexshr.h
worddoc.obj:  platform.h callback.h storobj.h worddoc.h
exclscan.obj: platform.h callback.h storobj.h exclscan.h excelrep.h
excelshr.obj: excelshr.h
excelrep.obj: excelshr.h excelrep.h

nepescan.obj: platform.h navexshr.h callback.h nepescan.h winconst.h winsig.h
neperep.obj: platform.h navexshr.h callback.h neperep.h winconst.h
winsig.obj:   platform.h navexshr.h winsig.h

javascan.obj: platform.h navexshr.h callback.h javascan.h javashr.h javasig.h
javasig.obj:  platform.h navexshr.h javasig.h

%EndIf

#ENDUPDATE#

%EndIf


