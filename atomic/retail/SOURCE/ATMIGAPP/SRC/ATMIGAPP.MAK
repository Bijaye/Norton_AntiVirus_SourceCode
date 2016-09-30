#########################################################################
#
# File: AtMigApp.mak
#
# $Header:   S:/ATMIGAPP/VCS/ATMIGAPP.MAv   1.1   02 Jun 1998 17:02:24   SEDWARD  $
#
#########################################################################
# $Log:   S:/ATMIGAPP/VCS/ATMIGAPP.MAv  $
#  
#     Rev 1.1   02 Jun 1998 17:02:24   SEDWARD
#  Changed target to 'ATMIGAPP.EXE' so we won't overwrite 'MIGAPP.EXE'.
#
#     Rev 1.0   27 May 1998 17:27:52   SEDWARD
#  Initial revision.
#
#
#########################################################################

TARGET_TYPE=EXE
PROGRAM_NAME = ATMIGAPP.EXE
USE_MFC_DLL=FALSE

%if "$(CV)"  == "YES" || "$(CV)"  == "Yes" ||  "$(CV)"  == "yes" || "$(CV)"  == "1"
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OPT_NOCV=-Os -Ob2 /MD /GX
XOPTS += -W3 -Zp1
FIRSTLIBS += MFCS42$(_AFXDEBUG).lib
LIBS += COMCTL32.LIB

$(PROGRAM_NAME):        STDAFX.OBJ \
                        MIGAPP.OBJ \
                        MIGDLG.OBJ \
                        MIGAPP.RES