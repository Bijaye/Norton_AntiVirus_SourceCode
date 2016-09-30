#
# $Header:   S:/SCHEDHLP/VCS/schedhlp.mav   1.9   22 Jul 1998 16:44:52   EANDERS  $
#
# Makes help for Norton Schedule for Windows 95
#
#-------------------------------------------------------------------------
# $Log:   S:/SCHEDHLP/VCS/schedhlp.mav  $
#  
#     Rev 1.9   22 Jul 1998 16:44:52   EANDERS
#  schedhlp.mak -> S:\SCHEDHLP\VCS\schedhlp.mav
#  wn
#  
#     Rev 1.8   22 Jul 1998 16:33:12   EANDERS
#  schedhlp.mak -> S:\SCHEDHLP\VCS\schedhlp.mav
#  added warning
#  
#     Rev 1.7   27 Jun 1995 08:24:22   EANDERS
#  schedhlp.mak -> S:\schedhlp\vcs\schedhlp.mav
#  added symw4006.mrb
#  
#     Rev 1.6   19 Jun 1995 03:10:54   EANDERS
#  schedhlp.mak -> S:\schedhlp\vcs\schedhlp.mav
#  added mrbs
#  
#     Rev 1.5   24 May 1995 20:10:46   BARRY
#  Added nps_hlp.h to list of dependencies.  (Gary)
#  
#     Rev 1.4   21 Apr 1995 18:47:36   MFALLEN
#  
#     Rev 1.3   21 Apr 1995 18:21:24   MFALLEN
#  Renamed schedule.hlp to nsched32.hlp
#  
#     Rev 1.2   24 Mar 1995 10:59:30   DALLEE
#  Added contents file.
#  
#     Rev 1.1   13 Mar 1995 10:22:26   MFALLEN
#  Fixed for scheduler files.
#  
#     Rev 1.0   13 Mar 1995 10:20:08   MFALLEN
#  Initial revision.
#  
#-------------------------------------------------------------------------

HELP_FILE=NSCHED32.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE): \
    NPS_HLP.H \
    NSCHED32.HPJ \
    NSCHED32.CNT \
    SCHEDULE.RTF \
    SYMW4003.MRB \
    symw4005.mrb \
    symw4002.mrb \
    symw4006.mrb \
    warning.bmp \
