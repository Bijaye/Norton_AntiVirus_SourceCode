########################################################################
# Copyright 1993 Symantec Corporation                                   
########################################################################
#                                                                       
# $Header:   S:/VIRSCAN/VCS/virscan.mav   1.45   18 Jun 1997 16:48:30   MKEATIN  $
#                                                                       
# Description:                                                          
#   Contains File, Memory and Boot Scanning routines
#                                                                       
########################################################################


%if "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""
    TARGET_NAME=V32SCAN
    DEF_FILE=
    AVAPI=N32CORES
%else
    TARGET_NAME=VIRSCAN
    DEF_FILE=$(TARGET_NAME).def
    AVAPI=NAVCORES
%endif

OBJS = virscan.obj 

%If "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""
# WINDOWS

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).DLL

$(PROGRAM_NAME): $(STUB) \
 $(OBJS)          \
 $(Q_SYMKRNL).LIB \
 $(DEF_FILE)      \
 virscan.res      \
 $(AVAPI).LIB     \
 $(Q_SYMDEBUG).LIB\
 N32CALL.LIB

create_implib: $(TARGET_NAME).lib

$(TARGET_NAME).lib: $(DEF_FILE)



%ElseIf "$(PLATFORM)" == ".DX" || "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" == ".NCX"
# DX

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(Q_VIRSCAN).LIB


$(PROGRAM_NAME):    $(OBJS) \
                    NAVCORES.LIB

%endif


#UPDATE#
virscan.obj:    platform.h avapi.h virscan.h
#ENDUPDATE#

