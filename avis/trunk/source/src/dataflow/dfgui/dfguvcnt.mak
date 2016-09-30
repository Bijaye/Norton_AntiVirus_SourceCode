#---------------------------------------------------------
# Makefile begins.
#---------------------------------------------------------
# SCC: @(#) 18 1/29/99 14:45:27 AVNADMIN32 1.3 @(#)

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
        set NO_EXTERNAL_DEPS=1 

depend: setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OBJD)\DATAFLOW     mkdir $(OBJD)\DATAFLOW
        @if not exist $(OBJD)\DATAFLOW\DFGui mkdir $(OBJD)\DATAFLOW\DFGui
        @if not exist $(OUTD)              mkdir $(OUTD)
        @if not exist $(OUTD)\NT           mkdir $(OUTD)\NT
        @if not exist $(RESD)              mkdir $(RESD)
        @if not exist $(RESD)\DATAFLOW     mkdir $(RESD)\DATAFLOW
        @if not exist $(RESD)\DATAFLOW\DFGui     mkdir $(RESD)\DATAFLOW\DFGui
        @echo Depend done

compile: setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
        -@m.cmd compile
        @echo Compile done

help:   setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
        @echo Help done

library: setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
        @echo Library done

exedll: setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
	-@m.cmd exedll
        @echo ExeDll done

clean:  setup
        @cd $(SRCD)\dataflow\DFGui
        @$(SRCDRIVE)
        -@m.cmd clean
        @if exist $(OBJD)\DFGui\DFGui.mk erase $(OBJD)\DFGui\DFGui.mk
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
