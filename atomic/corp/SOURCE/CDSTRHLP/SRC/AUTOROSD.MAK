#
# $Header:   S:/CDSTRHLP/VCS/autorosd.mav   1.3   25 Aug 1997 21:13:20   GWESTER  $
#
# Makes WinHelp file that launches CDStart Applications
#
#-------------------------------------------------------------------------
# $Log:   S:/CDSTRHLP/VCS/autorosd.mav  $
#  
#     Rev 1.3   25 Aug 1997 21:13:20   GWESTER
#  Added EXITBTN.BMP as a button.
#  
#     Rev 1.2   17 Aug 1997 22:44:28   GWESTER
#  Added CD.BMP as a button.
#  
#     Rev 1.1   28 Jul 1997 00:28:34   EANDERS
#  autotesl.mak -> S:\CDSTRHLP\VCS\autotesl.mav
#  added symw4002.bmp as a button
#  
#     Rev 1.0   19 Jul 1997 16:47:22   GWESTER
#  Initial revision.
#  
#-------------------------------------------------------------------------

HELP_FILE=AUTOROSD.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    AUTOROSD.HPJ    \
    AUTORUN.RTF    \
    CDSTRHLP.H       \
    HELP2.BMP       \
    CD.BMP       \
    EXITBTN.BMP       \
    SYMW4002.BMP     \    
    
