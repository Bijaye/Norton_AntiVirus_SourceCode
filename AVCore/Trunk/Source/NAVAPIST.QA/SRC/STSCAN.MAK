# Copyright 1998 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/NAVAPIST.QA/VCS/stscan.mav   1.12   24 Aug 1998 11:28:36   NMILLER  $
#
# Description: Makefile for STScan
#
#************************************************************************
# $Log:   S:/NAVAPIST.QA/VCS/stscan.mav  $
#  
#     Rev 1.12   24 Aug 1998 11:28:36   NMILLER
#  Backed out Mike's pre-init changes
#  
#     Rev 1.11   21 Aug 1998 18:22:36   NMILLER
#  Now using QUAKE_CUSTOM for Win16, now using same source file for NLM.
#  
#     Rev 1.10   20 Aug 1998 18:27:52   NMILLER
#  Removed SYMNAV Dependency
#  
#     Rev 1.9   19 Aug 1998 17:32:56   NMILLER
#  Pulled NLM makefile into this one.
#  
#     Rev 1.8   19 Aug 1998 17:00:26   NMILLER
#  Got the think MKUPDATE'ing.
#  
#     Rev 1.7   18 Aug 1998 19:44:36   MKEATIN
#  Added some objs for DVX startup code.
#  
#     Rev 1.6   18 Aug 1998 17:25:44   mmendon
#  Added MT compiler switch for multithreaded libs
#
#     Rev 1.5   17 Aug 1998 16:33:42   NMILLER
#  Ripped out callback DLL stuff
#
#     Rev 1.4   14 Aug 1998 16:48:14   NMILLER
#  Switched from cdecl to stdcall (/Gz)
#
#     Rev 1.3   12 Aug 1998 17:00:00   NMILLER
#  Removed dependency on APICall
#
#     Rev 1.2   11 Aug 1998 21:55:06   MKEATIN
#  Changed some settings for Win16
#
#     Rev 1.1   11 Aug 1998 09:40:30   mmendon
#  Fixed Win16 compile and link options.
#
#     Rev 1.0   10 Aug 1998 12:25:16   NMILLER
#  Initial revision.
#
#************************************************************************

QUAKE_CUSTOM=

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# WIN32

%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=STSCAN32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    LIBS += $(Q_SYMKRNL).LIB
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX
    LIBS += FILESERV.LIB
    LIBS += LOGQA.LIB
    OBJS += Main.obj
    OBJS += EventObj.obj
    $(PROGRAM_NAME): $(OBJS) $(LIBS) stscan.def

# WIN16
# -----

%ElseIf ("$(CORE_WIN16)" != "")
    SYSLIBS= LLIBCEWQ.LIB
    FIRSTLIBS=
    MODEL=L
    TARGET_NAME=STSCAN16
    LIBS=  LIBW.LIB OLDNAMES.LIB COMMDLG.LIB SHELL.LIB
    LIBS += $(Q_SYMKRNL).LIB
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX 
    LIBS += FILESERV.LIB
    LIBS += LOGQA.LIB
    OBJS += Main.obj
    OBJS += EventObj.obj
    $(PROGRAM_NAME): $(OBJS) $(LIBS) stscan.def

# DX
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    TARGET_NAME=STSCANDX
    MODEL = L
    LIBS += NAVAPI.LIB
    LIBS += $(DOSVMM)
    LIBS += dosio.lib
    LIBS += dosui.lib
    LIBS += dostool.lib
    LIBS += strings.lib
    LIBS += $(Q_SYMKRNL).LIB
#    OBJS += lnipatch.obj
#    OBJS += libhookl.obj
#    OBJS += cfginit.obj
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE) 
    CLOPTS += /GX
    LIBS += FILESERV.LIB
    LIBS += LOGQA.LIB
    OBJS += Main.obj
    OBJS += EventObj.obj
$(PROGRAM_NAME): $(OBJS) $(LIBS) stscan.def


%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")
    TARGET_NAME=STSCAN
    PROGRAM_NAME    = $(TARGET_NAME).nlm
    $(PROGRAM_NAME):    main.obj   \
                        $(Q_SYMKRNL).lib \
                        navapi.imp \
                        navapi.exp \
                        nlmstscan.den \
                        nlmstscan.exp
%Endif



#UPDATE#
Main.obj:         Main.cpp  navapi.h
EventObj.obj:	  EventObj.cpp EventObj.h
#ENDUPDATE#
