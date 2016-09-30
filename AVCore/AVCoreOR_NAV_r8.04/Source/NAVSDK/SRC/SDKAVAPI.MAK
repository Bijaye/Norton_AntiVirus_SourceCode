#-------------------------------------------------------------------------
#
# $Header:   S:/NAVSDK/VCS/sdkavapi.mav   1.0   09 Dec 1997 18:42:18   DALLEE  $
#
# Description:
#   NAVSDK makefile to compile AVAPI.
#   This is required to remove the export hints from the VLxxx functions.
#   We do not want to expose these API's from NAVSDK.DLL
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVSDK/VCS/sdkavapi.mav  $
#  
#     Rev 1.0   09 Dec 1997 18:42:18   DALLEE
#  Initial revision.
#-------------------------------------------------------------------------

# Setting TARGETTYPE=DLL and using a target name with a .LIB extension
# allows .DLL optimization flags and still creates a static lib to
# link other files to create a .DLL. This is how the build system
# currently works.                                      KLM 10/21/94

#---------------------------------------
# Win32 platforms
#---------------------------------------
%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=SDKAVAPI
    TARGET_TYPE=DLL

DEF+= /DQCUST /DQCUSTNOX

OBJS= \
    avapi.obj   \
    boot.obj    \
    static.obj  \
    PAM32.LIB   \
    AVENGE.LIB

#---------------------------------------
# Invalid platform
#---------------------------------------
%Else

BAD_PLATFORM:
    beep
    Echo '$(PLATFORM)' is not yet supported by the AVAPI lib makefile.

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)


#UPDATE#
boot.obj:       platform.h dbcs_str.h disk.h
avapi.obj:      platform.h dbcs_str.h ctsn.h xapi.h virscan.h cts4.h \
                virusinf.h disk.h file.h repair.h avapi.h callback.h
static.obj:     platform.h xapi.h avapi.h navex.h
#ENDUPDATE#
