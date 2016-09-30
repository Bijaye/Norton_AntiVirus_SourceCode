# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=NetBrApp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to NetBrApp - Win32 Debug.
!ENDIF

!IF "$(CFG)" != "NetBrExt - Win32 Release" && "$(CFG)" !=\
 "NetBrExt - Win32 Debug" && "$(CFG)" != "NetBrApp - Win32 Release" && "$(CFG)"\
 != "NetBrApp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE
!MESSAGE NMAKE /f "NetBrEVC.mak" CFG="NetBrApp - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "NetBrExt - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetBrExt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetBrApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NetBrApp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
################################################################################
# Begin Project
# PROP Target_Last_Scanned "NetBrApp - Win32 Debug"

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AppR"
# PROP BASE Intermediate_Dir "NetExtR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AppR"
# PROP Intermediate_Dir "NetExtR"
# PROP Target_Dir ""
OUTDIR=.\AppR
INTDIR=.\NetExtR

ALL : "$(OUTDIR)\NETBREXT.dll"

CLEAN :
	-@erase "$(INTDIR)\DISPMSG.OBJ"
	-@erase "$(INTDIR)\NetBrEVC.pch"
	-@erase "$(INTDIR)\NetBrExt.obj"
	-@erase "$(INTDIR)\NetBrExt.res"
	-@erase "$(INTDIR)\NETBRWSD.OBJ"
	-@erase "$(INTDIR)\NetTree.obj"
	-@erase "$(INTDIR)\NRESCONT.OBJ"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Wndsize.obj"
	-@erase "$(OUTDIR)\NETBREXT.dll"
	-@erase "$(OUTDIR)\NETBREXT.exp"
	-@erase "$(OUTDIR)\NETBREXT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D\
 "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE"\
 /Fp"$(INTDIR)/NetBrEVC.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c
CPP_OBJS=.\NetExtR/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NetBrExt.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NetBrEVC.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 mpr.lib /nologo /subsystem:windows /dll /machine:I386 /out:"AppR/NETBREXT.dll"
LINK32_FLAGS=mpr.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/NETBREXT.pdb" /machine:I386 /def:".\NetBrExt.def"\
 /out:"$(OUTDIR)/NETBREXT.dll" /implib:"$(OUTDIR)/NETBREXT.lib"
DEF_FILE= \
	".\NetBrExt.def"
LINK32_OBJS= \
	"$(INTDIR)\DISPMSG.OBJ" \
	"$(INTDIR)\NetBrExt.obj" \
	"$(INTDIR)\NetBrExt.res" \
	"$(INTDIR)\NETBRWSD.OBJ" \
	"$(INTDIR)\NetTree.obj" \
	"$(INTDIR)\NRESCONT.OBJ" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Wndsize.obj"

"$(OUTDIR)\NETBREXT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AppD"
# PROP BASE Intermediate_Dir "NetExtD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AppD"
# PROP Intermediate_Dir "NetExtD"
# PROP Target_Dir ""
OUTDIR=.\AppD
INTDIR=.\NetExtD

ALL : "$(OUTDIR)\NETBREXT.dll"

CLEAN :
	-@erase "$(INTDIR)\DISPMSG.OBJ"
	-@erase "$(INTDIR)\NetBrEVC.pch"
	-@erase "$(INTDIR)\NetBrExt.obj"
	-@erase "$(INTDIR)\NetBrExt.res"
	-@erase "$(INTDIR)\NETBRWSD.OBJ"
	-@erase "$(INTDIR)\NetTree.obj"
	-@erase "$(INTDIR)\NRESCONT.OBJ"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\Wndsize.obj"
	-@erase "$(OUTDIR)\NETBREXT.dll"
	-@erase "$(OUTDIR)\NETBREXT.exp"
	-@erase "$(OUTDIR)\NETBREXT.ilk"
	-@erase "$(OUTDIR)\NETBREXT.lib"
	-@erase "$(OUTDIR)\NETBREXT.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT"\
 /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE"\
 /Fp"$(INTDIR)/NetBrEVC.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c
CPP_OBJS=.\NetExtD/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NetBrExt.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NetBrEVC.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 mpr.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"AppD/NETBREXT.dll"
LINK32_FLAGS=mpr.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/NETBREXT.pdb" /debug /machine:I386 /def:".\NetBrExt.def"\
 /out:"$(OUTDIR)/NETBREXT.dll" /implib:"$(OUTDIR)/NETBREXT.lib"
DEF_FILE= \
	".\NetBrExt.def"
