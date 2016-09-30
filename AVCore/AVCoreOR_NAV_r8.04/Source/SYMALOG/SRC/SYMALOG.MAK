# Copyright 1992 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/SYMALOG/VCS/symalog.mav   1.45   28 Apr 1997 20:09:42   JBRENNA  $ #
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMALOG/VCS/symalog.mav  $ 
#  
#     Rev 1.45   28 Apr 1997 20:09:42   JBRENNA
#  Convert to new CORE_* platform specification.
#  
#     Rev 1.44   13 Feb 1997 13:30:12   MKEATIN
#  Added NCX
#  
#     Rev 1.43   19 Sep 1996 13:30:26   JALLEE
#  Added DVX
#  
#     Rev 1.42   15 Aug 1996 09:35:30   JBRENNA
#  Use $(QUAKE_VER) instead of hard coding the quake version
#  
#########################################################################

MODEL=L
TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK


%If "$(CORE_WIN16)" != "" || "$(CORE_WIN32)" != ""

IMPEXT=DLL 

PROGRAM_NAME=$(Q_SYMALOG).DLL

LIBS= S32NAV$(QUAKE_VER).LIB $(Q_NPT).LIB 

# ##############################################################
#   This dead code should never be used. Use NPT
# ##############################################################
# symaprt.obj   \
# ##############################################################

$(PROGRAM_NAME) : symalog.obj   \
                  symaadd.obj   \
                  dllinit.obj   \
                  logio.obj     \
                  trimlog.obj   \
                  symalog.res   \
                  symalog.def

%ElseIf "$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".X86" || \
        "$(PLATFORM)" == ".DX"  || "$(PLATFORM)" == ".DVX" || \
        "$(PLATFORM)" == ".NCX"

PROGRAM_NAME=$(Q_SYMALOG).LIB
  
$(PROGRAM_NAME) : symalog.obj   \
                  symaadd.obj   \
                  logio.obj     \
                  trimlog.obj   \
                  symalogs.obj

%ElseIf "$(PLATFORM)" == ".NLM"

PROGRAM_NAME=$(Q_SYMALOG).LIB

$(PROGRAM_NAME) : logio.obj     \
                  trimlog.obj   \
                  symaadd.obj

%EndIf

#UPDATE#
dllinit.obj:	platform.h symalogi.h symalog.h logio.h 
dumplog.obj:	platform.h symalogi.h symalog.h logio.h stddos.h ibmpc.h \
		file.h 
logio.obj:	platform.h symserve.h symtime.h file.h logio.h 
symaadd.obj:	symalogi.h platform.h symalog.h symintl.h xapi.h file.h \
		logio.h 
symaadj.obj:    symalogi.h platform.h symalog.h ctl3d.h file.h \
		ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h \
		xapi.h logio.h 
symalog.obj:	symalogi.h platform.h symalog.h symintl.h xapi.h file.h \
		logio.h  
symaprt.obj:	symalogi.h platform.h symalog.h ctl3d.h xapi.h file.h \
                nmail.h version.h quakever.h ngui.h wndclass.h \
		passwd.h stockbmp.h npt.h hstring.h symarc.h symaprt.c
trimlog.obj:	platform.h xapi.h logio.h file.h symserve.h symtime.h \
		stdio.h
symalog.res:	symalmsg.rc symalog.rc symarc.h symalog.stb symalog.dlg \
                ngui.h version.h quakever.h wndclass.h passwd.h stockbmp.h 

symalmsg.rc:    symalmsg.mc
#ENDUPDATE#
