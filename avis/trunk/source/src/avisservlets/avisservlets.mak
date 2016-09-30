# Microsoft Developer Studio Generated NMAKE File, Based on AVISServlets.dsp
!IF "$(CFG)" == ""
CFG=AVISServlets - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AVISServlets - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AVISServlets - Win32 Release" && "$(CFG)" != "AVISServlets - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVISServlets.mak" CFG="AVISServlets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVISServlets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVISServlets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AVISServlets - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\$(OUTD)\AVISServlets.dll"


CLEAN :
	-@erase "$(INTDIR)\agetsamp.obj"
	-@erase "$(INTDIR)\agetsig.obj"
	-@erase "$(INTDIR)\agetstat.obj"
	-@erase "$(INTDIR)\apstsig.obj"
	-@erase "$(INTDIR)\apststat.obj"
	-@erase "$(INTDIR)\asubsamp.obj"
	-@erase "$(INTDIR)\avisredirector.obj"
	-@erase "$(INTDIR)\AVISServlets.obj"
	-@erase "$(INTDIR)\dbconn.obj"
	-@erase "$(INTDIR)\dbconnhnd.obj"
	-@erase "$(INTDIR)\dbconpool.obj"
	-@erase "$(INTDIR)\dllentry.obj"
	-@erase "$(INTDIR)\goservletclient.obj"
	-@erase "$(INTDIR)\goservlethost.obj"
	-@erase "$(INTDIR)\gosession.obj"
	-@erase "$(INTDIR)\gwapiprx.obj"
	-@erase "$(INTDIR)\processclient.obj"
	-@erase "$(INTDIR)\redirectorstate.obj"
	-@erase "$(INTDIR)\sessionclient.obj"
	-@erase "$(INTDIR)\sessionhost.obj"
	-@erase "$(INTDIR)\sigcache.obj"
	-@erase "$(INTDIR)\sigdlselect.obj"
	-@erase "$(INTDIR)\sigfile.obj"
	-@erase "$(INTDIR)\sigfilehnd.obj"
	-@erase "$(INTDIR)\stringmap.obj"
	-@erase "$(INTDIR)\strutil.obj"
	-@erase "$(INTDIR)\utilexception.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AVISServlets.exp"
	-@erase "$(OUTDIR)\AVISServlets.lib"
	-@erase ".\$(OUTD)\AVISServlets.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(SRCD)" /I "$(SRCD)/avisunzip" /I "$(SRCD)/avisunpack" /I "$(SRCD)/avispolicy" /I "$(SRCD)/avisfile" /I "$(SRCD)/aviscommon/system" /I "$(SRCD)/avisfilter" /I "$(SRCD)/aviscommon" /I "$(SRCD)/avisdb" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /Fp"$(INTDIR)\AVISServlets.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OBJD)/ea/AvisServlets/AVISServlets.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=vdbunpacker.lib avispolicy.lib aviscommon.lib avisfile.lib avisfilter.lib avisdb.lib httpdapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OBJD)/AVISServlets.pdb" /machine:I386 /def:".\avisservlets.def" /out:"$(OUTD)/AVISServlets.dll" /implib:"$(OUTDIR)\AVISServlets.lib" /libpath:"d:\additionalLibraryPath" /libpath:"$(OUTD)" /libpath:"$(OBJD)" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt" 
DEF_FILE= \
	".\avisservlets.def"
