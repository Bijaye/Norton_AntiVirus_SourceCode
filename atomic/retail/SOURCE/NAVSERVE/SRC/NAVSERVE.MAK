#########################################################################
# Copyright 1993 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/NAVSERVE/VCS/navserve.mav   1.13   22 Dec 1997 17:28:54   CEATON  $
#                                                                       #
# Description:                                                          #
#       Makes the SERVENFY.DLL which is the Utility DLL for the Norton  #
# AntiVirus for Windows and its related Dynamic Link Libraries.         #
#       Also makes the static link library, SERVENFY.LIB, for DOS       #
#       programs.                                                       #
#                                                                       #
#########################################################################
# $Log:   S:/NAVSERVE/VCS/navserve.mav  $
#  
#     Rev 1.13   22 Dec 1997 17:28:54   CEATON
#  Removed WSOCK32.LIB from the list of libraries to search.
#  
#  
#     Rev 1.12   19 Jun 1997 09:54:08   DSACKIN
#  Cleaned up a comment and changed UNILIB to UNIPATH for consistancy
#
#     Rev 1.11   18 Jun 1997 21:20:28   DSACKIN
#  Switched to a MFC DLL, re-organized the OBJS/LIBS thing and added
#  alert95.obj.
#
#     Rev 1.10   04 May 1997 17:59:38   DSACKIN
#  WIN95/WINNT need to link to different NAVXUTIL DLLs.
#
#     Rev 1.9   02 May 1997 18:26:26   ksackin
#  Added target specification for NAVW and NAVWNT so as to change the navserve
#  DLL to XNTSERVE.DLL for the NAVWNT target.
#
#     Rev 1.8   30 Apr 1997 12:29:28   JBRENNA
#  Correct some problems with the last change. No longer use TARGET_NAME when
#  doing the dependency for the RES and DEF files.
#
#     Rev 1.7   30 Apr 1997 12:21:06   JBRENNA
#  Change TARGET_NAME to N32SERVE for Win32 platform.
#
#     Rev 1.6   30 Apr 1997 09:41:24   JBRENNA
#  Convert to use new CORE_* platform spec.
#
#     Rev 1.5   18 Apr 1997 14:01:20   DSACKIN
#  Ported from Tesla
#
#     Rev 1.1   19 Mar 1997 13:58:38   DSACKIN
#  Changed s32nave to $(SYM_KERNL)
#
#     Rev 1.0   13 Mar 1997 15:13:56   RFULLER
#  Initial revision
#
#     Rev 1.1   13 Dec 1996 13:58:06   DSACKIN
#  Added NLMEnum.obj as one of the files we need to include.
#
#     Rev 1.0   08 Nov 1996 11:53:08   DKEEFER
#  Initial revision
#
#     Rev 1.7   20 Aug 1996 02:33:58   DSACKIN
#  Added NLMMsgr.obj.
#
#     Rev 1.6   25 Jul 1996 13:54:26   PVENKAT
#  Added MPR.LIB to enumerate Netware Servers.
#
#     Rev 1.5   30 May 1996 20:04:00   jmillar
#  change S32NAVD to S32NAVE to match last core branch
#
#     Rev 1.4   22 May 1996 13:50:52   jmillar
#  fix fat finger in the last fix - should have been xntxutil.lib
#
#     Rev 1.3   22 May 1996 09:15:14   jmillar
#  don't need netalert.obj - we aren't currently using this, and symnet isn't
#  really available for Win32 - delete symnet from the build as well
#
#     Rev 1.2   22 May 1996 09:05:48   jmillar
#  change dependency from symkrnl to s32navd - nav doesn't use symkrnl
#
#     Rev 1.1   17 May 1996 14:17:52   PVENKAT
#  Added nntxutil.lib, wsock32.lib and removed n32thnk.lib
#
#     Rev 1.0   30 Jan 1996 15:54:18   BARRY
#  Initial revision.
#
#     Rev 1.0   30 Jan 1996 14:39:56   BARRY
#  Initial revision.
#########################################################################


#####################################################################
## Setting up a Target Name and platform and then including
## Platform.mak

%If ("$(PLATFORM)" == ".DOS")

   TARGET_TYPE=LIB

%ElseIf ("$(CORE_WIN32)" != "")

   TARGET_TYPE=DLL
   USE_MFC_LIB=YES
   USE_DEFAULT_LIBS=YES

   %If ("$[u,$(TARGET)]" == "NAVWNT")

   TARGET_NAME=XNTSERVE

   %ElseIf ("$[u,$(TARGET)]" == "NAVW")

   TARGET_NAME=N32SERVE

   %EndIf

%ElseIf ("$(CORE_WIN16)" != "")

TARGET_TYPE=DLL

%EndIf

                                        ## Now set the program name
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

                                        ## Include platform.mak
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

#####################################################################
## Object files


%If ("$(PLATFORM)" == ".DOS")

                                        ## For use with Windows Programs only!
   STUB  =

   OBJS  = servenfy.obj
   OBJS += serv_dll.obj

   LIBS  =

   RESS  =

   DEFS  =

%ElseIf ("$(CORE_WIN32)" != "")

UNIPATH  = $(PROJPRE)LIB.$(CORE_PLAT)$(CORE_BMODE)U
UNILIBS  = $(UNIPATH)\CAlert.LIB
UNILIBS += $(UNIPATH)\CVAlert.LIB

   %If ("$[u,$(TARGET)]" == "NAVWNT")

      NAVXUTIL=XNTXUTIL

   %ElseIf ("$[u,$(TARGET)]" == "NAVW")

      NAVXUTIL=N32XUTIL

   %EndIf

   OBJS  = servenfy.obj
   OBJS += NLMMsgr.obj
   OBJS += NLMENUM.obj
   OBJS += Alert95.obj

   LIBS  = $(NAVXUTIL).LIB
   LIBS += $(Q_SYMKRNL).LIB
   LIBS += MPR.LIB
                                        ## Virus Alert stuff
   LIBS += N32Alert.LIB
   LIBS += RPCRT4.LIB
   LIBS += $(UNILIBS)

   RESS += NAVSERVE.RES

   DEFS += NAVSERVE.DEF

%ElseIf ("$(CORE_WIN16)" != "")

   OBJS  = servenfy.obj
   OBJS += serv_dll.obj

   LIBS  = $(Q_SYMNET).LIB
   LIBS += $(Q_SYMKRNL).LIB

   RESS += NAVSERVE.RES

   DEFS += NAVSERVE.DEF

%EndIf


$(PROGRAM_NAME): $(STUB) $(OBJS) $(LIBS) $(RESS) $(DEFS)


#UPDATE#
serv_dll.obj:	platform.h dbcs_str.h
servenfy.obj:	platform.h dbcs_str.h symnet.h sympipe.h symtime.h \
		servenfy.h
netalert.obj:	dos.h platform.h dbcs_str.h xapi.h dosnet.h stddos.h \
		ibmpc.h undoc.h symintl.h navutil.h ctsn.h options.h \
		navopts.h exclude.h cts4.h netobj.h symnet.h register.h \
		symcfg.h nonav.h syminteg.h netalert.h servenfy.h navserid.h \
		stdio.h
NLMMsgr.obj:    platform.h NLMMsgr.h servenfy.h
#ENDUPDATE#
