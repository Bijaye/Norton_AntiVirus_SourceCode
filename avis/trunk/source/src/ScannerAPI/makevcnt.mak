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
#		path		path to search for binaries
#		include		path to search for includes
#		lib		path to search for libraries

.IGNORE:

#  In most cases you should only have to define the project name.
#  In those cases where the project name and the directory name are
#  different then define the DIRNAME separately.

PROJECTNAME=ScannerAPI
DIRNAME=$(PROJECTNAME)

OBJD=$(OBJD)\$(DIRNAME)
OUTD=$(OUTD)\nt
LOGFILE=$(OBJD)\logFile.txt

!IF "$(BLDDEBUG)" == ""
DIRNAME=$(DIRNAME)\Release
!ELSE
DIRNAME=$(DIRNAME)\Debug
!ENDIF

#	This project does not require any clean-up
clean:
	

#	This project does not build any dependency information
depend:


#	This project does not build any OBJ or LIB or DLL files
compile:


#	Pre-compiled binaries must be copied to the appropriate output directories
library:
	copy $(SRCD)\$(DIRNAME)\*.lib $(LIBD)
	copy $(SRCD)\$(DIRNAME)\*.dll $(OUTD)
	copy $(SRCD)\$(DIRNAME)\*.vxd $(OUTD)


#	This project does not build any online help files
help:


#	This project does not build any EXE files
exedll:


doit:
