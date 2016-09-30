@Echo Off
Set Beep=
Set Chk=
Set Res=
Set Error=
Echo Making S:\NAVCS\SRC\NAVCS.mak (-nologo -b L:\SOURCE\INCLUDE\SRC\Builtins -f S:\NAVCS\SRC\NAVCS.mak -Batch Makefile.Bat):> Make.Err
Echo *** ENV= >>Make.Err
Echo *** MODEL= >>Make.Err
Echo *** PLATFORM=.IRA >>Make.Err
Echo *** LIBS= clusapi.lib resutils.lib   >>Make.Err
Echo *** SYSLIBS=LIBCE>>Make.Err
Echo *** CV=%CV%   >>Make.Err
Type Make.Err
Echo @Echo Off > MkClnUp.BAT
if not exist clusinst.CPP Echo S:\NAVCS\SRC\clusinst.CPP copied to current directory C:\build\. >>clusinst.err
if not exist clusinst.CPP Echo S:\NAVCS\SRC\clusinst.CPP copied to current directory C:\build\.
if not exist clusinst.CPP Copy  S:\NAVCS\SRC\clusinst.CPP >nul
Attrib +r clusinst.CPP >nul
Echo if exist clusinst.CPP Attrib -r clusinst.CPP >>MkClnUp.BAT
Echo if exist clusinst.CPP Del  clusinst.CPP >>MkClnUp.BAT
set cl=-nologo  %_PCHOPTS%
Echo  CL clusinst.CPP (-c -W2 -A -G2 -O1 -GX ) ÄÄÄÄ>clusinst.err
Echo  CL clusinst.CPP (  %_PCHOPTS%) ÄÄÄÄ>clusinst.err
@echo CL clusinst.CPP (-c -W2 -A -G2 -O1 -GX )
@echo CL clusinst.CPP (  %_PCHOPTS%)
CL -c -W2 -A -G2 -O1 -GX   clusinst.CPP >>clusinst.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>clusinst.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type clusinst.err
Copy  make.err+clusinst.err >nul
if .%Beep% == . del clusinst.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if not exist clusnav.cpp Echo S:\NAVCS\SRC\clusnav.cpp copied to current directory C:\build\. >>clusnav.err
if not exist clusnav.cpp Echo S:\NAVCS\SRC\clusnav.cpp copied to current directory C:\build\.
if not exist clusnav.cpp Copy  S:\NAVCS\SRC\clusnav.cpp >nul
Attrib +r clusnav.cpp >nul
Echo if exist clusnav.cpp Attrib -r clusnav.cpp >>MkClnUp.BAT
Echo if exist clusnav.cpp Del  clusnav.cpp >>MkClnUp.BAT
set cl=-nologo  %_PCHOPTS%
Echo  CL clusnav.cpp (-c -W2 -A -G2 -O1 -GX ) ÄÄÄÄ>clusnav.err
Echo  CL clusnav.cpp (  %_PCHOPTS%) ÄÄÄÄ>clusnav.err
@echo CL clusnav.cpp (-c -W2 -A -G2 -O1 -GX )
@echo CL clusnav.cpp (  %_PCHOPTS%)
CL -c -W2 -A -G2 -O1 -GX   clusnav.cpp >>clusnav.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>clusnav.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type clusnav.err
Copy  make.err+clusnav.err >nul
if .%Beep% == . del clusnav.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if not exist clusnave.cpp Echo S:\NAVCS\SRC\clusnave.cpp copied to current directory C:\build\. >>clusnave.err
if not exist clusnave.cpp Echo S:\NAVCS\SRC\clusnave.cpp copied to current directory C:\build\.
if not exist clusnave.cpp Copy  S:\NAVCS\SRC\clusnave.cpp >nul
Attrib +r clusnave.cpp >nul
Echo if exist clusnave.cpp Attrib -r clusnave.cpp >>MkClnUp.BAT
Echo if exist clusnave.cpp Del  clusnave.cpp >>MkClnUp.BAT
set cl=-nologo  %_PCHOPTS%
Echo  CL clusnave.cpp (-c -W2 -A -G2 -O1 -GX ) ÄÄÄÄ>clusnave.err
Echo  CL clusnave.cpp (  %_PCHOPTS%) ÄÄÄÄ>clusnave.err
@echo CL clusnave.cpp (-c -W2 -A -G2 -O1 -GX )
@echo CL clusnave.cpp (  %_PCHOPTS%)
CL -c -W2 -A -G2 -O1 -GX   clusnave.cpp >>clusnave.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>clusnave.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type clusnave.err
Copy  make.err+clusnave.err >nul
if .%Beep% == . del clusnave.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if not exist nhsdebug.cpp Echo S:\NAVCS\SRC\nhsdebug.cpp copied to current directory C:\build\. >>nhsdebug.err
if not exist nhsdebug.cpp Echo S:\NAVCS\SRC\nhsdebug.cpp copied to current directory C:\build\.
if not exist nhsdebug.cpp Copy  S:\NAVCS\SRC\nhsdebug.cpp >nul
Attrib +r nhsdebug.cpp >nul
Echo if exist nhsdebug.cpp Attrib -r nhsdebug.cpp >>MkClnUp.BAT
Echo if exist nhsdebug.cpp Del  nhsdebug.cpp >>MkClnUp.BAT
set cl=-nologo  %_PCHOPTS%
Echo  CL nhsdebug.cpp (-c -W2 -A -G2 -O1 -GX ) ÄÄÄÄ>nhsdebug.err
Echo  CL nhsdebug.cpp (  %_PCHOPTS%) ÄÄÄÄ>nhsdebug.err
@echo CL nhsdebug.cpp (-c -W2 -A -G2 -O1 -GX )
@echo CL nhsdebug.cpp (  %_PCHOPTS%)
CL -c -W2 -A -G2 -O1 -GX   nhsdebug.cpp >>nhsdebug.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>nhsdebug.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type nhsdebug.err
Copy  make.err+nhsdebug.err >nul
if .%Beep% == . del nhsdebug.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if not exist PropList.cpp Echo S:\NAVCS\SRC\PropList.cpp copied to current directory C:\build\. >>PropList.err
if not exist PropList.cpp Echo S:\NAVCS\SRC\PropList.cpp copied to current directory C:\build\.
if not exist PropList.cpp Copy  S:\NAVCS\SRC\PropList.cpp >nul
Attrib +r PropList.cpp >nul
Echo if exist PropList.cpp Attrib -r PropList.cpp >>MkClnUp.BAT
Echo if exist PropList.cpp Del  PropList.cpp >>MkClnUp.BAT
set cl=-nologo  %_PCHOPTS%
Echo  CL PropList.cpp (-c -W2 -A -G2 -O1 -GX ) ÄÄÄÄ>PropList.err
Echo  CL PropList.cpp (  %_PCHOPTS%) ÄÄÄÄ>PropList.err
@echo CL PropList.cpp (-c -W2 -A -G2 -O1 -GX )
@echo CL PropList.cpp (  %_PCHOPTS%)
CL -c -W2 -A -G2 -O1 -GX   PropList.cpp >>PropList.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>PropList.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type PropList.err
Copy  make.err+PropList.err >nul
if .%Beep% == . del PropList.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
@echo OBJ.DLL
if .%CHK% == . Echo ÿLINK navcs.dll ( link32 ) ÄÄÄÄ>LINK.ERR
if .%CHK% == . Echo ÿLINK navcs.dll ( link32 )

