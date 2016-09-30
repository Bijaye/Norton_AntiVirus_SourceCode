#
# $Header:   S:/NAVWHLP5/VCS/NAVWHLP.MAv   1.6   22 Jul 1998 22:02:04   EANDERS  $
#
# Makes help for Norton AntiVirus for Windows 95
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVWHLP5/VCS/NAVWHLP.MAv  $
#  
#     Rev 1.6   22 Jul 1998 22:02:04   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added warning.bmp
#  
#     Rev 1.5   21 Jul 1998 18:42:00   EANDERS
#  NAVNTHLP.MAK -> S:\NAVWHLP5\VCS\NAVNTHLP.MAv
#  added tip and warning
#  
#     Rev 1.4   21 Jul 1998 18:03:18   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added tip and warning bmps replacing symw4003 and 4005
#  
#     Rev 1.3   21 Jul 1998 17:54:18   EANDERS
#  support.mak -> S:\NAVWHLP5\VCS\support.mav
#  added bulit.bmp
#  
#     Rev 1.2   09 Jul 1998 19:00:22   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added navdic.rtf
#  
#     Rev 1.1   29 Jun 1998 23:33:48   EANDERS
#  NAVNTSRV.MAK -> S:\NAVWHLP5\VCS\NAVNTSRV.MAv
#  reorg of files; navwhlp5.h
#  
#     Rev 1.0   19 May 1998 15:23:08   EANDERS
#  Initial revision.
#  
#     Rev 1.2   23 Aug 1997 17:50:12   EANDERS
#  navwhlp.mak -> S:\NAVWHLP\VCS\navwhlp.mav
#  added cd.bmp
#  
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

HELP_FILE=NAVW32.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    NAVW32.HPJ      \
    NAVW32.RTF      \
    NAVCHELP.RTF    \
    NAVINTRO.RTF    \
    NAVTASK.RTF     \
    NAVHBTN.RTF      \
    NAVOPTS.RTF    \
    NAVDIC.RTF     \
    Navtblsht.rtf    \
    NAVW32.CNT      \
    NAVWHLP5.H       \
    navw32.hh       \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \
    CD.BMP          \
    BULIT.BMP       \
    TIP.BMP         \
    WARNING2.BMP     \
    WARNING.BMP      \