LINK32_OBJS= \
	"$(INTDIR)\DISPMSG.OBJ" \
	"$(INTDIR)\NetBrExt.obj" \
	"$(INTDIR)\NetBrExt.res" \
	"$(INTDIR)\NETBRWSD.OBJ" \
	"$(INTDIR)\NetTree.obj" \
	"$(INTDIR)\NRESCONT.OBJ" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Wndsize.obj"

"$(OUTDIR)\NETBREXT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AppR"
# PROP BASE Intermediate_Dir "NetAppR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AppR"
# PROP Intermediate_Dir "NetAppR"
# PROP Target_Dir ""
OUTDIR=.\AppR
INTDIR=.\NetAppR

ALL : "NetBrExt - Win32 Release" "$(OUTDIR)\NETBRAPP.exe"

CLEAN :
	-@erase "$(INTDIR)\NetBrApD.obj"
	-@erase "$(INTDIR)\NetBrApp.obj"
	-@erase "$(INTDIR)\NetBrApp.res"
	-@erase "$(INTDIR)\NetBrEVC.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\NETBRAPP.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)/NetBrEVC.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c
CPP_OBJS=.\NetAppR/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NetBrApp.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NetBrEVC.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 AppR\NetBrExt.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"AppR/NETBRAPP.exe"
LINK32_FLAGS=AppR\NetBrExt.lib /nologo /entry:"wWinMainCRTStartup"\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/NETBRAPP.pdb" /machine:I386\
 /out:"$(OUTDIR)/NETBRAPP.exe"
LINK32_OBJS= \
	"$(INTDIR)\NetBrApD.obj" \
	"$(INTDIR)\NetBrApp.obj" \
	"$(INTDIR)\NetBrApp.res" \
	"$(INTDIR)\StdAfx.obj" \
	"$(OUTDIR)\NETBREXT.lib"

"$(OUTDIR)\NETBRAPP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AppD"
# PROP BASE Intermediate_Dir "NetAppD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "AppD"
# PROP Intermediate_Dir "NetAppD"
# PROP Target_Dir ""
OUTDIR=.\AppD
INTDIR=.\NetAppD

ALL : "NetBrExt - Win32 Debug" "$(OUTDIR)\NETBRAPP.exe"

CLEAN :
	-@erase "$(INTDIR)\NetBrApD.obj"
	-@erase "$(INTDIR)\NetBrApp.obj"
	-@erase "$(INTDIR)\NetBrApp.res"
	-@erase "$(INTDIR)\NetBrEVC.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\NETBRAPP.exe"
	-@erase "$(OUTDIR)\NETBRAPP.ilk"
	-@erase "$(OUTDIR)\NETBRAPP.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)/NetBrEVC.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c
CPP_OBJS=.\NetAppD/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/NetBrApp.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/NetBrEVC.bsc"
BSC32_SBRS= \

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 AppD\NetBrExt.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"AppD/NETBRAPP.exe"
LINK32_FLAGS=AppD\NetBrExt.lib /nologo /entry:"wWinMainCRTStartup"\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/NETBRAPP.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/NETBRAPP.exe"
LINK32_OBJS= \
	"$(INTDIR)\NetBrApD.obj" \
	"$(INTDIR)\NetBrApp.obj" \
	"$(INTDIR)\NetBrApp.res" \
	"$(INTDIR)\StdAfx.obj" \
	"$(OUTDIR)\NETBREXT.lib"

"$(OUTDIR)\NETBRAPP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF

################################################################################
# Begin Target

# Name "NetBrExt - Win32 Release"
# Name "NetBrExt - Win32 Debug"

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

!ENDIF

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrExt.cpp

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_NETBR=\
	{$(INCLUDE)}"\DllRCSta.h"\
	{$(INCLUDE)}"\stdafx.h"\


"$(INTDIR)\NetBrExt.obj" : $(SOURCE) $(DEP_CPP_NETBR) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_NETBR=\
	{$(INCLUDE)}"\DllRCSta.h"\
	{$(INCLUDE)}"\stdafx.h"\


"$(INTDIR)\NetBrExt.obj" : $(SOURCE) $(DEP_CPP_NETBR) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrExt.def

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_STDAF=\
	{$(INCLUDE)}"\stdafx.h"\

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D\
 "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE"\
 /Fp"$(INTDIR)/NetBrEVC.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \


"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetBrEVC.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_STDAF=\
	{$(INCLUDE)}"\stdafx.h"\

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT"\
 /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE"\
 /Fp"$(INTDIR)/NetBrEVC.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE) \


"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetBrEVC.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrExt.rc

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_RSC_NETBRE=\
	".\NETICONS.BMP"\


