#
#	  ScsComms	makefile	  using CodeWarrior
#

# Set Project
!define PROJECT_NAME ScsComms
!define NLMNAME ScsComms
!define VERSION_NUMBER "1,00,0"
!define COPYRIGHT_STRING "Copyright 1999 - 2005	 Symantec Corporation. All rights reserved."
!define DESCRIPTION_STRING "Symantec ScsComms NLM"
!define INITIAL_THREAD_NAME "ScsComms_Main"
!define SCREEN_NAME "none"

# Set Output directory
!ifdef %DEBUG
!define OUTDIR .\debug
!else
!define OUTDIR .\release
!endif

# Set Tools
!define COMPILER_CW MWCCNLM.exe
!define LINKER_CW	  MWLDNLM.exe

# Set include directories
!define CORE_TECH ..\..\..\..\..\Core_Technology\LDVP_Shared\Src

!define PROJECT_INCS &
	-I.\ &
	-I..\include &
	-I..\..\..\Common\src\include &
	-I..\..\..\Common\src\ScsComms &
	-I$(CORE_TECH)\include\SecureComms &
	-I$(CORE_TECH)\include &
	-I..\..\..\..\..\Shareable_code\licensed\SSL-C\netware\library\include &
	-I..\..\..\..\..\Release\AMS\include &
	-I..\..\..\..\..\tools\Crypto-C\6.1.1\netware\include &
	-I..\..\..\..\..\tools\Crypto-C\6.1.1\Cert-C\netware\include &
	-I..\..\..\Common\src\Utils\sym_synch\include &
	-I..\..\..\..\..\Norton_Internet_Security\src\CC_SDK\INCLUDE &
	-I$(CORE_TECH)\Common\Misc &
	-I..\..\..\Common\src\PScan &
	-I..\..\..\..\..\Shareable_code\unlicensed\DebugNET\include &
	-I..\..\..\..\..\Shareable_code\unlicensed\SymSafer\include

!define SYS_INCS &
	-I- &
	-I$(NETWARE_NDK_DIR)\include &
	-I$(NETWARE_NDK_DIR)\include\arpa &
	-I$(NETWARE_NDK_DIR)\include\winsock &
	-I$(METROWERKS_DIR)\Headers &
	-I$(METROWERKS_DIR)\Libraries\MSLC__~1\Include &
	-I$(NOVELLNDK)\Include &
	-I$(NOVELLNDK)\Include\NLM

#
# Set Compiler Flags
#
!define DEFINES -DNLM -DSYM_NLM -DSYM_LIBC_NLM -DN_PLAT_NLM -DSCSCOMMS -DSERVER -DWS_32 &
					 -D__push=push -D__pop=pop -DMemCheck &
					 -DNO_VSNPRINTF -D_MSL_NO_WCHART_C_SUPPORT -D_MSL_NO_MATH_LIB &
					 -DNO_HDW -DPOSIX -D_MBCS

!ifdef %DEBUG
!define DFLAGS -sym internal
!define DEFS $(DEFINES) $(DBG_THREADS)
!define CFLAGS
!else
!define DEFS $(DEFINES) -DNDEBUG
!define CFLAGS
!endif

!define COMPILER_OPTS -c -nodefaults -relax_pointers -ext obj -cwd include &
							 -o $(OUTDIR) -opt level=2 -bool on -maxerrors 5 &
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

!define LINK_FLAGS1 $(DFLAGS) -entry _LibCPrelude -exit _LibCPostlude -stacksize 34000 &
						  -map $(OUTDIR)\$(PROJECT_NAME).map
!define LINK_FLAGS2 -nlmversion $(VERSION_NUMBER) -type generic -nostdlib &
						  -o $(OUTDIR)\$(PROJECT_NAME).nlm

!define LIB_DIRS &
	-L$(NETWARE_NDK_DIR)\imports &
	-L$(METROWERKS_DIR)\libraries\MSLC__~1\output\libc &
	-L$(METROWERKS_DIR)\libraries\MSLC__~1 &
	-L$(METROWERKS_DIR)\libraries\runtime &
	-L$(METROWERKS_DIR)\libraries\runtime\output\libc &
	-L..\..\..\..\..\Release\AMS\lib\NW &
	-L..\..\..\..\..\Shareable_code\licensed\SSL-C\netware\library\lib &
	-L..\..\..\..\..\tools\Crypto-C\6.1.1\Cert-C\netware\lib\netware\release &
	-L..\..\..\..\..\tools\Crypto-C\6.1.1\netware\lib\netware

!define CWLD $(COMPILER_DIR)\$(LINKER_CW) $(LINK_FLAGS1) $(LINK_FLAGS2) $(LIB_DIRS)

#
# Primary Dependency
#

all : $(OUTDIR)\$(NLMNAME).nlm .SYMBOLIC

#
# Compile everything
#compilation order:
#local
#core_tech
#pscan
#scscomms

$(OUTDIR)\main.obj : main.cpp .AUTODEPEND
 *$(CWCPP) main.cpp

$(OUTDIR)\stdafx.obj : stdafx.cpp .AUTODEPEND
 *$(CWCPP) stdafx.cpp

