#
#	  RTVScan  makefile	 using CodeWarrior
#

# Set Project
!define PROJECT_NAME RTVScan
!define NLMNAME RTVScan
!define VERSION_NUMBER "10,00,0"
!define COPYRIGHT_STRING "Copyright 1999 - 2004	 Symantec Corporation. All rights reserved."
!define DESCRIPTION_STRING "Symantec Runtime Virus Protect NLM"
!define INITIAL_THREAD_NAME "RTVScan"
!define SCREEN_NAME "Symantec RTVScan"

# Set Output directory
!ifdef %DEBUG
!    ifdef %LIC
!        define OUTDIR .\debuglic
!    elseifdef %RTVSCAN_SCALEABILITY_TEST
!        define OUTDIR .\debugScalability
!    else
!    define OUTDIR .\debug
!    endif
!else
!    ifdef %LIC
!        define OUTDIR .\releaselic
!    elseifdef %RTVSCAN_SCALEABILITY_TEST
!        define OUTDIR .\releaseScalability
!    else
!        define OUTDIR .\release
!    endif
!endif

!MESSAGE Outdir is $(OUTDIR)

# Set Tools
!define COMPILER_CW	 MWCCNLM.exe
!define LINKER_CW		 MWLDNLM.exe
!define METROWERKS_WIN	R:\cw83pdk5\win32-x86_Support\Headers

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
	-I..\..\..\..\..\Shareable_code\unlicensed\SymSafer\include &
	-I..\..\..\..\..\Release\SESA_Collector_Plugins\SCF\include

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
!define DEFINES -DNLM -DSYM_NLM -DN_PLAT_NLM -DSERVER -DWS_32 &
					 -D__push=push -D__pop=pop -DMemCheck &
					 -DNO_VSNPRINTF -D_MSL_NO_WCHART_C_SUPPORT -D_MSL_NO_MATH_LIB &
					 -D_MBCS -DEVENT_CUSTOM_INFO_CREATE

!ifdef %DEBUG
!    define DFLAGS -sym internal
!    ifdef %LIC
!        define DEFS $(DEFINES) -DENFORCE_LICENSING $(DBG_THREADS)
!    elseifdef %RTVSCAN_SCALEABILITY_TEST
!        define DEFS $(DEFINES) -DSCALABILITY_TESTING $(DBG_THREADS)
!    else
!        define DEFS $(DEFINES) $(DBG_THREADS)
!    endif
!    define CFLAGS
!else
!    ifdef %LIC
!        define DEFS $(DEFINES) -DNDEBUG -DENFORCE_LICENSING
!    elseifdef %RTVSCAN_SCALEABILITY_TEST
!        define DEFS $(DEFINES) -DNDEBUG -DSCALABILITY_TESTING
!    else
!        define DEFS $(DEFINES) -DNDEBUG
!    endif
!    define CFLAGS
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

!define LINK_FLAGS1 $(DFLAGS) -stacksize 512000 -map $(OUTDIR)\$(PROJECT_NAME).map &
						  -check UnloadCheck
!define LINK_FLAGS2 -nlmversion $(VERSION_NUMBER) -type generic -nostdlib &
						  -o $(OUTDIR)\$(PROJECT_NAME).nlm

!define LIB_DIRS &
	-L$(NOVELLNDK)\imports &
	-L$(METROWERKS_DIR)\libraries\runtime &
	-L..\tools\intel.lib &
	-L.\ &
	-L.\..\..\..\..\..\Release\Dec3\lib\netware\$(LIBDIR) &
	-L$(METROWERKS_DIR)\Librar~1\Runtime\Output\CLib &
	-L$(METROWERKS_DIR)\Librar~1\MSLC__~1\Output\CLib &
	-L$(METROWERKS_DIR)\Librar~1\MSLC__~1 &
	-L..\..\..\..\..\Release\SLIC\key\release\netware &
	-L..\..\..\..\..\Release\SLIC\lib\$(LIBDIR)\netware &
	-L.\..\..\..\..\..\Release\ams\lib\nw

