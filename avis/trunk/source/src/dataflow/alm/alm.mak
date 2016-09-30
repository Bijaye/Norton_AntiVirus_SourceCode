# Microsoft Developer Studio Generated NMAKE File, Based on ALM.DSP
!IF "$(CFG)" == ""
CFG=ALM - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ALM - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ALM - Win32 Release" && "$(CFG)" != "ALM - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ALM.MAK" CFG="ALM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ALM - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ALM - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ALM - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ALM.exe"

!ELSE 

ALL : "$(OUTDIR)\ALM.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ALM.obj"
	-@erase "$(INTDIR)\ALM.pch"
	-@erase "$(INTDIR)\ALM.res"
	-@erase "$(INTDIR)\ALMDlg.obj"
	-@erase "$(INTDIR)\DFListCtrl.obj"
	-@erase "$(INTDIR)\ListCtrlEx.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\ALM.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\ALM.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ALM.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ALM.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\ALM.pdb" /machine:I386 /out:"$(OUTDIR)\ALM.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ALM.obj" \
	"$(INTDIR)\ALM.res" \
	"$(INTDIR)\ALMDlg.obj" \
	"$(INTDIR)\DFListCtrl.obj" \
	"$(INTDIR)\ListCtrlEx.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\..\lib\DFLauncher.lib" \
	"..\dfeval\Debug\DFEval.lib" \
	"..\DFManager\Debug\DFManager.lib"

"$(OUTDIR)\ALM.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ALM.exe"

!ELSE 

ALL : "$(OUTDIR)\ALM.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ALM.obj"
	-@erase "$(INTDIR)\ALM.pch"
	-@erase "$(INTDIR)\ALM.res"
	-@erase "$(INTDIR)\ALMDlg.obj"
	-@erase "$(INTDIR)\DFListCtrl.obj"
	-@erase "$(INTDIR)\ListCtrlEx.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\ALM.exe"
	-@erase "$(OUTDIR)\ALM.ilk"
	-@erase "$(OUTDIR)\ALM.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "e:\newdev\src\dataflow\inc" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\ALM.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ALM.res" /i "d:\mydev\com" /d "_DEBUG" /d\
 "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ALM.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\ALM.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ALM.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ALM.obj" \
	"$(INTDIR)\ALM.res" \
	"$(INTDIR)\ALMDlg.obj" \
	"$(INTDIR)\DFListCtrl.obj" \
	"$(INTDIR)\ListCtrlEx.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\..\lib\DFLauncher.lib" \
	"..\dfeval\Debug\DFEval.lib" \
	"..\DFManager\Debug\DFManager.lib"

"$(OUTDIR)\ALM.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "ALM - Win32 Release" || "$(CFG)" == "ALM - Win32 Debug"
SOURCE=.\ALM.cpp

!IF  "$(CFG)" == "ALM - Win32 Release"

DEP_CPP_ALM_C=\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ALM_C=\
	".\dfevalstate.h"\
	".\dfevalstatus.h"\
	".\DFJob.h"\
	".\DFManager.h"\
	".\DFMatrix.h"\
	".\DFSample.h"\
	

"$(INTDIR)\ALM.obj" : $(SOURCE) $(DEP_CPP_ALM_C) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

DEP_CPP_ALM_C=\
	"..\inc\dfevalstate.h"\
	"..\inc\dfevalstatus.h"\
	"..\inc\dfjob.h"\
	"..\inc\dfmanager.h"\
	"..\inc\dfmatrix.h"\
	"..\inc\dfsample.h"\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	

"$(INTDIR)\ALM.obj" : $(SOURCE) $(DEP_CPP_ALM_C) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ENDIF 

SOURCE=.\ALM.rc
DEP_RSC_ALM_R=\
	".\res\ALM.ico"\
	".\res\ALM.rc2"\
	".\res\bitmap1.bmp"\
	

"$(INTDIR)\ALM.res" : $(SOURCE) $(DEP_RSC_ALM_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ALMDlg.cpp

!IF  "$(CFG)" == "ALM - Win32 Release"

DEP_CPP_ALMDL=\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	".\StdAfx.h"\
	
NODEP_CPP_ALMDL=\
	".\dferror.h"\
	".\dfevalstate.h"\
	".\dfevalstatus.h"\
	".\DFJob.h"\
	".\DFLauncherImp.h"\
	".\DFManager.h"\
	".\DFMatrix.h"\
	".\dfmsg.h"\
	".\DFSample.h"\
	

"$(INTDIR)\ALMDlg.obj" : $(SOURCE) $(DEP_CPP_ALMDL) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

DEP_CPP_ALMDL=\
	"..\inc\dferror.h"\
	"..\inc\dfevalstate.h"\
	"..\inc\dfevalstatus.h"\
	"..\inc\dfjob.h"\
	"..\inc\dflauncherimp.h"\
	"..\inc\dfmanager.h"\
	"..\inc\dfmatrix.h"\
	"..\inc\dfmsg.h"\
	"..\inc\dfsample.h"\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	

"$(INTDIR)\ALMDlg.obj" : $(SOURCE) $(DEP_CPP_ALMDL) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ENDIF 

SOURCE=.\DFListCtrl.cpp

!IF  "$(CFG)" == "ALM - Win32 Release"

DEP_CPP_DFLIS=\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	".\StdAfx.h"\
	
NODEP_CPP_DFLIS=\
	".\dferror.h"\
	".\dfevalstate.h"\
	".\dfevalstatus.h"\
	".\DFJob.h"\
	".\DFManager.h"\
	".\DFMatrix.h"\
	".\DFSample.h"\
	

"$(INTDIR)\DFListCtrl.obj" : $(SOURCE) $(DEP_CPP_DFLIS) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

DEP_CPP_DFLIS=\
	"..\inc\dferror.h"\
	"..\inc\dfevalstate.h"\
	"..\inc\dfevalstatus.h"\
	"..\inc\dfjob.h"\
	"..\inc\dfmanager.h"\
	"..\inc\dfmatrix.h"\
	"..\inc\dfsample.h"\
	".\ALM.h"\
	".\ALMDlg.h"\
	".\DFListCtrl.h"\
	".\ListCtrlEx.h"\
	

"$(INTDIR)\DFListCtrl.obj" : $(SOURCE) $(DEP_CPP_DFLIS) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ENDIF 

SOURCE=.\ListCtrlEx.cpp

!IF  "$(CFG)" == "ALM - Win32 Release"

DEP_CPP_LISTC=\
	".\ListCtrlEx.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ListCtrlEx.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

DEP_CPP_LISTC=\
	".\ListCtrlEx.h"\
	

"$(INTDIR)\ListCtrlEx.obj" : $(SOURCE) $(DEP_CPP_LISTC) "$(INTDIR)"\
 "$(INTDIR)\ALM.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ALM - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\ALM.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ALM.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ALM - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "e:\newdev\src\dataflow\inc"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\ALM.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ALM.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

