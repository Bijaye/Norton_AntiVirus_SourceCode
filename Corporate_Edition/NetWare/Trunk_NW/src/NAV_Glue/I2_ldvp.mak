#
#	  NAVGlue  makefile	 using CodeWarrior
#

# Set Project
!define PROJECT_NAME I2_LDVP
!define NLMNAME I2_LDVP
!define VERSION_NUMBER "10,00,0"
!define COPYRIGHT_STRING "Copyright 1999 - 2004	 Symantec Corporation. All rights reserved."
!define DESCRIPTION_STRING "Symantec NAVAPI Interface NLM"
!define INITIAL_THREAD_NAME "I2_LDVP_Main"
!define SCREEN_NAME "none"

# Set Output directory
!ifdef %DEBUG
!define OUTDIR .\debug
!else
!define OUTDIR .\release
!endif

# Set Tools
!define COMPILER_CW	 MWCCNLM.exe
!define LINKER_CW		 MWLDNLM.exe

# Set include directories
!define CORE_TECH ..\..\..\..\..\Core_Technology\LDVP_Shared\Src

!define PROJECT_INCS &
	-I.\ &
	-I..\..\..\Common\Src\PScan &
	-I..\..\..\Common\Src\Nav_Glue &
	-I$(CORE_TECH)\Include &
	-I$(CORE_TECH)\Include\SecureComms &
	-I..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Netware\src\include &
	-I$(CORE_TECH)\Common\Misc &
	-I..\..\..\..\QuarantineServer\Shared\Include &
	-I..\..\..\..\Shared\MD5\Include &
	-I.\..\..\..\..\..\Release\Dec3\include &
	-I..\..\..\Win32\src\avcore\include\src &
	-I..\..\..\..\..\Release\ams\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\SymSafer\include

!define SYS_INCS &
	-I- &
	-I$(NOVELLNDK)\Include &
	-I$(NOVELLNDK)\Include\NLM &
	-I$(NOVELLNDK)\Include\NLM\nit &
	-I$(NOVELLNDK)\include\nlm\obsolete &
	-I$(METROWERKS_DIR)\Headers &
	-I..\..\..\..\..\Release\ECOM\include &
	-I$(METROWERKS_DIR)\Libraries\MSLC__~1\Include &
	-I..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Netware\src\include

#
# Set Compiler Flags
# Remove the push and pop defines when NAVAPI.h gets fixed.
#
!define DEFINES -DNLM -DSYM_NLM -DN_PLAT_NLM -DSERVER -DWS_32 -D__push=push -D__pop=pop &
					 -DNO_VSNPRINTF -DMemCheck -D_MBCS -DNO_WCHAR_T

!ifdef %DEBUG
!define DFLAGS -sym internal
!define DEFS $(DEFINES) $(DBG_THREADS)
!define CFLAGS
!else
!define DFLAGS
!define DEFS $(DEFINES) -DNDEBUG
!define CFLAGS
!endif

!define COMPILER_OPTS -c -nodefaults -relax_pointers -align 1 -ext obj -cwd include &
							 -o $(OUTDIR) -opt level=2 -bool on -maxerrors 5 &
							 -include NWGluePrefix.h &
							 $(DFLAGS) $(CFLAGS) $(DEFS) $(PROJECT_INCS) $(SYS_INCS)
!define CPP_OPTS -dialect c++ -iso_templates on -rtti on 

!define CWCC  $(COMPILER_DIR)\$(COMPILER_CW) $(COMPILER_OPTS)
!define CWCPP $(COMPILER_DIR)\$(COMPILER_CW) $(COMPILER_OPTS) $(CPP_OPTS)

#
# Set Linker Flags
#
!ifdef %DEBUG
!define DEBUG_LIBS d
!define LIBDIR debug
!else
!define DEBUG_LIBS
!define LIBDIR release
!endif

!define LINK_FLAGS1 $(DFLAGS) -stacksize 512000 -map $(OUTDIR)\$(PROJECT_NAME).map &
						  -check UnloadCheck
