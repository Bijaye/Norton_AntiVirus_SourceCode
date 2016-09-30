# Microsoft Developer Studio Generated NMAKE File, Based on testgate.dsp
!IF "$(CFG)" == ""
CFG=testgate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to testgate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "testgate - Win32 Release" && "$(CFG)" != "testgate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "testgate.mak" CFG="testgate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testgate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "testgate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testgate - Win32 Release"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\loadgate
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\testgate.exe"


CLEAN :
	-@erase "$(INTDIR)\BROWSER.OBJ"
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\FILESET.OBJ"
	-@erase "$(INTDIR)\MOD_CRCCalculator.obj"
	-@erase "$(INTDIR)\MOD_md5.obj"
	-@erase "$(INTDIR)\MOD_MD5Calculator.obj"
	-@erase "$(INTDIR)\PERSOBJ.OBJ"
	-@erase "$(INTDIR)\SOCKUTIL.OBJ"
	-@erase "$(INTDIR)\TESTGATE.OBJ"
	-@erase "$(INTDIR)\TREEWALK.OBJ"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\testgate.exe"
	-@erase "\\\OBJD\loadgate\testgate.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\testgate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ///LIBD///\ssleay32.lib ///LIBD///\libeay32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\testgate.pdb" /map:"$(INTDIR)\testgate.map" /machine:I386 /out:"$(OUTDIR)\testgate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BROWSER.OBJ" \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\FILESET.OBJ" \
	"$(INTDIR)\MOD_CRCCalculator.obj" \
	"$(INTDIR)\MOD_md5.obj" \
	"$(INTDIR)\MOD_MD5Calculator.obj" \
	"$(INTDIR)\PERSOBJ.OBJ" \
	"$(INTDIR)\SOCKUTIL.OBJ" \
	"$(INTDIR)\TESTGATE.OBJ" \
	"$(INTDIR)\TREEWALK.OBJ" \
	"$(INTDIR)\UTILS.OBJ"

"$(OUTDIR)\testgate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "testgate - Win32 Debug"

OUTDIR=///OUTD///\nt
INTDIR=///OBJD///\loadgate
# Begin Custom Macros
OutDir=///OUTD///\nt
# End Custom Macros

ALL : "$(OUTDIR)\testgate.exe"


CLEAN :
	-@erase "$(INTDIR)\BROWSER.OBJ"
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\FILESET.OBJ"
	-@erase "$(INTDIR)\MOD_CRCCalculator.obj"
	-@erase "$(INTDIR)\MOD_md5.obj"
	-@erase "$(INTDIR)\MOD_MD5Calculator.obj"
	-@erase "$(INTDIR)\PERSOBJ.OBJ"
	-@erase "$(INTDIR)\SOCKUTIL.OBJ"
	-@erase "$(INTDIR)\TESTGATE.OBJ"
	-@erase "$(INTDIR)\TREEWALK.OBJ"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\testgate.exe"
	-@erase "$(OUTDIR)\testgate.pdb"
	-@erase "\\\OBJD\loadgate\testgate.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\testgate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ///LIBD///\ssleay32.lib ///LIBD///\libeay32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\testgate.pdb" /map:"$(INTDIR)\testgate.map" /debug /machine:I386 /out:"$(OUTDIR)\testgate.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\BROWSER.OBJ" \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\FILESET.OBJ" \
	"$(INTDIR)\MOD_CRCCalculator.obj" \
	"$(INTDIR)\MOD_md5.obj" \
	"$(INTDIR)\MOD_MD5Calculator.obj" \
	"$(INTDIR)\PERSOBJ.OBJ" \
	"$(INTDIR)\SOCKUTIL.OBJ" \
	"$(INTDIR)\TESTGATE.OBJ" \
	"$(INTDIR)\TREEWALK.OBJ" \
	"$(INTDIR)\UTILS.OBJ"

"$(OUTDIR)\testgate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("testgate.dep")
!INCLUDE "testgate.dep"
!ELSE 
!MESSAGE Warning: cannot find "testgate.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "testgate - Win32 Release" || "$(CFG)" == "testgate - Win32 Debug"
SOURCE=.\BROWSER.CPP

"$(INTDIR)\BROWSER.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MOD_AVIS\CRC.CPP

!IF  "$(CFG)" == "testgate - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "testgate - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\FILESET.CPP

"$(INTDIR)\FILESET.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MOD_AVIS\MOD_CRCCalculator.cpp

!IF  "$(CFG)" == "testgate - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MOD_CRCCalculator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "testgate - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MOD_CRCCalculator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\MOD_AVIS\MOD_md5.cpp

!IF  "$(CFG)" == "testgate - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MOD_md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "testgate - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MOD_md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\MOD_AVIS\MOD_MD5Calculator.cpp

!IF  "$(CFG)" == "testgate - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MOD_MD5Calculator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "testgate - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "MOD_AVIS" /I "..\OpenSSL\inc32" /I "..\OpenSSL\inc32\openssl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\testgate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MOD_MD5Calculator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PERSOBJ.CPP

"$(INTDIR)\PERSOBJ.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SOCKUTIL.CPP

"$(INTDIR)\SOCKUTIL.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TESTGATE.CPP

"$(INTDIR)\TESTGATE.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TREEWALK.CPP

"$(INTDIR)\TREEWALK.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UTILS.CPP

"$(INTDIR)\UTILS.OBJ" : $(SOURCE) "$(INTDIR)"



!ENDIF 

