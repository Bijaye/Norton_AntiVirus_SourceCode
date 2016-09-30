#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/navexf.mav   1.4   09 Oct 1998 12:17:54   relniti  $
#
#  Description:
#       This is a make file for NAVEX - this includes def files for remote
#  offices.  Taken from revision 1.76 of the NAVEX makefile.
#
#  See Also:
#       VIRSCAN and AVAPI projects.
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/navexf.mav  $
#  
#     Rev 1.4   09 Oct 1998 12:17:54   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.3   16 Apr 1998 21:14:54   RELNITI
#  CHANGE XLVARSIG.WDF to XL97VAR.WDF for XL97SIGS
#  
#     Rev 1.2   15 Apr 1998 11:43:42   RELNITI
#  ADD XLNAMSIG.WDF and CHANGE WDVARSIG.WDF to XLVARSIG.WDF in XL97SIGS
#  
#     Rev 1.1   30 Mar 1998 13:51:10   AOONWAL
#  Added wd7sigs6.wdf
#  
#     Rev 1.0   02 Mar 1998 21:47:00   JWILBER
#  Initial revision.
#
#
#############################################################################

# This is NAVEXF.MAK!!!

# This calls MKMCRSG2.EXE instead of MKMCRSIG.EXE.  #include statements in
# MACROVID.H are parsed by this new program.

# Module objects

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

OBJS =  navexsf.obj \
        navexsb.obj \
        navexsp.obj \
        navexsm.obj \
        navexrf.obj \
        navexrb.obj \
        navexrp.obj \
        navexrh.obj \
        navexshr.obj \
        nvxbtshr.obj \
        booscan.obj \
        bootype.obj

# !!! PLEASE KEEP NAVEX.MAK synchronized with NAVEX.NTK !!!

MACROLIBS = \
        crc32.lib    \
        olessapi.lib \
        wd7api.lib   \
        o97api.lib   \
        xl5api.lib   \
        macroheu.lib

MACROOBJS = \
        wddecsig.obj \
        wd7sigs.obj  \
        wd8sigs.obj  \
        xl97sigs.obj \
        wd7xclsg.obj \
        wd8xclsg.obj \
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
        o97rpr15.obj \
        xl5scn15.obj \
        excelrep.obj \
        excelshr.obj \
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
        xl4mscan.obj \
        xl4mrep.obj  \
        $(MACROLIBS)

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

XOPTS += /DNDEBUG

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
                 mcrheuen.obj   \
                 heur.obj       \
                 callfake.obj   \
                 heurdx.lib     \
                 trojscan.obj   \
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
                 mcrheuen.obj   \
                 heur16.lib     \
                 heur.obj       \
                 callfake.obj   \
                 trojscan.obj   \
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


wd7sigs.cpp: wd7sigs.lst  \
             wdnamsig.wdf \
             wdjapnam.wdf \
             wdausnam.wdf \
             wdeurnam.wdf \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wd7sigs4.wdf \
             wd7sigs5.wdf \
             wd7sigs6.wdf \
             wd7sigs7.wdf \
             wdvarsig.wdf \
             wd7sigs.wdf  \
             wdjapsig.wdf \
             wdaussig.wdf \
             wdeursig.wdf
             if exist wd7sigsf.err del wd7sigsf.err
             if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
             if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
             if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd7sigs8.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd7sigs9.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd7sigsa.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigsb.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsC.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
             Echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
             @echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs
             mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
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
             if exist wd7sigs8.cpy del wdjapnam.wdf wd7sigs8.cpy
             if exist wd7sigs9.cpy del wdausnam.wdf wd7sigs9.cpy
             if exist wd7sigsa.cpy del wdeurnam.wdf wd7sigsa.cpy
             if exist wd7sigsb.cpy del wd7sigs6.wdf wd7sigsb.cpy
             if exist wd7sigsC.cpy del wd7sigs7.wdf wd7sigsC.cpy
             if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
             if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
             if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdnamsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdvarsig.wdf:

wd8sigs.cpp: wd8sigs.lst  \
             wdnamsig.wdf \
             wdjapnam.wdf \
             wdausnam.wdf \
             wdeurnam.wdf \
             wd7sigs.wdf  \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wd7sigs4.wdf \
             wd7sigs5.wdf \
             wd7sigs6.wdf \
             wd7sigs7.wdf \
             wdjapsig.wdf \
             wdaussig.wdf \
             wdeursig.wdf \
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
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd8sigs9.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd8sigsa.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd8sigsb.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd8sigsc.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sigsD.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
             Echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
             @echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst wd8sigs
             mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst wd8sigs > wd8sigsf.err
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
             if exist wd8sigs9.cpy del wdjapnam.wdf wd8sigs9.cpy
             if exist wd8sigsa.cpy del wdausnam.wdf wd8sigsa.cpy
             if exist wd8sigsb.cpy del wdeurnam.wdf wd8sigsb.cpy
             if exist wd8sigsc.cpy del wd7sigs6.wdf wd8sigsc.cpy
             if exist wd8sigsD.cpy del wd7sigs7.wdf wd8sigsD.cpy
             if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
             if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
             if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy


wdnamsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:
wd8sigs.wdf:
wdvarsig.wdf:

xl97sigs.cpp: xl97sigs.lst \
              xljapsig.wdf \
              xlaussig.wdf \
              xleursig.wdf \
              xl97var.wdf \
              xlnamsig.wdf \
              xl97sigs.wdf \
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist xl97var.wdf copy %proj%\src\xl97var.wdf > xl97sgs2.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
              if not exist xljapsig.wdf copy %proj%\src\xljapsig.wdf > xljapsig.cpy
              if not exist xlaussig.wdf copy %proj%\src\xlaussig.wdf > xlaussig.cpy
              if not exist xleursig.wdf copy %proj%\src\xleursig.wdf > xleursig.cpy
              Echo mkmcrsg2.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs 컴컴> xl97sgsf.err
              @echo mkmcrsg2.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs
              mkmcrsg2.exe -qaPASSED -tyXL97 -lxl97sigs.lst xl97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del xl97var.wdf xl97sgs2.cpy
              if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy
              if exist xljapsig.cpy del xljapsig.wdf xljapsig.cpy
              if exist xlaussig.cpy del xlaussig.wdf xlaussig.cpy
              if exist xleursig.cpy del xleursig.wdf xleursig.cpy

xl97sigs.wdf:
xljapsig.wdf:
xlaussig.wdf:
xleursig.wdf:
xl97var.wdf:
xlnamsig.wdf:

wd7xclsg.cpp: wd7xclsg.xdf
              if exist wd7xclsg.err del wd7xclsf.err
              if not exist wd7xclsg.xdf copy %proj%\src\wd7xclsg.xdf > wd7xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf wd7xclsg 컴컴> wd7xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf wd7xclsg
              mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf wd7xclsg > wd7xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7xclsf.err
              if errorlevel ne 0 type wd7xclsf.err
              if errorlevel eq 0 del wd7xclsf.err > nul
              if exist wd7xclsg.cpy del wd7xclsg.xdf wd7xclsg.cpy

wd7xclsg.xdf:

wd8xclsg.cpp: wd8xclsg.xdf
              if exist wd8xclsg.err del wd8xclsf.err
              if not exist wd8xclsg.xdf copy %proj%\src\wd8xclsg.xdf > wd8xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf wd8xclsg 컴컴> wd8xclsf.err
              @echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf wd8xclsg
              mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf wd8xclsg > wd8xclsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8xclsf.err
              if errorlevel ne 0 type wd8xclsf.err
              if errorlevel eq 0 del wd8xclsf.err > nul
              if exist wd8xclsg.cpy del wd8xclsg.xdf wd8xclsg.cpy

wd8xclsg.xdf:

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
              wd7sigs2.wdf wd7sigs3.wdf wdvarsig.wdf wd7sigs4.wdf wd7sigs5.wdf \
              wd7sigs6.wdf wd7sigs7.wdf
wd8sigs.h   : wd8sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wd8sigs.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
xl97sigs.h  : xl97sigs.lst xlnamsig.wdf xl97var.wdf xl97sigs.wdf
wd7sigs.cpp : wd7sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wdvarsig.wdf wd7sigs4.wdf wd7sigs5.wdf \
              wd7sigs6.wdf wd7sigs7.wdf
wd8sigs.cpp : wd8sigs.lst wdnamsig.wdf wd7sigs.wdf \
              wd7sigs2.wdf wd7sigs3.wdf wd8sigs.wdf wdvarsig.wdf wd7sigs4.wdf \
              wd7sigs5.wdf wd7sigs6.wdf wd7sigs7.wdf
xl97sigs.cpp: xl97sigs.lst xlnamsig.wdf xl97var.wdf xl97sigs.wdf
wddecsig.obj: wddecsig.ddf wddecsig.cpp
wd7sigs.obj : wd7sigs.cpp
wd8sigs.obj : wd8sigs.cpp
xl97sigs.obj: xl97sigs.cpp
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

javascan.obj: platform.h navexshr.h callback.h javascan.h javashr.h javasig.h
javasig.obj:  platform.h navexshr.h javasig.h

%EndIf

#ENDUPDATE#

%EndIf
