##************************************************************************
## Copyright 1997 Symantec Corporation
## _______________________________________________________________________
##
## $Header:   S:/NSPLUGIN/VCS/NSPLUGIN.MAv   1.0   21 May 1997 16:37:50   MDUNN  $
##
## 05-20-97 16:52:47 MDUNN Created by MKMK 1.0
##                   from IDE makefile AVCORE:NSPLUGIN NPlugIn.mak
##
##************************************************************************
##  $Log:   S:/NSPLUGIN/VCS/NSPLUGIN.MAv  $
#  
#     Rev 1.0   21 May 1997 16:37:50   MDUNN
#  Initial revision.
##************************************************************************

XOPTS += -GX                            # Enable unwinding of locals from
                                        # an exception handler.
 
TARGET_TYPE=EXE    ## Console or Win32 App (x86)

#######################################################################
# Uncomment the next 2 lines to use the DLL versions of MFC and the CRT
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE
#######################################################################
# Uncomment the next 2 lines to use the static versions of MFC and the CRT
#USE_MFC_LIB=TRUE
#USE_CRT_LIB=TRUE
#######################################################################

CHAR_SET=A

%if "$(CV)" != ""
					## Debug Versions of MFC
	DEBUG_AFX=TRUE
	DEBUG_CRT=TRUE
%else
	DEBUG_AFX=
	DEBUG_CRT=
%endif


.INCLUDE $(QUAKEINC)\PLATFORM.MAK


#MKMK-UPDATE#

ALL:    NSPlugIn.exe # NPlugIn.bsc NPlugIn.hlp 

NSPlugIn.exe:    StdAfx.obj NPlugIn.res NPlugIn.obj NPlugDlg.obj \
	

NPlugIn.bsc:    NPlugDlg.sbr NPlugIn.sbr StdAfx.sbr 

#NPlugIn.hlp:    

#UPDATE#
NPlugDlg.obj:   NPlugDlg.cpp NPlugDlg.h NPlugIn.h StdAfx.h NPlugIn.pch \
	

#NPlugDlg.sbr:   NPlugDlg.cpp NPlugDlg.h NPlugIn.h StdAfx.h NPlugIn.pch \
	

NPlugIn.obj:    NPlugIn.cpp NPlugDlg.h NPlugIn.h StdAfx.h NPlugIn.pch \
	

NPlugIn.pch:    StdAfx.cpp StdAfx.h 

NPlugIn.res:    NPlugIn.rc NPlugIn.ico

#NPlugIn.sbr:    NPlugIn.cpp NPlugDlg.h NPlugIn.h StdAfx.h NPlugIn.pch \
	

StdAfx.obj:     StdAfx.cpp StdAfx.h 

#StdAfx.sbr:     StdAfx.cpp StdAfx.h 

#ENDUPDATE#
#MKMK-ENDUPDATE#
