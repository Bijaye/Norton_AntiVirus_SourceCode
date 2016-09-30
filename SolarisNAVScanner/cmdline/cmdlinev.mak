# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmdlinev.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\CMDLINE.EXE $(OUTDIR)/cmdlinev.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "VC20" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "VC20"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"cmdlinev.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"cmdlinev.bsc" 
BSC32_SBRS= \
	$(INTDIR)/CMDSTR.SBR \
	$(INTDIR)/GETINFO.SBR \
	$(INTDIR)/CRCSCAN.SBR \
	$(INTDIR)/CMDPARSE.SBR \
	$(INTDIR)/ALGLOAD.SBR \
	$(INTDIR)/SCANFILE.SBR \
	$(INTDIR)/REPARAUX.SBR \
	$(INTDIR)/BOOTREP.SBR \
	$(INTDIR)/SCANINIT.SBR \
	$(INTDIR)/MEMLOAD.SBR \
	$(INTDIR)/INFOAUX.SBR \
	$(INTDIR)/ENGCOPYF.SBR \
	$(INTDIR)/VIRLIST.SBR \
	$(INTDIR)/PATHNAME.SBR \
	$(INTDIR)/SCANMEM.SBR \
	$(INTDIR)/MSGSTR.SBR \
	$(INTDIR)/REPORT.SBR \
	$(INTDIR)/FILEFIND.SBR \
	$(INTDIR)/BOOLOAD.SBR \
	$(INTDIR)/AVCACHE.SBR \
	$(INTDIR)/CLCB.SBR \
	$(INTDIR)/AVENGE.SBR \
	$(INTDIR)/ALGSCAN.SBR \
	$(INTDIR)/FSTART.SBR \
	$(INTDIR)/CMDLINE.SBR \
	$(INTDIR)/MEMSCAN.SBR \
	$(INTDIR)/CRCLOAD.SBR \
	$(INTDIR)/DELENTRY.SBR \
	$(INTDIR)/HELPSTR.SBR \
	$(INTDIR)/CMDHELP.SBR \
	$(INTDIR)/EXT.SBR \
	$(INTDIR)/VDATFILE.SBR \
	$(INTDIR)/FILEREP.SBR \
	$(INTDIR)/SCANBOOT.SBR

$(OUTDIR)/cmdlinev.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386 /OUT:"CMDLINE.EXE"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"cmdlinev.pdb" /MACHINE:I386 /OUT:"CMDLINE.EXE" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/CMDSTR.OBJ \
	$(INTDIR)/GETINFO.OBJ \
	$(INTDIR)/CRCSCAN.OBJ \
	$(INTDIR)/CMDPARSE.OBJ \
	$(INTDIR)/ALGLOAD.OBJ \
	$(INTDIR)/SCANFILE.OBJ \
	$(INTDIR)/REPARAUX.OBJ \
	$(INTDIR)/BOOTREP.OBJ \
	$(INTDIR)/SCANINIT.OBJ \
	$(INTDIR)/MEMLOAD.OBJ \
	$(INTDIR)/INFOAUX.OBJ \
	$(INTDIR)/ENGCOPYF.OBJ \
	$(INTDIR)/VIRLIST.OBJ \
	$(INTDIR)/PATHNAME.OBJ \
	$(INTDIR)/SCANMEM.OBJ \
	$(INTDIR)/MSGSTR.OBJ \
	$(INTDIR)/REPORT.OBJ \
	$(INTDIR)/FILEFIND.OBJ \
	$(INTDIR)/BOOLOAD.OBJ \
	$(INTDIR)/AVCACHE.OBJ \
	$(INTDIR)/CLCB.OBJ \
	$(INTDIR)/AVENGE.OBJ \
	$(INTDIR)/ALGSCAN.OBJ \
	$(INTDIR)/FSTART.OBJ \
	$(INTDIR)/CMDLINE.OBJ \
	$(INTDIR)/MEMSCAN.OBJ \
	$(INTDIR)/CRCLOAD.OBJ \
	$(INTDIR)/DELENTRY.OBJ \
	$(INTDIR)/HELPSTR.OBJ \
	$(INTDIR)/CMDHELP.OBJ \
	$(INTDIR)/EXT.OBJ \
	$(INTDIR)/VDATFILE.OBJ \
	$(INTDIR)/FILEREP.OBJ \
	$(INTDIR)/SCANBOOT.OBJ

