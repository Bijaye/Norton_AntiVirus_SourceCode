# Copyright 1994 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/SYMKRNL/VCS/KRNLTHK.MAv   1.3   04 Dec 1996 17:28:44   BMCCORK  $ #
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMKRNL/VCS/KRNLTHK.MAv  $ #
#  
#     Rev 1.3   04 Dec 1996 17:28:44   BMCCORK
#  Changed to statically link CRT to solve OS bug on FreeLibrary() from dllmain
#  
#     Rev 1.2   02 Dec 1996 16:45:28   BMCCORK
#  Added test platform support
#  
#     Rev 1.1   18 Jul 1996 18:01:24   BMCCORK
#  Moved to CRT DLL
#  
#     Rev 1.0   26 Jan 1996 20:22:50   JREARDON
#  Initial revision.
#  
#     Rev 1.12   05 Jan 1996 12:39:46   JWORDEN
#  Added dependency for krnlthk.res
#  
#     Rev 1.11   01 Aug 1995 12:28:44   RJACKSO
#
#     Rev 1.10   17 Jul 1995 11:23:20   SCOTTP
#  bumped this to use the b950sdk off the r:drive
#
#     Rev 1.9   10 May 1995 11:27:24   JREARDON
#  Changed RC to b347
#
#     Rev 1.8   08 Mar 1995 19:30:08   BRIANF
#  Macro'd the thunk DLLs so that I can change their name for Quake Custom
#
#     Rev 1.7   06 Mar 1995 09:06:38   HENRI
#  Update the RC program path
#
#     Rev 1.6   14 Jan 1995 00:37:02   SCOTTP
#  Bumped rev number to 7
#
#
#########################################################################


TARGET_NAME=THUNK
TARGET_TYPE=DLL
## Statically link with the CRT DLL to avoid a bug in the Windows DLL processing
## Calling FreeLibrary() on the thunk DLL from Symkrnl can cause the CRT DLL to
## unload while Symkrnl is still using it!!  This can be avoided by statically linking.
## USE_CRT_DLL=TRUE

.INCLUDE <PLATFORM.MAK>
.INCLUDE <ML.MAK>
.INCLUDE <THUNK.MAK>

%If "$(PLATFORM)" == ".W32" || ("$(CORE_WIN32)" != "" && "$(CORE_PLAT)" == "I")

PROGRAM_NAME=$(Q_SYMKRNLTHUNK).DLL
CLOPTS+= /DSYM_COMPILEDTHUNK

$(PROGRAM_NAME):  $(STUB) \
                  $(TARGET_NAME).obj \
                  krnlthk.res \
                  32to16.obj \
		          krnlthk.d32


create_implib: $(PROGRAM_NAME).lib

#UPDATE#
32to16.asm: symkrnl.thk
#ENDUPDATE#

%ElseIf ("$(PLATFORM)" == ".32S")

PROGRAM_NAME=TKKES327.DLL
CLOPTS+= /DSYM_RUNTIMETHUNK

$(PROGRAM_NAME):  $(STUB)         \
                  $(TARGET_NAME).obj \
                  krnlthk.res \
                  krnlths.def

%ElseIf ("$(PLATFORM)" == ".WIN" || "$(CORE_WIN16)" != "")

PROGRAM_NAME=$(Q_SYMKRNLTHUNK).DLL
ASMOPTS+= /DIS_16
LIBS = $(Q_SYMKRNL).LIB TOOLHELP.LIB
RC=R:\WIN32SDK\MSTOOLS\BINW16\RC
RCBINDOPTS=-T -40

$(PROGRAM_NAME):  $(STUB) \
                  $(TARGET_NAME).obj \
                  32to16.obj \
                  krnlthk.res \
                  krnlthk.def
%EndIf
