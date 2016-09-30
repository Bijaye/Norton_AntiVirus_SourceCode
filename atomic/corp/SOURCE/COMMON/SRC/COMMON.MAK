# Copyright 1992-1993 Symantec, Peter Norton Product Group
########################################################################
#
# $Header:   S:/COMMON/VCS/COMMON.MAv   1.3   21 Nov 1997 17:06:40   jtaylor  $
#
# Description:
#       This builds either the NAVWC.LIB, NAVNETWC.LIB, NAVDC.LIB, or
#       NAVNETC.LIB libraries based on the target.
########################################################################
# $Log:   S:/COMMON/VCS/COMMON.MAv  $
#  
#     Rev 1.3   21 Nov 1997 17:06:40   jtaylor
#  Added the n32pdll.lib file to the build for SYM_WIN platforms
#
#     Rev 1.2   28 Apr 1997 22:24:04   ksackin
#  Changed .W32 check to check for CORE_WIN32.  This is compliant with the new
#  platform specification standard.
#
#     Rev 1.1   27 Mar 1997 16:10:06   DALLEE
#  Removed various NAVNET and BANKERS targets - no longer built in this
#  codebase.
#
#  Once those were gone, we were left with just the default targets
#  for W32 and DOS, so I nuked the target support completely.
#
#     Rev 1.0   06 Feb 1997 20:50:10   RFULLER
#  Initial revision
#
#     Rev 1.3   31 Dec 1996 15:10:26   MKEATIN
#  Changed navc.lib to navdc.lib for the DOS/DX platforms.  NAVC.LIB conflicts
#  with NAVC.EXE
#
#     Rev 1.2   18 Oct 1996 13:53:38   TIVANOV
#  new target name for NAVNET32
#
#     Rev 1.1   17 Jul 1996 08:22:04   JWORDEN
#  Add NAVNETNT target
#
#     Rev 1.0   01 Mar 1996 18:27:12   BARRY
#  Initial revision.
#
#     Rev 1.0   30 Jan 1996 09:55:38   BARRY
#  Initial revision.
#
#     Rev 1.7   13 Sep 1995 10:52:02   JWORDEN
#  Alter to make NAVNETW target
########################################################################

                                        # Windows Scanner
%If "$(CORE_WIN32)" != ""
    TARGET_TYPE=DLL
    COMMONLIB=NAVWC.LIB
    DEF+= /DNAVW
    LIBS+= $(PROJPRE)LIB$(PROJVERS).$(CORE_PLAT)$(CORE_BMODE)a\N32PDLL.LIB
                                        # DOS Scanner
%ElseIf %member($(PLATFORM),.DOS .DX .DVX .NCX)
    TARGET_TYPE=LIB
    COMMONLIB=NAVDC.LIB
    DEF+= /DNAV

%Else

BAD_PLATFORM :
    beep
    echo '$(PLATFORM)' is not a valid platform for the COMMON project.
    echo Select a Win32 platform or .DOS, .DX, .DVX, .NCX.

%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK


$(COMMONLIB): \
 options.obj \
 readnns.obj \
 netuser.obj \
 $(LIBS)



#UPDATE#
netuser.obj:	platform.h options.h navopts.h exclude.h ctsn.h netobj.h \
		symnet.h register.h common.h
options.obj:	platform.h xapi.h file.h navutil.h ctsn.h options.h \
		navopts.h exclude.h netobj.h symnet.h register.h symcfg.h \
		npt.h quakever.h version.h navalog.h ctrlpanl.h navw.h \
                nonav.h syminteg.h navwstb.h stddos.h ibmpc.h \
                stddlg.h common.h inoc.h nns.h
readnns.obj:	platform.h xapi.h navutil.h ctsn.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h symcfg.h npt.h \
		quakever.h version.h common.h nns.h
#ENDUPDATE#
