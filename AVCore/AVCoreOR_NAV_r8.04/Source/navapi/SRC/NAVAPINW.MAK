#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAPI/VCS/navapinw.mav   1.5   03 Sep 1998 16:52:10   dhertel  $
#
# NAV API Library NLM (NAVAPI.NLM) makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVAPI/VCS/navapinw.mav  $
#  
#     Rev 1.5   03 Sep 1998 16:52:10   dhertel
#  Removed last fix, it's screwing up the build system
#  
#     Rev 1.4   02 Sep 1998 13:01:06   dhertel
#  Added dependency on navapi.lib
#  
#     Rev 1.3   13 Aug 1998 16:59:48   DHERTEL
#  Changed the name of NAVLIB.NLM to NAVAPI.NLM, and renamed NAVLIB.* source
#  files to NAVAPI (where possible) or NAVAPINW (where necessary)
#  
#     Rev 1.2   11 Aug 1998 17:34:38   spasnik
#  Goody! put navapi.lib into a syslib line to prevent
#  the idiot builtins from trying to compile it from the .def
#  file of the same name.
#  
#     Rev 1.1   05 Aug 1998 14:24:36   dhertel
#  
#     Rev 1.0   30 Jul 1998 21:53:24   DHERTEL
#  Initial revision.
#-------------------------------------------------------------------------

%If ("$[u,$(PLATFORM)]" == ".NLM")

TARGET_NAME=NAVAPI
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

SYSLIBS=NAVAPI.LIB

$(TARGET_NAME).nlm: \
    navapinw.den \
    navapinw.obj \
    navapi.imp \
    navapi.exp \
    $(Q_SYMKRNL).LIB 

 
%Else

navlib:
    Echo Nothing to build for this plaftform

%Endif


