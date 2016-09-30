#######################################################################
# $Header:   S:/ABOUTSW/VCS/aboutsw.mav   1.4   16 Nov 1998 15:16:08   CEATON  $
# $Log:   S:/ABOUTSW/VCS/aboutsw.mav  $
#  
#     Rev 1.4   16 Nov 1998 15:16:08   CEATON
#  Added reference to SYMNAV library (necessary for font mapping).
#  
#  
#     Rev 1.3   29 Oct 1998 18:13:34   CEATON
#  Added reference to behavior DLL in .MAK file (needed for NAV_IS_OEM).
#  
#  
#     Rev 1.2   20 Oct 1998 20:26:26   CEATON
#  Added files comprising the SystemWorks upsell dialog box.
#  
#  
#     Rev 1.1   13 Oct 1998 18:27:24   rchinta
#  Replaced aboutext.* aboutsw.*
#  Removed all the unused code.
#  
#
#######################################################################

.INCLUDE <ENCORE.MAK>

PROGRAM_NAME=ABOUTSW.DLL
XOPTS+= /Fp"Aboutsw.pch" /YX"StdAfx.h"
XTLNOPTS+= /EXPORT:DoSystemWorksAbout /EXPORT:GetSystemWorksKey

#######################################################################

$(PROGRAM_NAME): exports.obj  \
                 aboutsw.obj \
                 sysworks.obj \
                 n32pdll.lib  \
                 $(Q_SYMGUI).LIB \
                 aboutsw.res 

exports.obj: exports.cpp stdafx.h symlean.h regint.h aboutint.h \
        aboutsw.h regguid.h resource.h

aboutsw.obj: aboutsw.cpp stdafx.h resource.h symlean.h regguid.h \
        regint.h regprop.h aboutint.h aboutsw.h

sysworks.obj: sysworks.cpp stdafx.h resource.h symlean.h sysworks.h navdoabt.h

aboutsw.res: aboutsw.rc