if .%CHK% == .   if exist navcs.LNK del navcs.LNK
if .%CHK% == . Echo -out:navcs.dll >>navcs.LNK
if .%CHK% == . Echo -map:navcs.MAP >>navcs.LNK
if .%CHK% == . if exist clusinst.obj Echo clusinst.obj >>navcs.LNK
if .%CHK% == . if not exist clusinst.obj Echo clusinst.obj >>navcs.LNK
if .%CHK% == . if exist clusnav.obj Echo clusnav.obj >>navcs.LNK
if .%CHK% == . if not exist clusnav.obj Echo clusnav.obj >>navcs.LNK
if .%CHK% == . if exist clusnave.obj Echo clusnave.obj >>navcs.LNK
if .%CHK% == . if not exist clusnave.obj Echo clusnave.obj >>navcs.LNK
if .%CHK% == . if exist nhsdebug.obj Echo nhsdebug.obj >>navcs.LNK
if .%CHK% == . if not exist nhsdebug.obj Echo nhsdebug.obj >>navcs.LNK
if .%CHK% == . if exist PropList.obj Echo PropList.obj >>navcs.LNK
if .%CHK% == . if not exist PropList.obj Echo PropList.obj >>navcs.LNK
if .%CHK% == . Echo clusapi.lib >> navcs.LNK
if .%CHK% == . Echo resutils.lib >> navcs.LNK
if .%CHK% == . Echo LIBCE >> navcs.LNK
if .%CHK% == . LINK @navcs.LNK >>LINK.ERR 
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>LINK.ERR
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type LINK.ERR
Copy  make.err+LINK.ERR >nul
if .%Beep% == . del LINK.ERR >nul
Set Beep=
Set Error=
if NOT .%CHK% == .  Echo if exist navcs.dll del navcs.dll >>MkClnUp.BAT
if exist navcs.LNK  Echo del navcs.LNK >>MkClnUp.BAT
if NOT .%CHK% == . Set Chk=A
if NOT .%CHK% == . Echo Errors occurred...  LINK navcs.dll averted.
Set Chk=
set EDTF=Make.Err
if exist MkClnUp.BAT   Call MkClnUp.BAT >nul
if exist MkClnUp.BAT   Del  MkClnUp.BAT >nul
if exist makefile.cl Del  makefile.cl >nul
Set Beep=
Set Chk=
Set RES=
set __Batch=
