#-------------------------------------------------------------------------
#
# $Header:   S:/NAVAPI/VCS/navl312.mav   1.0   06 Aug 1998 21:07:20   DHERTEL  $
#
# NAV API Library Helper NLM (NAVL312.NLM) makefile.
#
#-------------------------------------------------------------------------
# $Log:   S:/NAVAPI/VCS/navl312.mav  $
#  
#     Rev 1.0   06 Aug 1998 21:07:20   DHERTEL
#  Initial revision.
#-------------------------------------------------------------------------

%If ("$[u,$(PLATFORM)]" == ".NLM")


TARGET_NAME=NAVL312
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

NAVL312.nlm: \
    navl312.den \
    navl312.obj \
    navl312.imp \
    navl312.exp 


%Else


NAVL312.NLM:
    Echo Nothing to build for this plaftform


%Endif


