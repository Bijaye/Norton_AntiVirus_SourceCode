@echo off
@if "%1"=="Debug"     goto start
@if "%1"=="Release"   goto start
@echo Debug or Release
@goto end

:start
    cd autoverv
    nmake -f autoverv.mak CFG="autoverv - Win32 %1"
    xcopy %1\autoverv.exe D:\bin
    cd ..

    cd PAMemulate
    nmake -f PAMemulate.mak CFG="PAMemulate - Win32 %1"
    xcopy %1\PAMemulate.exe D:\bin
    cd ..

    xcopy /D codow\*.exe D:\bin
    xcopy /D codow\*.opt D:\bin
    
    xcopy /D bindefpak\*.pl D:\bin
rem xcopy /D PAMemulate\*.pl D:\bin

    xcopy /D vanalysis.pl D:\bin

    xcopy /D PAMemulate\master.dat D:\bin
    xcopy /D PAMemulate\build.set0 D:\bin

    xcopy /D /s perl\lib\site\* D:\perl\lib\site\*

rem testing...
    if "%COMPUTERNAME%"=="SHINE" xcopy /D score\*.exe D:\bin
    if "%COMPUTERNAME%"=="SHINE" xcopy /D TestBinRepair\*.exe D:\bin
    if "%COMPUTERNAME%"=="SHINE" xcopy /D TestBinRepair\*.pm  D:\analysis\test

:end
