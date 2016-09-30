# Microsoft Developer Studio Generated NMAKE File, Based on DFGui.dsp
!IF "$(CFG)" == ""
CFG=DFGui - Win32 Debug IDE
!MESSAGE No configuration specified. Defaulting to DFGui - Win32 Debug IDE.
!ENDIF 

!IF "$(CFG)" != "DFGui - Win32 Release" && "$(CFG)" != "DFGui - Win32 Debug" && "$(CFG)" != "DFGui - Win32 Debug IDE" && "$(CFG)" != "DFGui - Win32 Release IDE"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DFGui.mak" CFG="DFGui - Win32 Debug IDE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DFGui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Debug IDE" (based on "Win32 (x86) Application")
!MESSAGE "DFGui - Win32 Release IDE" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "DFGui - Win32 Release"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\dataflow\avisdf
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\DFGui.exe"


CLEAN :
	-@erase "$(INTDIR)\computemd5checksum.obj"
	-@erase "$(INTDIR)\crashrecover.obj"
	-@erase "$(INTDIR)\DFConfigureDialog.obj"
	-@erase "$(INTDIR)\DFGui.obj"
	-@erase "$(INTDIR)\DFGui.pch"
	-@erase "$(INTDIR)\DFGui.res"
	-@erase "$(INTDIR)\DFGuiDoc.obj"
	-@erase "$(INTDIR)\DFGuiView.obj"
	-@erase "$(INTDIR)\DFSampleAttributes.obj"
	-@erase "$(INTDIR)\DFStatisticsDialog.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\PriorityDialog.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DFGui.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\avissendmail" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DFGui.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /subsystem:windows /pdb:none /machine:I386 /out:"$(OUTDIR)\DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/avisdf.exe" ///LIBD///\avisdfev.lib ///LIBD///\avisdfmg.lib ///LIBD///\AVISDFRL.lib ///LIBD///\AVISDB.lib ///LIBD///\AVISCOMMON.lib ///LIBD///\AVISFILE.lib ///LIBD///\avissendmail.lib 
LINK32_OBJS= \
	"$(INTDIR)\computemd5checksum.obj" \
	"$(INTDIR)\crashrecover.obj" \
	"$(INTDIR)\DFConfigureDialog.obj" \
	"$(INTDIR)\DFGui.obj" \
	"$(INTDIR)\DFGuiDoc.obj" \
	"$(INTDIR)\DFGuiView.obj" \
	"$(INTDIR)\DFSampleAttributes.obj" \
	"$(INTDIR)\DFStatisticsDialog.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\PriorityDialog.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\DFGui.res"

"$(OUTDIR)\DFGui.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\dataflow\avisdf
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\DFGui.exe"


CLEAN :
	-@erase "$(INTDIR)\computemd5checksum.obj"
	-@erase "$(INTDIR)\crashrecover.obj"
	-@erase "$(INTDIR)\DFConfigureDialog.obj"
	-@erase "$(INTDIR)\DFGui.obj"
	-@erase "$(INTDIR)\DFGui.pch"
	-@erase "$(INTDIR)\DFGui.res"
	-@erase "$(INTDIR)\DFGuiDoc.obj"
	-@erase "$(INTDIR)\DFGuiView.obj"
	-@erase "$(INTDIR)\DFSampleAttributes.obj"
	-@erase "$(INTDIR)\DFStatisticsDialog.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\PriorityDialog.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DFGui.exe"
	-@erase "$(OUTDIR)\DFGui.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/MDd /W3 /GX /Zi /Od /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DFGui.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/subsystem:windows /incremental:no /pdb:"$(OUTDIR)\DFGui.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DFGui.exe" /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /pdb:"///OUTD///\nt/DFGui.pdb" /out:"///OUTD///\nt/DFGui.exe" /out:"///OUTD///\nt/avisdf.exe" /pdb:"///OUTD///\nt/avisdf.pdb" /map:"///OBJD///\dataflow\DFGui/DFGui.map" ///LIBD///\avisdfev.lib ///LIBD///\avisdfmg.lib ///LIBD///\AVISDFRL.lib ///LIBD///\AVISDB.lib ///LIBD///\AVISCOMMON.lib ///LIBD///\AVISFILE.lib ///LIBD///\avissendmail.lib 
LINK32_OBJS= \
	"$(INTDIR)\computemd5checksum.obj" \
	"$(INTDIR)\crashrecover.obj" \
	"$(INTDIR)\DFConfigureDialog.obj" \
	"$(INTDIR)\DFGui.obj" \
	"$(INTDIR)\DFGuiDoc.obj" \
	"$(INTDIR)\DFGuiView.obj" \
	"$(INTDIR)\DFSampleAttributes.obj" \
	"$(INTDIR)\DFStatisticsDialog.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\PriorityDialog.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\DFGui.res"

"$(OUTDIR)\DFGui.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug IDE"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\avisdf.exe"


