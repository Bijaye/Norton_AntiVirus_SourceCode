# Copyright 1993 Symantec Corporation
########################################################################
#
# $Header:   S:/NAVEXCLU/VCS/navexclu.mav   1.5   19 May 1997 19:11:28   BILL  $
#
# Description:
#       General Control Panel Entry Make File.
#
########################################################################
# $Log:   S:/NAVEXCLU/VCS/navexclu.mav  $
#  
#     Rev 1.5   19 May 1997 19:11:28   BILL
#  Removed certlibn reference (unneeded)
#  
#     Rev 1.4   11 May 1997 22:30:08   DALLEE
#  Added UTIL_LIB=N16/32UTILS.LIB to pick up new filename manipulation
#  and matching routines.
#  
#     Rev 1.3   30 Apr 1997 19:19:22   JBRENNA
#  Ops ... missed a VXD and NTK reference ... fixed now.
#  
#     Rev 1.2   30 Apr 1997 08:17:18   JBRENNA
#  Convert to new CORE_* platfrom checks.
#  
#     Rev 1.1   21 Mar 1997 18:55:50   DALLEE
#  Removed old Win16, Mirrors, and BANKERS references.
#  
#  
#     Rev 1.0   06 Feb 1997 20:57:54   RFULLER
#  Initial revision
#  
#     Rev 1.9   21 Nov 1996 16:32:46   MKEATIN
#  Added the NCX platform
#  
#     Rev 1.8   19 Sep 1996 14:00:28   JALLEE
#  Added DVX platform.
#  
#     Rev 1.7   31 Jul 1996 15:38:54   DSACKIN
#  Make all string comparisons non-case sensitive.
########################################################################

%If "$(CORE_WIN32)" != ""
  %if ("$[u,$(TARGET)]" == "NAVWNT")
     TARGET_NAME=XNTEXCLU
     DEF += /DNAVSCANW /DNAVWNT
     NAVUTIL=XNTXUTIL
  %Elseif ("$[u,$(TARGET)]" == "NAVSCANW")
     TARGET_NAME=NNTEXCLU
     DEF += /DNAVSCANW
     NAVUTIL=NNTXUTIL
  %else
     TARGET_NAME=N32EXCLU
     NAVUTIL=N32XUTIL
     DEF += /D_MBCS
  %endif
     UTIL_LIB=N32UTILS.LIB
%Else
     TARGET_NAME=NAVEXCLU
     NAVUTIL=NAVXUTIL
     UTIL_LIB=N16UTILS.LIB
%Endif

OBJS = \
 exclstuf.obj \
 excludel.obj \
 exclutil.obj \
 terminat.obj \
 exclmtch.obj \
%IF ("$(CORE_SOFT)" == "V" || "$(CORE_SOFT)" == "K")
 excluden.obj
%ENDIF



%IF ("$[u,$(PLATFORM)]" == ".DOS" || "$[u,$(PLATFORM)]" == ".DX" || \
     "$(CORE_SOFT)"     == "V"    || "$(CORE_SOFT)"     == "K"   || \
     "$[u,$(PLATFORM)]" == ".DVX" || "$[u,$(PLATFORM)]" == ".NCX")

TARGET_TYPE=LIB
PROGRAM_NAME    = $(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): \
  $(OBJS) \
  $(UTIL_LIB)



%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")

TARGET_TYPE=DLL
PROGRAM_NAME    = $(TARGET_NAME).DLL
SCRAM=
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# navcomm.lib \

$(PROGRAM_NAME): \
 $(OBJS) \
 excldll.obj \
 navexclu.res  \
 cmbstr.lib \
 $(NAVUTIL).LIB \
 $(UTIL_LIB) \
 $(Q_SYMGUI).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMDEBUG).LIB

%ENDIF


#UPDATE#
exclstuf.obj:	platform.h dbcs_str.h xapi.h exclude.h cts4.h ctsn.h \
		callback.h excludel.h navutil.h options.h navopts.h netobj.h \
		symnet.h register.h symcfg.h symalog.h drvprblm.h file.h
terminat.obj:	platform.h dbcs_str.h xapi.h exclude.h cts4.h ctsn.h \
		callback.h excludel.h navutil.h options.h navopts.h netobj.h \
		symnet.h register.h symcfg.h symalog.h drvprblm.h file.h \
		tsr.h tsrcomm.h
excldll.obj:	platform.h dbcs_str.h
exclutil.obj:	platform.h dbcs_str.h xapi.h file.h exclude.h cts4.h \
		ctsn.h callback.h excludel.h
excludel.obj:	platform.h dbcs_str.h file.h excludel.h
exclmtch.obj:	platform.h dbcs_str.h xapi.h exclude.h cts4.h ctsn.h \
		callback.h excludel.h
excluden.obj:   platform.h exclude.h
#ENDUPDATE#
