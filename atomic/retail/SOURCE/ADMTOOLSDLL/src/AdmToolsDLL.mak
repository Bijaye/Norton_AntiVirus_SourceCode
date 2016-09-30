#######################################################################
## $Header:   S:/ADMTOOLSDLL/VCS/AdmToolsDLL.mav   1.2   23 Jun 1998 14:55:46   mdunn  $
## 05-28-98 12:14:16 mdunn Created by MKMK 1.0
##                   from IDE makefile ATOMIC:ADMTOOLSDLL atooldll.mak
#######################################################################
TARGET_TYPE=DLL    ## Win32 DLL (x86)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK


#MKMK-UPDATE#
LIBS += KERNEL32.LIB
LIBS += USER32.LIB
LIBS += GDI32.LIB
LIBS += WINSPOOL.LIB
LIBS += COMDLG32.LIB
LIBS += ADVAPI32.LIB
LIBS += SHELL32.LIB
LIBS += OLE32.LIB
LIBS += OLEAUT32.LIB
LIBS += UUID.LIB
LIBS += ODBC32.LIB
LIBS += ODBCCP32.LIB
LIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\SVCCTRL.LIB

ALL:	atooldll.dll 

atooldll.dll:	Atooldll.obj atooldll.res

#UPDATE#
Atooldll.obj:	Atooldll.cpp platform.h svcctrl.h platnlm.h os2win.h \
	platvxd.h platntk.h platmac.h platwin.h symvxd.h dbcs_str.h \
	dosvmm.h nlm_str.h othunk.h drvseg.h profiler.h timercli.h \
	

#ENDUPDATE#
#MKMK-ENDUPDATE#
