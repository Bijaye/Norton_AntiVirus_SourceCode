# Copyright 1995 Symantec, Peter Norton Product Group
#************************************************************************
#
# $Header:   S:/CALLBACK/VCS/CALLBACK.MAv   1.3   24 Jul 1997 15:43:10   MKEATIN  $
#
# Description: Makefile for the AV engine callback project.
#
#************************************************************************
# $Log:   S:/CALLBACK/VCS/CALLBACK.MAv  $
#  
#     Rev 1.3   24 Jul 1997 15:43:10   MKEATIN
#  Added the NLM platform.
#  
#     Rev 1.2   18 Jun 1997 16:30:54   MKEATIN
#  Removed v32scan.lib
#  
#     Rev 1.1   18 Jun 1997 13:26:48   MKEATIN
#  W32 dll name is now N32CALL.DLL
#  
#     Rev 1.0   18 Jun 1997 13:16:38   MKEATIN
#  Initial revision.
#  
#     Rev 1.3   17 Jun 1997 18:10:14   ksackin
#  Fixed makefile.  Need to set program type *BEFORE* including platform.mak.
#
#     Rev 1.2   17 Jun 1997 17:06:50   MKEATIN
#  Don't compile callstr.cpp for NTK.
#
#     Rev 1.1   17 Jun 1997 16:06:30   MKEATIN
#  Added the VXD and NTK platforms.
#
#     Rev 1.0   17 Jun 1997 15:09:08   MKEATIN
#  Initial revision.
#************************************************************************


%if "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""

    TARGET_NAME=N32CALL
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")

    TARGET_TYPE=LIB

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")

    TARGET_TYPE=LIB

%else

    TARGET_TYPE=LIB

%endif


.INCLUDE $(QUAKEINC)\PLATFORM.MAK

%if "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): callback.obj     \
                 edbxw32.obj      \
                 infbxw32.obj     \
                 msgbxw32.obj     \
                 naventry.obj     \
                 $(Q_SYMKRNL).LIB \
                 $(Q_SYMDEBUG).LIB\
                 callback.res     \

callback.res:    callback.rc      \
                 resource.h       \

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")

CALLBACK.LIB: callback.obj \
              callstr.obj

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")

CALLBACK.LIB: callback.obj

%elseif "$(PLATFORM)" == ".DOS" || \
	 	"$(PLATFORM)" == ".DX"  || \
        "$(PLATFORM)" == ".DVX" || \
        "$(PLATFORM)" == ".NCX"

CALLBACK.LIB: callback.obj \
              callstr.obj

%elseif "$(PLATFORM)" == ".NLM"

CALLBACK.LIB: callback.obj

%endif


#UPDATE#
callback.obj:	platform.h xapi.h disk.h navor.h navcb.h avenge.h
edbxw32.obj :   platform.h resource.h
infbxw32.obj:   platform.h resource.h
msgbxw32.obj:   platform.h resource.h
naventry.obj:	platform.h
#ENDUPDATE#
