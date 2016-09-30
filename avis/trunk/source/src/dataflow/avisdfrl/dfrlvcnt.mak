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
	set NO_EXTERNAL_DEPS=1

depend: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OBJD)\DATAFLOW     mkdir $(OBJD)\DATAFLOW
        @if not exist $(OBJD)\DATAFLOW\AVISDFRL mkdir $(OBJD)\DATAFLOW\AVISDFRL
        @if not exist $(OUTD)              mkdir $(OUTD)
        @if not exist $(OUTD)\NT           mkdir $(OUTD)\NT

        @echo Depend done

compile: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        -@AVISDFRL.cmd compile
        @echo Compile done

help: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        @echo Help done

library: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        -@AVISDFRL.cmd library
        @echo Library done

exedll: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        -@AVISDFRL.cmd exedll
        @echo ExeDll done

clean: setup
        @cd $(SRCD)\dataflow\AVISDFRL
        @$(SRCDRIVE)
        -@AVISDFRL.cmd clean
        @if exist $(OBJD)\AVISDFRL\AVISDFRL.mk erase $(OBJD)\AVISDFRL\AVISDFRL.mk
        @echo Clean done

#---------------------------------------------------------
# Makefile ends.
#---------------------------------------------------------
