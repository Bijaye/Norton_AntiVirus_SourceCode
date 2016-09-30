:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::  BUILD.BAT for HELP.DOS
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@Echo Off
call mk hlpnav.mak %1
if exist make.err copy build.err+make.err build.err

::call mk hlpnet.mak %1
::if exist make.err copy build.err+make.err build.err
