# Copyright 1996 Symantec Corporation                                   
#############################################################################
# 
# $Header:   S:/NAVFAMLY/VCS/avapidll.mav   1.13   29 Apr 1997 15:04:02   JBRENNA  $ 
# 
# Description:
# 
#############################################################################
# $Log:   S:/NAVFAMLY/VCS/avapidll.mav  $ 
#  
#     Rev 1.13   29 Apr 1997 15:04:02   JBRENNA
#  Ensure that this project does not build on the Trunk. Use AVAPI1 instead
#  of this project.
#  
#     Rev 1.12   25 Jul 1996 18:23:28   DALLEE
#  Added STATIC.OBJ and removed static link to NAVEX DLL.
#  
#     Rev 1.11   08 Apr 1996 18:51:12   CNACHEN
#  Added \'s to fix problems...
#  
#     Rev 1.10   27 Feb 1996 11:31:26   BARRY
#  Add PAM to dependencies
#  
#     Rev 1.9   09 Jan 1996 12:37:56   GDZIECI
#  Added NAVEX support. Fixed the header.
#############################################################################

all:
    @echo NAVFAMLY is obsolete. Build the AVAPI1 project instead.


%If ("$[u,$(PLATFORM)]" == ".WIN")
    MODEL=L
    TARGET_NAME= NAVCORE
%ElseIf ("$[u,$(PLATFORM)]" == ".W32")
    TARGET_NAME= N32CORE
%EndIf

OBJS= \
 aventry.obj    \
 avapi.obj      \
 boot.obj       \
 static.obj     \
 scanmem.obj 

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).DLL

$(PROGRAM_NAME): $(OBJS) \
 CERTLIBN.LIB \
 AVAPIDLL.DEF \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMDEBUG).LIB \
%If ("$[u,$(PLATFORM)]" == ".WIN")
 PAM16.LIB
%ElseIf ("$[u,$(PLATFORM)]" == ".W32")
 PAM32.LIB
%EndIf


#UPDATE#
boot.obj:	platform.h disk.h 
avapi.obj:	platform.h xapi.h virscan.h cts4.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		ctsn.h disk.h file.h repair.h avapi.h 
aventry.obj:	platform.h 
#ENDUPDATE#
