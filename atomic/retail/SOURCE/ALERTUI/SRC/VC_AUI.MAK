# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=AlertUI - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to AlertUI - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AlertUI - Win32 Release" && "$(CFG)" !=\
 "AlertUI - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "AlertUI.mak" CFG="AlertUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AlertUI - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AlertUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "AlertUI - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AlertUI - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\AlertUI.dll" "$(OUTDIR)\AlertUI.bsc"

CLEAN : 
	-@erase "$(INTDIR)\Alertmes.obj"
	-@erase "$(INTDIR)\Alertmes.sbr"
	-@erase "$(INTDIR)\Alertopt.obj"
	-@erase "$(INTDIR)\Alertopt.sbr"
	-@erase "$(INTDIR)\AlertUI.obj"
	-@erase "$(INTDIR)\AlertUI.pch"
	-@erase "$(INTDIR)\AlertUI.res"
	-@erase "$(INTDIR)\AlertUI.sbr"
	-@erase "$(INTDIR)\Editsmtp.obj"
	-@erase "$(INTDIR)\Editsmtp.sbr"
	-@erase "$(INTDIR)\EmailPSh.obj"
	-@erase "$(INTDIR)\EmailPSh.sbr"
	-@erase "$(INTDIR)\Forwrdpg.obj"
	-@erase "$(INTDIR)\Forwrdpg.sbr"
	-@erase "$(INTDIR)\HELPDLG.OBJ"
	-@erase "$(INTDIR)\HELPDLG.SBR"
	-@erase "$(INTDIR)\Netmespg.obj"
	-@erase "$(INTDIR)\Netmespg.sbr"
	-@erase "$(INTDIR)\Nlmpg.obj"
	-@erase "$(INTDIR)\Nlmpg.sbr"
	-@erase "$(INTDIR)\NQFONTS.OBJ"
	-@erase "$(INTDIR)\NQFONTS.SBR"
	-@erase "$(INTDIR)\Smtppg.obj"
	-@erase "$(INTDIR)\Smtppg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(OUTDIR)\AlertUI.bsc"
	-@erase "$(OUTDIR)\AlertUI.dll"
	-@erase "$(OUTDIR)\AlertUI.exp"
	-@erase "$(OUTDIR)\AlertUI.ilk"
	-@erase "$(OUTDIR)\AlertUI.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "SYM_WIN32" /D "SYM_MFC" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "SYM_WIN32" /D "SYM_MFC" /D "_UNICODE" /D "UNICODE"\
 /D "_USRDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/AlertUI.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/AlertUI.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AlertUI.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Alertmes.sbr" \
	"$(INTDIR)\Alertopt.sbr" \
	"$(INTDIR)\AlertUI.sbr" \
	"$(INTDIR)\Editsmtp.sbr" \
	"$(INTDIR)\EmailPSh.sbr" \
	"$(INTDIR)\Forwrdpg.sbr" \
	"$(INTDIR)\HELPDLG.SBR" \
	"$(INTDIR)\Netmespg.sbr" \
	"$(INTDIR)\Nlmpg.sbr" \
	"$(INTDIR)\NQFONTS.SBR" \
	"$(INTDIR)\Smtppg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\AlertUI.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 netbrext.lib navalert.lib xntxutil.lib s32utile.lib /nologo /subsystem:windows /dll /incremental:yes /machine:I386
# SUBTRACT LINK32 /map
LINK32_FLAGS=netbrext.lib navalert.lib xntxutil.lib s32utile.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/AlertUI.pdb"\
 /machine:I386 /def:".\AlertUI.def" /out:"$(OUTDIR)/AlertUI.dll"\
 /implib:"$(OUTDIR)/AlertUI.lib" 
DEF_FILE= \
	".\AlertUI.def"
LINK32_OBJS= \
	"$(INTDIR)\Alertmes.obj" \
	"$(INTDIR)\Alertopt.obj" \
	"$(INTDIR)\AlertUI.obj" \
	"$(INTDIR)\AlertUI.res" \
	"$(INTDIR)\Editsmtp.obj" \
	"$(INTDIR)\EmailPSh.obj" \
	"$(INTDIR)\Forwrdpg.obj" \
	"$(INTDIR)\HELPDLG.OBJ" \
	"$(INTDIR)\Netmespg.obj" \
	"$(INTDIR)\Nlmpg.obj" \
	"$(INTDIR)\NQFONTS.OBJ" \
	"$(INTDIR)\Smtppg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AlertUI.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\AlertUI.dll" "$(OUTDIR)\AlertUI.bsc"

