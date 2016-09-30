#############################################################################
#  Copyright 1995 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/worm.mav   1.3   26 Dec 1996 15:23:14   AOONWAL  $
#
#  Description:
#       This is a make file for WORM stub NAVEX.
#
#  See Also:
#       VIRSCAN and AVAPI projects.
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/worm.mav  $
#  
#     Rev 1.3   26 Dec 1996 15:23:14   AOONWAL
#  No change.
#  
#     Rev 1.2   02 Dec 1996 14:03:22   AOONWAL
#  No change.
#  
#     Rev 1.1   29 Oct 1996 13:00:34   AOONWAL
#  No change.
#  
#     Rev 1.0   25 Oct 1995 13:19:42   DCHI
#  Initial revision.
#  
#############################################################################

# Module objects


####################################################
#                     W 3 2                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".W32")

    TARGET_NAME=WNAVEX32
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex32.d32    \
                 worm.obj



####################################################
#                     W I N                        #
####################################################

%ElseIf ("$[u,$(PLATFORM)]" == ".WIN")

    TARGET_NAME=WNAVEX16
    TARGET_TYPE=DLL
    PROGRAM_NAME=$(TARGET_NAME).DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME): navex16.def    \
                 worm.obj

%Endif

#UPDATE#
worm.obj:  platform.h ctsn.h callback.h navex.h
#ENDUPDATE#


