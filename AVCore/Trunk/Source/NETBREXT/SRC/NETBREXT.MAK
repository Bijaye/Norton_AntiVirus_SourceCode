#####################################################################
## NETBREXT.MAK - Created - 11/13/96
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/NETBREXT/VCS/netbrext.mav   1.4   16 Dec 1997 12:24:46   KSACKIN  $
##
## Description:
##    Network Browser Extension DLL makefile.  This makefile will
##    take the browser from the NetBrows application and pack
##    it into an MFC Extension Dll.
##
#####################################################################
##
## $Log:   S:/NETBREXT/VCS/netbrext.mav  $
#  
#     Rev 1.4   16 Dec 1997 12:24:46   KSACKIN
#  Updated the target name for our UNICODE build to be NetBrExU.dll.
#
#     Rev 1.3   10 Dec 1997 20:46:04   KSACKIN
#  Now include the WndSize.lib file.  This will contain the CWindowResizer class
#  that we use with network browser dialog.
#
#     Rev 1.2   09 Dec 1997 12:27:28   DBuches
#  Removed SymNav dependencies.
#
#     Rev 1.1   21 Nov 1997 12:19:48   KSACKIN
#  Removed UNIUTIL.lib from the dependency list.
#
#     Rev 1.0   21 Nov 1997 11:52:06   KSACKIN
#  Initial revision.
#
#     Rev 1.4   15 Jun 1997 20:27:26   KSACKIN
#  Removed dependency on NavXUtil.dll and all other dependencies.  We will get
#  context menu help elsewhere, possibly from RoboHelp.
#
#     Rev 1.3   20 May 1997 18:33:58   BILL
#  Removed ZI
#
#     Rev 1.2   20 May 1997 12:24:40   DSACKIN
#  Updated this project to be able to build for ??A platforms.
#
#     Rev 1.1   29 Apr 1997 10:59:00   JBRENNA
#  Convert from W32 to new CORE_WIN32 platform specification.
#
#     Rev 1.0   18 Apr 1997 14:03:50   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:16:40   RFULLER
#  Initial revision
#
#     Rev 1.5   04 Feb 1997 14:54:34   ksackin
#  Added the new Version information header files to the list of dependencies
#  for our resource file.
#
#     Rev 1.4   03 Feb 1997 17:20:46   ksackin
#  Now including UniUtil.lib for Unicode Utility functions.
#
#     Rev 1.3   09 Jan 1997 03:36:46   ksackin
#  Added symutil and xntxutil to the make file for the Context and dialog help
#  routines.
#
#     Rev 1.2   24 Nov 1996 21:00:28   ksackin
#  Fixed packing problem, and also added defines for No OLE or OCC support.
#
#     Rev 1.1   20 Nov 1996 02:39:50   ksackin
#  Removed the .rc2 file from this make file.
#
#     Rev 1.0   20 Nov 1996 01:37:32   ksackin
#  Initial revision.
##
#####################################################################


#####################################################################
## General Switches


XOPTS += -GX

PACKING_ALIGNMENT=8

XOpts += /D_AFX_NO_OCC_SUPPORT /D_AFX_NO_OLE_SUPPORT /D_AFX_NO_DOCOBJECT_SUPPORT /DSYM_WIN /DSYM_WIN32 /DSYM_MFC

TARGET_TYPE=DLL


#####################################################################
## Quake Defines for proper MFC libraries.

%If "$(CORE_WIN32)" != ""

%If "$(CORE_CHARSET)" == "A"
    TARGET_NAME=NetBrExt
%Else
    TARGET_NAME=NetBrExU
    XOpt += /D_UNICODE /DUINCODE
%Endif

                                        ## Dynamic Linking...
                                        ## (Need to do this to be an AFX
                                        ## Extension DLL).
    BUILD_MFC_EXT=TRUE
    USE_MFC_DLL=TRUE
    USE_CRT_DLL=TRUE

                                        ## Static Linking
#    USE_MFC_LIB=TRUE
#    USE_CRT_LIB=TRUE

%Else

BAD_PLATFORM:
    beep
    echo the $(PLATFORM) platform is not supported for the project.

%Endif


#%if "$(CV)" != ""
#                                        ## Debug Versions of MFC
#        DEBUG_AFX=TRUE
#        DEBUG_CRT=TRUE
#%else
#        DEBUG_AFX=
#        DEBUG_CRT=
#%endif



#####################################################################
## Platform Quake Junk...

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

                                        ## This is to change the packing...
                                        ## (PACKING_ALIGNMENT does not work)

#%if "$(CV)" != ""
#        CLOPTS += -Zp8 -Zi
#%else
        CLOPTS += -Zp8
#%endif

#####################################################################
## Object Modules.


OBJS += NetTree.obj
OBJS += NetBrwsD.obj
OBJS += NResCont.obj
OBJS += stdafx.obj
OBJS += dispmsg.obj
OBJS += dllmain.obj


#####################################################################
## Library Modules.

                                        ## Common Libs.

LIBS += comctl32.lib
LIBS += wndsize.lib
LIBS += MPR.LIB


#####################################################################
## Target Specification


PROGRAM_NAME=$(TARGET_NAME).DLL


$(PROGRAM_NAME): $(OBJS) \
                 $(LIBS) \
                 NetBrExt.RES \
                 NetBrExt.DEF

#####################################################################
## Specific Updates.


#UPDATE#
NetTree.obj:    NetTree.cpp NetTree.h NResCont.h StdAfx.h
NetBrwsD.obj:   NetBrwsD.cpp DispMsg.h NetBrwsD.h NetTree.h NResCont.h \
                StdAfx.h platform.h wndsize.h stdhelp.h UniUtil.h
NResCont.obj:   NResCont.cpp NResCont.h StdAfx.h
StdAfx.obj:	StdAfx.cpp StdAfx.h
DISPMSG.OBJ:    DISPMSG.CPP
Dllmain.obj:    DLLMAIN.CPP
NetBrExt.res:   NetBrExt.rc NetBrExt.ico NetIcons.bmp verdefs.h \
#ENDUPDATE#

