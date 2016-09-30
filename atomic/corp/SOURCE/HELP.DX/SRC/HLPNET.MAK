#
# $Header:   S:/help.dx/VCS/hlpnet.mav   1.0   06 Feb 1997 20:50:52   RFULLER  $
#
#   Makefile for NAV network update help.
#
#*************************************************************************
# $Log:   S:/help.dx/VCS/hlpnet.mav  $
#  
#     Rev 1.0   06 Feb 1997 20:50:52   RFULLER
#  Initial revision
#  
#     Rev 1.1   27 Apr 1994 14:53:38   DALLEE
#  Trunk conversion.
#*************************************************************************

HELP_FILE=NAVNET.HLP

TARGET_TYPE=HELP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

navnet.hlp: navnet.hs incldnet.ihs

