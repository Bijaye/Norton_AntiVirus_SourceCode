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
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        @if not exist $(OBJD)              mkdir $(OBJD)
        @if not exist $(OBJD)\AVISMonitorLib  mkdir $(OBJD)\AVISMonitorLib
        @if not exist $(LIBD)              mkdir $(LIBD)
        @echo Depend done

compile: setup
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        -@AVISMLib.cmd compile
        @echo Compile done

help: setup
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        @echo Help done

library: setup
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        -@AVISMLib.cmd library
        @echo Library done

exedll: setup
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        @echo ExeDll done

clean: setup
        @cd $(SRCD)\AVISMonitorLib
        @$(SRCDRIVE)
        -@AVISMLib.cmd clean
        @if exist $(OBJD)\AVISMonitorLib\AVISMonitorLib.mk erase $(OBJD)\AVISMonitorLib\AVISMonitorLib.mk
        @echo Clean done

#---------------------------------------------------------
# Makefile ends.
#---------------------------------------------------------
