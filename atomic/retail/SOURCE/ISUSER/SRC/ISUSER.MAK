#######################################################################
## $Header:   S:/ISUSER/VCS/ISUser.mav   1.0   07 May 1997 17:09:34   KSACKIN  $
## 05/05/97 19:36:04 sedward Created by MKMK 1.0
##                   from IDE makefile ROSWELL:ISUser _Isuser.mak
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

ALL:    _Isuser.dll

_Isuser.dll:    ISUser.res ISUser.OBJ

#UPDATE#
ISUser.OBJ:    ISUser.C

ISUser.res:    ISUser.RC sdrc.h

#ENDUPDATE#
#MKMK-ENDUPDATE#
