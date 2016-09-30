# Microsoft Developer Studio Generated NMAKE File, Based on AVISDFRL.dsp
!IF "$(CFG)" == ""
CFG=AVISDFRL - Win32 IDE Debug
!MESSAGE No configuration specified. Defaulting to AVISDFRL - Win32 IDE Debug.
!ENDIF 

!IF "$(CFG)" != "AVISDFRL - Win32 Release" && "$(CFG)" != "AVISDFRL - Win32 Debug" && "$(CFG)" != "AVISDFRL - Win32 IDE Debug" && "$(CFG)" != "AVISDFRL - Win32 IDE Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISDFRL.mak" CFG="AVISDFRL - Win32 IDE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISDFRL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 IDE Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISDFRL - Win32 IDE Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AVISDFRL - Win32 Release"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\DataFlow\AVISDFRL
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\AVISDFRL.dll"


CLEAN :
	-@erase "$(INTDIR)\ActivityLogSetting.obj"
	-@erase "$(INTDIR)\AutoDisableErrorCount.obj"
	-@erase "$(INTDIR)\AVISDFRL.obj"
	-@erase "$(INTDIR)\AVISDFRL.pch"
	-@erase "$(INTDIR)\AVISDFRL.res"
	-@erase "$(INTDIR)\AVISDFRLView.obj"
	-@erase "$(INTDIR)\CookieStatistics.obj"
	-@erase "$(INTDIR)\DFResource.obj"
	-@erase "$(INTDIR)\listctex.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MachineStatistics.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ParamValue.obj"
	-@erase "$(INTDIR)\PerformanceMonitor.obj"
	-@erase "$(INTDIR)\ResourceDialog.obj"
	-@erase "$(INTDIR)\ResourceSummary.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISDFRL.dll"
	-@erase "$(OUTDIR)\AVISDFRL.exp"
	-@erase "$(OUTDIR)\AVISDFRL.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/MD /W3 /GX /O2 /Ob2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISDFRL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=avissendmail.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\AVISDFRL.def" /out:"$(OUTDIR)\AVISDFRL.dll" /implib:"$(OUTDIR)\AVISDFRL.lib" /libpath:"///LIBD///\\" /map:"///OBJD///\DataFlow\AVISDFRL/AVISDFRL.map" 
DEF_FILE= \
	".\AVISDFRL.def"
LINK32_OBJS= \
	"$(INTDIR)\listctex.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\ParamValue.obj" \
	"$(INTDIR)\PerformanceMonitor.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\ActivityLogSetting.obj" \
	"$(INTDIR)\AutoDisableErrorCount.obj" \
	"$(INTDIR)\AVISDFRL.obj" \
	"$(INTDIR)\AVISDFRLView.obj" \
	"$(INTDIR)\CookieStatistics.obj" \
	"$(INTDIR)\DFResource.obj" \
	"$(INTDIR)\MachineStatistics.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ResourceDialog.obj" \
	"$(INTDIR)\ResourceSummary.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISDFRL.res"

"$(OUTDIR)\AVISDFRL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 Debug"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\DataFlow\AVISDFRL
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\AVISDFRL.dll"


CLEAN :
	-@erase "$(INTDIR)\ActivityLogSetting.obj"
	-@erase "$(INTDIR)\AutoDisableErrorCount.obj"
	-@erase "$(INTDIR)\AVISDFRL.obj"
	-@erase "$(INTDIR)\AVISDFRL.pch"
	-@erase "$(INTDIR)\AVISDFRL.res"
	-@erase "$(INTDIR)\AVISDFRLView.obj"
	-@erase "$(INTDIR)\CookieStatistics.obj"
	-@erase "$(INTDIR)\DFResource.obj"
	-@erase "$(INTDIR)\listctex.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MachineStatistics.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ParamValue.obj"
	-@erase "$(INTDIR)\PerformanceMonitor.obj"
	-@erase "$(INTDIR)\ResourceDialog.obj"
	-@erase "$(INTDIR)\ResourceSummary.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISDFRL.dll"
	-@erase "$(OUTDIR)\AVISDFRL.exp"
	-@erase "$(OUTDIR)\AVISDFRL.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/MDd /W3 /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "_DEBUG" /d "_AFXDLL" /i ///SRCD///\DataFlow\AVISDFRL" " 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISDFRL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=avissendmail.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\AVISDFRL.def" /out:"$(OUTDIR)\AVISDFRL.dll" /implib:"$(OUTDIR)\AVISDFRL.lib" /libpath:"///LIBD///\\" /map:"///OBJD///\DataFlow\AVISDFRL/AVISDFRL.map" 
