# Microsoft Developer Studio Generated NMAKE File, Based on NAVLUCBK.dsp
!IF "$(CFG)" == ""
CFG=NAVLUCBK - Win32 Debug
!MESSAGE No configuration specified. Defaulting to NAVLUCBK - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NAVLUCBK - Win32 Release" && "$(CFG)" != "NAVLUCBK - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NAVLUCBK.mak" CFG="NAVLUCBK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NAVLUCBK - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NAVLUCBK - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\NAVLUCBK.dll" ".\NavLuCallback.h"


CLEAN :
	-@erase "$(INTDIR)\luNavCallBack.obj"
	-@erase "$(INTDIR)\Myutils.obj"
	-@erase "$(INTDIR)\NavLu.obj"
	-@erase "$(INTDIR)\NAVLUCBK.obj"
	-@erase "$(INTDIR)\NAVLUCBK.pch"
	-@erase "$(INTDIR)\NAVLUCBK.res"
	-@erase "$(INTDIR)\navluprog.obj"
	-@erase "$(INTDIR)\navluutils.obj"
	-@erase "$(INTDIR)\progdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\NAVLUCBK.dll"
	-@erase "$(OUTDIR)\NAVLUCBK.exp"
	-@erase "$(OUTDIR)\NAVLUCBK.lib"
	-@erase ".\NavLuCallback.h"
	-@erase ".\NavLuCallback.tlb"
	-@erase ".\NavLuCallback_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "s:\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "s:\include\src" /I "L:\source\include\src" /I "r:\liveupdate1.5\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /Fp"$(INTDIR)\NAVLUCBK.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NAVLUCBK.res" /i "Release" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NAVLUCBK.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=n32cores.lib S32DEBUG.lib s32navo.lib npsapi.lib n32call.lib ctaskapi.lib defutils.lib defannty.lib chan32i.lib patch32i.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\NAVLUCBK.pdb" /machine:I386 /def:".\NAVLUCBK.def" /out:"$(OUTDIR)\NAVLUCBK.dll" /implib:"$(OUTDIR)\NAVLUCBK.lib" /libpath:"r:\avcomp\defutils\release.004\lib.iRa" /libpath:"s:\lib.iRa" /libpath:"L:\source\lib.iRa" /libpath:"R:\AVCOMP\DEFUTILS\RELEASE.004\LIB.IRA" /libpath:"R:\AVCOMP\CHANNEL\RELEASE.001\LIB.IRA" /libpath:"R:\AVCOMP\PATCHAPP\RELEASE.001\LIB.IRA" /libpath:"N:\LIB.IRA" /libpath:"S:\LIB.IRA" /libpath:"S:\NAVLUCBK\LIB.IRA" /libpath:"N:\NAVLUCBK\LIB.IRA" /libpath:"L:\ENGLISH\LIB.IRA" /libpath:"L:\SOURCE\LIB.IRA" 
DEF_FILE= \
	".\NAVLUCBK.def"
LINK32_OBJS= \
	"$(INTDIR)\luNavCallBack.obj" \
	"$(INTDIR)\Myutils.obj" \
	"$(INTDIR)\NavLu.obj" \
	"$(INTDIR)\NAVLUCBK.obj" \
	"$(INTDIR)\navluprog.obj" \
	"$(INTDIR)\navluutils.obj" \
	"$(INTDIR)\progdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\NAVLUCBK.res"

"$(OUTDIR)\NAVLUCBK.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\NAVLUCBK.dll" ".\NavLuCallback.tlb" ".\NavLuCallback.h" ".\NavLuCallback_i.c" "$(OUTDIR)\NAVLUCBK.bsc"