CLEAN :
	-@erase "$(INTDIR)\computemd5checksum.obj"
	-@erase "$(INTDIR)\crashrecover.obj"
	-@erase "$(INTDIR)\DFConfigureDialog.obj"
	-@erase "$(INTDIR)\DFGui.obj"
	-@erase "$(INTDIR)\DFGui.pch"
	-@erase "$(INTDIR)\DFGui.res"
	-@erase "$(INTDIR)\DFGuiDoc.obj"
	-@erase "$(INTDIR)\DFGuiView.obj"
	-@erase "$(INTDIR)\DFSampleAttributes.obj"
	-@erase "$(INTDIR)\DFStatisticsDialog.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\PriorityDialog.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avisdf.exe"
	-@erase "$(OUTDIR)\avisdf.ilk"
	-@erase "$(OUTDIR)\avisdf.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\avissendmail" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /I "e:\newdev\src\AVISFILE" /I "e:\newdev\src\AVISFILE\MD5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DFGui.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=e:\newdev\lib\avisdfrl.lib e:\newdev\lib\avissendmail.lib e:\newdev\lib\avisdfmg.lib e:\newdev\lib\avisdfev.lib e:\newdev\lib\avisdb.lib e:\newdev\lib\aviscommon.lib e:\newdev\lib\avisfile.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\avisdf.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avisdf.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\computemd5checksum.obj" \
	"$(INTDIR)\crashrecover.obj" \
	"$(INTDIR)\DFConfigureDialog.obj" \
	"$(INTDIR)\DFGui.obj" \
	"$(INTDIR)\DFGuiDoc.obj" \
	"$(INTDIR)\DFGuiView.obj" \
	"$(INTDIR)\DFSampleAttributes.obj" \
	"$(INTDIR)\DFStatisticsDialog.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\PriorityDialog.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\DFGui.res"

"$(OUTDIR)\avisdf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DFGui - Win32 Release IDE"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\avisdf.exe"


CLEAN :
	-@erase "$(INTDIR)\computemd5checksum.obj"
	-@erase "$(INTDIR)\crashrecover.obj"
	-@erase "$(INTDIR)\DFConfigureDialog.obj"
	-@erase "$(INTDIR)\DFGui.obj"
	-@erase "$(INTDIR)\DFGui.pch"
	-@erase "$(INTDIR)\DFGui.res"
	-@erase "$(INTDIR)\DFGuiDoc.obj"
	-@erase "$(INTDIR)\DFGuiView.obj"
	-@erase "$(INTDIR)\DFSampleAttributes.obj"
	-@erase "$(INTDIR)\DFStatisticsDialog.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\PriorityDialog.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\avisdf.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DFGui.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DFGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\dfmanager\release\avisdfmg.lib e:\newdev\lib\avisdfrl.lib ..\dfeval\release\avisdfevl.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\avisdf.pdb" /machine:I386 /out:"$(OUTDIR)\avisdf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\computemd5checksum.obj" \
	"$(INTDIR)\crashrecover.obj" \
	"$(INTDIR)\DFConfigureDialog.obj" \
	"$(INTDIR)\DFGui.obj" \
	"$(INTDIR)\DFGuiDoc.obj" \
	"$(INTDIR)\DFGuiView.obj" \
	"$(INTDIR)\DFSampleAttributes.obj" \
	"$(INTDIR)\DFStatisticsDialog.obj" \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\PriorityDialog.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\DFGui.res"

"$(OUTDIR)\avisdf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DFGui.dep")
!INCLUDE "DFGui.dep"
!ELSE 
!MESSAGE Warning: cannot find "DFGui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DFGui - Win32 Release" || "$(CFG)" == "DFGui - Win32 Debug" || "$(CFG)" == "DFGui - Win32 Debug IDE" || "$(CFG)" == "DFGui - Win32 Release IDE"
SOURCE=..\com\computemd5checksum.cpp

"$(INTDIR)\computemd5checksum.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\crashrecover.cpp

"$(INTDIR)\crashrecover.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFConfigureDialog.cpp

"$(INTDIR)\DFConfigureDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFGui.cpp

"$(INTDIR)\DFGui.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFGui.rc

"$(INTDIR)\DFGui.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\DFGuiDoc.cpp

"$(INTDIR)\DFGuiDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFGuiView.cpp

"$(INTDIR)\DFGuiView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFSampleAttributes.cpp

"$(INTDIR)\DFSampleAttributes.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=.\DFStatisticsDialog.cpp

"$(INTDIR)\DFStatisticsDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=..\com\listvwex.cpp

"$(INTDIR)\listvwex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=..\com\paramvalue.cpp

"$(INTDIR)\paramvalue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\PriorityDialog.cpp

"$(INTDIR)\PriorityDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"


SOURCE=..\com\singleinstance.cpp

"$(INTDIR)\singleinstance.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "DFGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\avissendmail" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DFGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug"

CPP_SWITCHES=/MDd /W3 /GX /Zi /Od /I "..\inc" /I "..\dfgui" /I "..\com" /I "..\..\avisdb" /I "..\..\aviscommon" /I "..\..\inc" /I "..\..\avisfile" /I "..\..\avisfile\md5" /I "..\..\avissendmail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DFGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "DFGui - Win32 Debug IDE"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\avissendmail" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /I "e:\newdev\src\AVISFILE" /I "e:\newdev\src\AVISFILE\MD5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DFGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "DFGui - Win32 Release IDE"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "e:\newdev\src\dataflow\inc" /I "e:\newdev\src\dataflow\com" /I "e:\newdev\src\avisdb" /I "e:\newdev\src\aviscommon" /I "e:\newdev\src\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\DFGui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DFGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\com\titletip.cpp

"$(INTDIR)\titletip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DFGui.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

