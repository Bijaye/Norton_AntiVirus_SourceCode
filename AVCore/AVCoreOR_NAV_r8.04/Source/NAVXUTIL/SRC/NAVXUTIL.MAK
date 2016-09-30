#-------------------------------------------------------------------------
#
# $Header:   S:/NAVXUTIL/VCS/NAVXUTIL.MAv   1.12   23 Jun 1998 21:03:30   jtaylor  $
#
# Description:
#       NAVXUTIL makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVXUTIL/VCS/NAVXUTIL.MAv  $
#  
#     Rev 1.12   23 Jun 1998 21:03:30   jtaylor
#  Changed OEM versions to use .vir files.
#
#     Rev 1.11   12 May 1998 14:40:06   mdunn
#  Alpha still uses XNTINOC.LIB.
#
#     Rev 1.10   11 May 1998 16:54:10   DALLEE
#  Removed ifdef NAVWNT around service control libs.
#  Moving toward single binary - new LoadAutoProtect() routine
#  makes service control calls based on run-time platform check.
#
#     Rev 1.9   15 Jul 1997 14:57:50   DBUCHES
#  Added Ctl3d support for DEC Alpha
#
#     Rev 1.8   07 Jul 1997 18:19:44   DBUCHES
#  Removed NAVINOC from NT platform
#
#     Rev 1.7   18 Jun 1997 15:19:34   MKEATIN
#  Added initcall.obj to WIN and DOS platforms.
#
#     Rev 1.6   21 May 1997 13:10:26   BILL
#  Updated for Alpha
#
#     Rev 1.5   02 May 1997 23:28:48   DSACKIN
#  NAVWNT should be using XNTSERVE instead of N32SERVE
#
#     Rev 1.4   29 Apr 1997 19:22:48   JBRENNA
#  Convert to new platform CORE_* specification.
#
#     Rev 1.3   24 Feb 1997 19:14:30   DALLEE
#  Removed NETALERT.OBJ - This file migrated to NAVSERVE in HYDRA. gah!
#
#     Rev 1.2   24 Feb 1997 18:21:26   DALLEE
#  Removed WIN sections -- code is no longer really compiles for W16.
#  If we need this for W16, I'll donate my lunch break to redoing the
#  W16 support for this makefile.
#
#     Rev 1.1   24 Feb 1997 13:19:36   DALLEE
#  Port from HYDRA.15:
#
#             Rev 1.63   29 Oct 1996 16:23:44   CCOTRIL
#          Changed NAVSERVE.LIB to N32SERVE.LIB for W32 platform.
#
#     Rev 1.0   06 Feb 1997 21:05:18   RFULLER
#  Initial revision
#
#     Rev 1.21   24 Jan 1997 14:51:48   JBRENNA
#  Add OptsMask.cpp to .W32 object list.
#
#     Rev 1.20   22 Nov 1996 00:07:54   GWESTER
#  Added NAV95_CODE to list of defines for Win95. It will compile in the
#  proper window class name.
#
#     Rev 1.19   21 Nov 1996 16:38:34   MKEATIN
#  Added the NCX platform
#
#     Rev 1.18   08 Nov 1996 17:31:56   JBRENNA
#  Add NavName.cpp to WINOBJS.
#
#     Rev 1.17   10 Oct 1996 17:16:54   JBRENNA
#  Add AlignWnd.obj to the WINOBJS variable.
#
#     Rev 1.16   23 Sep 1996 16:40:36   GWESTER
#  Fixed build problem.
#
#     Rev 1.15   19 Sep 1996 14:06:08   JALLEE
#  Added DVX platform.
#
#     Rev 1.14   22 Aug 1996 12:09:22   JBRENNA
#  Add NETALERT.OBJ to DOSOBJS for DOS and DX.
#
#     Rev 1.13   19 Aug 1996 14:05:10   JBRENNA
#  Use LINKCLI.LIB for both WinNT and Win95.
#
#     Rev 1.12   31 Jul 1996 15:53:38   DSACKIN
#  Make sure all string comparisons are non-case sensitive.
#
#     Rev 1.11   31 Jul 1996 15:04:10   DSACKIN
#  Removed duplicate svcctrl.lib entry.
#
#     Rev 1.10   24 Jul 1996 17:02:34   PVENKAT
#  Added SVCCTRL.LIB
#
#     Rev 1.9   19 Jul 1996 01:34:18   DSACKIN
#  Use CNAVAPSVCClient to talk to the NAVAP Service.
#
#     Rev 1.8   20 May 1996 23:17:46   jmillar
#  add NAVWNWT platform support
#
#     Rev 1.7   08 May 1996 15:56:34   RSTANEV
#  Duh.
#
#     Rev 1.6   06 May 1996 22:27:50   RSTANEV
#  Added SYM_NTK support.
#
#     Rev 1.5   02 May 1996 16:59:02   DSACKIN
#  added the WinInfo.obj file for only the Win32 platform.
#
#     Rev 1.4   26 Apr 1996 13:42:48   DSACKIN
#  Added SVCCTRL.LIB to talk to the NAVAPSVC Service.
#
#     Rev 1.3   09 Apr 1996 18:32:04   DSACKING
#  Added DRVCOMM.LIB.
#
#     Rev 1.2   22 Feb 1996 19:17:08   BARRY
#  Changed nstdhelp to stdhelp
#
#     Rev 1.1   31 Jan 1996 15:55:22   MKEATIN
#
#
#     Rev 1.0   30 Jan 1996 15:56:34   BARRY
#  Initial revision.
#
#     Rev 1.0   30 Jan 1996 14:51:36   BARRY
#  Initial revision.
#
#     Rev 1.61   14 Jan 1996 16:58:10   DALLEE
#  Changed #ifdef NAVWNT to NAVSCANW.
#
#     Rev 1.60   05 Dec 1995 00:57:26   GWESTER
#  Changed NT target so that it uses NNTXUTIL for project NAVXUTIL,
#  NNTEXCLU for project NAVEXCLU, and NNTLIB01 for project NAVINOC.
#
#     Rev 1.59   04 Dec 1995 22:47:38   GWESTER
#  Split TARGET, so that this project compiles N32XUTIL.DLL for Win95 and
#  NNTXUTIL.DLL for NT.
#
#
#     Rev 1.58   30 Nov 1995 22:26:58   BARRY
#  Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
#
#     Rev 1.57   15 Nov 1995 21:45:22   DALLEE
#  Removed static link to S32STAT.DLL.
#  This is now loaded dynamically and called through covers in NAVXUTIL.
#
#     Rev 1.56   10 Jul 1995 14:15:58   MARKL
#  VxD platform now references NavHalt.obj
#
#     Rev 1.55   08 Jul 1995 22:31:46   BARRY
#  Moved NAVHALT.CPP to WINOBJS section
#
#     Rev 1.54   07 Jul 1995 11:27:10   MARKL
#  Added NAVHALT.OBJ
#
#     Rev 1.53   29 Jun 1995 10:30:56   BARRY
#  Fix build sisue with some platforms
#
#     Rev 1.52   23 May 1995 16:08:40   KEITH
#  Add another message case for a SYMSTAT lock for DriveAccessProblem()
#
#     Rev 1.51   23 May 1995 12:15:14   MARKL
#  Added w95stuff.obj
#
#     Rev 1.50   24 Apr 1995 17:12:40   DALLEE
#  Moved W95STUFF.OBJ to WINOBJS section.
#  This was breaking under non-WIN platforms.
#
#     Rev 1.49   19 Apr 1995 16:05:50   MFALLEN
#  Added Win95 stuff.
#
#     Rev 1.48   19 Apr 1995 15:56:14   MFALLEN
#  Added w95stuff.obj
#
#     Rev 1.47   05 Apr 1995 13:06:48   MFALLEN
#  Added validate.obj for the VXD platform.
#
#     Rev 1.46   31 Mar 1995 06:52:50   MFALLEN
#  validate.obj
#
#     Rev 1.45   13 Mar 1995 18:26:48   MFALLEN
#  Fixed dependencies after update dependencies.
#
#     Rev 1.44   09 Mar 1995 15:57:38   DALLEE
#  Updated dependencies.
#
#     Rev 1.43   08 Mar 1995 16:11:12   DALLEE
#  Put back utilstr.str for DOS.
#  Put back NDSTRINGS for DOS platform.  (DGROUP exploded once again).
#
#     Rev 1.42   10 Feb 1995 14:40:12   AMATIC
#  Build system is poor on IQ. Couldn't find commctrl for W16 platform.
#
#     Rev 1.41   10 Feb 1995 14:26:16   AMATIC
#  MKDEPS
#
#     Rev 1.40   10 Feb 1995 14:24:32   AMATIC
#  Added cntxhelp.obj for context sensitive help,.
#
#     Rev 1.39   23 Jan 1995 17:42:00   DALLEE
#  Added startup.obj (read/write from NAVSTART.DAT)
#
#     Rev 1.38   20 Jan 1995 19:13:34   MFALLEN
#  Removed UTILSTR.STR. This file is not linked in anymore.
#
#     Rev 1.37   19 Jan 1995 17:40:58   RSTANEV
#  Added inistuff.obj to VxD library.
#
#     Rev 1.36   29 Dec 1994 17:44:34   DALLEE
#  Removed MODEL=L from DOS and VXD compiles.
#  Use PLATFORM.MAK for DOS, not DOS7.MAK.
#  Minor clean-up consolidating a few lines (SHOULDN'T have any effect... <g>).
#
#     Rev 1.35   27 Dec 1994 19:25:36   MARKL
#  Added routines for spooling the Auto-Protect agent's log entries if
#  necessary.
#
#     Rev 1.34   27 Dec 1994 16:17:46   MARKL
#  Moved the activity log processing code that the Auto-Protect Agent
#  did for the VxD to NAVXUTIL.  The Agent STILL references these routines.
#
#
#     Rev 1.33   20 Dec 1994 18:05:56   MFALLEN
#  SYMMAIL is not needed. What's it doin' here?
#
#     Rev 1.32   20 Dec 1994 17:24:08   MFALLEN
#  I don't think NAVUTL.CPP is needed by VXD. Rad will let me know.
#
#     Rev 1.31   16 Dec 1994 12:50:38   MFALLEN
#  This Goldberg machinery is getting more compilcated.
#
#     Rev 1.30   16 Dec 1994 10:37:16   MFALLEN
#  Builds for W32
#
#
#     Rev 1.29   16 Dec 1994 10:15:04   MARKL
#  Updated further for VxD.
#
#     Rev 1.28   16 Dec 1994 09:46:38   MARKL
#  Fixed up for VxD
#
#     Rev 1.27   15 Dec 1994 20:14:20   MFALLEN
#  From nirvana
#
#-------------------------------------------------------------------------


