#
# $Header:   S:/NOEMHLP5/VCS/ntoemhlp.mav   1.3   17 Sep 1998 18:33:52   eanders  $
#
# Makes help for Norton AntiVirus for Windows NT
#
#-------------------------------------------------------------------------
# $Log:   S:/NOEMHLP5/VCS/ntoemhlp.mav  $
#  
#     Rev 1.3   17 Sep 1998 18:33:52   eanders
#  ntoemhlp.mak -> S:\NOEMHLP5\VCS\ntoemhlp.mav
#  added bulit.bmp
#  
#     Rev 1.2   08 Sep 1998 15:56:54   eanders
#  ntoemhlp.mak -> S:\NOEMHLP5\VCS\ntoemhlp.mav
#  added navdic.rtf
#  
#     Rev 1.1   08 Sep 1998 15:54:12   eanders
#  ntoemhlp.mak -> S:\NOEMHLP5\VCS\ntoemhlp.mav
#  added the oem stuff
#  
#     Rev 1.0   20 May 1998 22:56:30   EANDERS
#  Initial revision.
#  
#     Rev 1.1   19 May 1998 13:00:26   EANDERS
#  ntoemhlp.mak -> S:\NOEMHLP\VCS\ntoemhlp.mav
#  changed .hpj to navwnt.hpj
#  
#     Rev 1.0   19 May 1998 12:17:18   EANDERS
#  Initial revision.
#  
#     Rev 1.1   30 Apr 1998 16:31:58   EANDERS
#  navdcmhl.mak -> S:\NAVWHLP\VCS\navdcmhl.mav
#  changed suport file name
#  
#     Rev 1.0   30 Apr 1998 16:18:42   EANDERS
#  Initial revision.
#  
#     Rev 1.3   23 Aug 1997 18:44:52   EANDERS
#  navdecsr.mak -> S:\NAVWHLP\VCS\navdecsr.mav
#  added cd.bmp
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

HELP_FILE=NAVWNT.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    NAVWNT.HPJ      \
    NAVCHELP.RTF    \
    NAVINTRO.RTF    \
    NAVTASK.RTF     \
    NAVHBTN.RTF      \
    NAVWNT.CNT      \
    NAVWHLP5.H       \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \
    NAVOPTS.RTF       \
    NTINTRO.RTF    \
    NTTASK.RTF     \
    NTHBTN.RTF     \
    CD.BMP        \
    HELP2.BMP     \
    NAVDIC.RTF    \
    BULIT.BMP    \


