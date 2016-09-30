@Echo Off
Set Beep=
Set Chk=
Set Res=
Set Error=
Echo Making S:\NAVEX\SRC\navex.mak (-nologo -b L:\SOURCE\INCLUDE\SRC\Builtins -f navex.mak -Batch Makefile.Bat):> Make.Err
Echo *** PROGRAM=NAVEX16.DLL >>Make.Err
Echo *** ENV=WINDLL >>Make.Err
Echo *** MODEL=M >>Make.Err
Echo *** PLATFORM=.WIN >>Make.Err
Echo *** FIRSTLIBS=MDLLCEW   >>Make.Err
Echo *** SYSLIBS=LIBW COMMDLG SHELL TOOLHELP MMSYSTEM VER WIN87EM DDEML>>Make.Err
Type Make.Err
Echo @Echo Off > MkClnUp.BAT
if exist wd7sigsf.err del wd7sigsf.err
if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigs8.cpy
if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd7sigs9.cpy
if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd7sigsa.cpy
if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd7sigsb.cpy
if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd7sigsc.cpy
if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd7sigsd.cpy
if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd7sigse.cpy
if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsF.cpy
Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs ÄÄÄÄ> wd7sigsf.err
echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs
mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
if errorlevel ne 0 type wd7sigsf.err
if errorlevel eq 0 del wd7sigsf.err > nul
if exist wd7sigs0.cpy del wd7sigs.lst wd7sigs0.cpy
if exist wd7sigs1.cpy del wdnamsig.wdf wd7sigs1.cpy
if exist wd7sigs2.cpy del wd7sigs2.wdf wd7sigs2.cpy
if exist wd7sigs3.cpy del wd7sigs3.wdf wd7sigs3.cpy
if exist wd7sigs4.cpy del wdvarsig.wdf wd7sigs4.cpy
if exist wd7sigs5.cpy del wd7sigs.wdf wd7sigs5.cpy
if exist wd7sigs6.cpy del wd7sigs4.wdf wd7sigs6.cpy
if exist wd7sigs7.cpy del wd7sigs5.wdf wd7sigs7.cpy
if exist wd7sigs8.cpy del wd7sigs6.wdf wd7sigs8.cpy
if exist wd7sigs9.cpy del wdaussig.wdf wd7sigs9.cpy
if exist wd7sigsa.cpy del wdeursig.wdf wd7sigsa.cpy
if exist wd7sigsb.cpy del wdjapsig.wdf wd7sigsb.cpy
if exist wd7sigsc.cpy del wdausnam.wdf wd7sigsc.cpy
if exist wd7sigsd.cpy del wdeurnam.wdf wd7sigsd.cpy
if exist wd7sigse.cpy del wdjapnam.wdf wd7sigse.cpy
if exist wd7sigsF.cpy del wd7sigs7.wdf wd7sigsF.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd7sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd7sigs1.err
Echo  CL wd7sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd7sigs1.err
@echo CL wd7sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd7sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd7sigs1.CPP >>wd7sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd7sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd7sigs1.err
Copy  make.err+wd7sigs1.err >nul
if .%Beep% == . del wd7sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd7sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd7sigs2.err
Echo  CL wd7sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd7sigs2.err
@echo CL wd7sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd7sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd7sigs2.CPP >>wd7sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd7sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd7sigs2.err
Copy  make.err+wd7sigs2.err >nul
if .%Beep% == . del wd7sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd7sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd7sigs3.err
Echo  CL wd7sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd7sigs3.err
@echo CL wd7sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd7sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd7sigs3.CPP >>wd7sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd7sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd7sigs3.err
Copy  make.err+wd7sigs3.err >nul
if .%Beep% == . del wd7sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist wd8sigsf.err del wd8sigsf.err
if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd8sigs1.cpy
if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd8sigs2.cpy
if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd8sigs3.cpy
if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd8sigs4.cpy
if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd8sigs6.cpy
if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd8sigs7.cpy
if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd8sigs8.cpy
if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd8sigs9.cpy
if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd8sigsa.cpy
if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd8sigsb.cpy
if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd8sigsc.cpy
if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd8sigsd.cpy
if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd8sigse.cpy
if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd8sigsf.cpy
if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sig10.cpy
Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs ÄÄÄÄ> wd8sigsf.err
echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs
mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -lwd8sigs.lst -sFAR_DATA_WD8 wd8sigs > wd8sigsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
if errorlevel ne 0 type wd8sigsf.err
if errorlevel eq 0 del wd8sigsf.err > nul
if exist wd8sigs0.cpy del wd8sigs.lst wd8sigs0.cpy
if exist wd8sigs1.cpy del wdnamsig.wdf wd8sigs1.cpy
if exist wd8sigs2.cpy del wd7sigs.wdf wd8sigs2.cpy
if exist wd8sigs3.cpy del wd7sigs2.wdf wd8sigs3.cpy
if exist wd8sigs4.cpy del wd7sigs3.wdf wd8sigs4.cpy
if exist wd8sigs5.cpy del wd8sigs.wdf wd8sigs5.cpy
if exist wd8sigs6.cpy del wdvarsig.wdf wd8sigs6.cpy
if exist wd8sigs7.cpy del wd7sigs4.wdf wd8sigs7.cpy
if exist wd8sigs8.cpy del wd7sigs5.wdf wd8sigs8.cpy
if exist wd8sigs9.cpy del wd7sigs6.wdf wd8sigs9.cpy
if exist wd8sigsa.cpy del wdaussig.wdf wd8sigsa.cpy
if exist wd8sigsb.cpy del wdeursig.wdf wd8sigsb.cpy
if exist wd8sigsc.cpy del wdjapsig.wdf wd8sigsc.cpy
if exist wd8sigsd.cpy del wdausnam.wdf wd8sigsd.cpy
if exist wd8sigse.cpy del wdeurnam.wdf wd8sigse.cpy
if exist wd8sigsf.cpy del wdjapnam.wdf wd8sigsf.cpy
if exist wd8sig10.cpy del wd7sigs7.wdf wd8sig10.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd8sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd8sigs1.err
Echo  CL wd8sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd8sigs1.err
@echo CL wd8sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd8sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd8sigs1.CPP >>wd8sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd8sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd8sigs1.err
Copy  make.err+wd8sigs1.err >nul
if .%Beep% == . del wd8sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd8sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd8sigs2.err
Echo  CL wd8sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd8sigs2.err
@echo CL wd8sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd8sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd8sigs2.CPP >>wd8sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd8sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd8sigs2.err
Copy  make.err+wd8sigs2.err >nul
if .%Beep% == . del wd8sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd8sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd8sigs3.err
Echo  CL wd8sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd8sigs3.err
@echo CL wd8sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd8sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd8sigs3.CPP >>wd8sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd8sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd8sigs3.err
Copy  make.err+wd8sigs3.err >nul
if .%Beep% == . del wd8sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist xl97sgsf.err del xl97sgsf.err
if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
if not exist XL97VAR.WDF copy %proj%\src\XL97VAR.WDF > xl97sgs2.cpy
if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs ÄÄÄÄ> xl97sgsf.err
echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs
mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -lxl97sigs.lst -sFAR_DATA_SIGS x97sigs > xl97sgsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
if errorlevel ne 0 type xl97sgsf.err
if errorlevel eq 0 del xl97sgsf.err > nul
if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
if exist xl97sgs2.cpy del XL97VAR.WDF xl97sgs2.cpy
if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x97sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x97sigs1.err
Echo  CL x97sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x97sigs1.err
@echo CL x97sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x97sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x97sigs1.CPP >>x97sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x97sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x97sigs1.err
Copy  make.err+x97sigs1.err >nul
if .%Beep% == . del x97sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x97sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x97sigs2.err
Echo  CL x97sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x97sigs2.err
@echo CL x97sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x97sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x97sigs2.CPP >>x97sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x97sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x97sigs2.err
Copy  make.err+x97sigs2.err >nul
if .%Beep% == . del x97sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x97sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x97sigs3.err
Echo  CL x97sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x97sigs3.err
@echo CL x97sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x97sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x97sigs3.CPP >>x97sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x97sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x97sigs3.err
Copy  make.err+x97sigs3.err >nul
if .%Beep% == . del x97sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist xl95sgsf.err del xl95sgsf.err
if not exist xl95sigs.lst copy %proj%\src\xl95sigs.lst > xl95sgs0.cpy
if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xlnamsig.cpy
if not exist xl95sigs.wdf copy %proj%\src\xl95sigs.wdf > xl95sgs1.cpy
if not exist xlvarsig.wdf copy %proj%\src\xlvarsig.wdf > xl95sgs2.cpy
Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs ÄÄÄÄ> xl95sgsf.err
echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs
mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -lxl95sigs.lst -sFAR_DATA_SIGS x95sigs > xl95sgsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>xl95sgsf.err
if errorlevel ne 0 type xl95sgsf.err
if errorlevel eq 0 del xl95sgsf.err > nul
if exist xl95sgs0.cpy del xl95sigs.lst xl95sgs0.cpy
if exist xlnamsig.cpy del xlnamsig.wdf xlnamsig.cpy
if exist xl95sgs1.cpy del xl95sigs.wdf xl95sgs1.cpy
if exist xl95sgs2.cpy del xlvarsig.wdf xl95sgs2.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x95sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x95sigs1.err
Echo  CL x95sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x95sigs1.err
@echo CL x95sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x95sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x95sigs1.CPP >>x95sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x95sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x95sigs1.err
Copy  make.err+x95sigs1.err >nul
if .%Beep% == . del x95sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x95sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x95sigs2.err
Echo  CL x95sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x95sigs2.err
@echo CL x95sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x95sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x95sigs2.CPP >>x95sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x95sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x95sigs2.err
Copy  make.err+x95sigs2.err >nul
if .%Beep% == . del x95sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL x95sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>x95sigs3.err
Echo  CL x95sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>x95sigs3.err
@echo CL x95sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL x95sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _X95SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   x95sigs3.CPP >>x95sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>x95sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type x95sigs3.err
Copy  make.err+x95sigs3.err >nul
if .%Beep% == . del x95sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist xl4sgsf.err del xl4sgsf.err
if not exist xl4sigs.lst copy %proj%\src\xl4sigs.lst > xl4sgs0.cpy
if not exist xl4sigs.wdf copy %proj%\src\xl4sigs.wdf > xl4sgs1.cpy
Echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs ÄÄÄÄ> xl4sgsf.err
echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs
mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst -sFAR_DATA_SIGS xl4sigs > xl4sgsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>xl4sgsf.err
if errorlevel ne 0 type xl4sgsf.err
if errorlevel eq 0 del xl4sgsf.err > nul
if exist xl4sgs0.cpy del xl4sigs.lst xl4sgs0.cpy
if exist xl4sgs1.cpy del xl4sigs.wdf xl4sgs1.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL xl4sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>xl4sigs1.err
Echo  CL xl4sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>xl4sigs1.err
@echo CL xl4sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL xl4sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   xl4sigs1.CPP >>xl4sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>xl4sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type xl4sigs1.err
Copy  make.err+xl4sigs1.err >nul
if .%Beep% == . del xl4sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL xl4sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>xl4sigs2.err
Echo  CL xl4sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>xl4sigs2.err
@echo CL xl4sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL xl4sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   xl4sigs2.CPP >>xl4sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>xl4sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type xl4sigs2.err
Copy  make.err+xl4sigs2.err >nul
if .%Beep% == . del xl4sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL xl4sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>xl4sigs3.err
Echo  CL xl4sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>xl4sigs3.err
@echo CL xl4sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL xl4sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _XL4SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   xl4sigs3.CPP >>xl4sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>xl4sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type xl4sigs3.err
Copy  make.err+xl4sigs3.err >nul
if .%Beep% == . del xl4sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist wd7xclsg.err del wd7xclsf.err
if not exist wd7xclsg.xdf copy %proj%\src\wd7xclsg.xdf > wd7xclsg.cpy
Echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg ÄÄÄÄ> wd7xclsf.err
echo mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg
mkxclsig.exe -qaPASSED -tyWD7 wd7xclsg.xdf -sFAR_DATA_SIGS wd7xclsg > wd7xclsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>wd7xclsf.err
if errorlevel ne 0 type wd7xclsf.err
if errorlevel eq 0 del wd7xclsf.err > nul
if exist wd7xclsg.cpy del wd7xclsg.xdf wd7xclsg.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd7xclsg.cpp (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd7xclsg.err
Echo  CL wd7xclsg.cpp ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd7xclsg.err
@echo CL wd7xclsg.cpp (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd7xclsg.cpp ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD7XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd7xclsg.cpp >>wd7xclsg.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd7xclsg.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd7xclsg.err
Copy  make.err+wd7xclsg.err >nul
if .%Beep% == . del wd7xclsg.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist wd8xclsg.err del wd8xclsf.err
if not exist wd8xclsg.xdf copy %proj%\src\wd8xclsg.xdf > wd8xclsg.cpy
Echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg ÄÄÄÄ> wd8xclsf.err
echo mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg
mkxclsig.exe -qaPASSED -tyWD8 wd8xclsg.xdf -sFAR_DATA_SIGS wd8xclsg > wd8xclsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>wd8xclsf.err
if errorlevel ne 0 type wd8xclsf.err
if errorlevel eq 0 del wd8xclsf.err > nul
if exist wd8xclsg.cpy del wd8xclsg.xdf wd8xclsg.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL wd8xclsg.cpp (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>wd8xclsg.err
Echo  CL wd8xclsg.cpp ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>wd8xclsg.err
@echo CL wd8xclsg.cpp (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL wd8xclsg.cpp ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _WD8XCLSG -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   wd8xclsg.cpp >>wd8xclsg.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>wd8xclsg.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type wd8xclsg.err
Copy  make.err+wd8xclsg.err >nul
if .%Beep% == . del wd8xclsg.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist a2sgsf.err del a2sgsf.err
if not exist a2sigs.lst copy %proj%\src\a2sigs.lst > a2sgs0.cpy
if not exist a2sigs.wdf copy %proj%\src\a2sigs.wdf > a2sgs1.cpy
Echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs ÄÄÄÄ> a2sgsf.err
echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs
mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst -sFAR_DATA_SIGS a2sigs > a2sgsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>a2sgsf.err
if errorlevel ne 0 type a2sgsf.err
if errorlevel eq 0 del a2sgsf.err > nul
if exist a2sgs0.cpy del a2sigs.lst a2sgs0.cpy
if exist a2sgs1.cpy del a2sigs.wdf a2sgs1.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a2sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a2sigs1.err
Echo  CL a2sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a2sigs1.err
@echo CL a2sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a2sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a2sigs1.CPP >>a2sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a2sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a2sigs1.err
Copy  make.err+a2sigs1.err >nul
if .%Beep% == . del a2sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a2sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a2sigs2.err
Echo  CL a2sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a2sigs2.err
@echo CL a2sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a2sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a2sigs2.CPP >>a2sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a2sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a2sigs2.err
Copy  make.err+a2sigs2.err >nul
if .%Beep% == . del a2sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a2sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a2sigs3.err
Echo  CL a2sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a2sigs3.err
@echo CL a2sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a2sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A2SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a2sigs3.CPP >>a2sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a2sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a2sigs3.err
Copy  make.err+a2sigs3.err >nul
if .%Beep% == . del a2sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
if exist a97sgsf.err del a97sgsf.err
if not exist a97sigs.lst copy %proj%\src\a97sigs.lst > a97sgs0.cpy
if not exist a97sigs.wdf copy %proj%\src\a97sigs.wdf > a97sgs1.cpy
Echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs ÄÄÄÄ> a97sgsf.err
echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs
mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst -sFAR_DATA_SIGS a97sigs > a97sgsf.err
if errorlevel ne 0 set Beep=E
if errorlevel ne 0 set Chk=E
if errorlevel ne 0 Echo Error in processing last step.>>a97sgsf.err
if errorlevel ne 0 type a97sgsf.err
if errorlevel eq 0 del a97sgsf.err > nul
if exist a97sgs0.cpy del a97sigs.lst a97sgs0.cpy
if exist a97sgs1.cpy del a97sigs.wdf a97sgs1.cpy
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a97sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a97sigs1.err
Echo  CL a97sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a97sigs1.err
@echo CL a97sigs1.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a97sigs1.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS1 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a97sigs1.CPP >>a97sigs1.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a97sigs1.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a97sigs1.err
Copy  make.err+a97sigs1.err >nul
if .%Beep% == . del a97sigs1.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a97sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a97sigs2.err
Echo  CL a97sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a97sigs2.err
@echo CL a97sigs2.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a97sigs2.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS2 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a97sigs2.CPP >>a97sigs2.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a97sigs2.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a97sigs2.err
Copy  make.err+a97sigs2.err >nul
if .%Beep% == . del a97sigs2.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
set cl=-nologo /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%
Echo  CL a97sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO ) ÄÄÄÄ>a97sigs3.err
Echo  CL a97sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%) ÄÄÄÄ>a97sigs3.err
@echo CL a97sigs3.CPP (-c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO )
@echo CL a97sigs3.CPP ( /DSYM_WIN /DSYM_WIN16  /D_DLL -DQ_VER=\"O\" %_PCHOPTS%)
CL -c -W3 -J -G2ys -Zpe -Gx- -Alnw -NT _A97SIGS3 -GD -Os  /DNDEBUG /DMACROHEU /DUSE_BLOODHOUND_PICASSO   a97sigs3.CPP >>a97sigs3.err
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>a97sigs3.err
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type a97sigs3.err
Copy  make.err+a97sigs3.err >nul
if .%Beep% == . del a97sigs3.err >nul
Set Beep=
Set Error=
set cl=
set _PCHOPTS=
@echo DEF.DLL
if .%CHK% == . Echo ÿLINK NAVEX16.DLL ( /F /align:16 /MAP/NOD/NOE/B/SEGMENTS:512 /PACKF /ONERROR:NOEXE   /PACKCODE:49152 link32 ) ÄÄÄÄ>LINK.ERR
if .%CHK% == . Echo ÿLINK NAVEX16.DLL ( /F /align:16 /MAP/NOD/NOE/B/SEGMENTS:512 /PACKF /ONERROR:NOEXE   /PACKCODE:49152 link32 )

if .%CHK% == .   if exist NAVEX16.LNK del NAVEX16.LNK
if exist navex.obj Echo navex.obj+ >>NAVEX16.LNK
if not exist navex.obj Echo S:\NAVEX\LIB.WIN\navex.obj+ >>NAVEX16.LNK
if exist naventry.obj Echo naventry.obj+ >>NAVEX16.LNK
if not exist naventry.obj Echo S:\NAVEX\LIB.WIN\naventry.obj+ >>NAVEX16.LNK
if exist inifile.obj Echo inifile.obj+ >>NAVEX16.LNK
if not exist inifile.obj Echo S:\NAVEX\LIB.WIN\inifile.obj+ >>NAVEX16.LNK
if exist mcrheuen.obj Echo mcrheuen.obj+ >>NAVEX16.LNK
if not exist mcrheuen.obj Echo S:\NAVEX\LIB.WIN\mcrheuen.obj+ >>NAVEX16.LNK
if exist xl97eren.obj Echo xl97eren.obj+ >>NAVEX16.LNK
if not exist xl97eren.obj Echo S:\NAVEX\LIB.WIN\xl97eren.obj+ >>NAVEX16.LNK
if exist heur.obj Echo heur.obj+ >>NAVEX16.LNK
if not exist heur.obj Echo S:\NAVEX\LIB.WIN\heur.obj+ >>NAVEX16.LNK
if exist callfake.obj Echo callfake.obj+ >>NAVEX16.LNK
if not exist callfake.obj Echo S:\NAVEX\LIB.WIN\callfake.obj+ >>NAVEX16.LNK
if exist navexsf.obj Echo navexsf.obj+ >>NAVEX16.LNK
if not exist navexsf.obj Echo S:\NAVEX\LIB.WIN\navexsf.obj+ >>NAVEX16.LNK
if exist navexsb.obj Echo navexsb.obj+ >>NAVEX16.LNK
if not exist navexsb.obj Echo S:\NAVEX\LIB.WIN\navexsb.obj+ >>NAVEX16.LNK
if exist navexsp.obj Echo navexsp.obj+ >>NAVEX16.LNK
if not exist navexsp.obj Echo S:\NAVEX\LIB.WIN\navexsp.obj+ >>NAVEX16.LNK
if exist navexsm.obj Echo navexsm.obj+ >>NAVEX16.LNK
if not exist navexsm.obj Echo S:\NAVEX\LIB.WIN\navexsm.obj+ >>NAVEX16.LNK
if exist navexrf.obj Echo navexrf.obj+ >>NAVEX16.LNK
if not exist navexrf.obj Echo S:\NAVEX\LIB.WIN\navexrf.obj+ >>NAVEX16.LNK
if exist navexrb.obj Echo navexrb.obj+ >>NAVEX16.LNK
if not exist navexrb.obj Echo S:\NAVEX\LIB.WIN\navexrb.obj+ >>NAVEX16.LNK
if exist navexrp.obj Echo navexrp.obj+ >>NAVEX16.LNK
if not exist navexrp.obj Echo S:\NAVEX\LIB.WIN\navexrp.obj+ >>NAVEX16.LNK
if exist navexrh.obj Echo navexrh.obj+ >>NAVEX16.LNK
if not exist navexrh.obj Echo S:\NAVEX\LIB.WIN\navexrh.obj+ >>NAVEX16.LNK
if exist navexshr.obj Echo navexshr.obj+ >>NAVEX16.LNK
if not exist navexshr.obj Echo S:\NAVEX\LIB.WIN\navexshr.obj+ >>NAVEX16.LNK
if exist nvxbtshr.obj Echo nvxbtshr.obj+ >>NAVEX16.LNK
if not exist nvxbtshr.obj Echo S:\NAVEX\LIB.WIN\nvxbtshr.obj+ >>NAVEX16.LNK
if exist booscan.obj Echo booscan.obj+ >>NAVEX16.LNK
if not exist booscan.obj Echo S:\NAVEX\LIB.WIN\booscan.obj+ >>NAVEX16.LNK
if exist bootype.obj Echo bootype.obj+ >>NAVEX16.LNK
if not exist bootype.obj Echo S:\NAVEX\LIB.WIN\bootype.obj+ >>NAVEX16.LNK
if exist trojscan.obj Echo trojscan.obj+ >>NAVEX16.LNK
if not exist trojscan.obj Echo S:\NAVEX\LIB.WIN\trojscan.obj+ >>NAVEX16.LNK
if exist pespscan.obj Echo pespscan.obj+ >>NAVEX16.LNK
if not exist pespscan.obj Echo S:\NAVEX\LIB.WIN\pespscan.obj+ >>NAVEX16.LNK
if exist aolpw.obj Echo aolpw.obj+ >>NAVEX16.LNK
if not exist aolpw.obj Echo S:\NAVEX\LIB.WIN\aolpw.obj+ >>NAVEX16.LNK
if exist wddecsig.obj Echo wddecsig.obj+ >>NAVEX16.LNK
if not exist wddecsig.obj Echo S:\NAVEX\LIB.WIN\wddecsig.obj+ >>NAVEX16.LNK
if exist mcrscn15.obj Echo mcrscn15.obj+ >>NAVEX16.LNK
if not exist mcrscn15.obj Echo S:\NAVEX\LIB.WIN\mcrscn15.obj+ >>NAVEX16.LNK
if exist mcrrpr15.obj Echo mcrrpr15.obj+ >>NAVEX16.LNK
if not exist mcrrpr15.obj Echo S:\NAVEX\LIB.WIN\mcrrpr15.obj+ >>NAVEX16.LNK
if exist offcsc15.obj Echo offcsc15.obj+ >>NAVEX16.LNK
if not exist offcsc15.obj Echo S:\NAVEX\LIB.WIN\offcsc15.obj+ >>NAVEX16.LNK
if exist wdapsg15.obj Echo wdapsg15.obj+ >>NAVEX16.LNK
if not exist wdapsg15.obj Echo S:\NAVEX\LIB.WIN\wdapsg15.obj+ >>NAVEX16.LNK
if exist wdavsg15.obj Echo wdavsg15.obj+ >>NAVEX16.LNK
if not exist wdavsg15.obj Echo S:\NAVEX\LIB.WIN\wdavsg15.obj+ >>NAVEX16.LNK
if exist wdscan15.obj Echo wdscan15.obj+ >>NAVEX16.LNK
if not exist wdscan15.obj Echo S:\NAVEX\LIB.WIN\wdscan15.obj+ >>NAVEX16.LNK
if exist wdrepr15.obj Echo wdrepr15.obj+ >>NAVEX16.LNK
if not exist wdrepr15.obj Echo S:\NAVEX\LIB.WIN\wdrepr15.obj+ >>NAVEX16.LNK
if exist wdsigutl.obj Echo wdsigutl.obj+ >>NAVEX16.LNK
if not exist wdsigutl.obj Echo S:\NAVEX\LIB.WIN\wdsigutl.obj+ >>NAVEX16.LNK
if exist mcrsgutl.obj Echo mcrsgutl.obj+ >>NAVEX16.LNK
if not exist mcrsgutl.obj Echo S:\NAVEX\LIB.WIN\mcrsgutl.obj+ >>NAVEX16.LNK
if exist wd7scn15.obj Echo wd7scn15.obj+ >>NAVEX16.LNK
if not exist wd7scn15.obj Echo S:\NAVEX\LIB.WIN\wd7scn15.obj+ >>NAVEX16.LNK
if exist o97scn15.obj Echo o97scn15.obj+ >>NAVEX16.LNK
if not exist o97scn15.obj Echo S:\NAVEX\LIB.WIN\o97scn15.obj+ >>NAVEX16.LNK
if exist xl97rpr.obj Echo xl97rpr.obj+ >>NAVEX16.LNK
if not exist xl97rpr.obj Echo S:\NAVEX\LIB.WIN\xl97rpr.obj+ >>NAVEX16.LNK
if exist o97rpr15.obj Echo o97rpr15.obj+ >>NAVEX16.LNK
if not exist o97rpr15.obj Echo S:\NAVEX\LIB.WIN\o97rpr15.obj+ >>NAVEX16.LNK
if exist olessos.obj Echo olessos.obj+ >>NAVEX16.LNK
if not exist olessos.obj Echo S:\NAVEX\LIB.WIN\olessos.obj+ >>NAVEX16.LNK
if exist mcrheusc.obj Echo mcrheusc.obj+ >>NAVEX16.LNK
if not exist mcrheusc.obj Echo S:\NAVEX\LIB.WIN\mcrheusc.obj+ >>NAVEX16.LNK
if exist wd7heusc.obj Echo wd7heusc.obj+ >>NAVEX16.LNK
if not exist wd7heusc.obj Echo S:\NAVEX\LIB.WIN\wd7heusc.obj+ >>NAVEX16.LNK
if exist mvp.obj Echo mvp.obj+ >>NAVEX16.LNK
if not exist mvp.obj Echo S:\NAVEX\LIB.WIN\mvp.obj+ >>NAVEX16.LNK
if exist copy.obj Echo copy.obj+ >>NAVEX16.LNK
if not exist copy.obj Echo S:\NAVEX\LIB.WIN\copy.obj+ >>NAVEX16.LNK
if exist builtin.obj Echo builtin.obj+ >>NAVEX16.LNK
if not exist builtin.obj Echo S:\NAVEX\LIB.WIN\builtin.obj+ >>NAVEX16.LNK
if exist mcrxclud.obj Echo mcrxclud.obj+ >>NAVEX16.LNK
if not exist mcrxclud.obj Echo S:\NAVEX\LIB.WIN\mcrxclud.obj+ >>NAVEX16.LNK
if exist w97heusc.obj Echo w97heusc.obj+ >>NAVEX16.LNK
if not exist w97heusc.obj Echo S:\NAVEX\LIB.WIN\w97heusc.obj+ >>NAVEX16.LNK
if exist w97obj.obj Echo w97obj.obj+ >>NAVEX16.LNK
if not exist w97obj.obj Echo S:\NAVEX\LIB.WIN\w97obj.obj+ >>NAVEX16.LNK
if exist w97str.obj Echo w97str.obj+ >>NAVEX16.LNK
if not exist w97str.obj Echo S:\NAVEX\LIB.WIN\w97str.obj+ >>NAVEX16.LNK
if exist xl5scn.obj Echo xl5scn.obj+ >>NAVEX16.LNK
if not exist xl5scn.obj Echo S:\NAVEX\LIB.WIN\xl5scn.obj+ >>NAVEX16.LNK
if exist xl5rpr.obj Echo xl5rpr.obj+ >>NAVEX16.LNK
if not exist xl5rpr.obj Echo S:\NAVEX\LIB.WIN\xl5rpr.obj+ >>NAVEX16.LNK
if exist xl4scn.obj Echo xl4scn.obj+ >>NAVEX16.LNK
if not exist xl4scn.obj Echo S:\NAVEX\LIB.WIN\xl4scn.obj+ >>NAVEX16.LNK
if exist xl4rpr.obj Echo xl4rpr.obj+ >>NAVEX16.LNK
if not exist xl4rpr.obj Echo S:\NAVEX\LIB.WIN\xl4rpr.obj+ >>NAVEX16.LNK
if exist xlrprsht.obj Echo xlrprsht.obj+ >>NAVEX16.LNK
if not exist xlrprsht.obj Echo S:\NAVEX\LIB.WIN\xlrprsht.obj+ >>NAVEX16.LNK
if exist a2scn.obj Echo a2scn.obj+ >>NAVEX16.LNK
if not exist a2scn.obj Echo S:\NAVEX\LIB.WIN\a2scn.obj+ >>NAVEX16.LNK
if exist a97scn.obj Echo a97scn.obj+ >>NAVEX16.LNK
if not exist a97scn.obj Echo S:\NAVEX\LIB.WIN\a97scn.obj+ >>NAVEX16.LNK
if exist acsos.obj Echo acsos.obj+ >>NAVEX16.LNK
if not exist acsos.obj Echo S:\NAVEX\LIB.WIN\acsos.obj+ >>NAVEX16.LNK
if exist w7h2sght.obj Echo w7h2sght.obj+ >>NAVEX16.LNK
if not exist w7h2sght.obj Echo S:\NAVEX\LIB.WIN\w7h2sght.obj+ >>NAVEX16.LNK
if exist w7h2scan.obj Echo w7h2scan.obj+ >>NAVEX16.LNK
if not exist w7h2scan.obj Echo S:\NAVEX\LIB.WIN\w7h2scan.obj+ >>NAVEX16.LNK
if exist w7h2sigs.obj Echo w7h2sigs.obj+ >>NAVEX16.LNK
if not exist w7h2sigs.obj Echo S:\NAVEX\LIB.WIN\w7h2sigs.obj+ >>NAVEX16.LNK
if exist excelheu.obj Echo excelheu.obj+ >>NAVEX16.LNK
if not exist excelheu.obj Echo S:\NAVEX\LIB.WIN\excelheu.obj+ >>NAVEX16.LNK
if exist xlheusc.obj Echo xlheusc.obj+ >>NAVEX16.LNK
if not exist xlheusc.obj Echo S:\NAVEX\LIB.WIN\xlheusc.obj+ >>NAVEX16.LNK
if exist wd7sigs0.obj Echo wd7sigs0.obj+ >>NAVEX16.LNK
if not exist wd7sigs0.obj Echo S:\NAVEX\LIB.WIN\wd7sigs0.obj+ >>NAVEX16.LNK
if exist wd7sigs1.obj Echo wd7sigs1.obj+ >>NAVEX16.LNK
if not exist wd7sigs1.obj Echo S:\NAVEX\LIB.WIN\wd7sigs1.obj+ >>NAVEX16.LNK
if exist wd7sigs2.obj Echo wd7sigs2.obj+ >>NAVEX16.LNK
if not exist wd7sigs2.obj Echo S:\NAVEX\LIB.WIN\wd7sigs2.obj+ >>NAVEX16.LNK
if exist wd7sigs3.obj Echo wd7sigs3.obj+ >>NAVEX16.LNK
if not exist wd7sigs3.obj Echo S:\NAVEX\LIB.WIN\wd7sigs3.obj+ >>NAVEX16.LNK
if exist wd8sigs0.obj Echo wd8sigs0.obj+ >>NAVEX16.LNK
if not exist wd8sigs0.obj Echo S:\NAVEX\LIB.WIN\wd8sigs0.obj+ >>NAVEX16.LNK
if exist wd8sigs1.obj Echo wd8sigs1.obj+ >>NAVEX16.LNK
if not exist wd8sigs1.obj Echo S:\NAVEX\LIB.WIN\wd8sigs1.obj+ >>NAVEX16.LNK
if exist wd8sigs2.obj Echo wd8sigs2.obj+ >>NAVEX16.LNK
if not exist wd8sigs2.obj Echo S:\NAVEX\LIB.WIN\wd8sigs2.obj+ >>NAVEX16.LNK
if exist wd8sigs3.obj Echo wd8sigs3.obj+ >>NAVEX16.LNK
if not exist wd8sigs3.obj Echo S:\NAVEX\LIB.WIN\wd8sigs3.obj+ >>NAVEX16.LNK
if exist x97sigs0.obj Echo x97sigs0.obj+ >>NAVEX16.LNK
if not exist x97sigs0.obj Echo S:\NAVEX\LIB.WIN\x97sigs0.obj+ >>NAVEX16.LNK
if exist x97sigs1.obj Echo x97sigs1.obj+ >>NAVEX16.LNK
if not exist x97sigs1.obj Echo S:\NAVEX\LIB.WIN\x97sigs1.obj+ >>NAVEX16.LNK
if exist x97sigs2.obj Echo x97sigs2.obj+ >>NAVEX16.LNK
if not exist x97sigs2.obj Echo S:\NAVEX\LIB.WIN\x97sigs2.obj+ >>NAVEX16.LNK
if exist x97sigs3.obj Echo x97sigs3.obj+ >>NAVEX16.LNK
if not exist x97sigs3.obj Echo S:\NAVEX\LIB.WIN\x97sigs3.obj+ >>NAVEX16.LNK
if exist x95sigs0.obj Echo x95sigs0.obj+ >>NAVEX16.LNK
if not exist x95sigs0.obj Echo S:\NAVEX\LIB.WIN\x95sigs0.obj+ >>NAVEX16.LNK
if exist x95sigs1.obj Echo x95sigs1.obj+ >>NAVEX16.LNK
if not exist x95sigs1.obj Echo S:\NAVEX\LIB.WIN\x95sigs1.obj+ >>NAVEX16.LNK
if exist x95sigs2.obj Echo x95sigs2.obj+ >>NAVEX16.LNK
if not exist x95sigs2.obj Echo S:\NAVEX\LIB.WIN\x95sigs2.obj+ >>NAVEX16.LNK
if exist x95sigs3.obj Echo x95sigs3.obj+ >>NAVEX16.LNK
if not exist x95sigs3.obj Echo S:\NAVEX\LIB.WIN\x95sigs3.obj+ >>NAVEX16.LNK
if exist xl4sigs0.obj Echo xl4sigs0.obj+ >>NAVEX16.LNK
if not exist xl4sigs0.obj Echo S:\NAVEX\LIB.WIN\xl4sigs0.obj+ >>NAVEX16.LNK
if exist xl4sigs1.obj Echo xl4sigs1.obj+ >>NAVEX16.LNK
if not exist xl4sigs1.obj Echo S:\NAVEX\LIB.WIN\xl4sigs1.obj+ >>NAVEX16.LNK
if exist xl4sigs2.obj Echo xl4sigs2.obj+ >>NAVEX16.LNK
if not exist xl4sigs2.obj Echo S:\NAVEX\LIB.WIN\xl4sigs2.obj+ >>NAVEX16.LNK
if exist xl4sigs3.obj Echo xl4sigs3.obj+ >>NAVEX16.LNK
if not exist xl4sigs3.obj Echo S:\NAVEX\LIB.WIN\xl4sigs3.obj+ >>NAVEX16.LNK
if exist wd7xclsg.obj Echo wd7xclsg.obj+ >>NAVEX16.LNK
if not exist wd7xclsg.obj Echo S:\NAVEX\LIB.WIN\wd7xclsg.obj+ >>NAVEX16.LNK
if exist x95xclsg.obj Echo x95xclsg.obj+ >>NAVEX16.LNK
if not exist x95xclsg.obj Echo S:\NAVEX\LIB.WIN\x95xclsg.obj+ >>NAVEX16.LNK
if exist wd8xclsg.obj Echo wd8xclsg.obj+ >>NAVEX16.LNK
if not exist wd8xclsg.obj Echo S:\NAVEX\LIB.WIN\wd8xclsg.obj+ >>NAVEX16.LNK
if exist x97xclsg.obj Echo x97xclsg.obj+ >>NAVEX16.LNK
if not exist x97xclsg.obj Echo S:\NAVEX\LIB.WIN\x97xclsg.obj+ >>NAVEX16.LNK
if exist a2sigs0.obj Echo a2sigs0.obj+ >>NAVEX16.LNK
if not exist a2sigs0.obj Echo S:\NAVEX\LIB.WIN\a2sigs0.obj+ >>NAVEX16.LNK
if exist a2sigs1.obj Echo a2sigs1.obj+ >>NAVEX16.LNK
if not exist a2sigs1.obj Echo S:\NAVEX\LIB.WIN\a2sigs1.obj+ >>NAVEX16.LNK
if exist a2sigs2.obj Echo a2sigs2.obj+ >>NAVEX16.LNK
if not exist a2sigs2.obj Echo S:\NAVEX\LIB.WIN\a2sigs2.obj+ >>NAVEX16.LNK
if exist a2sigs3.obj Echo a2sigs3.obj+ >>NAVEX16.LNK
if not exist a2sigs3.obj Echo S:\NAVEX\LIB.WIN\a2sigs3.obj+ >>NAVEX16.LNK
if exist a97sigs0.obj Echo a97sigs0.obj+ >>NAVEX16.LNK
if not exist a97sigs0.obj Echo S:\NAVEX\LIB.WIN\a97sigs0.obj+ >>NAVEX16.LNK
if exist a97sigs1.obj Echo a97sigs1.obj+ >>NAVEX16.LNK
if not exist a97sigs1.obj Echo S:\NAVEX\LIB.WIN\a97sigs1.obj+ >>NAVEX16.LNK
if exist a97sigs2.obj Echo a97sigs2.obj+ >>NAVEX16.LNK
if not exist a97sigs2.obj Echo S:\NAVEX\LIB.WIN\a97sigs2.obj+ >>NAVEX16.LNK
if exist a97sigs3.obj Echo a97sigs3.obj+ >>NAVEX16.LNK
if not exist a97sigs3.obj Echo S:\NAVEX\LIB.WIN\a97sigs3.obj+ >>NAVEX16.LNK
if exist nepescan.obj Echo nepescan.obj+ >>NAVEX16.LNK
if not exist nepescan.obj Echo S:\NAVEX\LIB.WIN\nepescan.obj+ >>NAVEX16.LNK
if exist nepeshr.obj Echo nepeshr.obj+ >>NAVEX16.LNK
if not exist nepeshr.obj Echo S:\NAVEX\LIB.WIN\nepeshr.obj+ >>NAVEX16.LNK
if exist neperep.obj Echo neperep.obj+ >>NAVEX16.LNK
if not exist neperep.obj Echo S:\NAVEX\LIB.WIN\neperep.obj+ >>NAVEX16.LNK
if exist winsig.obj Echo winsig.obj+ >>NAVEX16.LNK
if not exist winsig.obj Echo S:\NAVEX\LIB.WIN\winsig.obj+ >>NAVEX16.LNK
if exist javascan.obj Echo javascan.obj+ >>NAVEX16.LNK
if not exist javascan.obj Echo S:\NAVEX\LIB.WIN\javascan.obj+ >>NAVEX16.LNK
if exist javashr.obj Echo javashr.obj+ >>NAVEX16.LNK
if not exist javashr.obj Echo S:\NAVEX\LIB.WIN\javashr.obj+ >>NAVEX16.LNK
if exist javasig.obj Echo javasig.obj+ >>NAVEX16.LNK
if not exist javasig.obj Echo S:\NAVEX\LIB.WIN\javasig.obj+ >>NAVEX16.LNK
Echo , >>NAVEX16.LNK
Echo NAVEX16.DLL, >>NAVEX16.LNK
Echo NAVEX16.MAP  /F /align:16 /MAP/NOD/NOE/B/SEGMENTS:512 /PACKF /ONERROR:NOEXE   /PACKCODE:49152, >>NAVEX16.LNK
Echo MDLLCEW+     >>NAVEX16.LNK
if exist heur16.lib Echo heur16.lib+ >>NAVEX16.LNK
if not exist heur16.lib Echo S:\LIB.WIN\heur16.lib+ >>NAVEX16.LNK
if exist crc32.lib Echo crc32.lib+ >>NAVEX16.LNK
if not exist crc32.lib Echo S:\LIB.WIN\crc32.lib+ >>NAVEX16.LNK
if exist olessapi.lib Echo olessapi.lib+ >>NAVEX16.LNK
if not exist olessapi.lib Echo S:\LIB.WIN\olessapi.lib+ >>NAVEX16.LNK
if exist wd7api.lib Echo wd7api.lib+ >>NAVEX16.LNK
if not exist wd7api.lib Echo S:\LIB.WIN\wd7api.lib+ >>NAVEX16.LNK
if exist o97api.lib Echo o97api.lib+ >>NAVEX16.LNK
if not exist o97api.lib Echo S:\LIB.WIN\o97api.lib+ >>NAVEX16.LNK
if exist xl5api.lib Echo xl5api.lib+ >>NAVEX16.LNK
if not exist xl5api.lib Echo S:\LIB.WIN\xl5api.lib+ >>NAVEX16.LNK
if exist macroheu.lib Echo macroheu.lib+ >>NAVEX16.LNK
if not exist macroheu.lib Echo S:\LIB.WIN\macroheu.lib+ >>NAVEX16.LNK
if exist acsapi.lib Echo acsapi.lib+ >>NAVEX16.LNK
if not exist acsapi.lib Echo S:\LIB.WIN\acsapi.lib+ >>NAVEX16.LNK
Echo.  LIBW COMMDLG SHELL TOOLHELP MMSYSTEM VER WIN87EM DDEML   >>NAVEX16.LNK
Echo navex16.def; End of list >>NAVEX16.LNK
if .%CHK% == . LINK @NAVEX16.LNK >>LINK.ERR 
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
if NOT .%CHK% == . Echo if exist NAVEX16.DLL del NAVEX16.DLL >>MkClnUp.BAT
if exist NAVEX16.LNK  Echo del NAVEX16.LNK >>MkClnUp.BAT
if NOT .%CHK% == . Set Chk=A
if NOT .%CHK% == . Echo Errors occurred...  LINK NAVEX16.DLL averted.
if .%CHK% == .   Echo MAPSYM NAVEX16.DLL ÄÄÄÄÄ>MAPSYM.ERR
if .%CHK% == .   Echo MAPSYM NAVEX16.DLL
if .%CHK% == .   MAPSYM NAVEX16.Map >>MAPSYM.ERR
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>MAPSYM.ERR
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type MAPSYM.ERR
Copy  make.err+MAPSYM.ERR >nul
if .%Beep% == . del MAPSYM.ERR >nul
Set Beep=
Set Error=
if NOT .%CHK% == .  Set Chk=A
if NOT .%CHK% == .  Echo Error in processing... MAPSYM NAVEX16.DLL averted.
if .%CHK% == .  Echo IMPLIB NAVEX16 ÄÄÄÄ>IMPLIB.ERR
if .%CHK% == .  Echo IMPLIB NAVEX16
if .%CHK% == . ImpLib NAVEX16.LIB navex16.def >>IMPLIB.ERR
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>IMPLIB.ERR
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type IMPLIB.ERR
Copy  make.err+IMPLIB.ERR >nul
if .%Beep% == . del IMPLIB.ERR >nul
Set Beep=
Set Error=
if .%CHK% ne . Set Chk=A
if .%CHK% ne . Echo Error in processing... IMPLIB NAVEX16.DLL averted.
iff .%CHK% == . then
iff .%RES% == . then
Echo RC -T -31  /D_DLL  NAVEX16.DLL ÄÄÄÄ>RES.ERR
@echo ÿRC -T -31  /D_DLL  NAVEX16.DLL
RC -T -31  /D_DLL  NAVEX16.DLL >>RES.ERR
else
Echo RC -T -31  /D_DLL NAVEX16.res NAVEX16.DLL ÄÄÄÄ>RES.ERR
@echo ÿRC -T -31  /D_DLL NAVEX16.res NAVEX16.DLL
RC -T -31  /D_DLL NAVEX16.res NAVEX16.DLL >>RES.ERR
endiff
if errorlevel 2 set error=A
if errorlevel 1 set error=A
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Chk=E
if NOT .%CHK% == .A if .%error% == .A if .%Beep% == . Set Beep=E
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Echo Error in processing last step.>>RES.ERR
if  .%CHK% == .E if .%error% == .A if .%Beep% == .E Type RES.ERR
Copy  make.err+RES.ERR >nul
if .%Beep% == . del RES.ERR >nul
Set Beep=
Set Error=
endiff
iff .%CHK% ne . then
Set Chk=A
@echo Error in processing... RC NAVEX16.DLL averted.
endiff
Set Chk=
set EDTF=Make.Err
if exist MkClnUp.BAT   Call MkClnUp.BAT >nul
if exist MkClnUp.BAT   Del  MkClnUp.BAT >nul
if exist makefile.cl Del  makefile.cl >nul
Set Beep=
Set Chk=
Set RES=
set __Batch=
