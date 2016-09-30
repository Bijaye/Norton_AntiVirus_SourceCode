#
# $Header:   S:/NAVWHLP5/VCS/supp9x.mav   1.0   01 Aug 1998 18:24:14   EANDERS  $
#
# Makes help for Norton AntiVirus for Windows 95
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVWHLP5/VCS/supp9x.mav  $
#  
#     Rev 1.0   01 Aug 1998 18:24:14   EANDERS
#  Initial revision.
#  
#     Rev 1.6   22 Jul 1998 22:02:24   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added warning.bmp
#  
#     Rev 1.5   21 Jul 1998 18:41:52   EANDERS
#  NAVNTHLP.MAK -> S:\NAVWHLP5\VCS\NAVNTHLP.MAv
#  added tip and warning
#  
#     Rev 1.4   21 Jul 1998 18:03:52   EANDERS
#  NAVWHLP.MAK -> S:\NAVWHLP5\VCS\NAVWHLP.MAv
#  added tip and warning bmps replacing symw4003 and 4005
#  
#     Rev 1.3   21 Jul 1998 17:54:02   EANDERS
#  support.mak -> S:\NAVWHLP5\VCS\support.mav
#  added bulit.bmp
#  
#     Rev 1.2   20 Jul 1998 00:14:54   EANDERS
#  support.mak -> S:\NAVWHLP5\VCS\support.mav
#  added bitmap and contents (again!); changed target to support.hlp
#  
#     Rev 1.1   29 Jun 1998 23:33:46   EANDERS
#  No change.
#  
#     Rev 1.0   03 Jun 1998 17:58:46   EANDERS
#  Initial revision.
#  
#     Rev 1.2   03 Jun 1998 12:08:56   EANDERS
#  sample.mak -> S:\NAVWHLP\VCS\sample.mav
#  added cnt
#  
#     Rev 1.1   03 Jun 1998 11:57:34   EANDERS
#  sample.mak -> S:\NAVWHLP\VCS\sample.mav
#  spaces
#  
#     Rev 1.0   03 Jun 1998 11:54:28   EANDERS
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

HELP_FILE=SUPP9X.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):      \
    SUPP9X.HPJ    \
    STRTPG9X.RTF     \
    SUPPORT.CNT     \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \
    CD.BMP          \
    HELP2.BMP       \
    BULIT.BMP       \
    TIP.BMP         \
    WARNING2.BMP     \
    WARNING.BMP     \
