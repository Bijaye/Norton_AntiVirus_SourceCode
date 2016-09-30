# Microsoft Developer Studio Project File - Name="bochsw" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=bochsw - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bochsw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bochsw.mak" CFG="bochsw - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bochsw - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "bochsw - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Avis100/Vides/bochsw", POPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bochsw - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Op /D "NDEBUG" /D "WATCH_ENABLED" /D "WIN32" /D "_CONSOLE" /D "BOCHS" /D "NWIN32DEBUG" /D "WIN32GUI" /D "MYSTUFF" /D "GNUREGEX" /D "HAVE_STRING_H" /D "USEWIN32VIRTUALMEM" /D "MEMMAPDISKIMAGES" /D "FOR_BOO" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:".\factory.exe"
# Begin Special Build Tool
TargetPath=.\factory.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy $(TargetPath) D:\is\progs\stage\bin	xcopy $(TargetPath)  D:\is\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "bochsw - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /I "D:\dev\progs\breed!\asax" /I "D:\dev\progs\breed!\Bochs" /I "D:\dev\progs\breed!\bochsw" /D "_DEBUG" /D "WIN32DEBUG3" /D "WATCH_ENABLED" /D "WIN32" /D "_CONSOLE" /D "BOCHS" /D "NWIN32DEBUG" /D "WIN32GUI" /D "MYSTUFF" /D "GNUREGEX" /D "HAVE_STRING_H" /D "USEWIN32VIRTUALMEM" /D "MEMMAPDISKIMAGES" /D "FOR_BOO" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "c:\Emulator Source\hasax" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:".\factory.exe"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "bochsw - Win32 Release"
# Name "bochsw - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\hasax\action.c
# End Source File
# Begin Source File

SOURCE=..\hasax\arglist.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\arith.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\arith2.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\asax.c
# End Source File
# Begin Source File

SOURCE=..\hasax\asaxio.c
# End Source File
# Begin Source File

SOURCE=..\hasax\asaxstack.c
# End Source File
# Begin Source File

SOURCE=..\hasax\asaxstring.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\bcd.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\bios.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\bit.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\bochs.rc"

!IF  "$(CFG)" == "bochsw - Win32 Release"

# ADD BASE RSC /l 0x409 /i "\avis200.081\src\avis100\Vides\bochs-96" /i "\src\Avis100\Vides\bochs-96" /i "\dev\progs\breed!\bochs-96" /i "\breed!\bochs-96" /i "\Emulator Source\bochs-96" /i "\dev\progs\bochs\bochs-96"
# ADD RSC /l 0x409 /i "\avis200.081\src\avis100\Vides\bochs-96" /i "\src\Avis100\Vides\bochs-96" /i "\dev\progs\breed!\bochs-96" /i "\breed!\bochs-96" /i "\Emulator Source\bochs-96" /i "\dev\progs\bochs\bochs-96" /i "D:\Dev\progs\bochs\bochs-96"

!ELSEIF  "$(CFG)" == "bochsw - Win32 Debug"

# ADD BASE RSC /l 0x409 /i "\avis200.081\src\avis100\Vides\bochs-96" /i "\src\Avis100\Vides\bochs-96" /i "\dev\progs\breed!\bochs-96" /i "\breed!\bochs-96" /i "\Emulator Source\bochs-96" /i "\dev\progs\bochs\bochs-96"
# SUBTRACT BASE RSC /i "c:\Emulator Source\hasax"
# ADD RSC /l 0x409 /i "\avis200.081\src\avis100\Vides\bochs-96" /i "\src\Avis100\Vides\bochs-96" /i "\dev\progs\breed!\bochs-96" /i "\breed!\bochs-96" /i "\Emulator Source\bochs-96" /i "\dev\progs\bochs\bochs-96" /i "D:\Dev\progs\bochs\bochs-96"
# SUBTRACT RSC /i "c:\Emulator Source\hasax"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\cmos.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\condition.c
# End Source File
# Begin Source File

SOURCE=..\hasax\conv_lib.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\ctrl_xfer.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\ctrl_xfer_pro.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\data_xfer.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\decode.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\decode_exgx.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\dma.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\dosfns.h"
# End Source File
# Begin Source File

