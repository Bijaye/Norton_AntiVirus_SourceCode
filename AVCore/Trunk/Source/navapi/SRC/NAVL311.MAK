#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAPI/VCS/navl311.mav   1.0   06 Aug 1998 21:07:12   DHERTEL  $
#
# NAV API Library Helper NLM (NAVL311.NLM) makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVAPI/VCS/navl311.mav  $
#  
#     Rev 1.0   06 Aug 1998 21:07:12   DHERTEL
#  Initial revision.
#-------------------------------------------------------------------------

%If ("$[u,$(PLATFORM)]" == ".NLM")


TARGET_NAME=NAVL311
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

NAVL311.nlm: \
    navl311.den \
    navl311.obj \
    navl311.imp \
    navl311.exp 


%Else


NAVL311.NLM:
    Echo Nothing to build for this plaftform


%Endif


