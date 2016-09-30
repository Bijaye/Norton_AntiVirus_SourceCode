########################################################################
# Copyright 1993 Symantec Corporation                                   
########################################################################
#                                                                       
# $Header:   S:/SYMINTEG/VCS/symintg.mav   1.21   07 May 1997 15:57:54   BILL  $
#                                                                       
# Description:                                                          
#                                                                       
########################################################################
# $Log:   S:/SYMINTEG/VCS/symintg.mav  $
#  
#     Rev 1.21   07 May 1997 15:57:54   BILL
#  Alpha changes
#  
#     Rev 1.20   28 Apr 1997 20:19:40   JBRENNA
#  Convert to new CORE_* platform specification.
#  
#     Rev 1.19   13 Feb 1997 13:28:48   MKEATIN
#  Added NCX
#  
#     Rev 1.18   19 Sep 1996 13:33:24   JALLEE
#  Added DVX platform.
#  
#     Rev 1.17   31 Jan 1996 19:36:24   MKEATIN
#  Added DX platform
#  
#     Rev 1.16   23 Jan 1995 13:04:38   DALLEE
#  Set target type to LIB for .DOS platform.
########################################################################

OBJS = symintg.obj
%If "$(CORE_PLAT)" == "I"
CERTLIB=CERTLIBN.LIB
%Endif

%If "$(CORE_WIN32)" != ""
    TARGET_NAME=S32INTEG
%Else
    TARGET_NAME=SYMINTEG
%EndIf

%If "$(CORE_WIN16)" != "" || "$(CORE_WIN32)" != ""

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).DLL

$(PROGRAM_NAME): \
 $(STUB) \
 $(OBJS) \
 $(Q_SYMKRNL).LIB \
 SYMINTG.RES \
 $(Q_SYMDEBUG).LIB \
 SYMINTG.DEF \
 $(CERTLIB) 


%elseif "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".DX" || \
        "$(PLATFORM)" == ".DVX" || "$(PLATFORM)" == ".NCX"

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)

%endif


#UPDATE#
symintg.obj:	platform.h platnlm.h ctsn.h cts4.h syminteg.h 
#ENDUPDATE#
