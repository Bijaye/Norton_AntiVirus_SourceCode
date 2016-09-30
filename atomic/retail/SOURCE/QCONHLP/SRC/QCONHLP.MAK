#
# $Header:   S:/QCONHLP/VCS/qconhlp.mav   1.1   21 May 1998 11:50:42   EANDERS  $
#
# Makes help for Quarantine console
#
#-------------------------------------------------------------------------

HELP_FILE=QCONSOLE.HLP
TARGET_TYPE=HLP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(HELP_FILE):       \
    QCONSOLE.HPJ      \
    QCONHLP.RTF    \
    QCONSOLE.CNT      \
    QCONHLP.H       \
    SYMW4002.MRB    \
    SYMW4003.MRB    \
    SYMW4005.MRB    \
    SYMW4006.MRB    \