%If "$(CORE_WIN32)" != ""
  %if ("$[u,$(TARGET)]" == "NAVWNT")
     TARGET_NAME=XNTXUTIL
     DEF += /DNAVSCANW /DNAVWNT
  %ElseIf "$[u,$(TARGET)]" == "NAVSCANW"
     TARGET_NAME=NNTXUTIL
     DEF += /DNAVSCANW
  %else
     TARGET_NAME=N32XUTIL
     DEF += /DNAV95_CODE
  %endif
%Else
     TARGET_NAME=NAVXUTIL
%Endif

%If "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""
  %If ("$[u,$(TARGET)]" == "NAVW")
     NAVSERVE=N32SERVE
  %Else
					## NAVWNT
     NAVSERVE=XNTSERVE
  %Endif
%else
  NAVSERVE=NAVSERVE
%endif

%If "$(CORE_SOFT)" == "K"

OBJS= \
 validate.obj

DOSOBJS=

WINOBJS=


%elseIf "$(CORE_SOFT)" == "V"

# navutl.obj     \

OBJS= \
 utildll.obj    \
 drvprblm.obj   \
 filestat.obj   \
 backup.obj     \
 navhalt.obj    \
 inistuff.obj   \
 navaplog.obj   \
 validate.obj   \
 startup.obj    \
 w95stuff.obj

