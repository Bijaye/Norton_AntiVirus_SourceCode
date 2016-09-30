# Copyright 1996-1997 Symantec, Peter Norton Product Group
#************************************************************************
# $Header:   S:/VIRLIST/VCS/virlist.mav   1.12   18 Jun 1997 18:45:14   MKEATIN  $
#
# Description:
#   Makefile for VirList project.
#
#************************************************************************
# $Log:   S:/VIRLIST/VCS/virlist.mav  $
#  
#     Rev 1.12   18 Jun 1997 18:45:14   MKEATIN
#  We now link to N32CALL.LIB
#  
#     Rev 1.11   17 Jun 1997 19:07:08   MKEATIN
#  Added n32callbk.lib.  This is currently checked into nobuilds since 
#  there's no CALLBACK project in AVCORE.
#  
#     Rev 1.10   21 May 1997 19:33:46   BILL
#  Removed certlibn for win32
#  
#     Rev 1.9   08 May 1997 15:36:28   BILL
#  Changed comment characters to # from // to make MAKE.exe happy.
#  
#     Rev 1.8   29 Apr 1997 15:31:16   JBRENNA
#  Use CORE_WIN32 for checking instead of W32.
# 
#************************************************************************

USESTATICAV=Yes


%if "$(CORE_WIN32)" != ""
    TARGET_NAME=N32VLIST

    %if "$(USESTATICAV)" == "Yes"
        AV_NAME=N32CORES
        CERTLIB=
    %else
        AV_NAME=N32CORE
        CERTLIB=
    %endif

    NAVUTIL=N32UTILS
%else
    MODEL=L
    TARGET_NAME=NAVVLIST

    %if "$(USESTATICAV)" == "Yes"
        AV_NAME=NAVCORES
        CERTLIB=CERTLIBN.LIB
    %else
        AV_NAME=NAVCORE
        CERTLIB=
    %endif

    NAVUTIL=N16UTILS
%endif

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
#PROGRAM_NAME=$(Q_VIRLIST).DLL
PROGRAM_NAME=$(TARGET_NAME).DLL


$(PROGRAM_NAME): \
 $(STUB) \
 V_INFO.OBJ \
 V_VIRLIB.OBJ \
 V_DLL.OBJ \
 V_ERROR.OBJ \
 V_VIRINF.OBJ \
 V_DELETE.OBJ \
 V_RESET.OBJ \
 V_PRINT.OBJ \
 V_VLIST.OBJ \
 $(AV_NAME).LIB \
 $(NAVUTIL).LIB \
 VIRLIST.DEF \
 VIRLIST.RES \
 $(Q_SYMDEBUG).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMUTIL).LIB \
 $(Q_SYMGUI).LIB \
 $(Q_NPT).LIB \
 $(CERTLIB) \
 N32CALL.LIB
 
VIRLIST.RES:    VIRLIST.RC VIRUSSTB.H VIRUSDLG.H CHECK.BMP

#UPDATE#
v_virlib.obj:	platform.h xapi.h file.h disk.h virusdbf.h ngui.h \
		version.h quakever.h wndclass.h passwd.h stockbmp.h avapi.h \
		ctsn.h callback.h virlist.h virusstb.h virusdlg.h stack.hpp 
v_info.obj:	platform.h ngui.h version.h quakever.h wndclass.h passwd.h \
		stockbmp.h xapi.h symintl.h npt.h ctl3d.h file.h stdhelp.h \
		virusdbf.h avapi.h ctsn.h callback.h virlist.h virusstb.h \
		virusdlg.h stack.hpp 
v_dll.obj:	platform.h virusdbf.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h avapi.h ctsn.h callback.h \
		virlist.h virusstb.h virusdlg.h stack.hpp 
v_error.obj:	platform.h ngui.h version.h quakever.h wndclass.h \
		passwd.h stockbmp.h virusdbf.h avapi.h ctsn.h callback.h \
		virlist.h virusstb.h virusdlg.h stack.hpp 
v_delete.obj:	platform.h ctl3d.h stdhelp.h virusdbf.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h avapi.h ctsn.h \
		callback.h virlist.h virusstb.h virusdlg.h stack.hpp 
v_virinf.obj:	platform.h ngui.h version.h quakever.h wndclass.h \
		passwd.h stockbmp.h xapi.h npt.h ctl3d.h stdhelp.h virusdbf.h \
		avapi.h ctsn.h callback.h virlist.h virusstb.h virusdlg.h \
		stack.hpp virusdlg.h
v_print.obj:	platform.h hstring.h virusdbf.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h avapi.h ctsn.h \
		callback.h virlist.h virusstb.h virusdlg.h stack.hpp 
v_reset.obj:	platform.h virusdbf.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h avapi.h ctsn.h callback.h \
		virlist.h virusstb.h virusdlg.h stack.hpp 
#ENDUPDATE#
