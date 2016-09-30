#######################################################################
## $Header:   S:/NAVINSTNT/VCS/NAVInstNT.MAv   1.14   18 Jul 1998 20:15:52   mdunn  $
## 04/28/97 19:49:16 sedward Created by MKMK 1.0
##                   from IDE makefile ROSWELL: NAVInst.mak
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
LIBS += DEFUTILS.LIB
LIBS += PROGMAN.LIB
LIBS += CHAN32$(CORE_PLAT).LIB
LIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\SVCCTRL.LIB
LIBS += VERSION.LIB

%if "$(CORE_BMODE)" == "D"
LIBS += S32DEBUG.LIB
%endif

# need one-byte packing to read navstart,dat correctly
CLOPTS += -Zp1


ALL:    NAVInstNT.dll

NAVInstNT.dll:    navuninst.obj \
		  navinst.obj \
		  NAVInst.res \
		  ISUTIL.OBJ \
		  CWSTRING.OBJ \
		  NAVInstMIF.obj \
		  CRegent.obj \
		  CUsgCnt.obj \
		  cverrsrc.obj

#UPDATE#
cverrsrc.obj:   cverrsrc.cpp cverrsrc.h
CWSTRING.OBJ:   CWSTRING.CPP cwstring.h


ISUTIL.OBJ:     ISUTIL.CPP cwstring.h IsRegUti.h IsUtil.h NAVInst.h \


NAVInst.res:    NAVInst.rc

navinst.obj:    navinst.cpp cwstring.h navapcom.h NAVInst.h profiler.h \
	svcctrl.h timercli.h

NAVInstMIF.obj: NAVInstMIF.cpp NAVInstMIF.h NAVInst.h

navuninst.obj:  navuninst.cpp cwstring.h IsRegUti.h IsUtil.h \
	navapcom.h NAVInst.h profiler.h svcctrl.h timercli.h \


#ENDUPDATE#
#MKMK-ENDUPDATE#
