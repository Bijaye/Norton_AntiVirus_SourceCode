# Copyright 1996 Symantec, Peter Norton Product Group
###########################################################################
#
# $Header:   S:/CMDLINE/VCS/cmdline.mav   1.3   27 Aug 1996 15:26:44   DCHI  $
#
# Description:
#  Makefile for CMDLINE
#
###########################################################################
#
# $Log:   S:/CMDLINE/VCS/cmdline.mav  $
#  
#     Rev 1.3   27 Aug 1996 15:26:44   DCHI
#  Added CLNAVEX.OBJ to list of objects.
#  
#     Rev 1.2   10 Jun 1996 11:55:08   RAY
#  Added scanboot.cpp
#  
#     Rev 1.1   04 Jun 1996 18:19:14   DCHI
#  Modifications to conform to new header file structure.
#  
#     Rev 1.0   04 Jun 1996 10:21:20   DCHI
#  Initial revision.
#  
###########################################################################

TARGET_NAME=CMDLINE
PROGRAM_NAME=$(TARGET_NAME).EXE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     W 3 2                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".W32")
# W32 is a Windows 95 console application
        XOPTS += /D "_CONSOLE"
        LINKOPTS += /SUBSYSTEM:console
%EndIf

####################################################
#                     D O S                        #
####################################################
####################################################
#                      D X                         #
####################################################

%If ("$[u,$(PLATFORM)]" == ".DOS") || \
    ("$[u,$(PLATFORM)]" == ".DX")
        MODEL = L
%EndIf


$(PROGRAM_NAME):        \
        clcb.obj        \
        clnavex.obj     \
        cmdhelp.obj     \
        cmdline.obj     \
        cmdparse.obj    \
        cmdstr.obj      \
        ext.obj         \
        filefind.obj    \
        helpstr.obj     \
        msgstr.obj      \
        pathname.obj    \
        report.obj      \
        scanfile.obj    \
        scaninit.obj    \
        scanboot.obj    \
        scanmem.obj     \
        virlist.obj     \
        avenge.lib      \
%If ("$[u,$(PLATFORM)]" == ".DOS") || ("$[u,$(PLATFORM)]" == ".DX")
        dosio.lib       \
        dosnet.lib      \
        dostool.lib     \
        dosui.lib       \
        strings.lib     \
        dosdrvwn.lib    \
        $(DOSVMM)       \
%EndIf
        $(Q_SYMKRNL).LIB


#UPDATE#

clcb.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h fileinfo.h pathname.h
clnavex.obj: clnavex.h scanglob.h
cmdhelp.obj: avtypes.h cmdhelp.h
cmdline.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h scaninit.h scanmem.h scanfile.h
cmdparse.obj: avenge.h avtypes.h scanglob.h cmdparse.h
cmdstr.obj: avtypes.h cmdparse.h
ext.obj: avtypes.h ext.h
filefind.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h filefind.h pathname.h
helpstr.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h
msgstr.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h msgstr.h
pathname.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h filefind.h pathname.h
report.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h fileinfo.h msgstr.h
scanfile.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h scanfile.h report.h msgstr.h clnavex.h
scaninit.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h scaninit.h report.h msgstr.h clnavex.h
scanmem.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h scanmem.h report.h msgstr.h
virlist.obj: avenge.h avcb.h avtypes.h clcb.h scanglob.h msgstr.h virlista.h

#ENDUPDATE#