!define CWLD $(COMPILER_DIR)\$(LINKER_CW) $(LINK_FLAGS1) $(LINK_FLAGS2) $(LIB_DIRS)

#
# Primary Dependency
#

all : $(OUTDIR)\$(NLMNAME).nlm .SYMBOLIC

#
# Compile everything

$(OUTDIR)\ScsCommsUtils.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\ScsCommsUtils.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\ScsCommsUtils.cpp

$(OUTDIR)\RegUtils.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RegUtils.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RegUtils.cpp

$(OUTDIR)\SIDUtils.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\SIDUtils.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\SIDUtils.cpp

$(OUTDIR)\vpstrutils.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vpstrutils.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\vpstrutils.cpp

$(OUTDIR)\RoleVector.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RoleVector.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RoleVector.cpp

$(OUTDIR)\SecureCommsEventInfo.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\SecureCommsEventInfo.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\SecureCommsEventInfo.cpp

$(OUTDIR)\EventCustomInfo.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\EventCustomInfo.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\EventCustomInfo.cpp

$(OUTDIR)\IssueCerts.obj : ..\..\..\Common\src\PScan\IssueCerts.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\IssueCerts.cpp

$(OUTDIR)\mynewop.obj : ..\..\..\Common\src\PScan\mynewop.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\mynewop.cpp

$(OUTDIR)\EventBlockWrapper.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\EventBlockWrapper.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\EventBlockWrapper.cpp

$(OUTDIR)\OSEventLogger.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\OSEventLogger.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\OSEventLogger.cpp

$(OUTDIR)\Checkin.obj : ..\..\..\Common\src\PScan\Checkin.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\Checkin.cpp

$(OUTDIR)\OpState.obj : ..\..\..\Common\src\PScan\OpState.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\OpState.cpp

$(OUTDIR)\OpStateMap.obj : ..\..\..\Common\src\PScan\OpStateMap.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\OpStateMap.cpp

$(OUTDIR)\ClientTrack.obj : ..\..\..\Common\src\PScan\ClientTrack.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ClientTrack.cpp

$(OUTDIR)\ClientTrackCheckin.obj : ..\..\..\Common\src\PScan\ClientTrackCheckin.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ClientTrackCheckin.cpp

$(OUTDIR)\ClientTrackData.obj : ..\..\..\Common\src\PScan\ClientTrackData.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ClientTrackData.cpp

$(OUTDIR)\ClientTrackQueue.obj : ..\..\..\common\src\pscan\ClientTrackQueue.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ClientTrackQueue.cpp

$(OUTDIR)\InitIIDS.obj : ..\..\..\Common\src\PScan\InitIIDS.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\PScan\InitIIDS.cpp

$(OUTDIR)\LogFile.obj : ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\LogFile.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\LogFile.cpp

$(OUTDIR)\Status.obj : ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\Status.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\Status.cpp

$(OUTDIR)\Directory.obj : ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\Directory.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Enterprise_Systems_SESACollectors\Collector_Framework_Plugins\Common\Connector\Directory.cpp

$(OUTDIR)\FrwdThrd.obj : ..\..\..\Common\src\PScan\FrwdThrd.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\FrwdThrd.cpp

$(OUTDIR)\FrwdLogs.obj : ..\..\..\Common\src\PScan\FrwdLogs.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\FrwdLogs.cpp

$(OUTDIR)\FrwdLogFile.obj : ..\..\..\Common\src\PScan\FrwdLogFile.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\FrwdLogFile.cpp

$(OUTDIR)\FrwdStatus.obj : ..\..\..\Common\src\PScan\FrwdStatus.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\FrwdStatus.cpp