SOURCE=..\hasax\emulate_reg.c
# End Source File
# Begin Source File

SOURCE=..\hasax\error.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\exception.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\expr_reg.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\extra.c"
# End Source File
# Begin Source File

SOURCE=.\watcher\fatparser\fatpars.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\flag_ctrl.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\floppy.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\forboo.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\globalVar.c
# End Source File
# Begin Source File

SOURCE=..\hasax\guardAct.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\harddrv.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\hga.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\hga_w32.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\hga_x.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\io.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\keyboard.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\lazy_flags.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\lib_reg.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\logical.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\main.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\memory.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\merge.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\mouse.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\NADF_file.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\ndp.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\paging.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\parallel.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\parse.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\pic.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\pit.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\printbox.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\proc_ctrl.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\protect_ctrl.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\rdescr.c
# End Source File
# Begin Source File

SOURCE=..\hasax\recov.c
# End Source File
# Begin Source File

SOURCE=..\hasax\regex.c
# End Source File
# Begin Source File

SOURCE=..\hasax\rule_decl.c
# End Source File
# Begin Source File

SOURCE=..\hasax\scan.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\segment_ctrl.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\serial.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\shift.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\soft_int.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\stack.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\std.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\string.c"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\tasking_stub.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\term.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\timers.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\trigger.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\unmapped.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\usage.c
# End Source File
# Begin Source File

SOURCE="..\bochs-96\vm8086.c"
# End Source File
# Begin Source File

SOURCE=..\hasax\watch_asax_fns.c
# End Source File
# Begin Source File

SOURCE=.\watcher\watch_ins.c
# End Source File
# Begin Source File

SOURCE=.\watcher\watch_ins_tbl.c
# End Source File
# Begin Source File

SOURCE=.\watcher\watch_memory.c
# End Source File
# Begin Source File

SOURCE=.\watcher\watch_regs.c
# End Source File
# Begin Source File

SOURCE=.\watcher\watcher.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\hasax\_ASAX__LIB.h
# End Source File
# Begin Source File

SOURCE=..\hasax\_PROTO__.h
# End Source File
# Begin Source File

SOURCE=..\hasax\asax.h
# End Source File
# Begin Source File

SOURCE=.\watcher\fatparser\basictypes.h
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\bios.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\bochs.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\iodev\bochs_icon"
# End Source File
# Begin Source File

SOURCE=.\watcher\fatparser\bootsec.h
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\cmos.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\config.h"
# End Source File
# Begin Source File

SOURCE=.\watcher\fatparser\direntry.h
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\dma.h"
# End Source File
# Begin Source File

SOURCE=.\watcher\fatparser\fatpars.h
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\fetch.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\floppy.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\forboo.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\harddrv.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\hga.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\hga_x.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\instr_proto.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\iodev\iodev.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\keyboard.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\lazy_flags.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\iodev\mem_map.h"
# End Source File
# Begin Source File

SOURCE="..\Bochs-96\memory.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\mouse.h"
# End Source File
# Begin Source File

SOURCE=..\hasax\NADF_flag.h
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\parallel.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\pic.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\pit.h"
# End Source File
# Begin Source File

SOURCE=..\hasax\predef.h
# End Source File
# Begin Source File

SOURCE=..\hasax\regex.h
# End Source File
# Begin Source File

SOURCE=..\hasax\SATX_error.h
# End Source File
# Begin Source File

SOURCE=..\hasax\SATX_sys.h
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\serial.h"
# End Source File
# Begin Source File

SOURCE="..\bochs-96\iodev\unmapped.h"
# End Source File
# Begin Source File

SOURCE=.\watcher\watch_ins_tbl.h
# End Source File
# Begin Source File

SOURCE=.\watcher\watcher.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE="..\bochs-96\bochs.ico"
# End Source File
# End Group
# Begin Source File

SOURCE=.\test.adf
# End Source File
# Begin Source File

SOURCE=.\test.asa
# End Source File
# Begin Source File

SOURCE=..\doc\watcher_doc.txt
# End Source File
# End Target
# End Project