CLEAN : 
	-@erase "$(INTDIR)\Alertmes.obj"
	-@erase "$(INTDIR)\Alertmes.sbr"
	-@erase "$(INTDIR)\Alertopt.obj"
	-@erase "$(INTDIR)\Alertopt.sbr"
	-@erase "$(INTDIR)\AlertUI.obj"
	-@erase "$(INTDIR)\AlertUI.pch"
	-@erase "$(INTDIR)\AlertUI.res"
	-@erase "$(INTDIR)\AlertUI.sbr"
	-@erase "$(INTDIR)\Editsmtp.obj"
	-@erase "$(INTDIR)\Editsmtp.sbr"
	-@erase "$(INTDIR)\EmailPSh.obj"
	-@erase "$(INTDIR)\EmailPSh.sbr"
	-@erase "$(INTDIR)\Forwrdpg.obj"
	-@erase "$(INTDIR)\Forwrdpg.sbr"
	-@erase "$(INTDIR)\HELPDLG.OBJ"
	-@erase "$(INTDIR)\HELPDLG.SBR"
	-@erase "$(INTDIR)\Netmespg.obj"
	-@erase "$(INTDIR)\Netmespg.sbr"
	-@erase "$(INTDIR)\Nlmpg.obj"
	-@erase "$(INTDIR)\Nlmpg.sbr"
	-@erase "$(INTDIR)\NQFONTS.OBJ"
	-@erase "$(INTDIR)\NQFONTS.SBR"
	-@erase "$(INTDIR)\Smtppg.obj"
	-@erase "$(INTDIR)\Smtppg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\AlertUI.bsc"
	-@erase "$(OUTDIR)\AlertUI.dll"
	-@erase "$(OUTDIR)\AlertUI.exp"
	-@erase "$(OUTDIR)\AlertUI.ilk"
	-@erase "$(OUTDIR)\AlertUI.lib"
	-@erase "$(OUTDIR)\AlertUI.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "SYM_WIN32" /D "SYM_MFC" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D\
 "SYM_WIN32" /D "SYM_MFC" /FR"$(INTDIR)/" /Fp"$(INTDIR)/AlertUI.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/AlertUI.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/AlertUI.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Alertmes.sbr" \
	"$(INTDIR)\Alertopt.sbr" \
	"$(INTDIR)\AlertUI.sbr" \
	"$(INTDIR)\Editsmtp.sbr" \
	"$(INTDIR)\EmailPSh.sbr" \
	"$(INTDIR)\Forwrdpg.sbr" \
	"$(INTDIR)\HELPDLG.SBR" \
	"$(INTDIR)\Netmespg.sbr" \
	"$(INTDIR)\Nlmpg.sbr" \
	"$(INTDIR)\NQFONTS.SBR" \
	"$(INTDIR)\Smtppg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\AlertUI.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 netbrext.lib navalert.lib xntxutil.lib s32utile.lib s32nave.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /incremental:no /map
LINK32_FLAGS=netbrext.lib navalert.lib xntxutil.lib s32utile.lib s32nave.lib\
 /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/AlertUI.pdb"\
 /debug /machine:I386 /def:".\AlertUI.def" /out:"$(OUTDIR)/AlertUI.dll"\
 /implib:"$(OUTDIR)/AlertUI.lib" 
DEF_FILE= \
	".\AlertUI.def"
LINK32_OBJS= \
	"$(INTDIR)\Alertmes.obj" \
	"$(INTDIR)\Alertopt.obj" \
	"$(INTDIR)\AlertUI.obj" \
	"$(INTDIR)\AlertUI.res" \
	"$(INTDIR)\Editsmtp.obj" \
	"$(INTDIR)\EmailPSh.obj" \
	"$(INTDIR)\Forwrdpg.obj" \
	"$(INTDIR)\HELPDLG.OBJ" \
	"$(INTDIR)\Netmespg.obj" \
	"$(INTDIR)\Nlmpg.obj" \
	"$(INTDIR)\NQFONTS.OBJ" \
	"$(INTDIR)\Smtppg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\AlertUI.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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

################################################################################
# Begin Target

# Name "AlertUI - Win32 Release"
# Name "AlertUI - Win32 Debug"

