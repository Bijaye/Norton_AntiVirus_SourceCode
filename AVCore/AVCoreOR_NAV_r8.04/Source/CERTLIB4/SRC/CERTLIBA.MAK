# Copyright 1992-1993 Symantec, Peter Norton Product Group
#########################################################################
#                                                                       #
# $Header:   S:\certlib4\vcs\certliba.mav   1.47   01 Jun 1995 13:23:14   DSACKING  $                                                            #
#                                                                       #
# Description:                                                          #
#       This is a generic Make file
#                                                                       #
#########################################################################
#
# $Log:   S:\certlib4\vcs\certliba.mav  $
#  
#     Rev 1.47   01 Jun 1995 13:23:14   DSACKING
#  Ported the changes from 3.0.6
#  
#     Rev 1.46   29 Nov 1994 15:29:40   DALLEE
#  Put back old defines (DOS16, SDOS16, WIN16) to work w/ QAK2 code base.
#  
#     Rev 1.45   18 May 1994 17:51:28   BRAD
#  Use ML.MAK
#  
#     Rev 1.44   13 Apr 1994 19:09:52   BRAD
#  Add space before adding SYM_TSR to ASMOPTS
#  
#     Rev 1.43   31 Mar 1994 13:57:30   BRAD
#  Use SYM_platform, rather than special defines
#  
#     Rev 1.42   17 Mar 1994 11:17:00   BRAD
#  Since ML uses a different syntax, need to use own .ASM.OBJ rules
#  
#     Rev 1.41   16 Mar 1994 13:40:16   BRAD
#  ML doesn't like options 'compressed
#  
#     Rev 1.40   16 Mar 1994 12:49:00   BRAD
#  Use ML, rather than MASM
#  
#     Rev 1.39   11 Mar 1994 16:44:32   BRAD
#  Add /ML to ASMOPTS
#  
#     Rev 1.38   09 Mar 1994 14:42:58   BRAD
#  Simplified
#  
#     Rev 1.37   08 Mar 1994 21:35:30   BRAD
#  Use PLATFORM.MAK
#  
#     Rev 1.36   08 Mar 1994 17:28:08   BRAD
#  Use PLATFORM.MAK
#  
#     Rev 1.35   11 Feb 1994 15:19:34   MARKL
#  Qak 2 changes moved to trunk
########################################################################

TARGET_NAME=CERTLIBA
PROGRAM_NAME=$(TARGET_NAME).LIB
%If "$(PLATFORM)" == ".TSR"
   MODEL=S
%Endif
TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

COMMONOBJS=\
    bantest.obj\
    cmbr.obj   \
    cmbx.obj   \
    cmbw.obj   \
    cmbd.obj   \
    cmbc.obj   \
    cmbo.obj   \
    cmbl.obj\
    dcvasc.obj \
    dcvasn.obj \
    dcvclus.obj \
    dcvlsn.obj \
    denc.obj   \
    dosvec.obj\
    dreset.obj \
    dwsetio.obj\
    fattr.obj  \
    fcdta.obj  \
    fclose.obj \
    fchdir.obj\
    fcdir.obj\
    fdtime.obj \
    fdel.obj   \
    fdup.obj   \
    fdir.obj  \
    fdrive.obj \
    ffindf.obj \
    fgdta.obj  \
    fgetexn.obj\
    fgetexd.obj\
    filcrcfl.obj\
    filst.obj  \
    fnext.obj  \
    fopen.obj  \
    fred.obj   \
    fseek.obj  \
    fwrit.obj  \
    fflush.obj \
    frename.obj\
    get2_13.obj\
    keyshst.obj\
    ngeta.obj  \
    ngetl.obj  \
    ngtuc.obj  \
    novtest.obj\
    nsend.obj  \
    nsendl.obj \
    sysgn.obj \
    sysenv.obj\
    tenc.obj

DOSOBJS=bbutt.obj\
    bopen.obj  \
    bclos.obj  \
    bprnt.obj  \
    bprn.obj   \
    sgatt.obj  \
    sblan.obj  \
    sgvid.obj  \
    fhton.obj  \
    memdest.obj\
    meminit.obj\
    memalloc.obj\
    memfree.obj\
    memlock.obj\
    memunlck.obj\
    memhma.obj \
    mumba.obj \
    mumbf.obj \
    mconva.obj\
    mconvf.obj\
    mxmsa.obj \
    mxmsf.obj \
    mememsa.obj \
    memsf.obj \
    memsp.obj \
    memsl.obj \
    memsu.obj \
    mxmsl.obj \
    mxmsu.obj \
    ddint13.obj\
    ddint25.obj\
    ddint26.obj\
    mdstos.obj \
    STRPR.OBJ\
    CHRPRN.OBJ\
    othcl.obj\
    dostsr.obj\
    dosunv.obj\
    dosmcb.obj\
    dosmyt.obj\
    memconv.obj\
    sysps2.obj\
    syssda.obj\
    syshalt.obj\
    scrdisp.obj\
    scrgat.obj\
    scrgcu.obj\
    scrgpg.obj\
    get13.obj \
    sysgv.obj \
    dll4.obj\
    dll5.obj\
    dll6.obj\
    ginips.obj\
    winips.obj\
    ginipi.obj\
    sysfenv.obj

WINOBJS=dwint13.obj\
%If "$(PLATFORM)" != ".MIR"
    dwint25.obj\
    dwint26.obj\
%EndIf
    ansif.obj  \
    mwstos.obj \
    mwconva.obj\
    mwconvf.obj\
    syswhalt.obj


########################################################################
# DOS version of CERTLIB
########################################################################
%If "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".X86"
ASMOPTS+= /DDOS16

$(PROGRAM_NAME): $(COMMONOBJS)\
                 $(DOSOBJS)


########################################################################
# TSR version of CERTLIB
########################################################################
%ElseIf "$(PLATFORM)" == ".TSR"
ASMOPTS+= /DSDOS16 /DVIRUS_SUBSET

$(PROGRAM_NAME): $(COMMONOBJS)\
                 $(DOSOBJS)


########################################################################
# NLM version of CERTLIB
########################################################################
%ElseIf "$(PLATFORM)" == ".NLM"


########################################################################
# WIN16 and WIN32 version of CERTLIB
########################################################################
%Else
ASMOPTS+= /DWIN16

$(PROGRAM_NAME): $(COMMONOBJS)\
                 $(WINOBJS)

%EndIf

#UPDATE#
#ENDUPDATE#
