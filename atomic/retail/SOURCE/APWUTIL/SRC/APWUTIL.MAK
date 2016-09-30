########################################################################
#
# $Header:   S:/APWUTIL/VCS/APWUTIL.MAv   1.6   13 Aug 1997 20:44:52   DSACKIN  $
#
# Description:
#   Makefile for APWUTIL.DLL.
#   Utility fuctions for NAVAPW UI and NAVAPW APWCMD Dll.
#
########################################################################
# $Log:   S:/APWUTIL/VCS/APWUTIL.MAv  $
#  
#     Rev 1.6   13 Aug 1997 20:44:52   DSACKIN
#  Added the $(FAMILYVERS) variable for accessing AVCORE (branch stuff)
#
#     Rev 1.5   11 Jun 1997 16:08:58   DALLEE
#  Added full path to linkcli.lib in family unicode area.
#
#     Rev 1.4   11 Jun 1997 13:37:48   DALLEE
#  Added LINKCLI.LIB
#
#     Rev 1.3   05 Jun 1997 19:48:20   DALLEE
#  Added Q_SYMDEBUG.
#
#     Rev 1.2   02 Jun 1997 03:12:02   ksackin
#  Left off a pesky '\' in the dependencies.
#
#     Rev 1.1   02 Jun 1997 03:01:24   DALLEE
#  Forgot to do dependencies first rev. oops.
#
#     Rev 1.0   02 Jun 1997 02:49:44   DALLEE
#  Initial revision.
########################################################################

TARGET_NAME=APWUTIL
TARGET_TYPE=DLL

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# BUILDING_APWUTIL *must* be defined to correctly export this DLL's exports.

DEF+= /DBUILDING_APWUTIL

OBJS  = apwutil.obj
OBJS += apwname.obj
OBJS += apwmsg.obj
OBJS += apwcfg.obj

LIBS += $(Q_SYMDEBUG).LIB
LIBS += $(FAMILYPRE)\LIB$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U\linkcli.lib


%IF ( "$(CORE_PLAT)" == "I" )
  LIBS += ctl3d32s.lib
  DEFFILE = apwutil.d32
%ELSE
  DEFFILE =
%ENDIF

$(PROGRAM_NAME): \
    $(OBJS) \
    $(DEFFILE) \
    apwutil.res

APWUTIL.RES: apwutil.rc



#UPDATE#
apwcfg.obj:     platform.h apw.h apwutil.h navbnhdr.h navopts.h \
                options.h exclude.h cts4.h ctsn.h callback.h callbk15.h \
                avtypes.h excludel.h navstart.h quarset.h custom.h \
                apwcfg.h apwcmd.h resource.h
apwname.obj:    apwutil.h navbnhdr.h navopts.h options.h exclude.h \
                cts4.h ctsn.h callback.h callbk15.h avtypes.h platform.h \
                excludel.h navstart.h quarset.h custom.h
apwutil.obj:    apw.h apwutil.h navbnhdr.h navopts.h options.h \
                exclude.h cts4.h ctsn.h callback.h callbk15.h \
                avtypes.h platform.h excludel.h navstart.h quarset.h \
                custom.h apwcfg.h apwcmd.h resource.h
apwmsg.obj:     apw.h apwutil.h pshpack1.h navbnhdr.h navopts.h options.h \
                exclude.h cts4.h ctsn.h callback.h callbk15.h avtypes.h \
                platform.h excludel.h navstart.h quarset.h custom.h \
                resource.h
#ENDUPDATE#