!IF  "$(CFG)" == "AlertUI - Win32 Release"

!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "AlertUI - Win32 Release"

!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AlertUI.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_ALERT=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Helpdlg.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\AlertUI.obj" : $(SOURCE) $(DEP_CPP_ALERT) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\AlertUI.sbr" : $(SOURCE) $(DEP_CPP_ALERT) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_ALERT=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Helpdlg.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\AlertUI.obj" : $(SOURCE) $(DEP_CPP_ALERT) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\AlertUI.sbr" : $(SOURCE) $(DEP_CPP_ALERT) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AlertUI.def

!IF  "$(CFG)" == "AlertUI - Win32 Release"

!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	{$(INCLUDE)}"\Stdafx.h"\
	

!IF  "$(CFG)" == "AlertUI - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "SYM_WIN32" /D "SYM_MFC" /D "_UNICODE" /D "UNICODE"\
 /D "_USRDLL" /FR"$(INTDIR)/" /Fp"$(INTDIR)/AlertUI.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\AlertUI.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D\
 "SYM_WIN32" /D "SYM_MFC" /FR"$(INTDIR)/" /Fp"$(INTDIR)/AlertUI.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\AlertUI.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AlertUI.rc
DEP_RSC_ALERTU=\
	".\Alert_tr.bmp"\
	

