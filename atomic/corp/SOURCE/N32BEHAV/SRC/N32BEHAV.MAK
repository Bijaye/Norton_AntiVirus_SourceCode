#-------------------------------------------------------------------------
#
# $Header:   S:/N32BEHAV/VCS/n32behav.mav   1.2   03 Sep 1998 12:35:42   rchinta  $
#
# Description:
#       Behavior dll makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/N32BEHAV/VCS/n32behav.mav  $
#  
#     Rev 1.2   03 Sep 1998 12:35:42   rchinta
#  Added a new TARGET, i.e SRVR_OEM
#  
#     Rev 1.1   04 Nov 1997 00:22:54   jtaylor
#  Added oem target define statment.
#
#     Rev 1.0   23 May 1997 14:30:34   DBUCHES
#  Initial revision.
#
#  Added files to scanner for handling of the Netscape downloading process.
#-------------------------------------------------------------------------

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=N32PDLL.DLL

DEF+= /DBUILDING_N32BEHAV

%if ("$[u,$(TARGET)]" == "SERVER")
DEF+= /DNAV_SERVER
%endif

# This is equivalent to defining WORKSTAT_OEM
%if ("$[u,$(TARGET)]" == "OEM")
DEF+= /DNAV_OEM
%endif

%if ("$[u,$(TARGET)]" == "SRVR_OEM")
DEF+= /DNAV_SERVER /DNAV_OEM
%endif


$(PROGRAM_NAME):	dllmain.obj \
                        n32behav.res

#UPDATE#
#ENDUPDATE#

