########################################################################
#
# $Header:   S:/APWCMD/VCS/apwcmd.mav   1.18   01 Jul 1998 14:35:18   CEATON  $
#
# Description:
#   ApwCmd.dll: DLL that NAVAPW links to. This provides the communication
#   to the VXD or NT AP Service.
#
########################################################################
# $Log:   S:/APWCMD/VCS/apwcmd.mav  $
#  
#     Rev 1.18   01 Jul 1998 14:35:18   CEATON
#  Added support files for Win32 file notifications.
#  
#  
#     Rev 1.17   27 Apr 1998 16:09:46   DALLEE
#  Added OLE32.LIB for COM junk.
#  
#     Rev 1.16   17 Mar 1998 10:45:50   tcashin
#  Added patch32i.lib and chan32i.lib.
#  
#     Rev 1.15   23 Feb 1998 15:09:52   DALLEE
#  Updated dependencies.
#  
#     Rev 1.14   13 Aug 1997 20:43:16   DSACKIN
#  Updated the UNICODE libs from AVCORE to include $(FAMILYVERS)
#
#     Rev 1.13   23 Jun 1997 20:15:36   DALLEE
#  Added CALERT and CVALERT libs.
#
#     Rev 1.12   11 Jun 1997 15:54:30   JBRENNA
#  Add Apw95Alert.OBJ and N32SERVE.LIB to the Win95 build.
#
#     Rev 1.11   10 Jun 1997 16:08:10   DALLEE
#  Added RC include for NAVXUTIL language area to pull in AP log strings.
#
#     Rev 1.10   06 Jun 1997 17:29:16   JBRENNA
#  Integrate the Hawking (CDefUtils) functionality to pick up definitions
#  from the shared directory.
#
#     Rev 1.9   05 Jun 1997 17:37:58   DALLEE
#  Added Q_SYMALOG to Win95 make.
#
#     Rev 1.8   05 Jun 1997 17:35:40   DALLEE
#  Added apw95log.obj to Win95 make.
#
#     Rev 1.7   02 Jun 1997 03:40:52   DALLEE
#  Added legit W95 build after checking in source.
#
#     Rev 1.6   01 Jun 1997 22:11:06   JTAYLOR
#  Changed the Win95 target area to be identical to the WinNT target area.
#  I did this because the Win95 files were not setup yet and it did not build.
#
#     Rev 1.5   31 May 1997 20:08:52   JBRENNA
#  Redo the dependencies.
#
#     Rev 1.4   29 May 1997 19:38:52   JBRENNA
#  Rename ApwCmd.RC to ApwCmd.HRC so that the build system does not get
#  confused.
#
#     Rev 1.3   29 May 1997 13:15:50   DALLEE
#  Define BUILDING_APWCMD so APWMCMDAPI macro becomes __declspec(dllexport).
#
#     Rev 1.2   28 May 1997 16:17:06   JBRENNA
#  Remove types.h and callbake.h from the dependency list. "mk" could not
#  figure out how to build these header files.
#
#     Rev 1.1   28 May 1997 15:40:12   JBRENNA
#  Change all "#include <...>" to '#include "..."'. This enables R:\MK\MkDeps
#  to function better.
#
#     Rev 1.0   28 May 1997 12:25:30   JBRENNA
#  Initial revision.
#
########################################################################

TARGET_NAME=ApwCmd
TARGET_TYPE=DLL

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# BUILDING_APWCMD *must* be defined to correctly export this DLL's exports.

DEF+= /DBUILDING_APWCMD

# List all of the common OBJ files.
OBJS = \
    dll32.obj

# Provide the TARGET specific information.
%if "$(TARGET)" == "WIN95"

    OBJS += apw95cmd.obj
    OBJS += apw95svc.obj
    OBJS += apw95log.obj
    OBJS += apw95def.obj
    OBJS += Apw95alert.obj
    OBJS += defutlex.obj

    LIBS += apwutil.lib
    LIBS += drvcomm.lib
    LIBS += $(Q_SYMDEBUG).LIB
    LIBS += $(Q_SYMALOG).LIB
    LIBS += DefUtils.lib
    LIBS += N32Serve.LIB
    LIBS += Patch32i.LIB
    LIBS += Chan32i.LIB
    LIBS += ole32.lib

    RESOBJ=apw95cmd.res

    RCOPTS += /DAPWCMD_INTERNALNAME="\"Apw95Cmd\"" /I$(LANGPRE)NAVXUTIL\SRC$(PROJVERS)

