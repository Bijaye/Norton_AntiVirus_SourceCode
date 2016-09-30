#########################################################################
#
#
# $Header:   S:/DEFANNOITY/VCS/defannty.mav   1.1   08 Apr 1998 19:47:06   RStanev  $
#
#########################################################################
# $Log:   S:/DEFANNOITY/VCS/defannty.mav  $ #
#  
#     Rev 1.1   08 Apr 1998 19:47:06   RStanev
#  Merged branch changes.
#  
#     Rev 1.0.1.3   08 Apr 1998 01:05:04   RStanev
#  Added wizardbitmap.obj.
#  
#     Rev 1.0.1.2   08 Apr 1998 00:59:44   RStanev
#  Added one more subscription panel.
#  
#     Rev 1.0.1.1   02 Apr 1998 03:04:16   RStanev
#  Added SymWorld.obj.
#  
#     Rev 1.0.1.0   30 Mar 1998 10:18:54   RStanev
#  Branch base for version QAKG
#  
#     Rev 1.0   30 Mar 1998 10:18:48   RStanev
#  Initial revision.
#
#
#########################################################################

TARGET_TYPE=DLL
PROGRAM_NAME = DefAnnty.dll
USE_MFC_DLL=TRUE
USE_DEFAULT_LIBS=TRUE


%if "$(CV)" != ""
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

PCH=StdAfx

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OPT_NOCV=-Os -Ob2 /MD /GX
XOPTS += -W3 -Zp1 /GX /D_MBCS

%if "$(PLATFORM)" == ".IRA" || "$(PLATFORM)" == ".ARA"
	LINKOPTS += /NODEFAULTLIB:LIBCMT.LIB
%else
	LINKOPTS += /NODEFAULTLIB:LIBCMTD.LIB
%endif

$(PROGRAM_NAME):         \
        StdAfx.obj       \
        DefAnnty.obj     \
        DefAnnuity.obj   \
        SubWizard.obj    \
        SymWorld.obj     \
        WizardPanelA.obj \
        WizardPanelBPre.obj \
        WizardPanelB.obj \
        WizardPanelC.obj \
        WizardPanelD.obj \
        WizardPanelE.obj \
        WizardPanelF.obj \
        WizardPanelG.obj \
        WizardPanelH.obj \
        WizardPanelI.obj \
        WizardPanelJ.obj \
        HyperLink.obj    \
        SessProc.obj     \
        SCValid.obj      \
        DefAnnty.res

#UPDATE#
#ENDUPDATE#
