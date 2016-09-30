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


OUTD=$(OUTD)\nt


clean:


#	Dependency info is stored and maintained in the project file
depend:


#	This builds everything (library and dll), comment out for
#	exe files

compile:
	@cd $(SRCD)\AVISdb\scripts
	@copy *.sql $(OUTD) > $(TMP)\null
	@copy *.bat $(OUTD) > $(TMP)\null


library:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@set BLDTARGET=library
#	@$(MAKE) -f makevcnt.mak doit /E

help:


#	This builds everything including the exe (comment out for
#	dlls)

exedll:
#	@cd $(SRCD)\$(DIRNAME)
#	@set MAKECMDS=
#	@set BLDTARGET=exedll
#	@$(MAKE) -f makevcnt.mak doit /E

