#
# $Header:   S:/NOEMHLP5/VCS/SUPPORT.MAv   1.0   20 May 1998 22:56:26   EANDERS  $
#
# Makes help for Norton AntiVirus for Windows 95
#
#-------------------------------------------------------------------------
# $Log:   S:/NOEMHLP5/VCS/SUPPORT.MAv  $
#  
#     Rev 1.0   20 May 1998 22:56:26   EANDERS
#  Initial revision.
#  
#     Rev 1.0   19 May 1998 12:17:46   EANDERS
#  Initial revision.
#  
#     Rev 1.6   04 May 1998 22:13:02   VGLOSTE
#  Added dependency on support.h.
#  
#     Rev 1.5   04 May 1998 21:19:02   VGLOSTE
#  Made it work.  The HPJ file needed to be named SUPPORT.HPJ.
#  
#     Rev 1.4   04 May 1998 11:50:36   EANDERS
#  renamed suport1.rtf suportm.rtf
#  
#     Rev 1.3   04 May 1998 11:44:06   EANDERS
#  supportm.mak -> S:\NAVWHLP\VCS\supportm.mav
#  trying to fix bug
#  
#     Rev 1.2   04 May 1998 11:28:44   EANDERS
#  supportm.mak -> S:\NAVWHLP\VCS\supportm.mav
#  removed space
#  
#     Rev 1.1   04 May 1998 11:25:48   EANDERS
#  supportm.mak -> S:\NAVWHLP\VCS\supportm.mav
#  filename change
#  
#     Rev 1.0   04 May 1998 11:20:10   EANDERS
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

HELP_FILE=SUPPORT.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    SUPPORT.HPJ     \
	SUPPORT.H       \
    SUPPORT.CNT     \
    SUPPORT.RTF

