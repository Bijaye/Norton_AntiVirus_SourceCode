#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/navexlgn.mav   1.1   22 Jul 1997 13:13:22   mdunn  $
#
#  Description:
#       This is a make file for NAVEXL
#
#############################################################################
#
#  $Log:   S:/NAVEX/VCS/navexlgn.mav  $
#  
#     Rev 1.1   22 Jul 1997 13:13:22   mdunn
#  Changed the name of the resulting LIB to NAVEXGN.LIB so as not to collide
#  w/existing NAVEX stuff.
#  
#     Rev 1.0   22 Jul 1997 10:13:40   mdunn
#  Initial revision.
#  
#     Rev 1.5   26 Dec 1996 15:23:16   AOONWAL
#  No change.
#  
#     Rev 1.4   02 Dec 1996 14:03:24   AOONWAL
#  No change.
#  
#     Rev 1.3   29 Oct 1996 13:00:36   AOONWAL
#  No change.
#  
#     Rev 1.2   10 May 1996 17:28:10   RSTANEV
#  Target name of the library should be NAVEX.LIB, not NAVEXL.LIB.
#
#     Rev 1.1   19 Apr 1996 14:39:34   RSTANEV
#  Now using NAVEXNTL.CPP instead of NAVEXNT.CPP.
#
#     Rev 1.0   17 Apr 1996 08:30:38   JMILLAR
#  Initial revision.
#
#############################################################################


####################################################
#                     N T K                        #
####################################################

%If ("$[u,$(PLATFORM)]" == ".NTK")

    TARGET_NAME=NAVEXGN
    TARGET_TYPE=LIB
    PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

    XOPTS += /DGRYNEC                   # See navexntl.cpp for an explanation

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

$(PROGRAM_NAME):        navexntl.obj

#UPDATE#
initnavx.obj:	platform.h dbcs_str.h dosvmm.h file.h ctsn.h callback.h \
		navex.h
x16_main.obj:	xpc_type.h xpc_dbug.h x16_enum.h x16_evnt.h x16_to32.h \
		x16_main.h                      7
naventry.obj:	platform.h dbcs_str.h dosvmm.h
navex.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexrb.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexrp.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexsb.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexsf.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h macrscan.h nepescan.h javascan.h
navexrf.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h macrorep.h neperep.h
navexsm.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexsp.obj:	platform.h dbcs_str.h dosvmm.h cts4.h ctsn.h callback.h \
		navex.h navexshr.h
navexshr.obj:	platform.h dbcs_str.h dosvmm.h
nechild.obj:	platform.h dbcs_str.h dosvmm.h ctsn.h callback.h \
		navexshr.h navex.h nlmload.h
worm.obj:	platform.h dbcs_str.h dosvmm.h callback.h ctsn.h navex.h
worddoc.obj:	storobj.h platform.h dbcs_str.h dosvmm.h callback.h \
		worddoc.h
storobj.obj:	storobj.h platform.h dbcs_str.h dosvmm.h callback.h \
		navexshr.h
olewrite.obj:	olewrite.h storobj.h platform.h dbcs_str.h dosvmm.h \
		callback.h navexshr.h
macrodel.obj:	macrodel.h worddoc.h platform.h dbcs_str.h dosvmm.h \
		callback.h storobj.h olewrite.h
macrscan.obj:	storobj.h platform.h dbcs_str.h dosvmm.h callback.h \
		worddoc.h macrscan.h ctsn.h navex.h mcrscndf.h
macrorep.obj:	storobj.h platform.h dbcs_str.h dosvmm.h callback.h \
		worddoc.h macrodel.h macrorep.h ctsn.h navex.h macrscan.h \
		mcrscndf.h mcrrepdf.h navexshr.h
mcrscndf.obj:	mcrscndf.h macrscan.h platform.h dbcs_str.h dosvmm.h \
		callback.h ctsn.h navex.h navexshr.h
mcrrepdf.obj:	mcrrepdf.h macrorep.h platform.h dbcs_str.h dosvmm.h \
		callback.h ctsn.h navex.h navexshr.h
docrepar.obj:	macrodel.cpp macrodel.h worddoc.h platform.h dbcs_str.h \
		dosvmm.h callback.h storobj.h olewrite.h macrorep.cpp \
		macrorep.h ctsn.h navex.h macrscan.h mcrscndf.h mcrrepdf.h \
		navexshr.h mcrscndf.cpp mcrrepdf.cpp olewrite.cpp storobj.cpp \
		worddoc.cpp
docscan.obj:	macrscan.cpp storobj.h platform.h dbcs_str.h dosvmm.h \
		callback.h worddoc.h macrscan.h ctsn.h navex.h mcrscndf.h \
		mcrscndf.cpp navexshr.h storobj.cpp worddoc.cpp
nepescan.obj:	nepescan.h platform.h dbcs_str.h dosvmm.h callback.h \
		ctsn.h navex.h navexshr.h winsig.h winconst.h nepeshr.h
winsig.obj:	winsig.h platform.h dbcs_str.h dosvmm.h callback.h \
		navexshr.h
winscan.obj:	nepescan.cpp nepescan.h platform.h dbcs_str.h dosvmm.h \
		callback.h ctsn.h navex.h navexshr.h winsig.h winconst.h \
		nepeshr.h nepeshr.cpp winsig.cpp
neperep.obj:	neperep.h platform.h dbcs_str.h dosvmm.h callback.h \
		ctsn.h navex.h navexshr.h winconst.h nepeshr.h
winrep.obj:	neperep.cpp neperep.h platform.h dbcs_str.h dosvmm.h \
		callback.h ctsn.h navex.h navexshr.h winconst.h nepeshr.h \
		nepeshr.cpp
nepeshr.obj:	nepeshr.h platform.h dbcs_str.h dosvmm.h callback.h \
		ctsn.h navex.h navexshr.h winconst.h
navexdx.obj:	platform.h dbcs_str.h dosvmm.h ctsn.h callback.h navex.h
javascan.obj:	javascan.h platform.h dbcs_str.h dosvmm.h callback.h \
		ctsn.h navex.h navexshr.h javasig.h javashr.h
javashr.obj:	javashr.h platform.h dbcs_str.h dosvmm.h callback.h \
		ctsn.h navex.h navexshr.h
javasig.obj:	javasig.h platform.h dbcs_str.h dosvmm.h callback.h \
		navexshr.h
java.obj:	javasig.cpp javasig.h platform.h dbcs_str.h dosvmm.h \
		callback.h navexshr.h javashr.cpp javashr.h ctsn.h navex.h \
		javascan.cpp javascan.h
navexnt.obj:	platform.h dbcs_str.h dosvmm.h avapi.h ctsn.h callback.h \
		navex.h
chk.obj:	CTSN.INC
#ENDUPDATE#

%EndIf