DOSOBJS=

WINOBJS=

%Else

#############################################################################

OBJS= navutl.obj \
 backup.obj \
 netobj.obj   \
 repfile.obj  \
 utildll.obj  \
 notify.obj   \
 creaback.obj \
 pointer.obj  \
 bldlog.obj   \
 winmisc.obj  \
 inistuff.obj \
 filestat.obj \
 updattsr.obj \
 yesno.obj    \
 drvprblm.obj \
 numbytes.obj \
 navaplog.obj \
 startup.obj  \
 validate.obj \
 initcall.obj

DOSOBJS=      \
 dosmisc.obj  \
 utilstr.obj

WINOBJS=      \
 navstat.obj  \
 w95stuff.obj \
 cntxhelp.obj \
 navhalt.obj  \
 AlignWnd.obj \
 NavName.obj

%Endif


%If ("$[u,$(PLATFORM)]" == ".DOS" || "$[u,$(PLATFORM)]" == ".DX" || \
     "$(CORE_SOFT)"     == "V"    || "$(CORE_SOFT)"     == "K"   || \
     "$[u,$(PLATFORM)]" == ".DVX" || "$[u,$(PLATFORM)]" == ".NCX")

%If ("$[u,$(PLATFORM)]" == ".DOS" || "$[u,$(PLATFORM)]" == ".DX" || \
     "$[u,$(PLATFORM)]" == ".DVX" || "$[u,$(PLATFORM)]" == ".NCX")
