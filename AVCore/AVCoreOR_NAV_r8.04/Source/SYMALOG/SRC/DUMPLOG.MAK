# Copyright 1994 Symantec Corporation                                   
#############################################################################
# 
# $Header:   S:/SYMALOG/VCS/DUMPLOG.MAv   1.8   22 Feb 1996 17:37:18   BARRY  $ 
# 
# Description:
# 
#############################################################################
# $Log:   S:/SYMALOG/VCS/DUMPLOG.MAv  $ 
#  
#     Rev 1.8   22 Feb 1996 17:37:18   BARRY
#  Remove mkdep for nstdhelp
#  
#     Rev 1.7   12 Jan 1996 12:14:00   JWORDEN
#  Port changes from Quake 6 and Quake 8
#  
#     Rev 1.6   12 Mar 1995 22:01:34   JMILLARD
#  
#  update for branched build system
#  
#     Rev 1.5   22 Aug 1994 17:26:50   JMILLARD
#  add standard file header
#  update dependencies
#############################################################################

TARGET_TYPE=EXE
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS= $(Q_SYMKRNL).LIB dosvmm.lib \
        dosdlg.lib      \
        dosdrvwn.lib    \
        dosio.lib       \
        dosnet.lib      \
        dostool.lib     \
        dosui.lib       \
        strings.lib

dumplog.exe:  logio.obj dumplog.obj \
              lnipatch.obj libhookl.obj

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
		passwd.h stockbmp.h npt.h hstring.h symarc.h 
trimlog.obj:	platform.h xapi.h logio.h file.h symserve.h symtime.h \
		stdio.h nwfile.h 
symalog.res:	symarc.h symalog.stb symalog.dlg ngui.h version.h \
                quakever.h wndclass.h passwd.h stockbmp.h 
#ENDUPDATE#