CLEAN :
	-@erase "$(INTDIR)\luNavCallBack.obj"
	-@erase "$(INTDIR)\luNavCallBack.sbr"
	-@erase "$(INTDIR)\Myutils.obj"
	-@erase "$(INTDIR)\Myutils.sbr"
	-@erase "$(INTDIR)\NavLu.obj"
	-@erase "$(INTDIR)\NavLu.sbr"
	-@erase "$(INTDIR)\NAVLUCBK.obj"
	-@erase "$(INTDIR)\NAVLUCBK.pch"
	-@erase "$(INTDIR)\NAVLUCBK.res"
	-@erase "$(INTDIR)\NAVLUCBK.sbr"
	-@erase "$(INTDIR)\navluprog.obj"
	-@erase "$(INTDIR)\navluprog.sbr"
	-@erase "$(INTDIR)\navluutils.obj"
	-@erase "$(INTDIR)\navluutils.sbr"
	-@erase "$(INTDIR)\progdlg.obj"
	-@erase "$(INTDIR)\progdlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\NAVLUCBK.bsc"
	-@erase "$(OUTDIR)\NAVLUCBK.dll"
	-@erase "$(OUTDIR)\NAVLUCBK.exp"
	-@erase "$(OUTDIR)\NAVLUCBK.ilk"
	-@erase "$(OUTDIR)\NAVLUCBK.lib"
	-@erase "$(OUTDIR)\NAVLUCBK.pdb"
	-@erase ".\NavLuCallback.h"
	-@erase ".\NavLuCallback.tlb"
	-@erase ".\NavLuCallback_i.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "s:\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "s:\include\src" /I "L:\source\include\src" /I "r:\liveupdate1.5\include" /D "_DEBUG" /D "SYM_WIN" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /D "SYM_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\NAVLUCBK.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NAVLUCBK.res" /i "Debug" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NAVLUCBK.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\luNavCallBack.sbr" \
	"$(INTDIR)\Myutils.sbr" \
	"$(INTDIR)\NavLu.sbr" \
	"$(INTDIR)\NAVLUCBK.sbr" \
	"$(INTDIR)\navluprog.sbr" \
	"$(INTDIR)\navluutils.sbr" \
	"$(INTDIR)\progdlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\NAVLUCBK.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=n32cores.lib S32DEBUG.lib s32navo.lib npsapi.lib n32call.lib ctaskapi.lib defutils.lib defannty.lib chan32i.lib patch32i.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\NAVLUCBK.pdb" /debug /machine:I386 /def:".\NAVLUCBK.def" /out:"$(OUTDIR)\NAVLUCBK.dll" /implib:"$(OUTDIR)\NAVLUCBK.lib" /pdbtype:sept /libpath:"r:\avcomp\defutils\release.004\lib.ida" /libpath:"s:\lib.ida" /libpath:"L:\source\lib.ida" /libpath:"R:\AVCOMP\DEFUTILS\RELEASE.004\LIB.IDA" /libpath:"R:\AVCOMP\CHANNEL\RELEASE.001\LIB.IDA" /libpath:"R:\AVCOMP\PATCHAPP\RELEASE.001\LIB.IDA" /libpath:"N:\LIB.IDA" /libpath:"S:\LIB.IDA" /libpath:"S:\NAVLUCBK\LIB.IDA" /libpath:"N:\NAVLUCBK\LIB.IDA" /libpath:"L:\ENGLISH\LIB.IDA" /libpath:"L:\SOURCE\LIB.IDA" 
DEF_FILE= \
	".\NAVLUCBK.def"
LINK32_OBJS= \
	"$(INTDIR)\luNavCallBack.obj" \
	"$(INTDIR)\Myutils.obj" \
	"$(INTDIR)\NavLu.obj" \
	"$(INTDIR)\NAVLUCBK.obj" \
	"$(INTDIR)\navluprog.obj" \
	"$(INTDIR)\navluutils.obj" \
	"$(INTDIR)\progdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\NAVLUCBK.res"

"$(OUTDIR)\NAVLUCBK.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("NAVLUCBK.dep")
!INCLUDE "NAVLUCBK.dep"
!ELSE 
!MESSAGE Warning: cannot find "NAVLUCBK.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "NAVLUCBK - Win32 Release" || "$(CFG)" == "NAVLUCBK - Win32 Debug"
SOURCE=.\luNavCallBack.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\luNavCallBack.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\luNavCallBack.obj"	"$(INTDIR)\luNavCallBack.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\Myutils.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\Myutils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\Myutils.obj"	"$(INTDIR)\Myutils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\NavLu.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\NavLu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\NavLu.obj"	"$(INTDIR)\NavLu.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\navlucallback.idl

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

MTL_SWITCHES=/nologo /D "NDEBUG" /tlb "NavLuCallback.tlb" /h "NavLuCallback.h" /iid "NavLuCallback_i.c" /win32 

".\NavLuCallback.tlb"	".\NavLuCallback.h"	".\NavLuCallback_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

MTL_SWITCHES=/nologo /D "_DEBUG" /tlb "NavLuCallback.tlb" /h "NavLuCallback.h" /iid "NavLuCallback_i.c" /win32 

".\NavLuCallback.tlb"	".\NavLuCallback.h"	".\NavLuCallback_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\NAVLUCBK.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\NAVLUCBK.obj" : $(SOURCE) "$(INTDIR)" ".\NavLuCallback_i.c" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\NAVLUCBK.obj"	"$(INTDIR)\NAVLUCBK.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\NAVLUCBK.rc

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\NAVLUCBK.res" : $(SOURCE) "$(INTDIR)" ".\NavLuCallback.tlb"
	$(RSC) /l 0x409 /fo"$(INTDIR)\NAVLUCBK.res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\NAVLUCBK.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\NAVLUCBK.res" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\navluprog.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\navluprog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\navluprog.obj"	"$(INTDIR)\navluprog.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\navluutils.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\navluutils.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\navluutils.obj"	"$(INTDIR)\navluutils.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\progdlg.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"


"$(INTDIR)\progdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"


"$(INTDIR)\progdlg.obj"	"$(INTDIR)\progdlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NAVLUCBK.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "NAVLUCBK - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "s:\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "s:\include\src" /I "L:\source\include\src" /I "r:\liveupdate1.5\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /Fp"$(INTDIR)\NAVLUCBK.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\NAVLUCBK.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "NAVLUCBK - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "s:\LIVEUPDATEI\SRC" /I "R:\AVCOMP\DEFUTILS\RELEASE.004\INCLUDE" /I "s:\include\src" /I "L:\source\include\src" /I "r:\liveupdate1.5\include" /D "_DEBUG" /D "SYM_WIN" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "SYM_WIN32" /D "SYM_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\NAVLUCBK.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\NAVLUCBK.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