NDSTRINGS=YES
%Endif

TARGET_TYPE=LIB
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME):    $(OBJS)        \
		    $(DOSOBJS)



%ElseIf "$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == ""

TARGET_TYPE=DLL
PROGRAM_NAME=$(TARGET_NAME).DLL
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

%If "$(CORE_PLAT)" == "I"
CERTLIB=CERTLIBN.LIB
%Endif

%If "$(CORE_PLAT)" == "A"
LIBS += CTL3D32.LIB
%Endif


$(PROGRAM_NAME): \
  $(OBJS) \
  $(WINOBJS) \
  OptsMask.obj \
  linkcli.lib \
  svcctrl.lib \
  apscli.lib \
 %If ("$[u,$(TARGET)]" == "NAVSCANW")
    NNTLIB01.LIB \
 %Else
     %If "$(CORE_PLAT)" == "A"
	 xntinoc.lib \
     %Else
	 N32INOC.LIB \
     %Endif
 %Endif
  V32SCAN.LIB \
  uuid.lib \
  ole32.lib \
  drvcomm.lib \
  wininfo.obj \
  $(Q_SYMDEBUG).LIB \
  $(Q_SYMALOG).LIB \
  $(Q_SYMGUI).LIB  \
  $(Q_SYMKRNL).LIB \
  $(Q_SYMUTIL).LIB \
  NAVXUTIL.RES \
  N32pdll.LIB \
  $(CERTLIB)


navxutil.res:  navxutil.dlg navxutil.stb resexts.h ngui.h stdhelp.h \
		     stbexts.h navw.ico navaplog.h navaplog.stb
%EndIf

