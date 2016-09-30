#####################################################################
## NAVALERT.MAK - Created - 10/28/96
##
## Copyright 1992-1996 Symantec, Peter Norton Product Group
#####################################################################
## $Header:   S:/NAVALERT/VCS/NAVALERT.MAv   1.5   13 Jun 1997 22:07:52   JBRENNA  $
##
## Description:
##    NAVALERT makefile.
##
#####################################################################
##
## $Log:   S:/NAVALERT/VCS/NAVALERT.MAv  $
#  
#     Rev 1.5   13 Jun 1997 22:07:52   JBRENNA
#  Add AT_OPT95.OBJ to the object list.
#
#     Rev 1.4   22 May 1997 14:30:34   DSACKIN
#  We added support to now build for either ANSI or UNICODE.
#
#     Rev 1.3   19 May 1997 17:43:24   BILL
#  Removed -Zi
#
#     Rev 1.2   09 May 1997 19:14:46   GDZIECI
#  Added AT_SNMP for SNMP trap targets.
#
#     Rev 1.1   29 Apr 1997 15:48:50   JBRENNA
#  Convert W32 platform check to use CORE_WIN32 check instead.
#
#     Rev 1.0   18 Apr 1997 13:56:52   DSACKIN
#  Ported from Tesla
#
#     Rev 1.0   13 Mar 1997 15:11:48   RFULLER
#  Initial revision
#
#     Rev 1.9   04 Feb 1997 15:32:52   ksackin
#  Updated the dependencies for the NavAlert.rc file. (New version information)
#
#     Rev 1.8   12 Dec 1996 12:49:36   TSmith
#  Added support for module 'AT_ALOG.CPP'.
#
#     Rev 1.7   05 Dec 1996 01:50:42   DSACKIN
#  Added at_smtp.obj
#
#     Rev 1.6   24 Nov 1996 21:04:54   ksackin
#  Added defines to remove OCC, OCX, and DOC support.
#
#     Rev 1.5   24 Nov 1996 21:02:54   ksackin
#  Fixed packing problem, as well as uncommented UNICODE defines.
#
#     Rev 1.4   23 Nov 1996 20:06:24   ksackin
#  Made temporary fix to packing alignment by adding a -zp8 to the end of the
#  clopts var.  This will still give a warning, but until the PACKING_ALIGNMENT
#  var is fixed and used, this will have to do.
#
#     Rev 1.3   13 Nov 1996 21:14:30   KSACKIN
#  Changed compilation options to be UNICODE instead of MBCS.
#
#     Rev 1.2   12 Nov 1996 23:21:44   KSACKIN
#  Fixed makefile so that it will compile correctly with the non-debug version
#  of MFC.
#
#     Rev 1.1   12 Nov 1996 22:57:20   KSACKIN
#  Changed DLL project to be an MFC Extension DLL, and also changed the packing
#  to be 8 in order to function with MFC.
#
#     Rev 1.3   07 Nov 1996 01:19:34   KSACKIN
#  Moved the UNICODE defines to a unicode section.
#
#     Rev 1.2   07 Nov 1996 01:11:32   KSACKIN
#  Cleaned up make file, as well as added support for debug and non debug MFC
#  libraries. (as well as build targets)
#
#     Rev 1.1   06 Nov 1996 18:25:26   KSACKIN
#  Now link with static MFC libraries, instead of dynamic.
#
#     Rev 1.0   29 Oct 1996 15:35:30   KSACKIN
#  Initial revision.
##
#####################################################################

%If ("$[u,$(CORE_CHARSET)]" == "U")

   TARGET_NAME=XNTALERT

%Elseif ("$[u,$(CORE_CHARSET)]" == "A")

   TARGET_NAME=N32ALERT

%Else

    beep
    echo the CORE_CHARSET variable set to $(CORE_CHARSET) is not supported for the project.

%endif

#####################################################################
## General Switches


XOPTS += -GX

PACKING_ALIGNMENT=8

                                        ## No OLE, OCC, or DOCOBJECT support
XOpts += /D_AFX_NO_OCC_SUPPORT /D_AFX_NO_OLE_SUPPORT /D_AFX_NO_DOCOBJECT_SUPPORT

                                        ## Character set options.
# CHAR_SET=U
# CHAR_SET=A


TARGET_TYPE=DLL


#####################################################################
## Quake Defines for proper MFC libraries.

%If "$(CORE_WIN32)" != ""

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

%if "$(CV)" != ""
        CLOPTS += -Zp8
#        CLOPTS += -Zi
%else
        CLOPTS += -Zp8
%endif

#####################################################################
## Object Modules.


OBJS  = at_opts.obj
OBJS += at_tgt.obj
OBJS += at_list.obj
OBJS += at_count.obj
OBJS += at_elog.obj
OBJS += at_alog.obj
OBJS += at_email.obj
OBJS += at_fwd.obj
OBJS += at_lmsg.obj
OBJS += at_nlm.obj
OBJS += at_nmsg.obj
OBJS += at_pager.obj
OBJS += at_smtp.obj
OBJS += at_snmp.obj
OBJS += at_opt95.obj
OBJS += dllmain.obj


#####################################################################
## Library Modules.

                                        ## Common Libs.

LIBS += comctl32.lib


#####################################################################
## Target Specification


PROGRAM_NAME=$(TARGET_NAME).DLL


$(PROGRAM_NAME): $(OBJS) \
                 $(LIBS) \
                 NAVALERT.RES \
                 NAVALERT.DEF


#####################################################################
## Specific Updates.


#UPDATE#

AT_OPTS.OBJ:    AT_OPTS.CPP AT_OPTS.H AT_TGT.H
AT_TGT.OBJ:     AT_TGT.CPP AT_OPTS.H AT_TGT.H
AT_LIST.OBJ:    AT_LIST.CPP AT_TGT.H
AT_COUNT.OBJ:   AT_COUNT.CPP AT_TGT.H
AT_EMAIL.obj:   AT_EMAIL.CPP AT_TGT.H
AT_ELOG.OBJ:    AT_ELOG.CPP AT_TGT.H
AT_ALOG.OBJ:    AT_ALOG.CPP AT_TGT.H
AT_FWD.OBJ:     AT_FWD.CPP AT_TGT.H
AT_LMSG.OBJ:    AT_LMSG.CPP AT_TGT.H
AT_NMSG.OBJ:    AT_NMSG.CPP AT_TGT.H
AT_NLM.OBJ:     AT_NLM.CPP AT_TGT.H
AT_PAGER.obj:   AT_PAGER.CPP AT_TGT.H
AT_SMTP.OBJ:    AT_SMTP.CPP AT_OPTS.H AT_TGT.H
AT_SNMP.OBJ:    AT_SNMP.CPP AT_TGT.H
DLLMAIN.OBJ:    DLLMAIN.CPP
NAVALERT.RES:   NAVALERT.RC nav.ver verdefs.h

#ENDUPDATE#
