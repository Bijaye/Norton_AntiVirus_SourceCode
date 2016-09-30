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

PROJECTNAME=AVISDFScan
DIRNAME=$(PROJECTNAME)

OBJD=$(OBJD)\$(DIRNAME)
OUTD=$(OUTD)\nt
LOGFILE=$(OBJD)\$(DIRNAME)\logFile.txt

!IF "$(BLDDEBUG)" == ""
CFG=Win32 Release
!ELSE
CFG=Win32 Debug
!ENDIF

FULL_CFG="$(PROJECTNAME) - $(CFG)"

MODIFYPROJECT=perl.exe $(UTILD)\SetVCProjDirectories.pl


clean:
	@$(SRCDRIVE)
	@cd $(SRCD)\$(DIRNAME)
	@Set MAKCMDS=/CLEAN
	@$(MAKE) -f makevcnt.mak doit /E
	@rmdir /s /q $(OBJD)


#	Dependency info is stored and maintained in the project file
depend:


#	This builds everything (library and dll), comment out for
#	exe files

compile:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@$(MAKE) -f makevcnt.mak doit /E


library:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@$(MAKE) -f makevcnt.mak doit /E

help:


#	This builds everything including the exe (comment out for
#	dlls)

exedll:
	@cd $(SRCD)\$(DIRNAME)
	@set MAKECMDS=
	@$(MAKE) -f makevcnt.mak doit /E


doit: $(PROJECTNAME)-tmp.dsp
	msdev.exe $(PROJECTNAME)-tmp.dsp /MAKE $(FULL_CFG) /USEENV $(MAKCMDS) /OUT $(LOGFILE)
	@erase $(PROJECTNAME)-tmp.dsp
	@type $(LOGFILE)
	@erase $(LOGFILE)
	@if exist $(PROJECTNAME)-tmp.plg erase $(PROJECTNAME)-tmp.plg

$(PROJECTNAME)-tmp.dsp: $(PROJECTNAME).dsp
	@if not exist $(OBJD) mkdir $(OBJD) 
	@if not exist $(OBJD)\$(DIRNAME) mkdir $(OBJD)\$(DIRNAME)
	@if not exist $(OUTD) mkdir $(OUTD)
	$(MODIFYPROJECT) $(PROJECTNAME) "$(CFG)" $(OUTD) $(OBJD)
