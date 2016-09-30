# Set include paths 
  
!define NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH \Norton_Antivirus\QuarantineServer
!define NOVELLSDK C:\Novell\NDK\nwsdk
!define COMPILER_DIR C:\Progra~1\Metrowerks\CodeWarrior
!define PROJECT_DIR \Norton_Antivirus\Navnlm\src\QSFwd

!define SYS_INCLUDES -I- -I$(NOVELLSDK)\include\ -I$(NOVELLSDK)\include\nlm -I$(NOVELLSDK)\include\nlm\nit -I$(NOVELLSDK)\include\nlm\sys -I$(COMPILER_DIR)\Win32-~1\Headers\Win32S~1

# Set compiler and linker options

!ifdef %DEBUG
!define OUTDIR .\Debug
!define DEBUG_FLAGS -g -align 1
!define LIB_PATHS -L$(PROJECT_DIR) -Lc:\PROGRA~1\Metrow~1\CodeWa~1\Novell~1\Metrow~1\Librar~1\Runtime\
!define QSF_LIBS -lQsFwd.exp -lmwcrtld.lib -lmwcppd.lib -lclib.imp -lthreads.imp -lnlmlib.imp -ltli.imp -lnit.imp -lrequestr.imp -lQsFwd.exp -lws2nlm.imp
!else
!define OUTDIR .\Release
!define LIB_PATHS -L$(PROJECT_DIR) -Lc:\PROGRA~1\Metrow~1\CodeWa~1\Novell~1\Metrow~1\Librar~1\Runtime\
!define QSF_LIBS -lQsFwd.exp -lmwcrtl.lib -lmwcpp.lib -lclib.imp -lthreads.imp -lnlmlib.imp -ltli.imp -lnit.imp -lrequestr.imp -lQsFwd.exp -lws2nlm.imp
!endif

!define COMPILER_OPTS -nolink -bool on -O2 -prefix QSFwd.pch $(DEBUG_FLAGS)

!define QSFWD_VERSION_NUMBER "7,50,8"
!define COPYRIGHT_STRING "Copyright (C) 1999-2000 Symantec Corporation"
!define DESCRIPTION_STRING "Quarantine Forwarding NLM"
!define INITIAL_THREAD_NAME "QsFwd_Main"
!define PROJNAME QSFwd

!define INCLUDE_PATHS -I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\Shared\include -I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\Include -I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsInfo -I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32 -I$(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsComm32 -I$(PROJECT_DIR) -I$(COMPILER_DIR)\Win32-~1\Headers\Win32S~1
!define LINK_FLAGS -stacksize 8192 -threadname $(INITIAL_THREAD_NAME) -osym $(PROJNAME).sym -map -sym codeview4 -sym internal -copy $(COPYRIGHT_STRING) -desc $(DESCRIPTION_STRING) -nlmversion $(QSFWD_VERSION_NUMBER) -type generic -stdlib  -o $(OUTDIR)\$(PROJNAME).nlm


all : $(OUTDIR)\QSFwd.nlm .SYMBOLIC

$(OUTDIR)\QSFwd.o : .\QSFwd.cpp .AUTODEPEND
 if not exist $(OUTDIR) md $(OUTDIR)
 *mwccnlm.exe $(INCLUDE_PATHS) $(SYS_INCLUDES) $(COMPILER_OPTS) .\QSFwd.cpp -o $(OUTDIR)\

$(OUTDIR)\Package.o : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32\Package.cpp .AUTODEPEND
 if not exist $(OUTDIR) md $(OUTDIR)
 *mwccnlm.exe $(PLAT_DEFINES) $(INCLUDE_PATHS) $(SYS_INCLUDES) $(COMPILER_OPTS) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsPak32\Package.cpp -o $(OUTDIR)\

$(OUTDIR)\QsInfo.o : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsInfo\QsInfo.cpp .AUTODEPEND
 if not exist $(OUTDIR) md $(OUTDIR)
 *mwccnlm.exe $(PLAT_DEFINES) $(INCLUDE_PATHS) $(SYS_INCLUDES) $(COMPILER_OPTS) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsInfo\QsInfo.cpp -o $(OUTDIR)\

$(OUTDIR)\Tli.o : $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsComm32\Tli.cpp .AUTODEPEND
 if not exist $(OUTDIR) md $(OUTDIR)
 *mwccnlm.exe $(PLAT_DEFINES) $(INCLUDE_PATHS) $(SYS_INCLUDES) $(COMPILER_OPTS) $(NORTON_ANTIVIRUS_QUARANTINE_SERVER_PATH)\QsComm32\Tli.cpp -o $(OUTDIR)\

$(OUTDIR)\QSFwd.nlm : $(OUTDIR)\QSFwd.o $(OUTDIR)\Package.o $(OUTDIR)\QsInfo.o $(OUTDIR)\Tli.o  .AUTODEPEND
 *mwldnlm.exe $(LINK_FLAGS) $(LIB_PATHS) $(QSF_LIBS) $(OUTDIR)\*.o
 #

