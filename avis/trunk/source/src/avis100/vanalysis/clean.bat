@rem @echo off
if "%COMPUTERNAME"=="SHINE" goto end

rd /s/q Autoverv\debug
rd /s/q Autoverv\release

rd /s/q PAMemulate\debug
rd /s/q PAMemulate\release

del autoverv\*.plg
del autoverv\*.ncb
del autoverv\*.opt

del PAMemulate\*.plg
del PAMemulate\*.ncb
del PAMemulate\*.opt

:end
