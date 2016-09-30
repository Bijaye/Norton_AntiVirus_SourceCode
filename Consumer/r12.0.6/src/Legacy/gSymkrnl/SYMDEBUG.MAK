#-------------------------------------------------------------------------
#                                                                       
# $Header:   S:/SYMKRNL/VCS/symdebug.mav   1.3   05 Dec 1996 16:37:28   MSPIEGE  $
#                                                                       
# Description:                                                          
#       symdebug makefile.
#                                                                       
#-------------------------------------------------------------------------
# $Log:   S:/SYMKRNL/VCS/symdebug.mav  $
#  
#     Rev 1.3   05 Dec 1996 16:37:28   MSPIEGE
#  Now USE_CRT_LIB rather than USE_CRT_DLL because this is what symkrnl
#  is now doing.  Copied Bruce's change from Symkrnl to here.
#  
#  
#     Rev 1.2   02 Dec 1996 16:45:28   BMCCORK
#  Added test platform support
#  
#     Rev 1.1   05 Aug 1996 18:52:32   BMCCORK
#  Changed to use CRT DLL
#  
#     Rev 1.0   26 Jan 1996 20:22:44   JREARDON
#  Initial revision.
#  
#     Rev 1.5   08 Nov 1994 11:38:00   BRUCE
#  Eliminated unused macros and simplified.  Also corrected targettype to be DLL
#  
#     Rev 1.4   02 Sep 1994 14:51:44   CAREY
#  Build for .DX platform, too.
#  Use Q_SYMDEBUG for program name.
#-------------------------------------------------------------------------

%If "$(PLATFORM)" != ".OS2"
    TARGET_TYPE=DLL
%Else
    TARGET_TYPE=LIB
%Endif

## Statically link with the CRT DLL to avoid a bug in the Windows DLL processing
## Calling FreeLibrary() on the thunk DLL from Symkrnl can cause the CRT DLL to
## unload while Symkrnl is still using it!!  This can be avoided by statically linking.
## Since Symkrnl now has to link staticly and may be used by symkrnl, we don't
## want to take a chance that it will pull in the dll so we will static link
## the runtime as well.
##USE_CRT_DLL=TRUE
USE_CRT_LIB=TRUE

.INCLUDE <PLATFORM.MAK>

$(Q_SYMDEBUG).lib: symasser.obj


#UPDATE#
symasser.obj:	platform.h
#ENDUPDATE#