%elseif "$(TARGET)" == "WINNT"

    OBJS += ApwNtCmd.obj

    RESOBJ=ApwNtCmd.res

    RCOPTS += /DAPWCMD_INTERNALNAME="\"ApwNtCmd\""

%else

BAD_TARGET:
    beep
    echo The $(TARGET) target is not supported for this project.

%endif


UNIDIR=$(PROJPRE)lib.$(CORE_PLAT)$(CORE_BMODE)U
FAMILYUNIDIR=$(FAMILYPRE)\lib$(FAMILYVERS).$(CORE_PLAT)$(CORE_BMODE)U

UNILIBS= \
    $(UNIDIR)\calert.lib \
    $(UNIDIR)\cvalert.lib \
    $(FAMILYUNIDIR)\linksrv.lib \
    $(FAMILYUNIDIR)\svcctrl.lib \
    $(FAMILYUNIDIR)\linkcli.lib \
    $(FAMILYUNIDIR)\apscli.lib


$(PROGRAM_NAME): \
    $(OBJS) \
    $(RESOBJ) \
    $(UNILIBS)


#UPDATE#
apwntcmd.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h navw.h navver.h \
		symcfg.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h callbk15.h n30type.h avtypes.h excludel.h netobj.h \
		symnet.h symnd2.h navstart.h quarset.h custom.h nonav.h \
		navutil.h npt.h quakever.h version.h virlist.h avapi.h \
	pamapi.h file.h datafile.h cpu.h opcode.h \
		profile.h page.h search.h sigex.h prepair.h pamcache.h \
		cover.h proto.h navcb.h avcb.h xapi.h symalog.h drvprblm.h \
		stdhelp.h syminteg.h ApwNtRes.h ApwRes.h ApwNtCmd.h ApwCmd.h \
		apw.h SvcCtrl.h profiler.h ApsCli.h 
dll32.obj:      windows.h 
defutlex.obj:   windows.h platform.h tchar.h
apw95cmd.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h drvcomm.h \
		pshpack1.h poppack.h apcomm.h navrpc.h linksrv.h linkcbk.h \
		apscomm.h servenfy.h NavAppIds.h apw.h apw95cmd.h apwcmd.h \
		DefUtils.h navbnhdr.h navopts.h options.h exclude.h cts4.h \
		ctsn.h callback.h callbk15.h n30type.h avtypes.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h quarset.h custom.h \
		apwutil.h msgreg.h 
apw95svc.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h drvcomm.h \
		pshpack1.h poppack.h apcomm.h linksrv.h apscomm.h apw.h \
		apwutil.h navbnhdr.h navopts.h options.h exclude.h cts4.h \
		ctsn.h callback.h callbk15.h n30type.h avtypes.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h quarset.h custom.h \
		apw95cmd.h apwcmd.h DefUtils.h apw95res.h apwres.h 
apw95log.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h drvcomm.h \
		pshpack1.h poppack.h apcomm.h linksrv.h apscomm.h apw.h \
		apwutil.h navbnhdr.h navopts.h options.h exclude.h cts4.h \
		ctsn.h callback.h callbk15.h n30type.h avtypes.h excludel.h \
		netobj.h symnet.h symnd2.h navstart.h quarset.h custom.h \
		apw95cmd.h apwcmd.h DefUtils.h actions.h symalog.h navaplog.h 
apw95def.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h drvcomm.h \
		pshpack1.h poppack.h apcomm.h linksrv.h apscomm.h DefUtils.h \
		process.h ApwUtil.h navbnhdr.h navopts.h options.h exclude.h \
		cts4.h ctsn.h callback.h callbk15.h n30type.h avtypes.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h quarset.h \
		custom.h ApwCmd.h apw.h Apw95Cmd.h 
apw95a~1.obj:   platform.h dbcs_str.h dosvmm.h nlm_str.h servenfy.h \
		drvcomm.h pshpack1.h poppack.h apcomm.h linksrv.h apscomm.h \
		actions.h apw.h apwutil.h navbnhdr.h navopts.h options.h \
		exclude.h cts4.h ctsn.h callback.h callbk15.h n30type.h \
		avtypes.h excludel.h netobj.h symnet.h symnd2.h navstart.h \
		quarset.h custom.h apw95cmd.h apwcmd.h DefUtils.h apw95res.h 
#ENDUPDATE#