.\CMDLINE.EXE : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\CMDLINE.EXE $(OUTDIR)/cmdlinev.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "VC20" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "VC20" /FR$(INTDIR)/ /Fp$(OUTDIR)/"cmdlinev.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"cmdlinev.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"cmdlinev.bsc" 
BSC32_SBRS= \
	$(INTDIR)/CMDSTR.SBR \
	$(INTDIR)/GETINFO.SBR \
	$(INTDIR)/CRCSCAN.SBR \
	$(INTDIR)/CMDPARSE.SBR \
	$(INTDIR)/ALGLOAD.SBR \
	$(INTDIR)/SCANFILE.SBR \
	$(INTDIR)/REPARAUX.SBR \
	$(INTDIR)/BOOTREP.SBR \
	$(INTDIR)/SCANINIT.SBR \
	$(INTDIR)/MEMLOAD.SBR \
	$(INTDIR)/INFOAUX.SBR \
	$(INTDIR)/ENGCOPYF.SBR \
	$(INTDIR)/VIRLIST.SBR \
	$(INTDIR)/PATHNAME.SBR \
	$(INTDIR)/SCANMEM.SBR \
	$(INTDIR)/MSGSTR.SBR \
	$(INTDIR)/REPORT.SBR \
	$(INTDIR)/FILEFIND.SBR \
	$(INTDIR)/BOOLOAD.SBR \
	$(INTDIR)/AVCACHE.SBR \
	$(INTDIR)/CLCB.SBR \
	$(INTDIR)/AVENGE.SBR \
	$(INTDIR)/ALGSCAN.SBR \
	$(INTDIR)/FSTART.SBR \
	$(INTDIR)/CMDLINE.SBR \
	$(INTDIR)/MEMSCAN.SBR \
	$(INTDIR)/CRCLOAD.SBR \
	$(INTDIR)/DELENTRY.SBR \
	$(INTDIR)/HELPSTR.SBR \
	$(INTDIR)/CMDHELP.SBR \
	$(INTDIR)/EXT.SBR \
	$(INTDIR)/VDATFILE.SBR \
	$(INTDIR)/FILEREP.SBR \
	$(INTDIR)/SCANBOOT.SBR

$(OUTDIR)/cmdlinev.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386 /OUT:"CMDLINE.EXE"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"cmdlinev.pdb" /DEBUG /MACHINE:I386 /OUT:"CMDLINE.EXE" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/CMDSTR.OBJ \
	$(INTDIR)/GETINFO.OBJ \
	$(INTDIR)/CRCSCAN.OBJ \
	$(INTDIR)/CMDPARSE.OBJ \
	$(INTDIR)/ALGLOAD.OBJ \
	$(INTDIR)/SCANFILE.OBJ \
	$(INTDIR)/REPARAUX.OBJ \
	$(INTDIR)/BOOTREP.OBJ \
	$(INTDIR)/SCANINIT.OBJ \
	$(INTDIR)/MEMLOAD.OBJ \
	$(INTDIR)/INFOAUX.OBJ \
	$(INTDIR)/ENGCOPYF.OBJ \
	$(INTDIR)/VIRLIST.OBJ \
	$(INTDIR)/PATHNAME.OBJ \
	$(INTDIR)/SCANMEM.OBJ \
	$(INTDIR)/MSGSTR.OBJ \
	$(INTDIR)/REPORT.OBJ \
	$(INTDIR)/FILEFIND.OBJ \
	$(INTDIR)/BOOLOAD.OBJ \
	$(INTDIR)/AVCACHE.OBJ \
	$(INTDIR)/CLCB.OBJ \
	$(INTDIR)/AVENGE.OBJ \
	$(INTDIR)/ALGSCAN.OBJ \
	$(INTDIR)/FSTART.OBJ \
	$(INTDIR)/CMDLINE.OBJ \
	$(INTDIR)/MEMSCAN.OBJ \
	$(INTDIR)/CRCLOAD.OBJ \
	$(INTDIR)/DELENTRY.OBJ \
	$(INTDIR)/HELPSTR.OBJ \
	$(INTDIR)/CMDHELP.OBJ \
	$(INTDIR)/EXT.OBJ \
	$(INTDIR)/VDATFILE.OBJ \
	$(INTDIR)/FILEREP.OBJ \
	$(INTDIR)/SCANBOOT.OBJ

