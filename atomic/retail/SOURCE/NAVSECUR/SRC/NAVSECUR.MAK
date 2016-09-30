########################################################################
# Copyright 1993 Symantec Corporation                                   
########################################################################
#                                                                       
# $Header:   S:/NAVSECUR/VCS/NAVSECUR.MAv   1.0   30 Jan 1996 15:54:06   BARRY  
#                                                                       
# Description:                                                          
#   Contains File, Memory and Boot Scanning routines
#                                                                       
########################################################################


%if "$(CORE_WIN32)" != ""
    TARGET_NAME=N32SECUR
#    XUTIL=N32XUTIL.LIB
    DEFINITION=NAVSECUR.D32
%else
    TARGET_NAME=NAVSECUR
    XUTIL=NAVXUTIL.LIB
    DEFINITION=NAVSECUR.DEF
%endif

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).DLL

navsecur.dll: \
 $(STUB) \
 securdll.obj \
 secinit.obj \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMGUI).LIB \
 $(DEFINITION) \
 $(XUTIL) \
 navsecur.res

#UPDATE#
securdll.obj:	platform.h security.h 
secinit.obj:	platform.h commdlg.h security.h options.h  
#ENDUPDATE#