$(OUTDIR)\CVirusEventSinkNoCC.obj : ..\..\..\Common\src\PScan\CVirusEventSinkNoCC.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\CVirusEventSinkNoCC.cpp

$(OUTDIR)\DecomposersdkNoCC.obj : ..\..\..\Common\src\PScan\DecomposersdkNoCC.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\DecomposersdkNoCC.cpp

$(OUTDIR)\md5c.obj : ..\..\..\..\shared\md5\md5util\md5c.c .AUTODEPEND
 *$(CWCC) ..\..\..\..\shared\md5\md5util\md5c.c

$(OUTDIR)\des.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\des.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\des.cpp

$(OUTDIR)\password.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\common\misc\password.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\common\misc\password.cpp

$(OUTDIR)\CSV.obj : ..\..\..\common\src\pscan\csv.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\csv.cpp

$(OUTDIR)\DataSet.obj : ..\..\..\common\src\pscan\DataSet.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\DataSet.cpp

$(OUTDIR)\ThreatCatSettings.obj : ..\..\..\common\src\pscan\ThreatCatSettings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ThreatCatSettings.cpp

$(OUTDIR)\LegacyInfectionHandler.obj : ..\..\..\common\src\pscan\LegacyInfectionHandler.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\LegacyInfectionHandler.cpp

$(OUTDIR)\action.obj : ..\..\..\common\src\pscan\action.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\action.cpp

$(OUTDIR)\rcopy.obj : ..\..\..\common\src\pscan\rcopy.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\rcopy.cpp

$(OUTDIR)\AccessChecker.obj : ..\..\..\common\src\pscan\AccessChecker.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\AccessChecker.cpp

$(OUTDIR)\afteract.obj : ..\..\..\common\src\pscan\afteract.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\afteract.cpp

$(OUTDIR)\altpass.obj : ..\..\..\common\src\pscan\altpass.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\altpass.cpp

$(OUTDIR)\ams.obj : ..\..\..\common\src\pscan\ams.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ams.cpp

$(OUTDIR)\BackrevSpamProcessor.obj : ..\..\..\common\src\pscan\BackrevSpamProcessor.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\BackrevSpamProcessor.cpp

$(OUTDIR)\callback.obj : ..\..\..\common\src\pscan\callback.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\callback.cpp

$(OUTDIR)\checksum.obj : ..\..\..\common\src\pscan\checksum.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\checksum.cpp

$(OUTDIR)\client.obj : ..\..\..\common\src\pscan\client.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\client.cpp

$(OUTDIR)\conn.obj : ..\..\..\common\src\pscan\conn.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\conn.cpp

$(OUTDIR)\decomp.obj : ..\..\..\common\src\pscan\decomp.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\decomp.cpp

$(OUTDIR)\delha.obj : ..\..\..\common\src\pscan\delha.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\delha.cpp

$(OUTDIR)\discover.obj : ..\..\..\common\src\pscan\netware\discover.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\netware\discover.cpp

$(OUTDIR)\filestor.obj : ..\..\..\common\src\pscan\filestor.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\filestor.cpp

$(OUTDIR)\findnode.obj : ..\..\..\common\src\pscan\findnode.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\findnode.cpp

$(OUTDIR)\ftp.obj : ..\..\..\common\src\pscan\ftp.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ftp.cpp

$(OUTDIR)\grc.obj : ..\..\..\common\src\pscan\grc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\grc.cpp

$(OUTDIR)\hsm.obj : ..\..\..\common\src\pscan\hsm.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\hsm.cpp

$(OUTDIR)\iface.obj : ..\..\..\common\src\pscan\iface.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\iface.cpp

$(OUTDIR)\is.obj : ..\..\..\common\src\pscan\is.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\is.cpp

$(OUTDIR)\license.obj : ..\..\..\common\src\pscan\license.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\license.cpp

$(OUTDIR)\licensealloc.obj : ..\..\..\common\src\pscan\licensealloc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\licensealloc.cpp

