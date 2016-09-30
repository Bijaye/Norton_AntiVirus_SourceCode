########################################################################
#
# $Header:   S:/NAVINOC/VCS/NAVINOC.MAv   1.3   07 Jul 1997 13:59:02   DBUCHES  $
#
# Description:
#       NAVINOC makefile.
#
########################################################################
# $Log:   S:/NAVINOC/VCS/NAVINOC.MAv  $
#  
#     Rev 1.3   07 Jul 1997 13:59:02   DBUCHES
#  Removed dependency on NAVEXCLU.LIB for DEC Alpha platform.
#
#     Rev 1.2   20 May 1997 17:10:14   BILL
#  Remove intel only calls (certlibn) and fail all calls from NT clients.
#
#     Rev 1.1   30 Apr 1997 10:55:26   JBRENNA
#  Convert to use new CORE_* platform checks.
#
#     Rev 1.0   06 Feb 1997 20:58:12   RFULLER
#  Initial revision
#
#     Rev 1.5   21 Nov 1996 19:37:48   MKEATIN
#  Added NCX platform
#
#     Rev 1.4   19 Sep 1996 13:57:04   JALLEE
#  Added DVX platform.
#
#     Rev 1.3   31 Jul 1996 15:41:40   DSACKIN
#  Make all string comparisons non-case sensitive.
#
#     Rev 1.2   26 Jun 1996 17:53:26   GWESTER
#  Ported in changes and fixes from LuigiPlus.
#
#     Rev 1.1   21 May 1996 00:31:42   jmillar
#  add navwnt target
#
#     Rev 1.0   30 Jan 1996 15:53:12   BARRY
#  Initial revision.
#
#     Rev 1.0   30 Jan 1996 13:22:16   BARRY
#  Initial revision.
#
#     Rev 1.34   14 Jan 1996 22:02:28   DALLEE
#  Changed #ifdef NAVWNT to NAVSCANW.
#
#     Rev 1.33   05 Dec 1995 00:56:18   GWESTER
#  Changed NT target so that it uses NNTXUTIL for project NAVXUTIL,
#  NNTEXCLU for project NAVEXCLU, and NNTLIB01 for project NAVINOC.
#
#     Rev 1.32   04 Dec 1995 18:55:10   GWESTER
#  Split TARGET, so that this project compiles N32INOC.DLL for Win95 and
#  NNTLIB01.DLL for NT.
#
#     Rev 1.31   13 Jun 1995 14:42:46   MARKL
#  Added resources back in!!!  Fixed reference to NAV32EXCLU.
#
#     Rev 1.30   11 Jun 1995 20:08:06   BARRY
#  Removed references to obsolete files
#
#     Rev 1.29   12 Apr 1995 11:31:40   MFALLEN
#  Removed resources.
#
#     Rev 1.28   31 Mar 1995 10:13:18   MFALLEN
#  Moved inoc/uninoc UI code to NAVW
#
#
#     Rev 1.27   03 Feb 1995 16:11:08   BRAD
#  Added N32EXCLU.LIB
#
#     Rev 1.26   25 Jan 1995 13:50:36   MFALLEN
#  Added SYMDEBUG.LIB
#
#     Rev 1.25   07 Dec 1994 17:09:08   MFALLEN
#  NAVWORK is part again.
#
#     Rev 1.24   28 Nov 1994 18:56:52   MFALLEN
#  CERTLIB for W16
#
#     Rev 1.23   28 Nov 1994 16:35:50   MFALLEN
#  Temporarely commented out NAVWORK
#
#     Rev 1.22   28 Nov 1994 14:13:40   MFALLEN
#  Almost there.
#
#     Rev 1.21   22 Aug 1994 17:36:42   DALLEE
#  For 16-bit platforms, change N16xxx back to NAVxxx.
#
#     Rev 1.20   15 Aug 1994 09:08:04   MFALLEN
#  N32INOC or N16INOC
#
#     Rev 1.19   14 Jul 1994 12:35:04   MFALLEN
#
#     Rev 1.18   13 Jul 1994 11:34:00   DALLEE
#  Added .DX platform.
#  Updated dependencies.
#
#     Rev 1.17   17 May 1994 17:43:46   MFALLEN
#
#     Rev 1.16   16 May 1994 18:32:10   MFALLEN
#  Added NAVALOG.LIB
#
#     Rev 1.15   26 Apr 1994 19:27:50   MFALLEN
#  More W32
#
#     Rev 1.14   26 Apr 1994 13:09:46   DALLEE
#  Trunk support for DOS - set TARGET_TYPE, include PLATFORM.MAK
#
#     Rev 1.13   25 Apr 1994 16:01:24   MFALLEN
#  W32
########################################################################

