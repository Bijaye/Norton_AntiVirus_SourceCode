# Microsoft Developer Studio Generated NMAKE File, Based on NavLogon.dsp
# This file has been modified to generate the 64-bit version of NavLogon.dll

# Modifications from the 32-bit makefile:
# /FD (use dependency information) compiler switch removed.
# /ZI (PDB for edit qand continue) compiler switch replaced with /Zi, because it's unsupported.
# /GZ (ctach release errors in debug build) compiler switch removed, because it's unsupported.
# /pdbtype:sept (keep debug info  in separate pdb file) linker option removed, because it's unsupported.
# Removed odbc32.lib and odbccp32.lib from the list of imported libraries for the linker.
# Linker option /machine:I386 changed to /machine:IA64, because Itanium chipset is the 64-bit CPU target.

# Other notes:
# Use compiler option /X to igonre the standard include paths specified in the PATH and INCLUDE env vars.
# Use linker option /LIBPATH to add search paths before the standard lirary paths specified in the LIB end var.

!IF "$(CFG)" == ""
CFG=NavLogon - x86 Debug
!MESSAGE No configuration specified. Defaulting to NavLogon - x86 Debug.
!ENDIF 

!IF "$(CFG)" != "NavLogon - x86 Release" && "$(CFG)" != "NavLogon - x86 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NavLogon.mak" CFG="NavLogon - x86 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NavLogon - x86 Release"
!MESSAGE "NavLogon - x86 Debug"
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 


#############################################################################################
#############################################################################################


!IF "$(CFG)" == "NavLogon - x86 Release"

OUTDIR=.\Release.x86
INTDIR=.\Release.x86
# Begin Custom Macros
OutDir=.\Release.x86
# End Custom Macros

ALL : "$(OUTDIR)\NavLogon.dll"


CLEAN :
	-@erase "$(INTDIR)\NavLogon.obj"
	-@erase "$(INTDIR)\NavLogon.pch"
	-@erase "$(INTDIR)\NavLogon.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc80.idb"
	-@erase "$(INTDIR)\vc80.pdb"
	-@erase "$(OUTDIR)\NavLogon.dll"
	-@erase "$(OUTDIR)\NavLogon.exp"
	-@erase "$(OUTDIR)\NavLogon.ilk"
	-@erase "$(OUTDIR)\NavLogon.lib"
	-@erase "$(OUTDIR)\NavLogon.map"
	-@erase "$(OUTDIR)\NavLogon.pdb"
	-@erase "$(OUTDIR)\postbld.dep"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\win64\x86\AMD64\cl.exe

CPP_PROJ=/nologo /MT /W3 /EHsc /O2 /Ge \
	/I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include" \
	/I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include\crt" \
        /I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include\crt\sys" \
	/I "..\..\..\..\..\Core_Technology\LDVP_Shared\src\Include" \
	/I "..\..\..\..\..\release\ams\include" \
	/I "..\..\..\..\..\Norton_Internet_Security\src\CC_SDK\include" \
	/I "..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include" \
	/D "NDEBUG" /D "WIN64" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVLOGON_EXPORTS" /D "NPAPI_EXPORTS" /Fp"$(INTDIR)\NavLogon.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c /GR 

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

MTL="..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\midl.exe"
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win64 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NavLogon.res" /i "..\Include" /d "NDEBUG" 
	
LINK32="..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\win64\x86\AMD64\link.exe"
LINK32_FLAGS=libcmt.lib kernel32.lib user32.lib gdi32.lib bufferoverflowu.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib \
	/LIBPATH:"..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Lib\AMD64" \
	/nologo /dll /incremental:no /pdb:"$(OUTDIR)\NavLogon.pdb" /machine:AMD64 /out:"$(OUTDIR)\NavLogon.dll" /implib:"$(OUTDIR)\NavLogon.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NavLogon.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\NavLogon.res"

"$(OUTDIR)\NavLogon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS) /OUT:$(OUTDIR)\NavLogon.dll
<<

TargetPath=.\Release.x86\NavLogon.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release.x86
# End Custom Macros

#Post-build step

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\NavLogon.dll"
   if not exist ..\bin.x86-release-ansi md ..\bin.x86-release-ansi
	copy .\Release.x86\NavLogon.dll ..\bin.x86-release-ansi
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"


#############################################################################################
#############################################################################################


