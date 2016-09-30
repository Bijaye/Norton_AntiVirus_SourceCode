#---------------------------------------------------------
# Makefile begins.
#---------------------------------------------------------
# SCC: @(#) 94 1/29/99 14:48:09 AVNADMIN32 1.2 @(#)


all: clean depend compile library exedll help

setup:
!IF ( "$(LANGTYPE)" == "DBCS" )
        set VCC_LANGTYPE_FLAG=_MBCS
        set DBCSLIB=$(LIBD)\dbcsvcnt.lib
!ELSE
        set VCC_LANGTYPE_FLAG=_SBCS
        set DBCSLIB=
!ENDIF
!IF ( "$(BLDDEBUG)" == "1" )
   SET OSSL_BUILD_TYPE=debug
   SET OSSL_OUT_DIR=out32dll.dbg
!ENDIF
!IF ( "$(BLDDEBUG)" == "" )
   SET OSSL_BUILD_TYPE=
   SET OSSL_OUT_DIR=out32dll
!ENDIF

	set NO_EXTERNAL_DEPS=1

depend: setup
        @cd $(SRCD)\openssl
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OUTD)              mkdir $(OUTD)
        @if not exist $(OUTD)\NT           mkdir $(OUTD)\NT
# Calling OpenSSL's original messy batch file to build their huge stuff.
# They also use Perl for build purposes.
!IF ( "$(BLDDEBUG)" == "1" )
        call ms\do_ms.bat debug
        nmake -a -f ms\ntdll.mak
        -move $(SRCD)\openssl\out32dll\*.pdb $(OUTD)\NT
        move $(SRCD)\openssl\out32dll.dbg\*.dll     $(OUTD)\NT
        move $(SRCD)\openssl\out32dll.dbg\*.lib     $(LIBD)
!ELSE
        call ms\do_ms.bat
        nmake -a -f ms\ntdll.mak
        move $(SRCD)\openssl\out32dll\*.dll         $(OUTD)\NT
        move $(SRCD)\openssl\out32dll\*.lib         $(LIBD)
!ENDIF
        @echo Depend done

compile: setup
        @echo Compile done

help: setup
        @echo Help done

library: setup
        @cd $(SRCD)\openssl
        @$(SRCDRIVE)
        @echo Library done


exedll: setup
        @echo ExeDll done


clean: setup
        -rmdir /s /q $(SRCD)\openssl\inc32
        -rmdir /s /q $(SRCD)\openssl\out32dll
        -rmdir /s /q $(SRCD)\openssl\out32dll.dbg
        -rmdir /s /q $(SRCD)\openssl\tmp32dll
        -rmdir /s /q $(SRCD)\openssl\tmp32dll.dbg
        -erase $(OUTD)\NT\*.dll 
        -erase $(OUTD)\NT\*.pdb
        -erase $(LIBD)\*.lib
        @echo Clean done

#---------------------------------------------------------
# Makefile ends.
#---------------------------------------------------------