%If "$(CORE_WIN32)" != ""
  %if ("$[u,$(TARGET)]" == "NAVWNT")
    TARGET_NAME=XNTINOC
    DEF += /DNAVSCANW /DNAVWNT
    NAVUTILITIES=XNTXUTIL
    NAVEXCLUSIONS=XNTEXCLU
  %elseif ("$[u,$(TARGET)]" == "NAVSCANW")
    TARGET_NAME=NNTLIB01
    DEF += /DNAVSCANW
    NAVUTILITIES=NNTXUTIL
    NAVEXCLUSIONS=NNTEXCLU
  %else
    TARGET_NAME=N32INOC
    NAVUTILITIES=N32XUTIL
    NAVEXCLUSIONS=N32EXCLU
  %endif
    ACTILOG=N32ALOG
    PASSWORDCHECK=N32SECUR
  %if "$(CORE_PLAT)" == "I"
	CERTLIB=CERTLIBN.LIB
  %Endif
%Else
    TARGET_NAME=NAVINOC
    ACTILOG=NAVALOG
    NAVUTILITIES=NAVXUTIL
    NAVEXCLUSIONS=NAVEXCLU
    PASSWORDCHECK=NAVSECUR
    CERTLIB=CERTLIB4.LIB
%Endif


OBJS= \
 inocadd.obj \
 inocdb.obj \
 inoclose.obj \
 inocopen.obj \
 inocdel.obj \
 inocrep.obj \
 inocfind.obj \
 inocsmrt.obj \
 inocget.obj


%If ("$[u,$(PLATFORM)]" == ".DOS") || ("$[u,$(PLATFORM)]" == ".DX") || \
    ("$[u,$(PLATFORM)]" == ".DVX") || ("$[u,$(PLATFORM)]" == ".NCX")

PROGRAM_NAME=$(TARGET_NAME).LIB

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):  $(OBJS)


%ElseIf ("$(CORE_WIN32)" != "") || ("$(CORE_WIN16)" != "")

PROGRAM_NAME=$(TARGET_NAME).DLL

TARGET_TYPE=DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): $(STUB) \
 $(OBJS) \
 INOCDLL.OBJ \
 $(PASSWORDCHECK).LIB \
 $(Q_SYMKRNL).LIB \
 $(Q_SYMGUI).LIB \
 $(Q_SYMUTIL).LIB \
 $(Q_SYMDEBUG).LIB \
 $(NAVUTILITIES).LIB \
%if ("$(CORE_PLAT)" != "A")
 $(NAVEXCLUSIONS).LIB \
%endif
 navinoc.res \
 NAVINOC.DEF \
 $(CERTLIB)

%Endif


#UPDATE#
inoclose.obj:	platform.h options.h navopts.h exclude.h ctsn.h netobj.h \
		symnet.h register.h cmbstr.h inoc.h symcfg.h
inocget.obj:	platform.h xapi.h ctsn.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h inoc.h symcfg.h
inocadd.obj:	platform.h file.h ctsn.h cmbstr.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h inoc.h symcfg.h \
		navutil.h npt.h quakever.h version.h
inocopen.obj:	platform.h options.h navopts.h exclude.h ctsn.h netobj.h \
		symnet.h register.h cmbstr.h inoc.h symcfg.h tsr.h tsrcomm.h
inocsmrt.obj:	platform.h ctsn.h cmbstr.h options.h navopts.h exclude.h \
		netobj.h symnet.h register.h inoc.h symcfg.h
inocdel.obj:	platform.h xapi.h disk.h ctsn.h cmbstr.h options.h \
		navopts.h exclude.h netobj.h symnet.h register.h inoc.h \
		symcfg.h
inocfind.obj:	platform.h options.h navopts.h exclude.h ctsn.h netobj.h \
		symnet.h register.h file.h cmbstr.h inoc.h symcfg.h navutil.h \
		npt.h quakever.h version.h
inocrep.obj:	platform.h ctsn.h options.h navopts.h exclude.h netobj.h \
		symnet.h register.h inoc.h symcfg.h file.h disk.h
inocdll.obj:	platform.h ctsn.h ctl3d.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h cmbstr.h options.h navopts.h \
		exclude.h netobj.h symnet.h register.h inoc.h symcfg.h
inocdb.obj:	platform.h options.h navopts.h exclude.h ctsn.h netobj.h \
		symnet.h register.h xapi.h cmbstr.h inoc.h symcfg.h file.h \
		disk.h
#ENDUPDATE#