!define LINK_FLAGS2 -nlmversion $(VERSION_NUMBER) -type generic -nostdlib &
						  -o $(OUTDIR)\$(PROJECT_NAME).nlm

# -osym $(PROJECT_NAME).sym -sym codeview4 -sym internal

!define LIB_DIRS &
	-L$(NOVELLNDK)\imports &
	-L$(METROWERKS_DIR)\libraries\runtime &
	-L..\tools\intel.lib &
	-L.\ &
	-L.\..\..\..\..\..\Release\Dec3\lib\netware\$(LIBDIR) &
	-L$(METROWERKS_DIR)\Librar~1\Runtime\Output\CLib &
	-L$(METROWERKS_DIR)\Librar~1\MSLC__~1\Output\CLib &
	-L$(METROWERKS_DIR)\Librar~1\MSLC__~1 &
	-L..\..\..\..\..\Release\ams\lib\nw

!define CWLD $(COMPILER_DIR)\$(LINKER_CW) $(LINK_FLAGS1) $(LINK_FLAGS2) $(LIB_DIRS)

#
# Primary Dependency
#

all : $(OUTDIR)\$(NLMNAME).nlm .SYMBOLIC

#
# Compile everything

$(OUTDIR)\mynewop.obj : ..\..\..\Common\src\PScan\mynewop.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\mynewop.cpp

$(OUTDIR)\winStrings.obj : ..\..\..\NetWare\src\common\winStrings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winStrings.cpp

$(OUTDIR)\winThreads.obj : ..\..\..\NetWare\src\common\winThreads.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winThreads.cpp

$(OUTDIR)\Misc3.obj : ..\..\..\Common\src\NAV_Glue\Misc3.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\NAV_GLUE\Misc3.cpp

$(OUTDIR)\eventsinknocc.obj : ..\..\..\Common\src\NAV_Glue\eventsinknocc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\NAV_GLUE\eventsinknocc.cpp

$(OUTDIR)\decomposersdknocc.obj : ..\..\..\Common\src\PScan\decomposersdknocc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\decomposersdknocc.cpp

$(OUTDIR)\navdecmp.obj : ..\..\..\Common\src\NAV_Glue\navdecmp.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\NAV_GLUE\navdecmp.cpp

$(OUTDIR)\debug.obj : ..\..\..\Common\src\NAV_Glue\debug.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\debug.cpp

$(OUTDIR)\debug2.obj : ..\..\..\Common\src\NAV_Glue\debug2.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\debug2.cpp

$(OUTDIR)\glue.obj : ..\..\..\Common\src\NAV_Glue\glue.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\glue.cpp

$(OUTDIR)\glue2.obj : ..\..\..\Common\src\NAV_Glue\glue2.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\glue2.cpp

$(OUTDIR)\glue3.obj : ..\..\..\Common\src\NAV_Glue\glue3.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\glue3.cpp

$(OUTDIR)\misc.obj : ..\..\..\Common\src\NAV_Glue\misc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\misc.cpp

$(OUTDIR)\misc2.obj : ..\..\..\Common\src\NAV_Glue\misc2.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\misc2.cpp

$(OUTDIR)\trckallc.obj : ..\..\..\Common\src\NAV_Glue\trckallc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\trckallc.cpp

$(OUTDIR)\decomp.obj : ..\..\..\Common\src\NAV_Glue\decomp.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\decomp.cpp

$(OUTDIR)\nlm.obj : ..\..\..\netware\src\common\nlm.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\netware\src\common\nlm.cpp

$(OUTDIR)\vdbversion.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vdbversion.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vdbversion.cpp

$(OUTDIR)\Filetime.obj : ..\..\..\Common\src\PScan\Filetime.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\PScan\Filetime.cpp

$(OUTDIR)\CSV.obj : ..\..\..\common\src\pscan\CSV.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\CSV.cpp

$(OUTDIR)\DataSet.obj : ..\..\..\common\src\pscan\DataSet.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\DataSet.cpp

