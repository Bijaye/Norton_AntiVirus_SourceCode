#######################################################################
# $Header:   S:/NAVABOUT/VCS/navabout.mav   1.5   02 Jun 1998 17:38:00   mdunn  $
# $Log:   S:/NAVABOUT/VCS/navabout.mav  $
#  
#     Rev 1.5   02 Jun 1998 17:38:00   mdunn
#  Using $(CORE_PLAT) when calling patch/channel LIBs.
#  
#     Rev 1.4   17 Apr 1998 16:15:00   RStanev
#  SmellyCat changes.
#
#     Rev 1.3   17 Mar 1998 10:31:02   tcashin
#  Added chan32i.lib and patch32i.lib.
#
#     Rev 1.2   20 Aug 1997 11:09:22   KSACKIN
#  renamed the personality library to the correct name.
#
#     Rev 1.1   20 Aug 1997 11:07:26   KSACKIN
#  Added the personality dll to our make file.
#
#     Rev 1.0   19 Aug 1997 14:09:38   KSACKIN
#  Initial revision.
#
#     Rev 1.1.1.0   12 Aug 1997 22:44:48   DALVIDR
#  Branch base for version QAKG
#
#     Rev 1.1   01 Jul 1997 14:42:48   TIVANOV
#  new lib files to support the navabout service function
#
#     Rev 1.0   06 Jun 1997 20:02:28   BMCCORK
#  Initial revision.
#
#######################################################################

.INCLUDE <ENCORE.MAK>

PROGRAM_NAME=NAVABOUT.DLL
XOPTS+= /Fp"NavAbout.pch" /YX"StdAfx.h"
XTLNOPTS+= /EXPORT:DoStdAbout

#######################################################################

$(PROGRAM_NAME): exports.obj  \
                 navabout.obj \
                 navabout.res \
                 defutils.lib \
                 n32vlist.lib \
                 n32call.lib  \
                 n32pdll.lib  \
                 DefAnnty.lib \
                 patch32$(CORE_PLAT).lib \
                 chan32$(CORE_PLAT).lib  \
                 $(Q_SYMKRNL).lib

exports.obj: exports.cpp stdafx.h symlean.h regint.h aboutint.h \
        navabout.h regguid.h resource.h navdoabt.h

navabout.obj: navabout.cpp stdafx.h resource.h symlean.h regguid.h \
        regint.h regprop.h aboutint.h navabout.h

navabout.res: navabout.rc