#UPDATE#
inistuff.obj:   platform.h dbcs_str.h dosvmm.h symcfg.h xapi.h file.h \
		options.h navopts.h exclude.h cts4.h ctsn.h callback.h \
		excludel.h netobj.h symnet.h navstart.h register.h navbin.h \
		navbnhdr.h navutil.h npt.h quakever.h version.h virlist.h \
		avapi.h symalog.h drvprblm.h stdhelp.h tsr.h tsrcomm.h \
		stbexts.h
numbytes.obj:   platform.h dbcs_str.h dosvmm.h symcfg.h symnet.h \
		dosnet.h file.h disk.h xapi.h stddos.h ibmpc.h tsr.h \
		tsrcomm.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h navstart.h register.h \
		npt.h quakever.h version.h virlist.h avapi.h symalog.h \
		drvprblm.h stdhelp.h syminteg.h nonav.h
winmisc.obj:    platform.h dbcs_str.h dosvmm.h xapi.h navopts.h cts4.h \
		ctsn.h callback.h navutil.h options.h exclude.h excludel.h \
		netobj.h symnet.h navstart.h register.h npt.h quakever.h \
		version.h virlist.h avapi.h symcfg.h symalog.h drvprblm.h \
		stdhelp.h disk.h virscan.h virusinf.h navapcom.h ngui.h \
		wndclass.h passwd.h stockbmp.h msgdisp.h navwcomm.h stbexts.h
n30_rw.obj:     platform.h dbcs_str.h dosvmm.h navutil.h ctsn.h callback.h \
		options.h navopts.h exclude.h cts4.h excludel.h netobj.h \
		symnet.h navstart.h register.h npt.h quakever.h version.h \
		virlist.h avapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		virscan.h virusinf.h file.h
validate.obj:   platform.h dbcs_str.h dosvmm.h xapi.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h navstart.h register.h npt.h quakever.h \
		version.h virlist.h avapi.h symcfg.h symalog.h drvprblm.h \
		stdhelp.h
navutl.obj:     platform.h dbcs_str.h dosvmm.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h navutil.h ctsn.h callback.h \
		options.h navopts.h exclude.h cts4.h excludel.h netobj.h \
		symnet.h navstart.h register.h npt.h virlist.h avapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h
netalert.obj:   stdio.h dos.h platform.h dbcs_str.h dosvmm.h xapi.h \
		dosnet.h stddos.h ibmpc.h undoc.h symintl.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h navstart.h register.h npt.h quakever.h \
		version.h virlist.h avapi.h symcfg.h symalog.h drvprblm.h \
		stdhelp.h nonav.h syminteg.h netalert.h cmbstr.h servenfy.h \
		stbexts.h
netobj.obj:     platform.h dbcs_str.h dosvmm.h xapi.h file.h symnet.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h
creaback.obj:   platform.h dbcs_str.h dosvmm.h xapi.h file.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h
notify.obj:     platform.h dbcs_str.h dosvmm.h xapi.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h ctl3d.h symcfg.h \
		navutil.h ctsn.h callback.h options.h navopts.h exclude.h \
		cts4.h excludel.h netobj.h symnet.h navstart.h register.h \
		npt.h virlist.h avapi.h symalog.h drvprblm.h stdhelp.h \
		resexts.h
getreg.obj:     platform.h dbcs_str.h dosvmm.h file.h cts4.h options.h \
		navopts.h exclude.h ctsn.h callback.h excludel.h netobj.h \
		symnet.h navstart.h register.h inoc.h symcfg.h
pointer.obj:    platform.h dbcs_str.h dosvmm.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h navstart.h register.h npt.h quakever.h \
		version.h virlist.h avapi.h symcfg.h symalog.h drvprblm.h \
		stdhelp.h
