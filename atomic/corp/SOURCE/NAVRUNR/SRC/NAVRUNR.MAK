# Copyright 1996-1997 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/NAVRUNR/VCS/navrunr.mav   1.2   03 Jul 1997 10:01:20   JBRENNA  $ 
#
# Description:
#
########################################################################
# $Log:   S:/NAVRUNR/VCS/navrunr.mav  $
#  
#     Rev 1.2   03 Jul 1997 10:01:20   JBRENNA
#  Add RC file and update dependencies.
#  
#     Rev 1.1   01 Jul 1997 10:02:32   JBRENNA
#  Add NavRunR.obj dependency and S32DEBUG.LIB.
#
########################################################################

TARGET_TYPE=exe
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# ---------------------------------------------------------------------
# Compile NAVRUNR

%If "$(CORE_WIN32)" != ""
    PROGRAM_NAME= NAVRUNR.$(TARGET_TYPE)
%Else

# Platform is not W32 (required for NT)
BAD_PLATFORM:
    beep
    echo The $(PLATFORM) platform is not supported for the project.
%EndIf

LIBS += S32DEBUG.LIB

# ---------------------------------------------------------------------

$(PROGRAM_NAME):\
 navrunr.obj \
 navrunr.res \
 kernel32.lib \
 user32.lib

#UPDATE#
navrunr.res:	resource.h nav.ver 
navrunr.obj:	platform.h dbcs_str.h dosvmm.h nlm_str.h platform.h \
		dbcs_str.h dosvmm.h nlm_str.h 
#ENDUPDATE#
