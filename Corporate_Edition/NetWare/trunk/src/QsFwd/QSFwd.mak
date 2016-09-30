#
#	QsFwd	 makefile	 using CodeWarrior
#

# Set Project
!define PROJECT_NAME QsFwd
!define NLMNAME QSFwd
!define QSFWD_VERSION_NUMBER "10,00,0"
!define COPYRIGHT_STRING "Copyright 1999 - 2003 Symantec Corporation. All rights reserved."
!define DESCRIPTION_STRING "Quarantine Forwarding NLM"
!define INITIAL_THREAD_NAME "QsFwd_Main"

# Set Output directory
!ifdef %DEBUG
!define OUTDIR .\debug
!else
!define OUTDIR .\release
!endif

# Set Tools
!define COMPILER_CW	 MWCCNLM.exe
!define LINKER_CW		 MWLDNLM.exe

# Set include paths
#!define PROJECT_DIR ..\..\..\..\..\Norton_Antivirus\Corporate_edition\NetWare\src\$(PROJECT_NAME)
!define PROJECT_DIR ..\..\..\NetWare\src\$(PROJECT_NAME)
##!define NORTON_ANTIVIRUS_CORPORATE_DIR ..\..\..\..\..\Norton_Antivirus\Corporate_Edition
!define NORTON_ANTIVIRUS_COMMON_PSCAN_DIR ..\..\..\..\..\Norton_Antivirus\Corporate_Edition\Common\Src\PScan
##!define LDVP_SHARED_INCLUDE_DIR ..\..\..\..\..\Core_technology\LDVP_Shared\Src\Include
##!define NORTON_ANTIVIRUS_MD5_DIR ..\..\..\..\..\Norton_Antivirus\Shared\MD5\Include
##!define SESA_INCLUDE_DIR ..\..\..\..\..\Release\SESA\Include
!define NORTON_ANTIVIRUS_QSERVER_DIR ..\..\..\..\..\Norton_Antivirus\QuarantineServer\Shared\Include
##!define LDVP_SHARED_MISC_DIR ..\..\..\..\..\Core_technology\LDVP_Shared\Src\Common\Misc
!define NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH ..\..\..\..\..\Norton_Antivirus\QuarantineServer
!define CORE_TECH ..\..\..\..\..\Core_Technology\LDVP_Shared\Src

!define PROJECT_INCS &
	-I.\ &
	-I$(PROJECT_DIR)\..\Include &
	-I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\Include &
	-I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32 &
	-I$(NORTON_ANTIVIRUS_QSERVER_DIR) &
	-I$(CORE_TECH)\Include &
	-I..\..\..\..\..\Release\ams\include &
	-I$(NORTON_ANTIVIRUS_COMMON_PSCAN_DIR) &
	-I..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\SymSafer\include

!define SYS_INCS &
	-I- &
	-I$(NOVELLNDK)\include\nlm &
	-I$(NOVELLNDK)\include &
	-I$(NOVELLNDK)\include\nlm\nit &
	-I$(NOVELLNDK)\include\nlm\sys &
	-I$(COMPILER_DIR_ROOT)\MSL\MSL_C\MSL_X86 &
	-I$(COMPILER_DIR_ROOT)\Win32-~1\Headers\Win32S~1 &
	-I$(COMPILER_DIR_ROOT)\Win32-~1\Headers\VCPPHe~1 &
	-I$(COMPILER_DIR_ROOT)\Win32-x86_Support\Headers\VCPP_Headers &
	-I$(COMPILER_NOVELL_SUPPORT)\Headers &
	-I$(COMPILER_DIR_ROOT)\Novell_Support\MetroWerks_Support\Libraries\MSLC__~1\Include

#
# Set Compiler Flags
#
!define DEFINES -DNLM -DSERVER -DNO_WCHAR_T

