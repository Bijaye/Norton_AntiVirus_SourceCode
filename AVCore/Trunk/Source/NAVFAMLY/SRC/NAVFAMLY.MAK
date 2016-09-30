#-------------------------------------------------------------------------
#                                                                       
# $Header:
#                                                                       
# Description:                                                          
#       symdebug makefile.
#                                                                       
#-------------------------------------------------------------------------
# $Log:
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Setting TARGETTYPE=DLL and using a target name with a .LIB extension
# allows .DLL optimization flags and still creates a static lib to
# link other files to create a .DLL. This is how the build system
# currently works.                                      KLM 10/21/94
#-------------------------------------------------------------------------

all:
    @echo NAVFAMLY is obsolete. Build the AVAPI1 project instead.


%If ("$[u,$(PLATFORM)]" == ".WIN")

    MODEL=L
    TARGET_NAME= N16UTILS
    TARGET_TYPE= DLL

%ElseIf ("$[u,$(PLATFORM)]" == ".W32")

    TARGET_NAME= N32UTILS
    TARGET_TYPE= LIB

%Else

    TARGET_TYPE= LIB

%EndIf

#-------------------------------------------------------------------------

OBJS= \
 stack.obj \
 copyfile.obj \
 navbtmap.obj \
 navgraph.obj \
%If ("$[u,$(PLATFORM)]" == ".W32")
 navanim.obj \
%EndIf
 findall.obj

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)


#UPDATE#
stack.obj:	    platform.h stack.hpp 
copyfile.obj:	platform.h xapi.h file.h copyfile.h 
navgraph.obj:   platform.h navgraph.hpp
navbtmap.obj:   platform.h navbtmap.h
findall.obj:    platform.h findall.hpp
navanim.obj:    platform.h stack.hpp navgraph.hpp navanim.hpp
#ENDUPDATE#


