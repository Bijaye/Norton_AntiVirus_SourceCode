#########################################################################
#
#
# $Header:   S:/NAVLU/VCS/NavLuUtils.mav   1.5   03 Jun 1997 14:30:58   JBRENNA  $
#
#########################################################################
# $Log:   S:/NAVLU/VCS/NavLuUtils.mav  $ #
#  
#     Rev 1.5   03 Jun 1997 14:30:58   JBRENNA
#  Update dependencies.
#  
#     Rev 1.4   03 Jun 1997 13:59:04   JBRENNA
#  Define _MBCS.
#  
#     Rev 1.3   02 Jun 1997 18:23:04   JBRENNA
#  Updated to remove MFC and NavLu.OBJ from linked. They are no longer needed
#  because the necessary NavLu.OBJ functionality was moved to NavLuUtils.OBJ.
#  
#     Rev 1.2   16 May 1997 11:26:20   JBRENNA
#  Remove SymKrnl from the build. It is no longer necessary.
#  
#     Rev 1.1   16 May 1997 11:20:30   JBRENNA
#  
#  Add support for MFC.
#  
#     Rev 1.0   10 May 1997 01:21:18   JBRENNA
#  Initial revision.
#  
#########################################################################

TARGET_TYPE=LIB
PROGRAM_NAME = NavLuUtils.LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

XOPTS += /D_MBCS

$(PROGRAM_NAME):        \
    NavLuUtils.obj

#UPDATE#
navluutils.obj:	StdAfx.h platform.h dbcs_str.h dosvmm.h nlm_str.h file.h \
		xapi.h NavLuUtils.h strings.h 
#ENDUPDATE#