!ifdef %DEBUG
!define DFLAGS -sym internal
!define DEFS $(DEFINES)
!define CFLAGS
!else
!define DFLAGS
!define DEFS $(DEFINES) -DNDEBUG
!define CFLAGS
!endif

!define COMPILER_OPTS -nolink -bool on -maxerrors 5 -nodefaults -O2 -align 1 -ext obj &
							 -o $(OUTDIR) -prefix $(PROJECT_NAME).pch &
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

!define LINK_FLAGS1 $(DFLAGS) -stacksize 8192 -map $(OUTDIR)\$(PROJECT_NAME).map &
						  -threadname $(INITIAL_THREAD_NAME) &
						  -osym $(PROJECT_NAME).sym -sym codeview4  &
						  -copy $(COPYRIGHT_STRING) -desc $(DESCRIPTION_STRING)
!define LINK_FLAGS2 -nlmversion $(QSFWD_VERSION_NUMBER) -type generic -nostdlib &
						  -o $(OUTDIR)\$(PROJECT_NAME).nlm

!define LIB_DIRS &
	-L$(NOVELLNDK)\imports &
	-L$(COMPILER_NOVELL_SUPPORT)\Librar~1\runtime\ &
	-L$(PROJECT_DIR) &
	-L$(COMPILER_NOVELL_SUPPORT)\Librar~1\Runtime\Output\CLib &
	-L$(COMPILER_NOVELL_SUPPORT)\Librar~1\MSLC__~1\Output\CLib &
	-L$(COMPILER_NOVELL_SUPPORT)\Librar~1\MSLC__~1

!define CWLD $(COMPILER_DIR)\$(LINKER_CW) $(LINK_FLAGS1) $(LINK_FLAGS2) $(LIB_DIRS)

#
# Primary Dependency
#

all : $(OUTDIR)\QSFwd.nlm .SYMBOLIC

#
# Compile everything

$(OUTDIR)\QSFwd.obj : QSFwd.cpp .AUTODEPEND
 *$(CWCPP) QSFwd.cpp

$(OUTDIR)\Package.obj : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32\Package.cpp .AUTODEPEND
 *$(CWCPP) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32\Package.cpp

$(OUTDIR)\QsInfo.obj : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsInfo\QsInfo.cpp .AUTODEPEND
 *$(CWCPP) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsInfo\QsInfo.cpp

$(OUTDIR)\Tli.obj : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsComm32\Tli.cpp .AUTODEPEND
 *$(CWCPP) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsComm32\Tli.cpp

$(OUTDIR)\winStrings.obj : ..\..\..\NetWare\src\common\winStrings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winStrings.cpp

$(OUTDIR)\winThreads.obj : ..\..\..\NetWare\src\common\winThreads.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winThreads.cpp

$(OUTDIR)\NetwareSymSaferRegistry.obj : ..\common\NetwareSymSaferRegistry.cpp .AUTODEPEND
 *$(CWCPP) ..\common\NetwareSymSaferRegistry.cpp

#
# Perform the link
#
!define LIB_FILES &
	-lmwcrtl$(DEBUG_LIBS).lib -lmwcpp$(DEBUG_LIBS).lib &
	-lclib.imp -lthreads.imp -lnlmlib.imp -llocnlm32.imp &
	-ltli.imp

$(OUTDIR)\$(NLMNAME).nlm : &
$(OUTDIR)\QSFwd.obj &
$(OUTDIR)\Package.obj &
$(OUTDIR)\QsInfo.obj &
$(OUTDIR)\Tli.obj &
$(OUTDIR)\winStrings.obj &
$(OUTDIR)\winThreads.obj &
$(OUTDIR)\NetwareSymSaferRegistry.obj &
.AUTODEPEND

 *$(CWLD) -l$(PROJECT_NAME).exp $(LIB_FILES) -commandfile QsFwd.def $(OUTDIR)\\*.obj &
			 $(NOVELLNDK)\imports\clibpre.o
