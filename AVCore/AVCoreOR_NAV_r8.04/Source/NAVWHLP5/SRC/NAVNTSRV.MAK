#
# $Header:   S:/NAVWHLP5/VCS/NAVNTSRV.MAv   1.9   22 Jul 1998 22:02:20   EANDERS  $
#
# Makes help for Norton AntiVirus for Windows NT
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVWHLP5/VCS/NAVNTSRV.MAv  $
#  
#     Rev 1.9   22 Jul 1998 22:02:20   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added warning.bmp
#  
#     Rev 1.8   21 Jul 1998 18:41:58   EANDERS
#  NAVNTHLP.MAK -> S:\NAVWHLP5\VCS\NAVNTHLP.MAv
#  added tip and warning
#  
#     Rev 1.7   21 Jul 1998 18:03:42   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added tip and warning bmps replacing symw4003 and 4005
#  
#     Rev 1.6   21 Jul 1998 17:54:22   EANDERS
#  support.mak -> S:\NAVWHLP5\VCS\support.mav
#  added bulit.bmp
#  
#     Rev 1.5   09 Jul 1998 19:00:50   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added navdic.rtf
#  
#     Rev 1.4   02 Jul 1998 18:05:56   EANDERS
#  navntsrv.mak -> S:\NAVWHLP5\VCS\navntsrv.mav
#  removed ntnintro.rtf
#  
#  
#     Rev 1.3   30 Jun 1998 11:33:24   mdunn
#  Added NTNINTRO.RTF
#  
#     Rev 1.2   29 Jun 1998 23:48:00   EANDERS
#  navntsrv.mak -> S:\NAVWHLP5\VCS\navntsrv.mav
#  spaces
#  
#     Rev 1.1   29 Jun 1998 23:33:14   EANDERS
#  NAVNTSRV.MAK -> S:\NAVWHLP5\VCS\NAVNTSRV.MAv
#  reorg of files; navwhlp5.h
#  
#     Rev 1.0   19 May 1998 15:23:20   EANDERS
#  Initial revision.
#  
#     Rev 1.1   23 Aug 1997 18:44:48   EANDERS
#  navdecsr.mak -> S:\NAVWHLP\VCS\navdecsr.mav
#  added cd.bmp
#  
#     Rev 1.0   06 Aug 1997 20:21:26   KSACKIN
#  Initial revision.
#
#     Rev 1.0   05 Aug 1997 16:52:24   EANDERS
#  Initial revision.
#
#     Rev 1.2.1.0   02 May 1997 14:59:36   dwilson
#  Branch base for version QAKF
#
#     Rev 1.2   26 Jan 1997 18:24:10   JMILLAR
#  update title comment to reflect NT
#
#     Rev 1.1   14 May 1996 22:39:16   EANDERS
#  navnthlp.mak -> S:\NAVWHLP\VCS\navnthlp.mav
#  changed output file to navwnt.hlp
#
#     Rev 1.0   14 May 1996 22:25:04   EANDERS
#  Initial revision.
#
#     Rev 1.1   07 May 1996 21:50:54   BGERHAR
#  Add NAVWHLP.H to dependencies so it gets copied to local dir during make
#
#     Rev 1.0   07 May 1996 17:49:56   BGERHAR
#  Initial revision.
#
#     Rev 1.0   07 May 1996 17:29:56   BGERHAR
#  Initial revision.
#-------------------------------------------------------------------------

HELP_FILE=NAVNTSRV.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    NAVNTSRV.HPJ    \
    NAVCHELP.RTF    \
    NAVINTRO.RTF    \
    NAVTASK.RTF     \
    NAVHBTN.RTF      \
    NTHBTN.RTF      \
    NTTASK.RTF       \
    NTINTRO.RTF       \
    NAVDIC.RTF        \
    NAVOPTS.RTF        \
    NAVWNT.CNT      \
    NAVWHLP5.H       \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \
    CD.BMP          \
    BULIT.BMP       \
    TIP.BMP         \
    WARNING2.BMP     \
    WARNING.BMP     \
