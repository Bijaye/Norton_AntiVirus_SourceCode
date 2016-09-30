#
# $Header:   S:/NOEMHLP5/VCS/n95oemhl.mav   1.4   08 Sep 1998 16:04:46   eanders  $
#
# Makes help for Norton AntiVirus for Windows 95
#
#-------------------------------------------------------------------------
# $Log:   S:/NOEMHLP5/VCS/n95oemhl.mav  $
#  
#     Rev 1.4   08 Sep 1998 16:04:46   eanders
#  n95oemhl.mak -> S:\NOEMHLP5\VCS\n95oemhl.mav
#  added navdic.rtf
#  
#     Rev 1.3   08 Sep 1998 16:02:28   eanders
#  n95oemhl.mak -> S:\NOEMHLP5\VCS\n95oemhl.mav
#  added bulit.bmp
#  
#     Rev 1.2   08 Sep 1998 15:47:18   eanders
#  n95oemhl.mak -> S:\NOEMHLP5\VCS\n95oemhl.mav
#  added bmpsw
#  
#     Rev 1.1   08 Sep 1998 14:48:02   eanders
#  n95oemhl.mak -> S:\NOEMHLP5\VCS\n95oemhl.mav
#  updated project files for oem 
#  
#     Rev 1.0   20 May 1998 22:56:22   EANDERS
#  Initial revision.
#  
#     Rev 1.1   19 May 1998 13:03:26   EANDERS
#  n95oemhl.mak -> S:\NOEMHLP\VCS\n95oemhl.mav
#  changed .hpj to navw32.hpj
#  
#     Rev 1.0   19 May 1998 12:18:18   EANDERS
#  Initial revision.
#  
#     Rev 1.1   30 Apr 1998 16:31:50   EANDERS
#  navdcmhl.mak -> S:\NAVWHLP\VCS\navdcmhl.mav
#  changed suport file name
#  
#     Rev 1.0   30 Apr 1998 16:18:44   EANDERS
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
    NAVCHELP.RTF    \
    NAVINTRO.RTF    \
    NAVTASK.RTF     \
    NAVHBTN.RTF      \
    NAVW32.CNT      \
    NAVWHLP5.H       \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \
    NAVOPTS.RTF   \
    HELP2.BMP     \
    CD.BMP        \
    BULIT.BMP     \
    NAVDIC.RTF    \
