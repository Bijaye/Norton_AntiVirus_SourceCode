########################################################################
#
# $Header:   S:/APWUI/VCS/apwui.mav   1.4   18 Feb 1998 18:30:58   DALLEE  $
#
# Description:
#   Makefile for APWUI - shared 95/NT NAVAPW UI.
#
########################################################################
# $Log:   S:/APWUI/VCS/apwui.mav  $
#  
#     Rev 1.4   18 Feb 1998 18:30:58   DALLEE
#  Removed N32SERVE.LIB.
#  InitNLMMessenger() call has been moved to APWCMD Apw95Cmd.cpp.
#  The APWUI project should only contain UI and rely on APWCMD for guts.
#  
#     Rev 1.3   17 Feb 1998 14:39:12   MKEATIN
#  
#     Rev 1.3   16 Jan 1998 12:43:06   CEATON
#  Added N32SERVE.LIB to the list of libraries necessary for linking.
#  
#     Rev 1.2   11 Jul 1997 17:42:28   MKEATIN
#  Added a trial version target
#  
#     Rev 1.1   10 Jun 1997 14:56:20   DALLEE
#  Added Q_SYMDEBUG.
#  Updated dependencies.
#  
#     Rev 1.0   02 Jun 1997 13:56:18   DALLEE
#  Initial revision.
########################################################################

%if "$[u,$(TARGET)]" == "NAVWPRV"       # Check for preview edition
    DEF    += /DNAV_PREVIEW
    RCOPTS += /DNAV_PREVIEW
    TARGET_NAME=NVPW32PR
%else
    TARGET_NAME=NAVAPW32
%endif

TARGET_TYPE=EXE

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OBJS  = apwmain.obj
OBJS += apwtray.obj

LIBS += apwutil.lib
LIBS += apwcmd.lib
LIBS += OLE32.lib
LIBS += $(Q_SYMDEBUG).lib

APWUI.CRC: $(PROGRAM_NAME)
    iff exist $(PROGRAM_NAME) then
        echo Stamping CRC... NAVCRC.EXE $(PROGRAM_NAME) >> make.err
        echo Stamping CRC... NAVCRC.EXE $(PROGRAM_NAME)
        NAVCRC.EXE $(PROGRAM_NAME)
        if %?% == 0 echo Date stamp for last crc %_DATE% %_TIME% > APWUI.CRC
    endiff

$(PROGRAM_NAME): \
    $(OBJS) \
    $(DEFFILE) \
    apwui.res

APWUI.RES: apwui.rc


#UPDATE#
apwtray.obj:    apw.h apwutil.h navbnhdr.h navopts.h options.h \
                exclude.h ctsn.h callback.h callbk15.h n30type.h \
                avtypes.h platform.h dbcs_str.h excludel.h \
                navstart.h quarset.h custom.h \
                apwcfg.h apwcmd.h apwtray.h resource.h
apwmain.obj:    platform.h dbcs_str.h resource.h apw.h \
                apwcfg.h apwutil.h navbnhdr.h navopts.h options.h \
                exclude.h ctsn.h callback.h callbk15.h n30type.h \
                avtypes.h excludel.h navstart.h \
                quarset.h custom.h apwcmd.h apwtray.h drvcomm.h \
		navwcomm.h 
#ENDUPDATE#

