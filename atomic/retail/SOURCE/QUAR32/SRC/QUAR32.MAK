##########################################################################
# $Header:   S:/QUAR32/VCS/quar32.mav   1.11   23 Jun 1998 15:22:12   mdunn  $
# MAKEFILE for QUAR32.DLL
#
#
##########################################################################
# $Log:   S:/QUAR32/VCS/quar32.mav  $
#  
#     Rev 1.11   23 Jun 1998 15:22:12   mdunn
#  Added quar32.res
#  
#     Rev 1.10   15 May 1998 18:40:52   jtaylor
#  Added global.obj to the build.
#
#     Rev 1.9   07 May 1998 16:24:26   jtaylor
#  Changed the chanel and patch lib files to be loaded from the correct platform.
#
#     Rev 1.8   24 Apr 1998 14:35:28   DBuches
#  Added MPR.LIB.
#
#     Rev 1.7   07 Apr 1998 22:38:18   SEDWARD
#  Added fslink.lib.
#
#     Rev 1.6   16 Mar 1998 19:00:48   SEDWARD
#  Added 'QuarOpts.obj' to object file list.
#
#     Rev 1.5   16 Mar 1998 18:11:12   SEDWARD
#  Added chan32i.lib and patch32i.lib (for microdef support).
#
#     Rev 1.4   06 Mar 1998 19:37:36   JTaylor
#  added necessary lib files
#
#     Rev 1.3   27 Feb 1998 11:52:58   DBuches
#  Use D32 file instead of DEF files for Win32 platforms.
#
#     Rev 1.2   26 Feb 1998 11:38:08   jtaylor
#  Added QuarAdd.lib
#
#     Rev 1.1   25 Feb 1998 14:00:44   DBuches
#  Added EnumQuarantineItems.OBJ.
#
#     Rev 1.0   24 Feb 1998 17:40:02   DBuches
#  Initial revision.
#
##########################################################################
TARGET_TYPE=DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

CLOPTS+= /GX
PROGRAM_NAME=QUAR32.DLL


                                        ## Common Libs

LIBS+= ole32.lib uuid.lib quaradd.lib n32cores.lib s32navn.lib n32call.lib s32debug.lib defutils.lib fslink.lib mpr.lib
LIBS+= chan32$(CORE_PLAT).lib
LIBS+= patch32$(CORE_PLAT).lib

$(PROGRAM_NAME): DLLMAIN.OBJ \
                 FACTORY.OBJ \
                 QuarantineDLL.OBJ \
                 QuarantineItem.obj \
                 QuarOpts.obj \
                 EnumQuarantineItems.obj \
                 Global.obj \
                 quar32.res \
                 QUAR32.D32

#UPDATE#
#ENDUPDATE#




