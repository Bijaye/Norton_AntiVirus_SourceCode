#
#	  VPRegEdt	makefile	  using CodeWarrior
#

# Set Project
!define PROJECT_NAME VPRegEdt
!define NLMNAME VPRegEdt
!define VERSION_NUMBER "10,00,0"
!define COPYRIGHT_STRING "Copyright 1999 - 2004	 Symantec Corporation. All rights reserved."
!define DESCRIPTION_STRING "Symantec VP Registry Editor NLM"
!define INITIAL_THREAD_NAME "VPRegEdt_Main"
!define SCREEN_NAME "Symantec VPRegEdt"

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
	-I$(CORE_TECH)\Include &
	-I..\include &
	-I$(CORE_TECH)\Common\Misc &
	-I..\..\..\..\QuarantineServer\Include &
	-I..\..\..\..\QuarantineServer\Shared\Include &
	-I..\..\..\..\Shared\MD5\Include &
	-I.\..\..\..\..\..\Release\Dec3\include &
	-I.\..\..\..\..\..\Release\SESA_Collector_Framework\r1.01\Include &
	-I..\..\..\..\..\Release\Norton_Internet_Security\include &
	-I..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Netware\src\include &
	-I..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Common\src\include &
	-I..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Netware\src\ScsComms &
	-I..\..\..\..\..\Enterprise_Systems_SESACollectors\COLLECTOR_FRAMEWORK_PLUGINS\COMMON\CONNECTOR\ &
	-I..\..\..\..\..\Release\SLIC\include\slic &
	-I$(CORE_TECH)\include\SecureComms &
	-I$(CORE_TECH)\collectorplugins\SCFSesa\Include &
	-I..\..\..\..\..\Release\ams\include &
	-I..\..\..\Common\src\Utils\sym_synch\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\SymSafer\include

!define SYS_INCS &
	-I- &
	-I$(NOVELLNDK)\Include &
	-I$(NOVELLNDK)\Include\NLM &
	-I$(NOVELLNDK)\Include\NLM\nit &
	-I$(NOVELLNDK)\include\nlm\obsolete &
	-I$(METROWERKS_DIR)\Headers &
	-I$(METROWERKS_DIR)\Libraries\MSLC__~1\Include &
	-I$(METROWERKS_WIN)\VCPP_HEADERS

#
# Set Compiler Flags
#
!define DEFINES -DNLM -DSYM_NLM -DSTART -DN_PLAT_NLM -D_MSL_NO_WCHART_C_SUPPORT &
					 -DNO_VSNPRINTF -D_MSL_NO_MATH_LIB -DMemCheck -DINSTALL -D_MBCS 

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
							 $(DFLAGS) $(CFLAGS) $(DEFS) $(SYS_INCS) $(PROJECT_INCS)
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

!define LINK_FLAGS1 $(DFLAGS) -stacksize 24000 -map $(OUTDIR)\$(PROJECT_NAME).map
!define LINK_FLAGS2 -nlmversion $(VERSION_NUMBER) -type generic -nostdlib &
						  -o $(OUTDIR)\$(PROJECT_NAME).nlm

!define LIB_DIRS &
	-L$(NOVELLNDK)\imports &
	-L$(METROWERKS_DIR)\libraries\runtime &
	-L..\tools\intel.lib &
	-L.\ &
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

$(OUTDIR)\RegEdit.obj : regedit.cpp .AUTODEPEND
 *$(CWCPP) regedit.cpp

$(OUTDIR)\regmem.obj : ..\vpreg\regmem.cpp .AUTODEPEND
 *$(CWCPP) ..\vpreg\regmem.cpp

$(OUTDIR)\winStrings.obj : ..\..\..\NetWare\src\common\winStrings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winStrings.cpp

$(OUTDIR)\winThreads.obj : ..\..\..\NetWare\src\common\winThreads.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winThreads.cpp

$(OUTDIR)\NetwareSymSaferRegistry.obj : ..\common\NetwareSymSaferRegistry.cpp .AUTODEPEND
 *$(CWCPP) ..\common\NetwareSymSaferRegistry.cpp

#
# Perform the link
#
#
!define LIB_FILES &
	-lmwcrtl$(DEBUG_LIBS).lib -lmwcpp$(DEBUG_LIBS).lib &
	-lclib.imp -lthreads.imp -lnlmlib.imp -llocnlm32.imp &
	-lcba.lib -lnts.lib

$(OUTDIR)\$(NLMNAME).nlm : &
$(OUTDIR)\regmem.obj &
$(OUTDIR)\RegEdit.obj &
$(OUTDIR)\winStrings.obj &
$(OUTDIR)\winThreads.obj &
$(OUTDIR)\NetwareSymSaferRegistry.obj &
.AUTODEPEND

 @%write	 $(NLMNAME).def COPYRIGHT $(COPYRIGHT_STRING)
 @%append $(NLMNAME).def DESCRIPTION $(DESCRIPTION_STRING)
 @%append $(NLMNAME).def SCREENNAME $(SCREEN_NAME)
 @%append $(NLMNAME).def THREADNAME $(INITIAL_THREAD_NAME)
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def module clib
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def import @clib.imp
 @%append $(NLMNAME).def import @..\include\nwsnut.imp
 @%append $(NLMNAME).def import @imps.imp
 @%append $(NLMNAME).def import @..\..\..\common\src\pscan\registry.exp
 @%append $(NLMNAME).def import ntohs
 @%append $(NLMNAME).def

 *$(CWLD) $(LIB_FILES) -commandfile $(NLMNAME).def $(OUTDIR)\\*.obj &
			 $(NOVELLNDK)\\imports\\clibpre.o
