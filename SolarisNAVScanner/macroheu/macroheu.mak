# Copyright 1997 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/MACROHEU/VCS/macroheu.mav   1.2   15 Aug 1997 13:30:14   DCHI  $
#
# Description:
#  Make file for MACROHEU library.
#
#########################################################################
#
# $Log:   S:/MACROHEU/VCS/macroheu.mav  $
#  
#     Rev 1.2   15 Aug 1997 13:30:14   DCHI
#  Added Office 97 emulation support.
#  
#     Rev 1.1   10 Jul 1997 15:37:10   DCHI
#  Added DS != SS flags for DX and WIN platforms.
#  
#     Rev 1.0   30 Jun 1997 16:24:02   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=MACROHEU
PROGRAM_NAME=$(TARGET_NAME).LIB

%If ("$[u,$(PLATFORM)]" == ".DOS") || \
    ("$[u,$(PLATFORM)]" == ".DX")
        MODEL = L
%EndIf

%If ("$[u,$(PLATFORM)]" == ".DX")
        XOPTS += -Alfw
%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")
        XOPTS += -Alnw
%EndIf

XOPTS += -DNDEBUG

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):     \
        control.obj  \
        env.obj      \
        eval.obj     \
        exec.obj     \
        external.obj \
        for.obj      \
        goto.obj     \
        gsm.obj      \
        ident.obj    \
        if.obj       \
        modenv.obj   \
        select.obj   \
        state.obj    \
        var.obj      \
        wbutil.obj   \
        while.obj    \
        o97call.obj  \
        o97ctrl.obj  \
        o97doop.obj  \
        o97doop0.obj \
        o97doop1.obj \
        o97doop2.obj \
        o97doop3.obj \
        o97doop4.obj \
        o97doop5.obj \
        o97doop6.obj \
        o97doop7.obj \
        o97doop8.obj \
        o97doop9.obj \
        o97doopa.obj \
        o97doopb.obj \
        o97doopc.obj \
        o97doopd.obj \
        o97doope.obj \
        o97doopf.obj \
        o97eval.obj  \
        o97env.obj   \
        o97exec.obj  \
        o97modev.obj \
        o97param.obj \
        o97str.obj   \
        o97var.obj

#UPDATE#

control.obj:  olessapi.h gsm.h wbutil.h wd7env.h
env.obj:      olessapi.h gsm.h wbutil.h wd7env.h
eval.obj:     olessapi.h gsm.h wbutil.h wd7env.h
exec.obj:     olessapi.h gsm.h wbutil.h wd7env.h
external.obj: olessapi.h gsm.h wbutil.h wd7env.h
for.obj:      olessapi.h gsm.h wbutil.h wd7env.h
goto.obj:     olessapi.h gsm.h wbutil.h wd7env.h
gsm.obj:      olessapi.h gsm.h wbutil.h wd7env.h
ident.obj:    olessapi.h gsm.h wbutil.h wd7env.h
if.obj:       olessapi.h gsm.h wbutil.h wd7env.h
modenv.obj:   olessapi.h gsm.h wbutil.h wd7env.h
select.obj:   olessapi.h gsm.h wbutil.h wd7env.h
state.obj:    olessapi.h gsm.h wbutil.h wd7env.h
var.obj:      olessapi.h gsm.h wbutil.h wd7env.h
wbutil.obj:   olessapi.h gsm.h wbutil.h wd7env.h
while.obj:    olessapi.h gsm.h wbutil.h wd7env.h

o97call.obj:  olessapi.h gsm.h wbutil.h o97env.h
o97ctrl.obj:  olessapi.h gsm.h wbutil.h o97env.h
o97doop.obj:  olessapi.h gsm.h wbutil.h o97env.h
o97doop0.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop1.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop2.obj: olessapi.h gsm.h wbutil.h o97env.h o97str.h
o97doop3.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop4.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop5.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop6.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop7.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop8.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doop9.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doopa.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doopb.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doopc.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doopd.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doope.obj: olessapi.h gsm.h wbutil.h o97env.h
o97doopf.obj: olessapi.h gsm.h wbutil.h o97env.h
o97eval.obj:  olessapi.h gsm.h wbutil.h o97env.h
o97env.obj:   olessapi.h gsm.h wbutil.h o97env.h o97str.h
o97exec.obj:  olessapi.h gsm.h wbutil.h o97env.h
o97modev.obj: olessapi.h gsm.h wbutil.h o97env.h
o97param.obj: olessapi.h gsm.h wbutil.h o97env.h
o97str.obj:   olessapi.h gsm.h wbutil.h o97env.h o97str.h
o97var.obj:   olessapi.h gsm.h wbutil.h o97env.h

#ENDUPDATE#


