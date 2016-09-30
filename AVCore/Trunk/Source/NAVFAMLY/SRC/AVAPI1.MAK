#-------------------------------------------------------------------------
#
# $Header:   S:/NAVFAMLY/VCS/AVAPI1.mav   1.4   29 Apr 1997 15:02:04   JBRENNA  $
#
# Description:
#       avapi makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVFAMLY/VCS/AVAPI1.mav  $
#  
#     Rev 1.4   29 Apr 1997 15:02:04   JBRENNA
#  Ensure that this project does not build on the Trunk. Use AVAPI1 instead
#  of this project.
#  
#     Rev 1.3   28 Apr 1997 19:04:22   MKEATIN
#  Fixed DX platform
#  
#     Rev 1.2   28 Apr 1997 18:11:48   MKEATIN
#  Added support for Avenge NTK
#  
#     Rev 1.1   28 Apr 1997 18:08:20   MKEATIN
#  Upgraded makefile for AVENGE support
#  
#     Rev 1.0   28 Apr 1997 17:28:40   JBRENNA
#  Initial revision.
#  
#     Rev 1.21   13 Feb 1997 12:01:06   MKEATIN
#  Added NCX
#  
#     Rev 1.20   11 Dec 1996 15:31:28   MZAREMB
#  Added Findall.cpp to the list of necessary files.
#  
#     Rev 1.19   19 Sep 1996 13:19:58   JALLEE
#  Added DVX platform.
#  
#     Rev 1.18   25 Jul 1996 18:25:46   DALLEE
#  For WIN16 and WIN32: Added STATIC.OBJ and removed link to NAVEX DLL.
#  
#     Rev 1.17   11 Apr 1996 07:45:30   JMILLAR
#  add NTK (NT kernel) platform
#  add override of stack check from Rad's .ntk build file
#  
#     Rev 1.16   09 Feb 1996 11:20:40   CNACHEN
#  Added PAM support.
#  
#     Rev 1.15   08 Feb 1996 16:25:58   DCHI
#  Added PAMVXD.LIB to VXD make.
#  
#     Rev 1.14   07 Feb 1996 15:34:20   CNACHEN
#  Added PAM support for NLM platform...
#  
#     Rev 1.13   07 Feb 1996 09:16:14   RSTANEV
#  VxD platform now includes STATIC.CPP
#
#     Rev 1.12   29 Nov 1995 18:04:18   GDZIECI
#  Checking in from VIRLIB 6.
#
#     Rev 1.11.1.2   26 Oct 1995 10:43:52   CNACHEN
#  Added REAL NAVEX supporT! :)
#
#     Rev 1.11.1.1   18 Oct 1995 11:20:42   CNACHEN
#  NAVEX support added.
#
#     Rev 1.11.1.0   16 Jan 1995 13:03:20   JJOHNSON
#  Branch base for version QAK6
#
#     Rev 1.11   19 Dec 1994 16:33:50   DALLEE
#  Put back MODEL=L for .WIN platform.  Way off in VIRLIST there are some
#  C++ contstructors that get confused with near / far pointers and work
#  with only for large model (for now...).
#
#     Rev 1.10   19 Dec 1994 14:13:42   DALLEE
#  Removed MODEL=L directive from .WIN platform.
#  Moved .DX and added .DOS to new section with type .LIB instead of .DLL.
#
#     Rev 1.9   07 Dec 1994 07:55:38   JMILLARD
#
#  add scanmem to object lists
#
#     Rev 1.8   01 Nov 1994 13:56:22   JMILLARD
#
#  add NLM
#
#     Rev 1.7   21 Oct 1994 12:57:56   KEITH
#  Cause the Win16 build to use DLL optimization and still create a LIB
#
#     Rev 1.6   15 Sep 1994 17:32:20   CAREY
#  removed avglob linking...
#
#     Rev 1.5   19 Aug 1994 15:50:44   MFALLEN
#  Compile large model.
#
#     Rev 1.4   19 Aug 1994 15:44:56   MFALLEN
#  Now builds correctly for 16-bit platform and QA suggested name changes.
#
#     Rev 1.3   19 Aug 1994 14:46:04   MFALLEN
#  Fixed names the way QA wants to see them.
#
#     Rev 1.2   15 Aug 1994 18:09:44   CAREY
#  added VXD platform
#
#     Rev 1.1   01 Aug 1994 16:15:26   DALLEE
#  Added DX platform support.  Updated dependencies.
#-------------------------------------------------------------------------

all:
    @echo NAVFAMLY is obsolete. Build the AVAPI1 project instead.

# Setting TARGETTYPE=DLL and using a target name with a .LIB extension
# allows .DLL optimization flags and still creates a static lib to
# link other files to create a .DLL. This is how the build system
# currently works.                                      KLM 10/21/94

OBJS= \
    avapi.obj   \
    findall.obj \
    scanmem.obj \
    boot.obj

%If ("$[u,$(PLATFORM)]" == ".WIN")
    MODEL=L
    TARGET_NAME= NAVCORES
    TARGET_TYPE=DLL

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAM16.LIB   \
    AVENGE.LIB


%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME= N32CORES
    TARGET_TYPE=LIB

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAM32.LIB   \
    AVENGE.LIB

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DOS") || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    TARGET_NAME=NAVCORES
    TARGET_TYPE=LIB

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAMDX.LIB   \
    AVENGE.LIB

%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")
    TARGET_NAME= NAVCORES
    TARGET_TYPE=LIB

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAMNLM.LIB   \
    AVENGE.LIB

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")
    TARGET_NAME= N32COREI
    TARGET_TYPE=LIB

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAMVXD.LIB   \
    AVENGE.LIB

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")
    TARGET_NAME= N32COREI
    TARGET_TYPE=LIB

XOPTS += /Gs10000

OBJS= \
    avapi.obj   \
    avapi_cb.obj\
    boot.obj    \
    static.obj  \
    PAMNTK.LIB  \
    AVENGE.LIB

%Else
BAD_PLATFORM:
    beep
    Echo '$(PLATFORM)' is not yet supported by the AVAPI lib makefile.

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)


#UPDATE#
boot.obj:	platform.h dbcs_str.h disk.h
avapi.obj:  platform.h dbcs_str.h ctsn.h xapi.h virscan.h cts4.h \
        virusinf.h disk.h file.h repair.h avapi.h callback.h
scanmem.obj:    platform.h dbcs_str.h xapi.h avapi.h ctsn.h callback.h
nlmload.obj: platform.h dbcs_str.h ctsn.h xapi.h virscan.h cts4.h \
        virusinf.h disk.h file.h repair.h avapi.h callback.h
static.obj: platform.h xapi.h avapi.h navex.h
#ENDUPDATE#