!ELSEIF  "$(CFG)" == "NavLogon - x86 Debug"

OUTDIR=.\Debug.x86
INTDIR=.\Debug.x86
# Begin Custom Macros
OutDir=.\Debug.x86
# End Custom Macros

ALL : "$(OUTDIR)\NavLogon.dll"


CLEAN :
	-@erase "$(INTDIR)\NavLogon.obj"
	-@erase "$(INTDIR)\NavLogon.pch"
	-@erase "$(INTDIR)\NavLogon.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc80.idb"
	-@erase "$(INTDIR)\vc80.pdb"
	-@erase "$(OUTDIR)\NavLogon.dll"
	-@erase "$(OUTDIR)\NavLogon.exp"
	-@erase "$(OUTDIR)\NavLogon.ilk"
	-@erase "$(OUTDIR)\NavLogon.lib"
	-@erase "$(OUTDIR)\NavLogon.map"
	-@erase "$(OUTDIR)\NavLogon.pdb"
	-@erase "$(OUTDIR)\postbld.dep"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\win64\x86\AMD64\cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od \
	/I "..\..\..\..\..\Core_Technology\LDVP_Shared\src\Include" \
	/I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include" \
	/I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include\crt" \
        /I "..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Include\crt\sys" \
	/I "..\..\..\..\..\release\ams\include" \
	/I "..\..\..\..\..\Norton_Internet_Security\src\CC_SDK\include" \
	/I "..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include" \
	/D "_DEBUG" /D "WIN64" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVLOGON_EXPORTS" /D "NPAPI_EXPORTS" /Fp"$(INTDIR)\NavLogon.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c /GR 

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

MTL=..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win64 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NavLogon.res" /i "..\Include" /d "_DEBUG" 
	
LINK32=..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Bin\win64\x86\AMD64\link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib bufferoverflowu.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib \
	/LIBPATH:"..\..\..\..\..\tools\PLATSDKW2K3SP1.BETA\Lib\AMD64" \
	/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\NavLogon.pdb" /debug /machine:AMD64 /out:"$(OUTDIR)\NavLogon.dll" /implib:"$(OUTDIR)\NavLogon.lib" 
LINK32_OBJS= \
	"$(INTDIR)\NavLogon.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\NavLogon.res"

"$(OUTDIR)\NavLogon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS) /OUT:$(OUTDIR)\NavLogon.dll
<<

TargetPath=.\Debug.x86\NavLogon.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug.x86
# End Custom Macros

#Post-build step

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\NavLogon.dll"
   if not exist ..\bin.x86-debug-ansi md ..\bin.x86-debug-ansi
	copy .\Debug.x86\NavLogon.dll ..\bin.x86-debug-ansi
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 





!IF "$(CFG)" == "NavLogon - x86 Release" || "$(CFG)" == "NavLogon - x86 Debug"
SOURCE=.\NavLogon.cpp

"$(INTDIR)\NavLogon.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\NavLogon.pch"


SOURCE=.\NavLogon.rc

"$(INTDIR)\NavLogon.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF "$(CFG)" == "NavLogon - x86 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /X \
	/I "..\..\..\..\..\Core_Technology\LDVP_Shared\src\Include" \
	/I "..\..\..\..\..\Tools\platform.sdk\include" \
	/I "..\..\..\..\..\Tools\platform.sdk\include\Win64" \
	/I "..\..\..\..\..\Tools\platform.sdk\include\Win64\crt" \
	/I "..\..\..\..\..\release\ams\include" \
	/D "NDEBUG" /D "WIN64" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVLOGON_EXPORTS" /D "NPAPI_EXPORTS" /Fp"$(INTDIR)\NavLogon.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c /GR 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\NavLogon.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<

!ELSEIF "$(CFG)" == "NavLogon - x86 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od \
	/I "..\..\..\..\..\Core_Technology\LDVP_Shared\src\Include" \
	/I "..\..\..\..\..\Tools\platform.sdk\include\Win64" \
	/I "..\..\..\..\..\Tools\platform.sdk\include\Win64\crt" \
	/I "..\..\..\..\..\release\ams\include" \
	/D "_DEBUG" /D "WIN64" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NAVLOGON_EXPORTS" /D "NPAPI_EXPORTS" /Fp"$(INTDIR)\NavLogon.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c /GR 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\NavLogon.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

