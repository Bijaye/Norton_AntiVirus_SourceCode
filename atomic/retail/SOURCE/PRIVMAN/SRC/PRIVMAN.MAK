# 1997 Copyright Symantec Corporation.
#######################################################################
## $Header:   S:/PRIVMAN/VCS/privman.mav   1.2   25 Jun 1997 18:32:40   DSACKIN  $
## 04/16/97 16:40:02 pvenkat Created by MKMK 1.0
##                   from IDE makefile MARCONI:MODUSR privman.mak
#######################################################################
## $Log:   S:/PRIVMAN/VCS/privman.mav  $
#  
#     Rev 1.2   25 Jun 1997 18:32:40   DSACKIN
#  Added a space for the LinkOpts command
#
#     Rev 1.1   25 Jun 1997 16:31:30   DSACKIN
#  the UNICODE defines are now taken care of by the .PLATFORM definition.
#
#     Rev 1.0   25 Jun 1997 16:06:28   DSACKIN
#  Initial revision.
#
#     Rev 1.3   08 May 1997 10:20:14   pvenkat
#  Added new src file (statdlg.cpp & statdlg.h).
#
#######################################################################
##

TARGET_TYPE=EXE    ## Console or Win32 App (x86)

USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LINKOPTS += /entry:"wWinMainCRTStartup"

#MKMK-UPDATE#
LIBS += NETAPI32.LIB
LIBS += comctl32.lib

ALL:    PrivMAN.exe PrivMAN.bsc

PrivMAN.exe:    StdAfx.obj Security.obj PMANDLG.obj StatDlg.obj PrivMAN.res \
	PrivMAN.obj

PrivMAN.bsc:    PrivMAN.sbr PMANDLG.sbr Security.sbr StdAfx.sbr \


#UPDATE#
PrivMAN.obj:    PrivMAN.cpp PrivMAN.h PMANDLG.h StdAfx.h PrivMAN.pch \


PrivMAN.pch:    StdAfx.cpp StdAfx.h

PrivMAN.res:    PrivMAN.rc compsel.bmp computer.bmp domain.bmp \
	DMNSEL.bmp PrivMAN.ico

PrivMAN.sbr:    PrivMAN.cpp PrivMAN.h PMANDLG.h StdAfx.h PrivMAN.pch \


PMANDLG.obj: PMANDLG.cpp PrivMAN.h PMANDLG.h StdAfx.h \
	PrivMAN.pch

PMANDLG.sbr: PMANDLG.cpp PrivMAN.h PMANDLG.h StdAfx.h \
	PrivMAN.pch

Security.obj:   Security.CPP NTSECAPI.H PrivMAN.pch

Security.sbr:   Security.CPP NTSECAPI.H PrivMAN.pch

StdAfx.obj:     StdAfx.cpp StdAfx.h

StdAfx.sbr:     StdAfx.cpp StdAfx.h

StatDlg.obj:    StatDlg.cpp StatDlg.h

StatDlg.sbr:    StatDlg.cpp StatDlg.h

#ENDUPDATE#
#MKMK-ENDUPDATE#