.\CMDLINE.EXE : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\CMDSTR.CPP
DEP_CMDST=\
	.\TYPES.H\
	.\CMDPARSE.H

$(INTDIR)/CMDSTR.OBJ :  $(SOURCE)  $(DEP_CMDST) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GETINFO.CPP
DEP_GETIN=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/GETINFO.OBJ :  $(SOURCE)  $(DEP_GETIN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CRCSCAN.CPP
DEP_CRCSC=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/CRCSCAN.OBJ :  $(SOURCE)  $(DEP_CRCSC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMDPARSE.CPP
DEP_CMDPA=\
	.\AVENGE.H\
	.\CMDPARSE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/CMDPARSE.OBJ :  $(SOURCE)  $(DEP_CMDPA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ALGLOAD.CPP
DEP_ALGLO=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/ALGLOAD.OBJ :  $(SOURCE)  $(DEP_ALGLO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCANFILE.CPP
DEP_SCANF=\
	.\SCANFILE.H\
	.\FILEFIND.H\
	.\PATHNAME.H\
	.\EXT.H\
	.\CLCB.H\
	.\REPORT.H\
	.\MSGSTR.H\
	.\SCANGLOB.H\
	.\TYPES.H\
	.\AVCB.H\
	.\FILEINFO.H\
	.\AVENGE.H\
	.\CMDPARSE.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/SCANFILE.OBJ :  $(SOURCE)  $(DEP_SCANF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\REPARAUX.CPP
DEP_REPAR=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/REPARAUX.OBJ :  $(SOURCE)  $(DEP_REPAR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BOOTREP.CPP
DEP_BOOTR=\
	.\TYPES.H\
	.\AVENGE.H\
	.\AVENGEL.H\
	.\VDATFILE.H\
	.\OBJECT.H\
	.\AVCB.H\
	.\BOOTREP.H\
	.\ENDIAN.H

$(INTDIR)/BOOTREP.OBJ :  $(SOURCE)  $(DEP_BOOTR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCANINIT.CPP
DEP_SCANI=\
	.\SCANINIT.H\
	.\CLCB.H\
	.\REPORT.H\
	.\MSGSTR.H\
	.\SCANGLOB.H\
	.\TYPES.H\
	.\AVCB.H\
	.\FILEINFO.H\
	.\AVENGE.H\
	.\CMDPARSE.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/SCANINIT.OBJ :  $(SOURCE)  $(DEP_SCANI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEMLOAD.CPP
DEP_MEMLO=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/MEMLOAD.OBJ :  $(SOURCE)  $(DEP_MEMLO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INFOAUX.CPP
DEP_INFOA=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/INFOAUX.OBJ :  $(SOURCE)  $(DEP_INFOA) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ENGCOPYF.CPP
DEP_ENGCO=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/ENGCOPYF.OBJ :  $(SOURCE)  $(DEP_ENGCO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VIRLIST.CPP
DEP_VIRLI=\
	.\VIRLISTA.H\
	.\MSGSTR.H\
	.\SCANGLOB.H\
	.\AVENGE.H\
	.\CMDPARSE.H\
	.\FILEINFO.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/VIRLIST.OBJ :  $(SOURCE)  $(DEP_VIRLI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PATHNAME.CPP
DEP_PATHN=\
	.\TYPES.H\
	.\FILEFIND.H\
	.\PATHNAME.H

$(INTDIR)/PATHNAME.OBJ :  $(SOURCE)  $(DEP_PATHN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCANMEM.CPP
DEP_SCANM=\
	.\AVENGE.H\
	.\SCANMEM.H\
	.\CLCB.H\
	.\REPORT.H\
	.\MSGSTR.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H\
	.\SCANGLOB.H\
	.\FILEINFO.H\
	.\CMDPARSE.H

$(INTDIR)/SCANMEM.OBJ :  $(SOURCE)  $(DEP_SCANM) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MSGSTR.CPP
DEP_MSGST=\
	.\MSGSTR.H

$(INTDIR)/MSGSTR.OBJ :  $(SOURCE)  $(DEP_MSGST) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\REPORT.CPP
DEP_REPOR=\
	.\TYPES.H\
	.\FILEINFO.H\
	.\SCANGLOB.H\
	.\REPORT.H\
	.\AVENGE.H\
	.\MSGSTR.H\
	.\CMDPARSE.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/REPORT.OBJ :  $(SOURCE)  $(DEP_REPOR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEFIND.CPP
DEP_FILEF=\
	.\TYPES.H\
	.\FILEFIND.H\
	.\PATHNAME.H

$(INTDIR)/FILEFIND.OBJ :  $(SOURCE)  $(DEP_FILEF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BOOLOAD.CPP
DEP_BOOLO=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/BOOLOAD.OBJ :  $(SOURCE)  $(DEP_BOOLO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AVCACHE.CPP
DEP_AVCAC=\
	.\AVENGEL.H\
	.\MUTEX.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/AVCACHE.OBJ :  $(SOURCE)  $(DEP_AVCAC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CLCB.CPP
DEP_CLCB_=\
	\PL\MSVC20\INCLUDE\SYS\STAT.H\
	.\AVENGE.H\
	.\CLCB.H\
	.\FILEINFO.H\
	.\PATHNAME.H\
	.\SCANGLOB.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H\
	.\CMDPARSE.H

$(INTDIR)/CLCB.OBJ :  $(SOURCE)  $(DEP_CLCB_) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AVENGE.CPP
DEP_AVENG=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/AVENGE.OBJ :  $(SOURCE)  $(DEP_AVENG) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ALGSCAN.CPP
DEP_ALGSC=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/ALGSCAN.OBJ :  $(SOURCE)  $(DEP_ALGSC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FSTART.CPP
DEP_FSTAR=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/FSTART.OBJ :  $(SOURCE)  $(DEP_FSTAR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMDLINE.CPP
DEP_CMDLI=\
	.\SCANINIT.H\
	.\SCANMEM.H\
	.\SCANFILE.H\
	.\VIRLISTA.H\
	.\CMDPARSE.H\
	.\PATHNAME.H\
	.\FILEFIND.H\
	.\CLCB.H\
	.\REPORT.H\
	.\MSGSTR.H\
	.\CMDHELP.H\
	.\SCANGLOB.H\
	.\TYPES.H\
	.\AVCB.H\
	.\FILEINFO.H\
	.\AVENGE.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/CMDLINE.OBJ :  $(SOURCE)  $(DEP_CMDLI) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MEMSCAN.CPP
DEP_MEMSC=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/MEMSCAN.OBJ :  $(SOURCE)  $(DEP_MEMSC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CRCLOAD.CPP
DEP_CRCLO=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/CRCLOAD.OBJ :  $(SOURCE)  $(DEP_CRCLO) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DELENTRY.CPP
DEP_DELEN=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/DELENTRY.OBJ :  $(SOURCE)  $(DEP_DELEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HELPSTR.CPP
DEP_HELPS=\
	.\TYPES.H

$(INTDIR)/HELPSTR.OBJ :  $(SOURCE)  $(DEP_HELPS) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMDHELP.CPP
DEP_CMDHE=\
	.\TYPES.H\
	.\CMDHELP.H

$(INTDIR)/CMDHELP.OBJ :  $(SOURCE)  $(DEP_CMDHE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EXT.CPP
DEP_EXT_C=\
	.\TYPES.H

$(INTDIR)/EXT.OBJ :  $(SOURCE)  $(DEP_EXT_C) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\VDATFILE.CPP
DEP_VDATF=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/VDATFILE.OBJ :  $(SOURCE)  $(DEP_VDATF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEREP.CPP
DEP_FILER=\
	.\AVENGEL.H\
	.\AVENGE.H\
	.\TYPES.H\
	.\AVCB.H\
	.\OBJECT.H\
	.\ENDIAN.H\
	.\VDATFILE.H

$(INTDIR)/FILEREP.OBJ :  $(SOURCE)  $(DEP_FILER) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SCANBOOT.CPP

$(INTDIR)/SCANBOOT.OBJ :  $(SOURCE)  $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
