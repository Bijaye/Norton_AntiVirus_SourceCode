@echo off

setlocal
if not "%1"=="" pushd %1

del pam.exe
del pamwrap.obj
del pam.obj
del log

set PATH=..\progs;%PATH%
set INCLUDE=.;..\PAM;..\include;..\include\dx
set LIB=.;..\lib\dx;..\lib
ml /c /Fm /Fl /Sa /DSTANDALONE pamwrap.asm
cl /Fm pam.c pamwrap.obj

if not "%1"=="" popd
endlocal
