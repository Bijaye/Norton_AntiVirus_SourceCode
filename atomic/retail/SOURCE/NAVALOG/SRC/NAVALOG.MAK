########################################################################
#
# $Header:   S:/NAVALOG/VCS/NAVALOG.MAv   1.6   31 Mar 1998 12:53:42   PVENKAT  $
#
# Description:
#       Activity log display module.
#
########################################################################
# $Log:   S:/NAVALOG/VCS/NAVALOG.MAv  $
#  
#     Rev 1.6   31 Mar 1998 12:53:42   PVENKAT
#  Fixed a build issue.  Remaned N32PDLL.DLL to N32PDLL.LIB.
#  
#  
#     Rev 1.5   25 Mar 1998 15:00:30   PVENKAT
#  Added N32PDLL.DLL.
#  
#  
#     Rev 1.4   15 Jul 1997 13:44:56   DBUCHES
#  Added Ctl3d support for DEC Alpha
#  
#     Rev 1.3   29 Apr 1997 20:16:00   JBRENNA
#  Convert to new CORE_* platform spec.
#  
#     Rev 1.2   09 Apr 1997 17:42:52   DALLEE
#  Oops! Forgot to remove STUB.OBJ last rev.
########################################################################

%If ("$(CORE_WIN32)" != "")

  %if ("$[u,$(TARGET)]" == "NAVWNT")
    TARGET_NAME=XNTALOG
    NAVUTILITY=XNTXUTIL
    DEF += /DNAVSCANW /DNAVWNT
  %ElseIf ("$[u,$(TARGET)]" == "NAVSCANW")
    TARGET_NAME=NNTALOG
    NAVUTILITY=NNTXUTIL
    DEF += /DNAVSCANW
  %else
    TARGET_NAME=N32ALOG
    NAVUTILITY=N32XUTIL
  %endif

  NAVUTILS=N32UTILS
  PASSWORDCHECK=N32SECUR
  TARGET_TYPE=DLL

%Else

DUMMY_TARGET:
    beep
    echo "$(PLATFORM)" is not a valid platform for the $(PROJNAME) project.

%EndIf


PROGRAM_NAME = $(TARGET_NAME).DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS= \
 $(Q_SYMALOG).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_NPT).LIB \
 $(Q_SYMUTIL).LIB \
 $(Q_SYMGUI).LIB \
 $(Q_SYMDEBUG).LIB \
 $(NAVUTILS).LIB \
 $(NAVUTILITY).LIB \
 $(PASSWORDCHECK).LIB \
 COMCTL32.LIB \
 N32PDLL.LIB

 # Add CTL3D for DEC Alpha platform.
%If ("$(CORE_PLAT)" == "A")
LIBS += CTL3D32.LIB
%endif
 
 

$(PROGRAM_NAME): \
 l_dll.obj \
 l_show.obj \
 l_filter.obj \
 l_clear.obj \
 navalog.res

$(TARGET_NAME).res: \
 navalog.rc \
 actilog.dlg \
 ngui.h \
 stdhelp.h \
 actdlg.h \
 actilog.stb \
 nav.ico

#UPDATE#
l_clear.obj:	actilogi.h platform.h platnlm.h options.h navopts.h \
		exclude.h cts4.h netobj.h symnet.h actilog.h symalog.h \
		ctl3d.h ngui.h version.h quakever.h wndclass.h passwd.h \
		stdhelp.h navutil.h symcfg.h navw_hlp.h actstb.h actdlg.h \
		cmbstr.h msgdisp.h
l_filter.obj:	actilogi.h platform.h platnlm.h options.h \
		navopts.h exclude.h cts4.h netobj.h symnet.h actilog.h \
		symalog.h symintl.h xapi.h ctl3d.h stdhelp.h ngui.h version.h \
		quakever.h wndclass.h passwd.h cmbstr.h navutil.h symcfg.h \
		actdlg.h actstb.h
l_show.obj:	actilogi.h platform.h platnlm.h options.h navopts.h \
		exclude.h cts4.h netobj.h symnet.h actilog.h symalog.h xapi.h \
		file.h npt.h quakever.h version.h ngui.h wndclass.h passwd.h \
		vwr.h ctl3d.h stdhelp.h msgdisp.h navw_hlp.h navutil.h \
		symcfg.h cmbstr.h security.h actdlg.h actstb.h
l_dll.obj:	actilogi.h platform.h platnlm.h options.h navopts.h \
		exclude.h cts4.h netobj.h symnet.h actilog.h symalog.h \
		cmbstr.h
stub.obj: platform.h security.h
#ENDUPDATE#
