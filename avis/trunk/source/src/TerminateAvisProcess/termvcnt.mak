#---------------------------------------------------------
# Makefile begins.
#---------------------------------------------------------

all: clean depend compile library exedll help

setup:
!IF ( "$(LANGTYPE)" == "DBCS" )
        set VCC_LANGTYPE_FLAG=_MBCS
        set DBCSLIB=$(LIBD)\dbcsvcnt.lib
!ELSE
        set VCC_LANGTYPE_FLAG=_SBCS
        set DBCSLIB=
!ENDIF
	set NO_EXTERNAL_DEPS=1

depend: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OBJD)\TerminateAvisProcess  mkdir $(OBJD)\TerminateAvisProcess
        @if not exist $(OUTD)              mkdir $(OUTD)
        @if not exist $(OUTD)\NT           mkdir $(OUTD)\NT

        @echo Depend done

compile: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        -@termavis.cmd compile
        @echo Compile done

help: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        @echo Help done

library: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        @echo Library done

exedll: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        -@termavis.cmd exedll
        @echo ExeDll done

clean: setup
        @cd $(SRCD)\TerminateAvisProcess
        @$(SRCDRIVE)
        -@termavis.cmd clean
        @if exist $(OBJD)\TerminateAvisProcess\TerminateAvisProcess.mk erase $(OBJD)\TerminateAvisProcess\TerminateAvisProcess.mk
        @echo Clean done

#---------------------------------------------------------
# Makefile ends.
#---------------------------------------------------------
