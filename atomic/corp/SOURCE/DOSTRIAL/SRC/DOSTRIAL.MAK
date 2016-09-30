#######################################################################
## $Header:   S:/DOSTRIAL/VCS/dostrial.mav   1.0   10 Jul 1997 13:33:04   MKEATIN  $
#######################################################################

TARGET_TYPE=LIB

PROGRAM_NAME=DOSTRIAL.LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): Trial.obj  \
                 sldate.obj

Trial.obj:  Trial.cpp \
	      Trial.h sldate.h Platform.h

sldate.obj:  sldate.cpp \
	          sldate.h Platform.h