$(OUTDIR)\licensehelper.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\licensehelper.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\licensehelper.cpp

$(OUTDIR)\log.obj : ..\..\..\common\src\pscan\log.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\log.cpp

$(OUTDIR)\misc.obj : ..\..\..\common\src\pscan\misc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\misc.cpp

$(OUTDIR)\ForwardEvents.obj : ..\..\..\common\src\pscan\ForwardEvents.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ForwardEvents.cpp

$(OUTDIR)\mngdprod.obj : ..\..\..\common\src\pscan\mngdprod.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\mngdprod.cpp

$(OUTDIR)\nlm.obj : ..\..\..\netware\src\common\nlm.cpp .AUTODEPEND
 *$(CWCPP) -DDO_NOT_INCLUDE_FINDFIRSTFILE ..\..\..\netware\src\common\nlm.cpp

$(OUTDIR)\winStrings.obj : ..\..\..\NetWare\src\common\winStrings.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winStrings.cpp

$(OUTDIR)\winThreads.obj : ..\..\..\NetWare\src\common\winThreads.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\NetWare\src\common\winThreads.cpp

$(OUTDIR)\pattman.obj : ..\..\..\common\src\pscan\pattman.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\pattman.cpp

$(OUTDIR)\process.obj : ..\..\..\common\src\pscan\process.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\process.cpp

$(OUTDIR)\ThreadBase.obj : ..\..\..\Common\src\common\ThreadBase.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\common\ThreadBase.cpp

$(OUTDIR)\ThreadStaticSize.obj : ..\..\..\Common\src\common\ThreadStaticSize.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\common\ThreadStaticSize.cpp

$(OUTDIR)\pscan.obj : ..\..\..\common\src\pscan\pscan.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\pscan.cpp

$(OUTDIR)\pushmult.obj : ..\..\..\common\src\pscan\pushmult.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\pushmult.cpp

$(OUTDIR)\snode.obj : ..\..\..\common\src\pscan\snode.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\snode.cpp

$(OUTDIR)\SortedVBinEnumerator.obj : ..\..\..\common\src\pscan\SortedVBinEnumerator.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\SortedVBinEnumerator.cpp

$(OUTDIR)\storage.obj : ..\..\..\common\src\pscan\storage.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\storage.cpp

$(OUTDIR)\readwrit.obj : ..\..\..\common\src\pscan\readwrit.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\readwrit.cpp

$(OUTDIR)\ScanStatus.obj : ..\..\..\common\src\pscan\ScanStatus.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\ScanStatus.cpp

$(OUTDIR)\trnsport.obj : ..\..\..\common\src\pscan\trnsport.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\trnsport.cpp

$(OUTDIR)\uimisc.obj : ..\..\..\common\src\pscan\uimisc.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\uimisc.cpp

$(OUTDIR)\UserAccounts.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\UserAccounts.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\UserAccounts.cpp

$(OUTDIR)\valert.obj : ..\..\..\common\src\pscan\valert.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\valert.cpp

$(OUTDIR)\vbin.obj : ..\..\..\common\src\pscan\vbin.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\vbin.cpp

$(OUTDIR)\vbinmon.obj : ..\..\..\common\src\pscan\vbinmon.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\vbinmon.cpp

$(OUTDIR)\walker.obj : ..\..\..\common\src\pscan\walker.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\walker.cpp

$(OUTDIR)\filetime.obj : ..\..\..\common\src\pscan\filetime.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\filetime.cpp

$(OUTDIR)\nlmmain.obj : nlmmain.cpp .AUTODEPEND
 *$(CWCPP) nlmmain.cpp

$(OUTDIR)\nlmmem.obj : nlmmem.cpp .AUTODEPEND
 *$(CWCPP) nlmmem.cpp

$(OUTDIR)\nui.obj : nui.cpp .AUTODEPEND
 *$(CWCPP) nui.cpp

