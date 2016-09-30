#########################################################################
#
#
# $Header:   S:/REMIND/VCS/remind.mav   1.0   08 Dec 1997 15:33:36   jtaylor  $
#
#########################################################################
# $Log:   S:/REMIND/VCS/remind.mav  $ #
#  
#     Rev 1.0   08 Dec 1997 15:33:36   jtaylor
#  Initial revision.
#

TARGET_TYPE=EXE
PROGRAM_NAME = N32RMD.EXE
USE_MFC_DLL=TRUE

%if "$(CV)"  == "YES" || "$(CV)"  == "Yes" ||  "$(CV)"  == "yes" || "$(CV)"  == "1"
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OPT_NOCV=-Os -Ob2 /MD /GX
XOPTS += -W3 -Zp1
FIRSTLIBS += MFCS42$(_AFXDEBUG).lib
LIBS += COMCTL32.LIB
LIBS += N32PDLL.LIB

$(PROGRAM_NAME):        STDAFX.OBJ \
                        N32RMD.OBJ \
                        N32RMDDLG.OBJ \
                        N32RMD.RES




