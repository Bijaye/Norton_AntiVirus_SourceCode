@if "%1" == ""        goto START_BUILD
@if "%1" == "debug"   goto START_BUILD
@if "%1" == "DEBUG"   goto START_BUILD
@goto SYNTAX
:START_BUILD
@cls
p:
cd \wrc\openSSL\latest
bx /deltree inc32        /noask
bx /deltree out32dll     /noask
bx /deltree out32dll.dbg /noask
bx /deltree tmp32dll     /noask
bx /deltree tmp32dll.dbg /noask
CALL ms\do_ms.bat %1
nmake -a -f ms\ntdll.mak
:
@if "%1" == "debug"   goto COPY_DEBUG
@if "%1" == "DEBUG"   goto COPY_DEBUG
@goto COPY_RELEASE

:COPY_RELEASE
bx /xcopy out32dll     p:\wrc\testgate\latest\SSL_REL *.dll *.lib       /noask
@goto ALMOST_DONE

:COPY_DEBUG
bx /xcopy out32dll.dbg p:\wrc\testgate\latest\SSL_DEB *.dll *.pdb *.lib /noask
@goto ALMOST_DONE

:ALMOST_DONE
@cd \wrc\testgate\latest
@goto DONE

:SYNTAX
@echo SYNTAX:   mssl [debug]
@goto DONE

:DONE
@echo SSL build done!
