#-------------------------------------------------------------------------
#                                                                       
# $Header:   S:/AVAPI1/VCS/navfamly.mav   1.14   13 May 1997 18:05:18   DALLEE  $
#                                                                       
# Description:
#       NAV utility library makefile.
#                                                                       
#-------------------------------------------------------------------------
# $Log:   S:/AVAPI1/VCS/navfamly.mav  $
#  
#     Rev 1.14   13 May 1997 18:05:18   DALLEE
#  Added ENUMDIR.OBJ and ENUMFILE.OBJ to W32 compiles.
#  
#     Rev 1.13   11 May 1997 20:48:02   DALLEE
#  Additions to work for VXD and NTK -- only compile fname.obj and match.obj
#  objects for these platforms.
#  
#     Rev 1.12   11 May 1997 20:02:26   DALLEE
#  Oops! I had CORE_W32 instead of CORE_WIN32 in one spot last rev.
#  
#     Rev 1.11   11 May 1997 19:48:46   DALLEE
#  Added MATCH.OBJ and FNAME.OBJ -- filename manipulation and matching routines.
#  Also added DX support.
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Setting TARGETTYPE=DLL and using a target name with a .LIB extension
# allows .DLL optimization flags and still creates a static lib to
# link other files to create a .DLL. This is how the build system
# currently works.                                      KLM 10/21/94
#-------------------------------------------------------------------------

%If ("$(CORE_WIN16)" != "")

    MODEL=L
    TARGET_NAME=N16UTILS
    TARGET_TYPE=DLL

%ElseIf ("$(CORE_WIN32)" != "")

    TARGET_NAME=N32UTILS
    TARGET_TYPE=LIB

%ElseIf %member( $(PLATFORM), .DX .DVX .NCX )

    TARGET_NAME=N16UTILS
    TARGET_TYPE=LIB

%Else

Bad_Platform:
    echo the ($PLATFORM) platform is not supported by NAVFAMLY.MAK

%EndIf

#-------------------------------------------------------------------------

OBJS= \
%if "$(CORE_WIN16)" != "" || ( "$(CORE_WIN32)" != ""  && "$(CORE_SOFT)" == "" )
  stack.obj \
  copyfile.obj \
  navbtmap.obj \
  navgraph.obj \
  %If ("$(CORE_WIN32)" != "")
    enumdir.obj \
    enumfile.obj \
    navanim.obj \
  %EndIf
  findall.obj \
%endif
  fname.obj \
  match.obj

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)


#UPDATE#
stack.obj:      platform.h stack.hpp
copyfile.obj:	platform.h xapi.h file.h copyfile.h 
navgraph.obj:   platform.h navgraph.hpp
navbtmap.obj:   platform.h navbtmap.h
findall.obj:    platform.h findall.hpp
navanim.obj:    platform.h stack.hpp navgraph.hpp navanim.hpp
fname.obj:      platform.h xapi.h file.h fname.h
match.obj:      platform.h fname.h
#ENDUPDATE#