$(OUTDIR)\clib.obj : clib.cpp .AUTODEPEND
 *$(CWCPP) clib.cpp

$(OUTDIR)\winStrings.obj : ..\common\winStrings.cpp .AUTODEPEND
 *$(CWCPP) ..\common\winStrings.cpp

$(OUTDIR)\winThreads.obj : ..\common\winThreads.cpp .AUTODEPEND
 *$(CWCPP) ..\common\winThreads.cpp

$(OUTDIR)\Des.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\Des.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\Des.cpp

$(OUTDIR)\Password.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\Password.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\Password.cpp

$(OUTDIR)\cript.obj : ..\..\..\Common\src\PScan\cript.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\PScan\cript.cpp

$(OUTDIR)\readwrit.obj : ..\..\..\Common\src\PScan\readwrit.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\PScan\readwrit.cpp

$(OUTDIR)\auth.obj : ..\..\..\Common\src\ScsComms\auth.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\auth.cpp

$(OUTDIR)\authlogin.obj : ..\..\..\Common\src\ScsComms\authlogin.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\authlogin.cpp

$(OUTDIR)\authremote.obj : ..\..\..\Common\src\ScsComms\authremote.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\authremote.cpp

$(OUTDIR)\authserver.obj : ..\..\..\Common\src\ScsComms\authserver.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\authserver.cpp

$(OUTDIR)\base64.obj : ..\..\..\Common\src\ScsComms\base64.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\base64.cpp

$(OUTDIR)\CertIssuer.obj : ..\..\..\Common\src\ScsComms\CertIssuer.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\CertIssuer.cpp

$(OUTDIR)\certobj.obj : ..\..\..\Common\src\ScsComms\certobj.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\certobj.cpp

$(OUTDIR)\CertSigningRequest.obj : ..\..\..\Common\src\ScsComms\CertSigningRequest.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\CertSigningRequest.cpp

$(OUTDIR)\Config.obj : ..\..\..\Common\src\ScsComms\Config.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\Config.cpp

$(OUTDIR)\ConnectionContext.obj : ..\..\..\Common\src\ScsComms\ConnectionContext.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\ConnectionContext.cpp

$(OUTDIR)\ConnectionMap.obj : ..\..\..\Common\src\ScsComms\ConnectionMap.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\ConnectionMap.cpp

$(OUTDIR)\CredentialStoreDelegator.obj : ..\..\..\Common\src\ScsComms\CredentialStoreDelegator.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\CredentialStoreDelegator.cpp

$(OUTDIR)\CredentialStoreImpl.obj : ..\..\..\Common\src\ScsComms\CredentialStoreImpl.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\CredentialStoreImpl.cpp

$(OUTDIR)\DebugPrint.obj : ..\..\..\Common\src\ScsComms\DebugPrint.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\DebugPrint.cpp

$(OUTDIR)\FileOpsImpl.obj : ..\..\..\Common\src\ScsComms\FileOpsImpl.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\FileOpsImpl.cpp

$(OUTDIR)\HandshakeMessage.obj : ..\..\..\Common\src\ScsComms\HandshakeMessage.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\HandshakeMessage.cpp

$(OUTDIR)\Hash.obj : ..\..\..\Common\src\ScsComms\Hash.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\Hash.cpp

$(OUTDIR)\Key.obj : ..\..\..\Common\src\ScsComms\Key.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\Key.cpp

$(OUTDIR)\legacytransport.obj : ..\..\..\Common\src\ScsComms\legacytransport.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\legacytransport.cpp

$(OUTDIR)\ListenThread.obj : ..\..\..\Common\src\ScsComms\ListenThread.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\ListenThread.cpp

$(OUTDIR)\MessageBuffer.obj : ..\..\..\Common\src\ScsComms\MessageBuffer.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\MessageBuffer.cpp

$(OUTDIR)\MessageBufferPool.obj : ..\..\..\Common\src\ScsComms\MessageBufferPool.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\MessageBufferPool.cpp

$(OUTDIR)\MessageManager.obj : ..\..\..\Common\src\ScsComms\MessageManager.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\MessageManager.cpp

$(OUTDIR)\MessageSignal.obj : ..\..\..\Common\src\ScsComms\MessageSignal.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\MessageSignal.cpp

$(OUTDIR)\MonitorThread.obj : ..\..\..\Common\src\ScsComms\MonitorThread.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\MonitorThread.cpp

$(OUTDIR)\Random.obj : ..\..\..\Common\src\ScsComms\Random.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\Random.cpp

$(OUTDIR)\RolePermissions.obj : ..\..\..\Common\src\ScsComms\RolePermissions.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\RolePermissions.cpp

$(OUTDIR)\SelectThread.obj : ..\..\..\Common\src\ScsComms\SelectThread.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SelectThread.cpp

$(OUTDIR)\SelectThreadPool.obj : ..\..\..\Common\src\ScsComms\SelectThreadPool.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SelectThreadPool.cpp

$(OUTDIR)\SockAddrOps.obj : ..\..\..\Common\src\ScsComms\SockAddrOps.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SockAddrOps.cpp

