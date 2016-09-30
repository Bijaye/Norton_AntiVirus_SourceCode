#---------------------------------------------------------
# Makefile begins.
#---------------------------------------------------------
# SCC: @(#) 99 1/29/99 14:44:03 AVNADMIN32 1.2 @(#)

all: clean depend compile library exedll help

!IF ( "$(LANG1)" == "ea" || "$(LANG1)" == "ja" )

setup:
!IF ( "$(LANGTYPE)" == "DBCS" )
        set VCC_LANGTYPE_FLAG=_MBCS
        set DBCSLIB=$(LIBD)\dbcsvcnt.lib
!ELSE
        set VCC_LANGTYPE_FLAG=_SBCS
        set DBCSLIB=
!ENDIF

depend:
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OBJD)\DATAFLOW     mkdir $(OBJD)\DATAFLOW
        @if not exist $(OBJD)\DATAFLOW\DFEVAL mkdir $(OBJD)\DATAFLOW\DFEVAL
        @if not exist $(OUTD)              mkdir $(OUTD)
        @if not exist $(OUTD)\NT           mkdir $(OUTD)\NT
        @if not exist $(RESD)              mkdir $(RESD)
        @if not exist $(RESD)\DATAFLOW     mkdir $(RESD)\DATAFLOW
        @if not exist $(RESD)\DATAFLOW\DFEVAL     mkdir $(RESD)\DATAFLOW\DFEVAL
        @echo Depend done

compile: setup
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        -@m.cmd compile
        @echo Compile done

help:
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        @echo Help done

library:
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        -@m.cmd library
        @echo Library done

exedll: setup
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        @echo ExeDll done

clean:
        @cd $(SRCD)\dataflow\dfeval
        @$(SRCDRIVE)
        -@m.cmd clean
        @if exist $(OBJD)\DFEVAL\DFEval.mk erase $(OBJD)\DFEVAL\DFEval.mk
        @echo Clean done
!ELSE
clean:
        @echo Only done for LANG1=ea and ja

depend:
        @echo Only done for LANG1=ea and ja

compile:
        @echo Only done for LANG1=ea and ja

library:
        @echo Only done for LANG1=ea and ja

help:
        @echo Only done for LANG1=ea and ja

exedll:
        @echo Only done for LANG1=ea and ja

!ENDIF

#---------------------------------------------------------
# Makefile ends.
#---------------------------------------------------------
