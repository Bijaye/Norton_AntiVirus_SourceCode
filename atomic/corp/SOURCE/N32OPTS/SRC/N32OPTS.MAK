##########################################################################
# $Header:   S:/N32OPTS/VCS/n32opts.mav   1.9   20 Oct 1998 20:34:18   CEATON  $
# MAKEFILE for N32OPTS.DLL - Configuruation options DLL for NAV
#
#
##########################################################################
# $Log:   S:/N32OPTS/VCS/n32opts.mav  $
#  
#     Rev 1.9   20 Oct 1998 20:34:18   CEATON
#  Added /DNAVWNT to builds for NT platform. Fixes problem where NAVOPTX.DAT is
#  created in root directory instead of NAV directory on NT.
#  
#  
#     Rev 1.8   19 Feb 1998 11:57:18   mdunn
#  Ported NEC changes from ROSNEC.
#  
#     Rev 1.1   23 Sep 1997 15:58:26   mdunn
#  Now linking with (n32|xnt)xutil.lib to get AlignWnd().
#  
#     Rev 1.0   12 Sep 1997 17:45:02   RFULLER
#  Initial revision
#  
#     Rev 1.7   25 Jun 1997 15:17:26   DBUCHES
#  Added iconbtn,OBJ.
#  
#     Rev 1.6   13 Jun 1997 22:26:58   JBRENNA
#  Add NETBREXT.LIB and N32ALERT.LIB to the LIBS.
#  
#     Rev 1.5   13 Jun 1997 13:51:30   DBUCHES
#  Added hounddlg.obj.
#  
#     Rev 1.4   27 May 1997 20:59:18   DBUCHES
#  Added personality DLL.
#  
#     Rev 1.3   21 May 1997 14:38:58   DBUCHES
#  Added CustResp.OBJ.
#
#     Rev 1.2   20 May 1997 18:49:54   KSACKIN
#  Updated .mak file to have targets.  This is so we can keep our dependencies
#  seperate for the Windows 95 and the Windows NT installs.
#
#     Rev 1.1   18 May 1997 23:28:34   DBUCHES
#  Added N32SERVE.LIB.
#
#     Rev 1.0   14 May 1997 15:10:46   DBUCHES
#  Initial revision.
##########################################################################
#BUILD_MFC_EXT=TRUE
TARGET_TYPE=DLL
USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

CLOPTS+= /GX
XOPTS+= /DBUILDING_DLL


                                        ## Common Libs
LIBS+= comctl32.lib
LIBS+= $(Q_SYMKRNL).LIB
LIBS+= n32pdll.lib						## Personality DLL.

%If ("$[u,$(TARGET)]" == "NAVW")

        LIBS+= n32exclu.lib
        LIBS+= n32serve.lib
        LIBS+= n32xutil.lib
        LIBS+= netbrext.lib

        PROGRAM_NAME=N32OPTS.DLL

%ElseIf ("$[u,$(TARGET)]" == "NAVWNT")

        LIBS+= xntexclu.lib
        LIBS+= xntserve.lib
        LIBS+= xntxutil.lib
        LIBS+= netbrext.lib

        PROGRAM_NAME=XNTOPTS.DLL
        DEF += /DNAVWNT
%Endif

LIBS+= n32alert.lib


$(PROGRAM_NAME):    AlertPge.OBJ	\
					APPage.OBJ      \
					cinclude.OBJ    \
					ExclPage.OBJ    \
					EXPORTS.OBJ     \
					ExtList.OBJ     \
					GenPage.OBJ     \
					HELPDLG.OBJ     \
					InocPage.OBJ    \
					LogPage.OBJ     \
					N32OPTS.OBJ     \
					PassPage.OBJ    \
					ScanPage.OBJ    \
					Sheet.OBJ       \
					Startpge.OBJ    \
					custresp.OBJ	\
					StdAfx.OBJ      \
					hounddlg.OBJ	\
					iconbtn.OBJ		\
					n32opts.res		\
                    n32opts.def

#UPDATE#
#ENDUPDATE#