$(OUTDIR)\SocketIOThread.obj : ..\..\..\Common\src\ScsComms\SocketIOThread.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SocketIOThread.cpp

$(OUTDIR)\SocketIOThreadPool.obj : ..\..\..\Common\src\ScsComms\SocketIOThreadPool.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SocketIOThreadPool.cpp

$(OUTDIR)\SslThread.obj : ..\..\..\Common\src\ScsComms\SslThread.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\SslThread.cpp

$(OUTDIR)\ThreadBase.obj : ..\..\..\Common\src\common\ThreadBase.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\common\ThreadBase.cpp

$(OUTDIR)\ThreadStaticSize.obj : ..\..\..\Common\src\common\ThreadStaticSize.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\common\ThreadStaticSize.cpp

$(OUTDIR)\user.obj : ..\..\..\Common\src\ScsComms\user.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\Common\src\ScsComms\user.cpp

$(OUTDIR)\RoleVector.obj : ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RoleVector.cpp .AUTODEPEND
 *$(CWCPP) ..\..\..\..\..\Core_Technology\LDVP_Shared\src\Common\Misc\RoleVector.cpp

$(OUTDIR)\NetwareSymSaferRegistry.obj : ..\common\NetwareSymSaferRegistry.cpp .AUTODEPEND
 *$(CWCPP) ..\common\NetwareSymSaferRegistry.cpp

#
# Perform the link
#
!define LIB_FILES &
	-lmwcrtl$(DEBUG_LIBS).lib -lmwcpp$(DEBUG_LIBS).lib &
	-lsslc.lib -lcryptoc.lib -lcertc.lib -lcertcsp.lib

#compilation order:
#local
#core_tech
#pscan
#scscomms

$(OUTDIR)\$(NLMNAME).nlm : &
$(OUTDIR)\main.obj &
$(OUTDIR)\stdafx.obj &
$(OUTDIR)\clib.obj &
$(OUTDIR)\winStrings.obj &
$(OUTDIR)\winThreads.obj &
$(OUTDIR)\RoleVector.obj &
$(OUTDIR)\Des.obj &
$(OUTDIR)\Password.obj &
$(OUTDIR)\cript.obj &
$(OUTDIR)\readwrit.obj &
$(OUTDIR)\auth.obj &
$(OUTDIR)\authlogin.obj &
$(OUTDIR)\authremote.obj &
$(OUTDIR)\authserver.obj &
$(OUTDIR)\base64.obj &
$(OUTDIR)\CertIssuer.obj &
$(OUTDIR)\certobj.obj &
$(OUTDIR)\CertSigningRequest.obj &
$(OUTDIR)\Config.obj &
$(OUTDIR)\ConnectionContext.obj &
$(OUTDIR)\ConnectionMap.obj &
$(OUTDIR)\CredentialStoreDelegator.obj &
$(OUTDIR)\CredentialStoreImpl.obj &
$(OUTDIR)\DebugPrint.obj &
$(OUTDIR)\FileOpsImpl.obj &
$(OUTDIR)\HandshakeMessage.obj &
$(OUTDIR)\Hash.obj &
$(OUTDIR)\Key.obj &
$(OUTDIR)\legacytransport.obj &
$(OUTDIR)\ListenThread.obj &
$(OUTDIR)\MessageBuffer.obj &
$(OUTDIR)\MessageBufferPool.obj &
$(OUTDIR)\MessageManager.obj &
$(OUTDIR)\MessageSignal.obj &
$(OUTDIR)\MonitorThread.obj &
$(OUTDIR)\Random.obj &
$(OUTDIR)\RolePermissions.obj &
$(OUTDIR)\SelectThread.obj &
$(OUTDIR)\SelectThreadPool.obj &
$(OUTDIR)\SockAddrOps.obj &
$(OUTDIR)\SocketIOThread.obj &
$(OUTDIR)\SocketIOThreadPool.obj &
$(OUTDIR)\SslThread.obj &
$(OUTDIR)\ThreadBase.obj &
$(OUTDIR)\ThreadStaticSize.obj &
$(OUTDIR)\user.obj &
$(OUTDIR)\NetwareSymSaferRegistry.obj &
.AUTODEPEND

 @%write	 $(NLMNAME).def COPYRIGHT $(COPYRIGHT_STRING)
 @%append $(NLMNAME).def DESCRIPTION $(DESCRIPTION_STRING)
 @%append $(NLMNAME).def SCREENNAME $(SCREEN_NAME)
 @%append $(NLMNAME).def THREADNAME $(INITIAL_THREAD_NAME)
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def module libc.nlm,vpreg
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def import @libc.imp
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def import @ws2nlm.imp
 @%append $(NLMNAME).def import @vpreg.imp
 @%append $(NLMNAME).def import @imps.imp
 @%append $(NLMNAME).def
 @%append $(NLMNAME).def export @ScsComms.exp
 @%append $(NLMNAME).def

  *$(CWLD) $(LIB_FILES) -commandfile $(NLMNAME).def $(OUTDIR)\\*.obj &
			  $(NETWARE_NDK_DIR)\imports\libcpre.o
