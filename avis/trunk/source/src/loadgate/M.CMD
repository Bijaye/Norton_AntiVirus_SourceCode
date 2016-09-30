: Syntax: M [-a] debug/release
:         M [-a]    -d/-r
: Note: 
:    Use environment variable:   SET FORCE_BUILD=release/debug
:    to ignore command line build target.

@if "%1" == "" goto SYNTAX
@cls
:-----------------------------------------
@SET PROJECTNAME=testgate
:
@\cmd\VERSION VERSION.INI %PROJECTNAME% /version_file=version.inc /inc_build
@if errorlevel 1 pause
:
@del %PROJECTNAME%.exe
@call \cmd\unimake.cmd %PROJECTNAME% %2 %1
:::::::::::::::::::::::::::::::::::::::::::::::::::::
@goto FINAL_EXIT
:::::::::::::::::::::::::::::::::::::::::::::::::::::
:
@if "%1" == "debug"   goto GET_SSL_DEBUG
@if "%1" == "DEBUG"   goto GET_SSL_DEBUG
@if "%2" == "debug"   goto GET_SSL_DEBUG
@if "%2" == "DEBUG"   goto GET_SSL_DEBUG
@if "%1" == "release" goto GET_SSL_RELEASE
@if "%1" == "RELEASE" goto GET_SSL_RELEASE
@if "%2" == "release" goto GET_SSL_RELEASE
@if "%2" == "RELEASE" goto GET_SSL_RELEASE
:
:GET_SSL_DEBUG
bx /xcopy SSL_DEB . *.dll *.pdb /overall /noask
@goto EXIT
:
:GET_SSL_RELEASE
bx /xcopy SSL_REL . *.dll /overall /noask
@goto EXIT
:-----------------------------------------
:
@goto EXIT

:SYNTAX
@echo Syntax:
@echo    %0 [-a] debug/release
@echo    %0 [-a]    -d/-r
@echo Note: 
@echo    Use environment variable:   SET FORCE_BUILD=release/debug
@echo    to ignore command line build target.
@goto EXIT

:EXIT
bx /ren %PROJECTNAME%.exe loadgate.exe /replace /noask
@SET PROJECTNAME=
:FINAL_EXIT