"$(INTDIR)\NetBrExt.res" : $(SOURCE) $(DEP_RSC_NETBRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_RSC_NETBRE=\
	".\NETICONS.BMP"\


"$(INTDIR)\NetBrExt.res" : $(SOURCE) $(DEP_RSC_NETBRE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Wndsize.cpp

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_WNDSI=\
	{$(INCLUDE)}"\stdafx.h"\
	{$(INCLUDE)}"\wndsize.h"\


"$(INTDIR)\Wndsize.obj" : $(SOURCE) $(DEP_CPP_WNDSI) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_WNDSI=\
	{$(INCLUDE)}"\stdafx.h"\
	{$(INCLUDE)}"\wndsize.h"\


"$(INTDIR)\Wndsize.obj" : $(SOURCE) $(DEP_CPP_WNDSI) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NETBRWSD.CPP

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_NETBRW=\
	".\dispmsg.h"\
	".\DllRCSta.h"\
	".\NetBrwsD.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\
	".\wndsize.h"\


"$(INTDIR)\NETBRWSD.OBJ" : $(SOURCE) $(DEP_CPP_NETBRW) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_NETBRW=\
	".\dispmsg.h"\
	".\DllRCSta.h"\
	".\NetBrwsD.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\
	".\wndsize.h"\


"$(INTDIR)\NETBRWSD.OBJ" : $(SOURCE) $(DEP_CPP_NETBRW) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetTree.cpp

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_NETTR=\
	".\DllRCSta.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\


"$(INTDIR)\NetTree.obj" : $(SOURCE) $(DEP_CPP_NETTR) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_NETTR=\
	".\DllRCSta.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\


"$(INTDIR)\NetTree.obj" : $(SOURCE) $(DEP_CPP_NETTR) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NRESCONT.CPP

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_NRESC=\
	".\dispmsg.h"\
	".\DllRCSta.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\


"$(INTDIR)\NRESCONT.OBJ" : $(SOURCE) $(DEP_CPP_NRESC) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_NRESC=\
	".\dispmsg.h"\
	".\DllRCSta.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NRESCONT.OBJ" : $(SOURCE) $(DEP_CPP_NRESC) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DISPMSG.CPP

!IF  "$(CFG)" == "NetBrExt - Win32 Release"

DEP_CPP_DISPM=\
	{$(INCLUDE)}"\stdafx.h"\
	

"$(INTDIR)\DISPMSG.OBJ" : $(SOURCE) $(DEP_CPP_DISPM) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ELSEIF  "$(CFG)" == "NetBrExt - Win32 Debug"

DEP_CPP_DISPM=\
	{$(INCLUDE)}"\stdafx.h"\
	

"$(INTDIR)\DISPMSG.OBJ" : $(SOURCE) $(DEP_CPP_DISPM) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "NetBrApp - Win32 Release"
# Name "NetBrApp - Win32 Debug"

!IF  "$(CFG)" == "NetBrApp - Win32 Release"

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "NetBrApp - Win32 Release"

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrApp.cpp
DEP_CPP_NETBRA=\
	".\NetBrApD.h"\
	".\NetBrApp.h"\
	".\NetBrwsD.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\
	".\wndsize.h"\
	

"$(INTDIR)\NetBrApp.obj" : $(SOURCE) $(DEP_CPP_NETBRA) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	{$(INCLUDE)}"\stdafx.h"\
	

!IF  "$(CFG)" == "NetBrApp - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)/NetBrEVC.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetBrEVC.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)/NetBrEVC.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\NetBrEVC.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrApp.rc
DEP_RSC_NETBRAP=\
	".\NetBrApp.ico"\
	

"$(INTDIR)\NetBrApp.res" : $(SOURCE) $(DEP_RSC_NETBRAP) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\NetBrApD.cpp
DEP_CPP_NETBRAPD=\
	".\NetBrApD.h"\
	".\NetBrApp.h"\
	".\NetBrwsD.h"\
	".\NetTree.h"\
	".\NResCont.h"\
	".\stdafx.h"\
	".\wndsize.h"\
	

"$(INTDIR)\NetBrApD.obj" : $(SOURCE) $(DEP_CPP_NETBRAPD) "$(INTDIR)"\
 "$(INTDIR)\NetBrEVC.pch"


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "NetBrExt"

!IF  "$(CFG)" == "NetBrApp - Win32 Release"

"NetBrExt - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\NetBrEVC.mak" CFG="NetBrExt - Win32 Release" 

!ELSEIF  "$(CFG)" == "NetBrApp - Win32 Debug"

"NetBrExt - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\NetBrEVC.mak" CFG="NetBrExt - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
# End Project
################################################################################