"$(INTDIR)\AlertUI.res" : $(SOURCE) $(DEP_RSC_ALERTU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Smtppg.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_SMTPP=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Smtppg.obj" : $(SOURCE) $(DEP_CPP_SMTPP) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Smtppg.sbr" : $(SOURCE) $(DEP_CPP_SMTPP) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_SMTPP=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Smtppg.obj" : $(SOURCE) $(DEP_CPP_SMTPP) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Smtppg.sbr" : $(SOURCE) $(DEP_CPP_SMTPP) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editsmtp.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_EDITS=\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nqfonts.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Editsmtp.obj" : $(SOURCE) $(DEP_CPP_EDITS) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Editsmtp.sbr" : $(SOURCE) $(DEP_CPP_EDITS) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_EDITS=\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nqfonts.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Editsmtp.obj" : $(SOURCE) $(DEP_CPP_EDITS) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Editsmtp.sbr" : $(SOURCE) $(DEP_CPP_EDITS) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Alertopt.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_ALERTO=\
	".\Alertmes.h"\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Emailpsh.h"\
	".\Forwrdpg.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Netmespg.h"\
	".\Nlmpg.h"\
	".\Nqfonts.h"\
	".\Smtppg.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dbcs_str.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\dosvmm.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\drvseg.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\os2win.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\othunk.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platmac.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platntk.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platvxd.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\platwin.h"\
	"L:\SOURCE\INCLUDE\SRCQAKE\symvxd.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Alertopt.obj" : $(SOURCE) $(DEP_CPP_ALERTO) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Alertopt.sbr" : $(SOURCE) $(DEP_CPP_ALERTO) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_ALERTO=\
	".\Alertmes.h"\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Editsmtp.h"\
	".\Emailpsh.h"\
	".\Forwrdpg.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Netmespg.h"\
	".\Nlmpg.h"\
	".\Nqfonts.h"\
	".\Smtppg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Alertopt.obj" : $(SOURCE) $(DEP_CPP_ALERTO) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Alertopt.sbr" : $(SOURCE) $(DEP_CPP_ALERTO) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Nlmpg.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_NLMPG=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nlmpg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Nlmpg.obj" : $(SOURCE) $(DEP_CPP_NLMPG) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Nlmpg.sbr" : $(SOURCE) $(DEP_CPP_NLMPG) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_NLMPG=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nlmpg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Nlmpg.obj" : $(SOURCE) $(DEP_CPP_NLMPG) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Nlmpg.sbr" : $(SOURCE) $(DEP_CPP_NLMPG) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Forwrdpg.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_FORWR=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Forwrdpg.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Forwrdpg.obj" : $(SOURCE) $(DEP_CPP_FORWR) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Forwrdpg.sbr" : $(SOURCE) $(DEP_CPP_FORWR) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_FORWR=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Forwrdpg.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Forwrdpg.obj" : $(SOURCE) $(DEP_CPP_FORWR) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Forwrdpg.sbr" : $(SOURCE) $(DEP_CPP_FORWR) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Alertmes.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_ALERTM=\
	".\Alertmes.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nqfonts.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Alertmes.obj" : $(SOURCE) $(DEP_CPP_ALERTM) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Alertmes.sbr" : $(SOURCE) $(DEP_CPP_ALERTM) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_ALERTM=\
	".\Alertmes.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Nqfonts.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Alertmes.obj" : $(SOURCE) $(DEP_CPP_ALERTM) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Alertmes.sbr" : $(SOURCE) $(DEP_CPP_ALERTM) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Netmespg.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_NETME=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Netmespg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Netmespg.obj" : $(SOURCE) $(DEP_CPP_NETME) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Netmespg.sbr" : $(SOURCE) $(DEP_CPP_NETME) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_NETME=\
	".\Alertopt.h"\
	".\Alertui.h"\
	".\Help_ids.h"\
	".\Helpdlg.h"\
	".\Netmespg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Avapi.h"\
	{$(INCLUDE)}"\Callback.h"\
	{$(INCLUDE)}"\Cts4.h"\
	{$(INCLUDE)}"\Ctsn.h"\
	{$(INCLUDE)}"\Drvprblm.h"\
	{$(INCLUDE)}"\Exclude.h"\
	{$(INCLUDE)}"\Excludel.h"\
	{$(INCLUDE)}"\Navopts.h"\
	{$(INCLUDE)}"\Navstart.h"\
	{$(INCLUDE)}"\Navutil.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Netobj.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Npt.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Nvetwhlp.h"\
	{$(INCLUDE)}"\Options.h"\
	{$(INCLUDE)}"\Quakever.h"\
	{$(INCLUDE)}"\Register.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Stdhelp.h"\
	{$(INCLUDE)}"\Symalog.h"\
	{$(INCLUDE)}"\Symcfg.h"\
	{$(INCLUDE)}"\Symnd2.h"\
	{$(INCLUDE)}"\Symnet.h"\
	{$(INCLUDE)}"\Version.h"\
	{$(INCLUDE)}"\Virlist.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\Netmespg.obj" : $(SOURCE) $(DEP_CPP_NETME) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\Netmespg.sbr" : $(SOURCE) $(DEP_CPP_NETME) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HELPDLG.CPP

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_HELPD=\
	".\Alertui.h"\
	".\Helpdlg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\HELPDLG.OBJ" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\HELPDLG.SBR" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_HELPD=\
	".\Alertui.h"\
	".\Helpdlg.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\HELPDLG.OBJ" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\HELPDLG.SBR" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EmailPSh.cpp

!IF  "$(CFG)" == "AlertUI - Win32 Release"

DEP_CPP_EMAIL=\
	".\Alertui.h"\
	".\Emailpsh.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Platform.h"\
	{$(INCLUDE)}"\Platnlm.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\EmailPSh.obj" : $(SOURCE) $(DEP_CPP_EMAIL) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\EmailPSh.sbr" : $(SOURCE) $(DEP_CPP_EMAIL) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ELSEIF  "$(CFG)" == "AlertUI - Win32 Debug"

DEP_CPP_EMAIL=\
	".\Alertui.h"\
	".\Emailpsh.h"\
	{$(INCLUDE)}"\At_count.h"\
	{$(INCLUDE)}"\At_opts.h"\
	{$(INCLUDE)}"\At_tgt.h"\
	{$(INCLUDE)}"\At_uiexp.h"\
	{$(INCLUDE)}"\Netbrwsd.h"\
	{$(INCLUDE)}"\Nettree.h"\
	{$(INCLUDE)}"\Nrescont.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	{$(INCLUDE)}"\Wndsize.h"\
	

"$(INTDIR)\EmailPSh.obj" : $(SOURCE) $(DEP_CPP_EMAIL) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\EmailPSh.sbr" : $(SOURCE) $(DEP_CPP_EMAIL) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NQFONTS.CPP
DEP_CPP_NQFON=\
	".\Nqfonts.h"\
	{$(INCLUDE)}"\codepage.h"\
	{$(INCLUDE)}"\dbcs.h"\
	{$(INCLUDE)}"\Stdafx.h"\
	

"$(INTDIR)\NQFONTS.OBJ" : $(SOURCE) $(DEP_CPP_NQFON) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"

"$(INTDIR)\NQFONTS.SBR" : $(SOURCE) $(DEP_CPP_NQFON) "$(INTDIR)"\
 "$(INTDIR)\AlertUI.pch"


# End Source File
# End Target
# End Project
################################################################################
