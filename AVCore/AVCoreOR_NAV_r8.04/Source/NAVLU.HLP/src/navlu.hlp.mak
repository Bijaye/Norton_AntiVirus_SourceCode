#
# $Header:   S:/NAVLU.HLP/VCS/navlu.mav   1.2   04 Aug 1997 16:50:46   EANDERS  $
#
# Makes help for NAV LiveUpdate
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVLU.HLP/VCS/navlu.mav  $
#  
#     Rev 1.2   04 Aug 1997 16:50:46   EANDERS
#  navlu.mak -> S:\NAVLU.HLP\VCS\navlu.mav
#  added info bitmap
#  
#     Rev 1.1   04 Aug 1997 16:42:18   EANDERS
#  navlu.mak -> S:\NAVLU.HLP\VCS\navlu.mav
#  added the bullet bitmap
#  
#     Rev 1.0   16 Jun 1997 16:22:06   JBRENNA
#  Initial revision.
#  
#-------------------------------------------------------------------------

HELP_FILE=NavLu32.hlp
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE): \
    NavLu.hpj \
    NavLu.rtf \
    symw4002.mrb \
    symw4006.mrb  \