DEF_FILE= \
	".\AVISDFRL.def"
LINK32_OBJS= \
	"$(INTDIR)\listctex.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\ParamValue.obj" \
	"$(INTDIR)\PerformanceMonitor.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\ActivityLogSetting.obj" \
	"$(INTDIR)\AutoDisableErrorCount.obj" \
	"$(INTDIR)\AVISDFRL.obj" \
	"$(INTDIR)\AVISDFRLView.obj" \
	"$(INTDIR)\CookieStatistics.obj" \
	"$(INTDIR)\DFResource.obj" \
	"$(INTDIR)\MachineStatistics.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ResourceDialog.obj" \
	"$(INTDIR)\ResourceSummary.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISDFRL.res"

"$(OUTDIR)\AVISDFRL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AVISDFRL.dll"


CLEAN :
	-@erase "$(INTDIR)\ActivityLogSetting.obj"
	-@erase "$(INTDIR)\AutoDisableErrorCount.obj"
	-@erase "$(INTDIR)\AVISDFRL.obj"
	-@erase "$(INTDIR)\AVISDFRL.pch"
	-@erase "$(INTDIR)\AVISDFRL.res"
	-@erase "$(INTDIR)\AVISDFRLView.obj"
	-@erase "$(INTDIR)\CookieStatistics.obj"
	-@erase "$(INTDIR)\DFResource.obj"
	-@erase "$(INTDIR)\listctex.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MachineStatistics.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ParamValue.obj"
	-@erase "$(INTDIR)\PerformanceMonitor.obj"
	-@erase "$(INTDIR)\ResourceDialog.obj"
	-@erase "$(INTDIR)\ResourceSummary.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISDFRL.dll"
	-@erase "$(OUTDIR)\AVISDFRL.exp"
	-@erase "$(OUTDIR)\AVISDFRL.lib"
	-@erase "$(OUTDIR)\AVISDFRL.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISDFRL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=avissendmail.lib /nologo /subsystem:windows /dll /profile /map:"$(INTDIR)\AVISDFRL.map" /debug /machine:I386 /def:".\AVISDFRL.def" /out:"$(OUTDIR)\AVISDFRL.dll" /implib:"$(OUTDIR)\AVISDFRL.lib" /libpath:"..\..\..\dlib\ea" 
DEF_FILE= \
	".\AVISDFRL.def"
LINK32_OBJS= \
	"$(INTDIR)\listctex.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\ParamValue.obj" \
	"$(INTDIR)\PerformanceMonitor.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\ActivityLogSetting.obj" \
	"$(INTDIR)\AutoDisableErrorCount.obj" \
	"$(INTDIR)\AVISDFRL.obj" \
	"$(INTDIR)\AVISDFRLView.obj" \
	"$(INTDIR)\CookieStatistics.obj" \
	"$(INTDIR)\DFResource.obj" \
	"$(INTDIR)\MachineStatistics.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ResourceDialog.obj" \
	"$(INTDIR)\ResourceSummary.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISDFRL.res"

"$(OUTDIR)\AVISDFRL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\AVISDFRL.dll"


