#########################################################################
# $Header:   S:/NTASKMGR/VCS/NTaskMgr.mav   1.1   09 Apr 1998 11:11:36   mdunn  $
#
# Makefile for NTaskMgr.
#
#########################################################################
# $Log:   S:/NTASKMGR/VCS/NTaskMgr.mav  $
#  
#     Rev 1.1   09 Apr 1998 11:11:36   mdunn
#  Now including mstask.lib on Intel only.
#  
#     Rev 1.0   06 Apr 1998 17:56:30   mdunn
#  Initial revision.
#
#########################################################################

TARGET_TYPE=EXE
PROGRAM_NAME = NTaskMgr.exe
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE

%if "$(CV)" != ""
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

XOPTS += /D_MBCS

LIBS += ctaskapi.lib npsapi.lib ole32.lib shell32.lib uuid.lib

%if "$[upper,$(CORE_PLAT)]" == "I"
LIBS += mstask.lib
%endif

$(PROGRAM_NAME): ntaskmgr.obj ntaskmgr.res $(LIBS)