$(OUTDIR)\pscan_n.obj : pscan_n.cpp .AUTODEPEND
 *$(CWCPP) pscan_n.cpp

$(OUTDIR)\scrnsave.obj : scrnsave.cpp .AUTODEPEND
 *$(CWCPP) scrnsave.cpp

$(OUTDIR)\getfinalaction.obj : ..\..\..\common\src\pscan\getfinalaction.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\common\src\pscan\getfinalaction.cpp

$(OUTDIR)\NetwareSymSaferRegistry.obj : ..\common\NetwareSymSaferRegistry.cpp .AUTODEPEND
 *$(CWCPP) ..\common\NetwareSymSaferRegistry.cpp

#
# Perform the link
#
!define LIB_FILES &
	-lmwcrtl$(DEBUG_LIBS).lib -lmwcpp$(DEBUG_LIBS).lib &
	-lclib.imp -lthreads.imp -lnlmlib.imp -llocnlm32.imp &
	-lcba.lib -lnts.lib -lctypes -lsymtnef -lgrenleaf -lunarj -llib95 &
	-lsymamg32 -lsymgzip -lsymlha -lsymoless -lsymrar -lsymzlib &
	-lrec2lib -lrec2 &
	-ldecsdk -ldec2id -ldec2amg -ldec2arj -ldec2gzip -ldec2lha -ldec2ole1 -ldec2rar &
	-ldec2rtf -ldec2ss -ldec2tar -ldec2tnef -ldec2zip -lsymlz -ldec2as -ldec2lz &
	-ldec2mb3 -ldec2 -ldec2lib -ldec2text &
	-lkeymgr -llogmgr -lslic.lib -lbsafe_alloc.obj -lbsafe52.lib -lslic_prodkey.obj