repfile.obj:    platform.h dbcs_str.h dosvmm.h file.h xapi.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h inoc.h virscan.h virusinf.h tsrcomm.h
bldlog.obj:     platform.h dbcs_str.h dosvmm.h xapi.h symalog.h stddos.h \
		ibmpc.h navutil.h ctsn.h callback.h options.h navopts.h \
		exclude.h cts4.h excludel.h netobj.h symnet.h navstart.h \
		register.h npt.h quakever.h version.h virlist.h avapi.h \
		symcfg.h drvprblm.h stdhelp.h syminteg.h nonav.h stbexts.h
updatdlg.obj:   platform.h dbcs_str.h dosvmm.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h ctl3d.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		virlist.h avapi.h symcfg.h symalog.h drvprblm.h stdhelp.h \
		cmbstr.h resexts.h stbexts.h tsr.h
updattsr.obj:   platform.h dbcs_str.h dosvmm.h disk.h file.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h insert.h tsr.h tsrcomm.h
utildll.obj:    platform.h dbcs_str.h dosvmm.h
yesno.obj:      platform.h dbcs_str.h dosvmm.h ngui.h version.h quakever.h \
		wndclass.h passwd.h stockbmp.h ctl3d.h navutil.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h navstart.h register.h npt.h virlist.h \
		avapi.h symcfg.h symalog.h drvprblm.h stdhelp.h resexts.h
drvprblm.obj:   platform.h dbcs_str.h dosvmm.h ctl3d.h ngui.h version.h \
		quakever.h wndclass.h passwd.h stockbmp.h symcfg.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		virlist.h avapi.h symalog.h drvprblm.h stdhelp.h resexts.h \
		stbexts.h navw_hlp.h navwhlp.h symstat.h
dosmisc.obj:    platform.h dbcs_str.h dosvmm.h xapi.h stddos.h ibmpc.h \
		stddlg.h dosnet.h symnet.h navutil.h ctsn.h callback.h \
		options.h navopts.h exclude.h cts4.h excludel.h netobj.h \
		navstart.h register.h npt.h quakever.h version.h virlist.h \
		avapi.h symcfg.h symalog.h drvprblm.h stdhelp.h navdprot.h \
		nethelp.h navhelp.h syminteg.h nonav.h cmbstr.h
filestat.obj:   platform.h dbcs_str.h dosvmm.h xapi.h disk.h syminteg.h \
		nonav.h options.h navopts.h exclude.h cts4.h ctsn.h \
		callback.h excludel.h netobj.h symnet.h navstart.h register.h \
		navutil.h npt.h quakever.h version.h virlist.h avapi.h \
		symcfg.h symalog.h drvprblm.h stdhelp.h
backup.obj:     platform.h dbcs_str.h dosvmm.h xapi.h file.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h
navaplog.obj:   platform.h dbcs_str.h dosvmm.h file.h msgdisp.h ctsn.h \
		callback.h options.h navopts.h exclude.h cts4.h excludel.h \
		netobj.h symnet.h navstart.h register.h symalog.h navaplog.h \
		navutil.h npt.h quakever.h version.h virlist.h avapi.h \
		symcfg.h drvprblm.h stdhelp.h stbexts.h
startup.obj:    platform.h dbcs_str.h dosvmm.h xapi.h file.h options.h \
		navopts.h exclude.h cts4.h ctsn.h callback.h excludel.h \
		netobj.h symnet.h navstart.h register.h navutil.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h tsr.h tsrcomm.h stbexts.h
cntxhelp.obj:   platform.h dbcs_str.h dosvmm.h stdhelp.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h navw_hlp.h navwhlp.h navwstb.h
w95stuff.obj:   platform.h dbcs_str.h dosvmm.h xapi.h file.h navutil.h \
		ctsn.h callback.h options.h navopts.h exclude.h cts4.h \
		excludel.h netobj.h symnet.h navstart.h register.h npt.h \
		quakever.h version.h virlist.h avapi.h symcfg.h symalog.h \
		drvprblm.h stdhelp.h
navstat.obj:    platform.h dbcs_str.h dosvmm.h symstat.h version.h \
		quakever.h navstat.h
#ENDUPDATE#