$(OUTDIR)\ThreatCatSettings.obj : ..\..\..\common\src\pscan\ThreatCatSettings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ThreatCatSettings.cpp

$(OUTDIR)\gluemem.obj : ..\..\..\Common\src\NAV_Glue\gluemem.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\gluemem.cpp

$(OUTDIR)\savvirusinfo.obj : ..\..\..\Common\src\NAV_Glue\savvirusinfo.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\NAV_Glue\savvirusinfo.cpp

$(OUTDIR)\NetwareSymSaferRegistry.obj : ..\common\NetwareSymSaferRegistry.cpp .AUTODEPEND
 *$(CWCPP) ..\common\NetwareSymSaferRegistry.cpp

$(OUTDIR)\vpstrutils.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vpstrutils.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vpstrutils.cpp

#
# Perform the link
#
#
!define LIB_FILES &
	-lmwcrtl$(DEBUG_LIBS).lib -lmwcpp$(DEBUG_LIBS).lib &
	-lclib.imp -lthreads.imp -lnlmlib.imp -llocnlm32.imp &
	-lcba.lib -lnts.lib -lctypes -lsymtnef -lgrenleaf -lunarj -llib95 &
	-lsymamg32 -lsymgzip -lsymlha -lsymoless -lsymrar -lsymzlib &
	-lrec2lib -lrec2 &
	-ldecsdk -ldec2id -ldec2amg -ldec2arj -ldec2gzip -ldec2lha -ldec2ole1 -ldec2rar &
	-ldec2rtf -ldec2ss -ldec2tar -ldec2tnef -ldec2zip -lsymlz -ldec2as -ldec2lz &
	-ldec2mb3 -ldec2 -ldec2lib -ldec2text -lkeymgr -llogmgr

$(OUTDIR)\$(NLMNAME).nlm : &
$(OUTDIR)\Misc3.obj &
$(OUTDIR)\eventsinknocc.obj &
$(OUTDIR)\mynewop.obj &
$(OUTDIR)\winStrings.obj &
$(OUTDIR)\winThreads.obj &
$(OUTDIR)\decomposersdknocc.obj &
$(OUTDIR)\navdecmp.obj &
$(OUTDIR)\debug.obj &
$(OUTDIR)\debug2.obj &
$(OUTDIR)\glue.obj &
$(OUTDIR)\glue2.obj &
$(OUTDIR)\glue3.obj &
$(OUTDIR)\misc.obj &
$(OUTDIR)\misc2.obj &
$(OUTDIR)\trckallc.obj &
$(OUTDIR)\decomp.obj &
$(OUTDIR)\nlm.obj &
$(OUTDIR)\vdbversion.obj &
$(OUTDIR)\Filetime.obj &
$(OUTDIR)\CSV.obj &
$(OUTDIR)\DataSet.obj &
$(OUTDIR)\ThreatCatSettings.obj &
$(OUTDIR)\gluemem.obj &
$(OUTDIR)\savvirusinfo.obj &
$(OUTDIR)\NetwareSymSaferRegistry.obj &
$(OUTDIR)\vpstrutils.obj &
.AUTODEPEND

 @%write	 $(NLMNAME).def COPYRIGHT $(COPYRIGHT_STRING)
 @%append $(NLMNAME).def DESCRIPTION $(DESCRIPTION_STRING)
 @%append $(NLMNAME).def SCREENNAME $(SCREEN_NAME)
 @%append $(NLMNAME).def THREADNAME $(INITIAL_THREAD_NAME)
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def module clib
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def import scanThreadIDArray
 @%append $(NLMNAME).def import dec_bScanExclude
 @%append $(NLMNAME).def import @clib.imp
 @%append $(NLMNAME).def import @imps.imp
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def export @I2_LDVP.EXP
 @%append $(NLMNAME).def export CreateDecThreadArray
 @%append $(NLMNAME).def

 *$(CWLD) $(LIB_FILES) -commandfile $(NLMNAME).def $(OUTDIR)\\*.obj &
			 $(NOVELLNDK)\\imports\\clibpre.o
