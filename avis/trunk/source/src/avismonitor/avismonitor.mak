# Microsoft Developer Studio Generated NMAKE File, Based on AVISMonitor.dsp
!IF "$(CFG)" == ""
CFG=AVISMonitor - Win32 IDE Debug
!MESSAGE No configuration specified. Defaulting to AVISMonitor - Win32 IDE Debug.
!ENDIF 

!IF "$(CFG)" != "AVISMonitor - Win32 Release" && "$(CFG)" != "AVISMonitor - Win32 Debug" && "$(CFG)" != "AVISMonitor - Win32 IDE Debug" && "$(CFG)" != "AVISMonitor - Win32 IDE Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISMonitor.mak" CFG="AVISMonitor - Win32 IDE Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISMonitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 IDE Debug" (based on "Win32 (x86) Application")
!MESSAGE "AVISMonitor - Win32 IDE Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "AVISMonitor - Win32 Release"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\AVISMonitor
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitor.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISModule.obj"
	-@erase "$(INTDIR)\AVISMonitor.obj"
	-@erase "$(INTDIR)\AVISMonitor.pch"
	-@erase "$(INTDIR)\AVISMonitor.res"
	-@erase "$(INTDIR)\AVISMonitorView.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ModuleDialog.obj"
	-@erase "$(INTDIR)\MonitorSetupDialog.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISMonitor.exe"
	-@erase "\\\OBJD\AVISMonitor\AVISMonitor.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISMonitor.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AVISMonitorLib.lib psapi.lib AVISSendMail.lib /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AVISMonitor.pdb" /map:"$(INTDIR)\AVISMonitor.map" /machine:I386 /out:"$(OUTDIR)\AVISMonitor.exe" /libpath:"///LIBD///" 
LINK32_OBJS= \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\AVISModule.obj" \
	"$(INTDIR)\AVISMonitor.obj" \
	"$(INTDIR)\AVISMonitorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ModuleDialog.obj" \
	"$(INTDIR)\MonitorSetupDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISMonitor.res"

"$(OUTDIR)\AVISMonitor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 Debug"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\AVISMonitor
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitor.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISModule.obj"
	-@erase "$(INTDIR)\AVISMonitor.obj"
	-@erase "$(INTDIR)\AVISMonitor.pch"
	-@erase "$(INTDIR)\AVISMonitor.res"
	-@erase "$(INTDIR)\AVISMonitorView.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ModuleDialog.obj"
	-@erase "$(INTDIR)\MonitorSetupDialog.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISMonitor.exe"
	-@erase "$(OUTDIR)\AVISMonitor.pdb"
	-@erase "\\\OBJD\AVISMonitor\AVISMonitor.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/MDd /W3 /Gm /GX /Zi /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISMonitor.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AVISMonitorLib.lib psapi.lib AVISSendMail.lib /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AVISMonitor.pdb" /map:"$(INTDIR)\AVISMonitor.map" /debug /machine:I386 /out:"$(OUTDIR)\AVISMonitor.exe" /pdbtype:sept /libpath:"///LIBD///" 
LINK32_OBJS= \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\AVISModule.obj" \
	"$(INTDIR)\AVISMonitor.obj" \
	"$(INTDIR)\AVISMonitorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ModuleDialog.obj" \
	"$(INTDIR)\MonitorSetupDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISMonitor.res"

"$(OUTDIR)\AVISMonitor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitor.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISModule.obj"
	-@erase "$(INTDIR)\AVISMonitor.obj"
	-@erase "$(INTDIR)\AVISMonitor.pch"
	-@erase "$(INTDIR)\AVISMonitor.res"
	-@erase "$(INTDIR)\AVISMonitorView.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ModuleDialog.obj"
	-@erase "$(INTDIR)\MonitorSetupDialog.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISMonitor.exe"
	-@erase "$(OUTDIR)\AVISMonitor.ilk"
	-@erase "$(OUTDIR)\AVISMonitor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISMonitor.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AVISMonitorLib.lib psapi.lib AVISSendMail.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\AVISMonitor.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AVISMonitor.exe" /pdbtype:sept /libpath:"..\..\dlib\ea" 
LINK32_OBJS= \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\AVISModule.obj" \
	"$(INTDIR)\AVISMonitor.obj" \
	"$(INTDIR)\AVISMonitorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ModuleDialog.obj" \
	"$(INTDIR)\MonitorSetupDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISMonitor.res"

"$(OUTDIR)\AVISMonitor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\AVISMonitor.exe"


CLEAN :
	-@erase "$(INTDIR)\AVISModule.obj"
	-@erase "$(INTDIR)\AVISMonitor.obj"
	-@erase "$(INTDIR)\AVISMonitor.pch"
	-@erase "$(INTDIR)\AVISMonitor.res"
	-@erase "$(INTDIR)\AVISMonitorView.obj"
	-@erase "$(INTDIR)\listvwex.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\ModuleDialog.obj"
	-@erase "$(INTDIR)\MonitorSetupDialog.obj"
	-@erase "$(INTDIR)\paramvalue.obj"
	-@erase "$(INTDIR)\singleinstance.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\titletip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISMonitor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AVISMonitor.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AVISMonitor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=AVISMonitorLib.lib psapi.lib AVISSendMail.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AVISMonitor.pdb" /machine:I386 /out:"$(OUTDIR)\AVISMonitor.exe" /libpath:"..\..\rlib\ea" 
LINK32_OBJS= \
	"$(INTDIR)\listvwex.obj" \
	"$(INTDIR)\paramvalue.obj" \
	"$(INTDIR)\singleinstance.obj" \
	"$(INTDIR)\titletip.obj" \
	"$(INTDIR)\AVISModule.obj" \
	"$(INTDIR)\AVISMonitor.obj" \
	"$(INTDIR)\AVISMonitorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\ModuleDialog.obj" \
	"$(INTDIR)\MonitorSetupDialog.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AVISMonitor.res"

"$(OUTDIR)\AVISMonitor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AVISMonitor.dep")
!INCLUDE "AVISMonitor.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVISMonitor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVISMonitor - Win32 Release" || "$(CFG)" == "AVISMonitor - Win32 Debug" || "$(CFG)" == "AVISMonitor - Win32 IDE Debug" || "$(CFG)" == "AVISMonitor - Win32 IDE Release"
SOURCE=..\dataflow\com\listvwex.cpp

"$(INTDIR)\listvwex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dataflow\com\paramvalue.cpp

"$(INTDIR)\paramvalue.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dataflow\com\singleinstance.cpp

"$(INTDIR)\singleinstance.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dataflow\com\titletip.cpp

"$(INTDIR)\titletip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\AVISModule.cpp

"$(INTDIR)\AVISModule.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\AVISMonitor.cpp

"$(INTDIR)\AVISMonitor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\AVISMonitor.rc

"$(INTDIR)\AVISMonitor.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\AVISMonitorView.cpp

"$(INTDIR)\AVISMonitorView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\MainFrm.cpp

"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\ModuleDialog.cpp

"$(INTDIR)\ModuleDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\MonitorSetupDialog.cpp

"$(INTDIR)\MonitorSetupDialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AVISMonitor.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AVISMonitor - Win32 Release"

CPP_SWITCHES=/MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 Debug"

CPP_SWITCHES=/MDd /W3 /Gm /GX /Zi /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AVISMonitor - Win32 IDE Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\dataflow\com" /I "..\inc" /I "..\AVISMonitorLib" /I "..\AVISSendMail" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\AVISMonitor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AVISMonitor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