CLEAN :
	-@erase "$(INTDIR)\ActivityLogSetting.obj"
	-@erase "$(INTDIR)\AutoDisableErrorCount.obj"
	-@erase "$(INTDIR)\AVISDFRL.obj"
	-@erase "$(INTDIR)\AVISDFRL.pch"
	-@erase "$(INTDIR)\AVISDFRL.res"
	-@erase "$(INTDIR)\AVISDFRLView.obj"
	-@erase "$(INTDIR)\CookieStatistics.obj"
	-@erase "$(INTDIR)\DFResource.obj"
	-@erase "$(INTDIR)\listctex.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MachineStatistics.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ParamValue.obj"
	-@erase "$(INTDIR)\PerformanceMonitor.obj"
	-@erase "$(INTDIR)\ResourceDialog.obj"
	-@erase "$(INTDIR)\ResourceSummary.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISDFRL.dll"
	-@erase "$(OUTDIR)\AVISDFRL.exp"
	-@erase "$(OUTDIR)\AVISDFRL.lib"
	-@erase "$(OUTDIR)\AVISDFRL.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MD /W3 /GX /Zi /O2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISDFRL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=avissendmail.lib /nologo /subsystem:windows /dll /profile /map:"$(INTDIR)\AVISDFRL.map" /debug /machine:I386 /def:".\AVISDFRL.def" /out:"$(OUTDIR)\AVISDFRL.dll" /implib:"$(OUTDIR)\AVISDFRL.lib" /libpath:"..\..\..\rlib\ea" 
DEF_FILE= \
	".\AVISDFRL.def"
LINK32_OBJS= \
	"$(INTDIR)\listctex.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\ParamValue.obj" \
	"$(INTDIR)\PerformanceMonitor.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\ActivityLogSetting.obj" \
	"$(INTDIR)\AutoDisableErrorCount.obj" \
	"$(INTDIR)\AVISDFRL.obj" \
	"$(INTDIR)\AVISDFRLView.obj" \
	"$(INTDIR)\CookieStatistics.obj" \
	"$(INTDIR)\DFResource.obj" \
	"$(INTDIR)\MachineStatistics.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ResourceDialog.obj" \
	"$(INTDIR)\ResourceSummary.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISDFRL.res"

"$(OUTDIR)\AVISDFRL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AVISDFRL.dep")
!INCLUDE "AVISDFRL.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVISDFRL.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVISDFRL - Win32 Release" || "$(CFG)" == "AVISDFRL - Win32 Debug" || "$(CFG)" == "AVISDFRL - Win32 IDE Debug" || "$(CFG)" == "AVISDFRL - Win32 IDE Release"
SOURCE=..\com\listctex.cpp

"$(INTDIR)\listctex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\com\listvwex.cpp

"$(INTDIR)\listvwex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\com\ParamValue.cpp

"$(INTDIR)\ParamValue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\com\PerformanceMonitor.cpp

"$(INTDIR)\PerformanceMonitor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\com\titletip.cpp

"$(INTDIR)\titletip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ActivityLogSetting.cpp

"$(INTDIR)\ActivityLogSetting.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\AutoDisableErrorCount.cpp

"$(INTDIR)\AutoDisableErrorCount.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\AVISDFRL.cpp

"$(INTDIR)\AVISDFRL.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\AVISDFRLView.cpp

"$(INTDIR)\AVISDFRLView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\CookieStatistics.cpp

"$(INTDIR)\CookieStatistics.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\DFResource.cpp

"$(INTDIR)\DFResource.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\MachineStatistics.cpp

"$(INTDIR)\MachineStatistics.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\ResourceDialog.cpp

"$(INTDIR)\ResourceDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\ResourceSummary.cpp

"$(INTDIR)\ResourceSummary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISDFRL.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AVISDFRL - Win32 Release"

CPP_SWITCHES=/MD /W3 /GX /O2 /Ob2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISDFRL.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 Debug"

CPP_SWITCHES=/MDd /W3 /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISDFRL.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Debug"

CPP_SWITCHES=/MDd /W4 /Gm /Gi /GX /ZI /Od /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISDFRL.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Release"

CPP_SWITCHES=/MD /W3 /GX /Zi /O2 /I "..\inc" /I "..\com" /I "..\..\inc" /I "..\..\avissendmail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /Fp"$(INTDIR)\AVISDFRL.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISDFRL.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\AVISDFRL.rc

!IF  "$(CFG)" == "AVISDFRL - Win32 Release"


"$(INTDIR)\AVISDFRL.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 Debug"


"$(INTDIR)\AVISDFRL.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "_DEBUG" /d "_AFXDLL" /i ///SRCD///\DataFlow\AVISDFRL" " $(SOURCE)


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Debug"


"$(INTDIR)\AVISDFRL.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "AVISDFRL - Win32 IDE Release"


"$(INTDIR)\AVISDFRL.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\AVISDFRL.res" /d "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

SOURCE=.\version.rc

!ENDIF 