$(OUTDIR)\$(NLMNAME).nlm : &
$(OUTDIR)\md5c.obj &
$(OUTDIR)\RoleVector.obj &
$(OUTDIR)\IssueCerts.obj &
$(OUTDIR)\ScsCommsUtils.obj &
$(OUTDIR)\RegUtils.obj &
$(OUTDIR)\SIDUtils.obj &
$(OUTDIR)\vpstrutils.obj &
$(OUTDIR)\process.obj &
$(OUTDIR)\OpState.obj &
$(OUTDIR)\OpStateMap.obj &
$(OUTDIR)\ClientTrack.obj &
$(OUTDIR)\ClientTrackCheckin.obj &
$(OUTDIR)\ClientTrackData.obj &
$(OUTDIR)\ClientTrackQueue.obj &
$(OUTDIR)\Checkin.obj &
$(OUTDIR)\EventBlockWrapper.obj &
$(OUTDIR)\EventCustomInfo.obj &
$(OUTDIR)\InitIIDS.obj &
$(OUTDIR)\OSEventLogger.obj &
$(OUTDIR)\SecureCommsEventInfo.obj &
$(OUTDIR)\des.obj &
$(OUTDIR)\mynewop.obj &
$(OUTDIR)\winStrings.obj &
$(OUTDIR)\winThreads.obj &
$(OUTDIR)\password.obj &
$(OUTDIR)\ScanStatus.obj &
$(OUTDIR)\LegacyInfectionHandler.obj &
$(OUTDIR)\action.obj &
$(OUTDIR)\altpass.obj  &
$(OUTDIR)\callback.obj &
$(OUTDIR)\checksum.obj &
$(OUTDIR)\client.obj &
$(OUTDIR)\conn.obj &
$(OUTDIR)\decomp.obj &
$(OUTDIR)\delha.obj &
$(OUTDIR)\discover.obj &
$(OUTDIR)\filestor.obj &
$(OUTDIR)\findnode.obj &
$(OUTDIR)\ftp.obj &
$(OUTDIR)\grc.obj &
$(OUTDIR)\hsm.obj &
$(OUTDIR)\iface.obj &
$(OUTDIR)\is.obj &
$(OUTDIR)\license.obj &
$(OUTDIR)\licensealloc.obj &
$(OUTDIR)\licensehelper.obj &
$(OUTDIR)\log.obj &
$(OUTDIR)\misc.obj &
$(OUTDIR)\ForwardEvents.obj &
$(OUTDIR)\mngdprod.obj &
$(OUTDIR)\nlm.obj &
$(OUTDIR)\pattman.obj &
$(OUTDIR)\ThreadBase.obj &
$(OUTDIR)\ThreadStaticSize.obj &
$(OUTDIR)\pscan.obj &
$(OUTDIR)\pushmult.obj &
$(OUTDIR)\readwrit.obj &
$(OUTDIR)\snode.obj &
$(OUTDIR)\SortedVBinEnumerator.obj &
$(OUTDIR)\storage.obj &
$(OUTDIR)\trnsport.obj &
$(OUTDIR)\uimisc.obj &
$(OUTDIR)\UserAccounts.obj &
$(OUTDIR)\valert.obj &
$(OUTDIR)\vbin.obj &
$(OUTDIR)\vbinmon.obj &
$(OUTDIR)\walker.obj &
$(OUTDIR)\filetime.obj &
$(OUTDIR)\nlmmem.obj &
$(OUTDIR)\nlmmain.obj &
$(OUTDIR)\nui.obj &
$(OUTDIR)\pscan_n.obj &
$(OUTDIR)\CSV.obj &
$(OUTDIR)\DataSet.obj &
$(OUTDIR)\ThreatCatSettings.obj &
$(OUTDIR)\scrnsave.obj &
$(OUTDIR)\FrwdThrd.obj &
$(OUTDIR)\FrwdLogs.obj &
$(OUTDIR)\FrwdLogFile.obj &
$(OUTDIR)\FrwdStatus.obj &
$(OUTDIR)\CVirusEventSinkNoCC.obj &
$(OUTDIR)\afteract.obj &
$(OUTDIR)\rcopy.obj &
$(OUTDIR)\DecomposersdkNoCC.obj &
$(OUTDIR)\AccessChecker.obj &
$(OUTDIR)\ams.obj &
$(OUTDIR)\LogFile.obj &
$(OUTDIR)\Status.obj &
$(OUTDIR)\Directory.obj &
$(OUTDIR)\getfinalaction.obj &
$(OUTDIR)\NetwareSymSaferRegistry.obj &
$(OUTDIR)\BackrevSpamProcessor.obj &
.AUTODEPEND

 @%write	 $(NLMNAME).def COPYRIGHT $(COPYRIGHT_STRING)
 @%append $(NLMNAME).def DESCRIPTION $(DESCRIPTION_STRING)
 @%append $(NLMNAME).def SCREENNAME $(SCREEN_NAME)
 @%append $(NLMNAME).def THREADNAME $(INITIAL_THREAD_NAME)
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def module clib,nwsnut,calnlm32,clxnlm32,vpreg,netdb
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def export scanThreadIDArray
 @%append $(NLMNAME).def export dec_bScanExclude
 @%append $(NLMNAME).def import @clib.imp
 @%append $(NLMNAME).def import @..\include\nwsnut.imp
 @%append $(NLMNAME).def import @socklib.imp
 @%append $(NLMNAME).def import @calnlm32.imp
 @%append $(NLMNAME).def import @clxnlm32.imp
 @%append $(NLMNAME).def import @..\..\..\common\src\pscan\registry.exp
 @%append $(NLMNAME).def import @imps.imp
 @%append $(NLMNAME).def import CreateDecThreadArray
 @%append $(NLMNAME).def

 *$(CWLD) $(LIB_FILES) -commandfile $(NLMNAME).def $(OUTDIR)\\*.obj &
			 $(NOVELLNDK)\\imports\\clibpre.o