LINK32_OBJS= \
	"$(INTDIR)\agetsamp.obj" \
	"$(INTDIR)\agetsig.obj" \
	"$(INTDIR)\agetstat.obj" \
	"$(INTDIR)\apstsig.obj" \
	"$(INTDIR)\apststat.obj" \
	"$(INTDIR)\asubsamp.obj" \
	"$(INTDIR)\avisredirector.obj" \
	"$(INTDIR)\AVISServlets.obj" \
	"$(INTDIR)\dbconn.obj" \
	"$(INTDIR)\dbconnhnd.obj" \
	"$(INTDIR)\dbconpool.obj" \
	"$(INTDIR)\dllentry.obj" \
	"$(INTDIR)\goservletclient.obj" \
	"$(INTDIR)\goservlethost.obj" \
	"$(INTDIR)\gosession.obj" \
	"$(INTDIR)\gwapiprx.obj" \
	"$(INTDIR)\processclient.obj" \
	"$(INTDIR)\redirectorstate.obj" \
	"$(INTDIR)\sessionclient.obj" \
	"$(INTDIR)\sessionhost.obj" \
	"$(INTDIR)\sigcache.obj" \
	"$(INTDIR)\sigdlselect.obj" \
	"$(INTDIR)\sigfile.obj" \
	"$(INTDIR)\sigfilehnd.obj" \
	"$(INTDIR)\stringmap.obj" \
	"$(INTDIR)\strutil.obj" \
	"$(INTDIR)\utilexception.obj"

".\$(OUTD)\AVISServlets.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AVISServlets - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\$(OUTD)\AVISServlets.dll"


CLEAN :
	-@erase "$(INTDIR)\agetsamp.obj"
	-@erase "$(INTDIR)\agetsig.obj"
	-@erase "$(INTDIR)\agetstat.obj"
	-@erase "$(INTDIR)\apstsig.obj"
	-@erase "$(INTDIR)\apststat.obj"
	-@erase "$(INTDIR)\asubsamp.obj"
	-@erase "$(INTDIR)\avisredirector.obj"
	-@erase "$(INTDIR)\AVISServlets.obj"
	-@erase "$(INTDIR)\dbconn.obj"
	-@erase "$(INTDIR)\dbconnhnd.obj"
	-@erase "$(INTDIR)\dbconpool.obj"
	-@erase "$(INTDIR)\dllentry.obj"
	-@erase "$(INTDIR)\goservletclient.obj"
	-@erase "$(INTDIR)\goservlethost.obj"
	-@erase "$(INTDIR)\gosession.obj"
	-@erase "$(INTDIR)\gwapiprx.obj"
	-@erase "$(INTDIR)\processclient.obj"
	-@erase "$(INTDIR)\redirectorstate.obj"
	-@erase "$(INTDIR)\sessionclient.obj"
	-@erase "$(INTDIR)\sessionhost.obj"
	-@erase "$(INTDIR)\sigcache.obj"
	-@erase "$(INTDIR)\sigdlselect.obj"
	-@erase "$(INTDIR)\sigfile.obj"
	-@erase "$(INTDIR)\sigfilehnd.obj"
	-@erase "$(INTDIR)\stringmap.obj"
	-@erase "$(INTDIR)\strutil.obj"
	-@erase "$(INTDIR)\utilexception.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVISServlets.exp"
	-@erase "$(OUTDIR)\AVISServlets.lib"
	-@erase ".\$(OBJD)\AVISServlets.pdb"
	-@erase ".\$(OUTD)\AVISServlets.dll"
	-@erase ".\$(OUTD)\AVISServlets.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SRCD)" /I "$(SRCD)/avisunzip" /I "$(SRCD)/avisunpack" /I "$(SRCD)/avispolicy" /I "$(SRCD)/avisfile" /I "$(SRCD)/aviscommon/system" /I "$(SRCD)/avisfilter" /I "$(SRCD)/aviscommon" /I "$(SRCD)/avisdb" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVISSERVLETS_EXPORTS" /Fp"$(INTDIR)\AVISServlets.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OBJD)/ea/AvisServlets/AVISServlets.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=vdbunpacker.lib avispolicy.lib aviscommon.lib avisfile.lib avisfilter.lib avisdb.lib httpdapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OBJD)/AVISServlets.pdb" /debug /debugtype:both /machine:I386 /def:".\avisservlets.def" /out:"$(OUTD)/AVISServlets.dll" /implib:"$(OUTDIR)\AVISServlets.lib" /pdbtype:sept /libpath:"$(LIBD)\ea" /libpath:"$(OUTD)" /libpath:"$(OBJD)" /libpath:"$(LIBD)" /libpath:"$(LIBD)\nt" 
