#########################################################################
#
#
# $Header:   S:/NAVINST95/VCS/instscan.mav   1.3   13 Aug 1997 21:42:24   DSACKIN  $
#
#########################################################################
# $Log:   S:/NAVINST95/VCS/instscan.mav  $ #
#  
#     Rev 1.3   13 Aug 1997 21:42:24   DSACKIN
#  Added $(FAMILYVERS) to suppor the AVCORE branch.
#
#     Rev 1.2   02 Jul 1997 15:15:52   jtaylor
#  -- Added the navinst.rc navinst.res file to the instscan.dll.
#
#     Rev 1.1   18 Jun 1997 22:00:32   JBRENNA
#  Add linking to N32CALL.LIB for NAVSetDefsPath() function.
#
#     Rev 1.0   01 Jun 1997 19:59:54   jtaylor
#  Initial revision.
#
#
#########################################################################
TARGET_TYPE=DLL    ## Win32 DLL (x86)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

#MKMK-UPDATE#
## These libs are used for memory scanning.
LIBS += N32CORES.LIB
LIBS += $(Q_SYMKRNL).LIB
LIBS += N32CALL.LIB

%If "$(CORE_BMODE)" == "D"
LIBS += S32DEBUG.LIB
%EndIf

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

# need one-byte packing to read navstart.dat correctly
CLOPTS += -Zp1

ALL:    InstScan.dll

InstScan.dll:    scanmem.obj navinst.res

#UPDATE#
ScanMem.obj:    ScanMem.cpp ScanMem.h platform.h resource.h svcctrl.h cwstring.h \
                navstart.h avapi.h virscan.h xapi.h disk.h

NAVInst.res:    NAVInst.rc

#ENDUPDATE#
#MKMK-ENDUPDATE#