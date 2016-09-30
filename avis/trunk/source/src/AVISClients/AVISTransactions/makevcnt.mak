#
#	MS VC++ 6.0 Makefile Template
#
#	By Andy Klapper
#
#	Please modify this template to fit your project
#	I have tested this template with DLL and EXE builds
#
#	This make file uses the following environment variables
#		BLDDEBUG	if exists & not null build debug version
#		UTILD		directory where project manipulating perl script is
#		SRCD		directory where the source is
#		OBJD		where to put the final output
#		OUTD		where to put the intermediate files
#		TMP		where to put temporary files
#
#		path		path to search for binaries
#		include		path to search for includes
#		lib		path to search for libraries


.IGNORE:

#
#  In most cases you should only have to define the project name.
#  In those cases where the project name and the directory name are
#  different then define the DIRNAME separately.

PROJECTNAME=AVISTransactions
DIRNAME=AVISClients\$(PROJECTNAME)

OBJD=$(OBJD)\$(DIRNAME)
OUTD=$(OUTD)\nt
LOGFILE=$(OBJD)\logFile.txt

!IF "$(BLDDEBUG)" == ""
CFG=Win32 Release
!ELSE
CFG=Win32 Debug
!ENDIF

FULL_CFG="$(PROJECTNAME) - $(CFG)"

MODIFYPROJECT=perl.exe $(UTILD)\SetVCProjDirectories.pl


clean:
	@cd $(SRCD)\$(DIRNAME)
	@Set MAKCMDS=/CLEAN
	@Set BLDTARGET=clean
	erase $(OBJD)\$(PROJECTNAME)*.dsp 2> $(TMP)\null
	@$(MAKE) -f makevcnt.mak doit /E
	@erase $(LIBD)\$(PROJECTNAME).lib > $(TMP)\null
	@erase $(LIBD)\$(PROJECTNAME).exp > $(TMP)\null


#	Dependency info is stored and maintained in the project file
depend:


#	This builds everything (library and dll), comment out for
#	exe files

compile:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@set BLDTARGET=compile
#	@$(MAKE) -f makevcnt.mak doit /E


library:
	@cd $(SRCD)\$(DIRNAME)
	@set MAKECMDS=
	@set BLDTARGET=library
	@$(MAKE) -f makevcnt.mak doit /E

help:


#	This builds everything including the exe (comment out for
#	dlls)

exedll:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@set BLDTARGET=exedll
#	@$(MAKE) -f makevcnt.mak doit /E


doit:
	@mkdir $(OBJD) 2> $(TMP)\null
	@$(MODIFYPROJECT) $(PROJECTNAME) "$(CFG)" $(OUTD) $(OBJD)
	msdev.exe $(PROJECTNAME)-tmp.dsp /MAKE $(FULL_CFG) /USEENV $(MAKCMDS) /OUT $(LOGFILE)
	@type $(LOGFILE)
	@erase $(LOGFILE)
	@erase $(OBJD)\$(PROJECTNAME).dsp 2> $(TMP)\null
	@rmdir /S /Q $$(OUTD) 2> $(TMP)\null
	@move $(PROJECTNAME)-tmp.dsp $(OBJD)\$(PROJECTNAME)-$(BLDTARGET).dsp > $(TMP)\null
	@move $(OUTD)\$(PROJECTNAME).lib $(LIBD) > $(TMP)\null
	@move $(OUTD)\$(PROJECTNAME).exp $(LIBD) > $(TMP)\null