DEF_FILE= \
	".\avisservlets.def"
LINK32_OBJS= \
	"$(INTDIR)\agetsamp.obj" \
	"$(INTDIR)\agetsig.obj" \
	"$(INTDIR)\agetstat.obj" \
	"$(INTDIR)\apstsig.obj" \
	"$(INTDIR)\apststat.obj" \
	"$(INTDIR)\asubsamp.obj" \
	"$(INTDIR)\avisredirector.obj" \
	"$(INTDIR)\AVISServlets.obj" \
	"$(INTDIR)\dbconn.obj" \
	"$(INTDIR)\dbconnhnd.obj" \
	"$(INTDIR)\dbconpool.obj" \
	"$(INTDIR)\dllentry.obj" \
	"$(INTDIR)\goservletclient.obj" \
	"$(INTDIR)\goservlethost.obj" \
	"$(INTDIR)\gosession.obj" \
	"$(INTDIR)\gwapiprx.obj" \
	"$(INTDIR)\processclient.obj" \
	"$(INTDIR)\redirectorstate.obj" \
	"$(INTDIR)\sessionclient.obj" \
	"$(INTDIR)\sessionhost.obj" \
	"$(INTDIR)\sigcache.obj" \
	"$(INTDIR)\sigdlselect.obj" \
	"$(INTDIR)\sigfile.obj" \
	"$(INTDIR)\sigfilehnd.obj" \
	"$(INTDIR)\stringmap.obj" \
	"$(INTDIR)\strutil.obj" \
	"$(INTDIR)\utilexception.obj"

".\$(OUTD)\AVISServlets.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AVISServlets.dep")
!INCLUDE "AVISServlets.dep"
!ELSE 
!MESSAGE Warning: cannot find "AVISServlets.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVISServlets - Win32 Release" || "$(CFG)" == "AVISServlets - Win32 Debug"
SOURCE=.\agetsamp.cpp

"$(INTDIR)\agetsamp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\agetsig.cpp

"$(INTDIR)\agetsig.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\agetstat.cpp

"$(INTDIR)\agetstat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\apstsig.cpp

"$(INTDIR)\apstsig.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\apststat.cpp

"$(INTDIR)\apststat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\asubsamp.cpp

"$(INTDIR)\asubsamp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avisredirector.cpp

"$(INTDIR)\avisredirector.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AVISServlets.cpp

"$(INTDIR)\AVISServlets.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dbconn.cpp

"$(INTDIR)\dbconn.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dbconnhnd.cpp

"$(INTDIR)\dbconnhnd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dbconpool.cpp

"$(INTDIR)\dbconpool.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dllentry.cpp

"$(INTDIR)\dllentry.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\goservletclient.cpp

"$(INTDIR)\goservletclient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\goservlethost.cpp

"$(INTDIR)\goservlethost.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gosession.cpp

"$(INTDIR)\gosession.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gwapiprx.cpp

"$(INTDIR)\gwapiprx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\processclient.cpp

"$(INTDIR)\processclient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\redirectorstate.cpp

"$(INTDIR)\redirectorstate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sessionclient.cpp

"$(INTDIR)\sessionclient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sessionhost.cpp

"$(INTDIR)\sessionhost.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sigcache.cpp

"$(INTDIR)\sigcache.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sigdlselect.cpp

"$(INTDIR)\sigdlselect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sigfile.cpp

"$(INTDIR)\sigfile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sigfilehnd.cpp

"$(INTDIR)\sigfilehnd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\stringmap.cpp

"$(INTDIR)\stringmap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\strutil.cpp

"$(INTDIR)\strutil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\utilexception.cpp

"$(INTDIR)\utilexception.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

