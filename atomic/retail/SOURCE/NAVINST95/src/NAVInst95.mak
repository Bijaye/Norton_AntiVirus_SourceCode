#########################################################################
#
#
# $Header:   S:/NAVINST95/VCS/navinst95.mav   1.18   14 Jan 1999 19:36:22   CEATON  $
#
#########################################################################
# $Log:   S:/NAVINST95/VCS/navinst95.mav  $ #
#  
#     Rev 1.18   14 Jan 1999 19:36:22   CEATON
#  Removed SVCCTRL.LIB from LIBS list. Not needed to build project.
#  
#  
#     Rev 1.17   26 Oct 1998 12:45:52   mdunn
#  Hey!  Some gremlin stuck a ] in the file.  That wasn't very nice!
#  
#     Rev 1.16   26 Oct 1998 12:40:32   mdunn
#  Added cverrsrc.obj and version.lib
#  
#     Rev 1.15   14 Aug 1998 10:39:14   mdunn
#  Added dostrial.obj
#  
#     Rev 1.14   23 Jul 1998 19:27:18   mdunn
#  Nuked that define
#  
#     Rev 1.13   22 Jul 1998 21:07:10   mdunn
#  Added _LOGFILE_TRACE_ define for migration uninstall debugging.
#  Remove this before GM or else!!!
#
#     Rev 1.12   30 Jun 1998 12:05:26   tcashin
#  Removed specialdefutils.cpp.
#
#     Rev 1.11   18 Jun 1998 00:00:42   tcashin
#  Added SpecialDefUtils.cpp
#
#     Rev 1.10   26 Mar 1998 19:08:04   mdunn
#  Added the CHAN32I.LIB back in.
#
#     Rev 1.9   16 Mar 1998 18:46:02   mdunn
#  Backed out the last change - it wouldn't work!!  Tim's going to fix it in
#  CDefUtils.  (For those of you playing along at home, the problem is that
#  linking with the patch32/chan32 DLLs is impossible since they won't be
#  installed when the install DLL is loaded!)
#
#     Rev 1.8   16 Mar 1998 18:33:14   mdunn
#  Added Microdefs LIBs to the list of libs.
#
#     Rev 1.7   13 Aug 1997 21:42:12   DSACKIN
#  Added $(FAMILYVERS) to suppor the AVCORE branch.
#
#     Rev 1.6   18 Jul 1997 11:25:06   JALLEE
#  Ported registry utility changes from BORG.
#   -- Removed ISReguti.obj.
#   -- Added CRegent.obj
#   -- Added CusgCnt.obj
#
#     Rev 1.5   02 Jun 1997 01:29:10   jtaylor
#  Added defutil.lib for Hawking.
#
#     Rev 1.4   01 Jun 1997 20:00:16   jtaylor
#  Removed the memory scanning libs.
#
#
#########################################################################
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
LIBS += VERSION.LIB
LIBS += CHAN32$(CORE_PLAT).LIB

# need one-byte packing to read navstart.dat correctly
CLOPTS += -Zp1

ALL:    NAVInst95.dll

NAVInst95.dll:  navuninst.obj \
                navinst.obj \
                NAVInst.res \
                ISUTIL.OBJ \
                CWSTRING.OBJ \
                NAVInstMIF.obj \
                SIWSched.obj \
                CRegent.OBJ \
                CUsgCnt.OBJ \
                cverrsrc.obj \
                dostrial.obj



#UPDATE#
CWSTRING.OBJ:	CWSTRING.CPP cwstring.h

# ISREGUTI.OBJ:   ISREGUTI.CPP cwstring.h IsRegUti.h IsUtil.h NAVInst.h \


ISUTIL.OBJ:	ISUTIL.CPP cwstring.h IsRegUti.h IsUtil.h NAVInst.h \


NAVInst.res:	NAVInst.rc

navinst.obj:	navinst.cpp cwstring.h navapcom.h NAVInst.h profiler.h \
	svcctrl.h timercli.h

NAVInstMIF.obj: NAVInstMIF.cpp NAVInstMIF.h NAVInst.h

navuninst.obj:	navuninst.cpp cwstring.h IsRegUti.h IsUtil.h \
	navapcom.h NAVInst.h profiler.h svcctrl.h timercli.h \

SIWSched.obj: SIWSched.cpp NAVInst.h


#ENDUPDATE#
#MKMK-ENDUPDATE#
