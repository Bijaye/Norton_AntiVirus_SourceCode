# Copyright 1996 Symantec Corporation
#############################################################################
#
# $Header:   S:/NAVWINX5/VCS/NAVWINX5.mav   1.0   01 Jun 1998 16:10:14   DBuches  $
#
# Description:
#
#############################################################################
# $Log:   S:/NAVWINX5/VCS/NAVWINX5.mav  $
#  
#     Rev 1.0   01 Jun 1998 16:10:14   DBuches
#  Initial revision.
#
#     Rev 1.1   19 May 1998 15:51:04   PAGBABI
#  Added About.obj
#
#     Rev 1.0   01 Mar 1998 18:58:54   pagbabi
#  Initial revision.
#############################################################################

TARGET_TYPE=DLL
CHAR_SET=U                                      # UNICODE only.
USE_MFC_DLL=TRUE
USE_CRT_DLL=TRUE
PACKING_ALIGNMENT=8
PDB=TRUE

.INCLUDE        $(QUAKEINC)\PLATFORM.MAK

PROGRAM_NAME=NAVWINX5.DLL

FIRSTLIBS += ole32.lib oleaut32.lib oledlg.lib comctl32.lib uuid.lib netapi32.lib rpcrt4.lib

# ** Specify whether to use static MFC or CRT libraries. Remove if not used.
%if "$(CV)" != ""

DEBUG_CRT=YES
DEBUG_AFX=YES

CLOPTS+= /Zi /GR /GX /FR /Yc"stdafx.h" /Fp"navwinx5.pch" /Yu"stdafx.h" /FD /c

FIRSTLIBS += msvcprtd.lib

LINKOPTS= /nologo /subsystem:windows /dll /debug /pdb:"navwinx5.pdb" /nod

%else

CLOPTS+= /GR /GX /FR /Yc"stdafx.h" /Fp"navwinx5.pch" /Yu"stdafx.h" /FD /c
# CLOPTS+= /Od

FIRSTLIBS += msvcprt.lib

%endif

RCBINDOPTS+= -K
LIBS+= MMC.LIB

navwinx.dll :   About.obj               \
                NavNode.obj             \
                NavView.obj             \
                NavWinX.obj             \
                NavWinX.d32             \
                NavWinX.res             \
			    stdafx.obj              \
			    mmcbase.lib

#UPDATE#
navnode.obj:	stdafx.h mmcbase.h afxtempl.h resource.h NavWinX.h rpc.h \
		rpcndr.h oaidl.h windows.h ole2.h objidl.h unknwn.h wtypes.h \
		ocidl.h oleidl.h NSDU_I.h register.h node.h baseC.h \
		namespace.h scopres.h navnode.h dataobj.h obquery.h \
		nodeview.h navview.h menuitem.h mmc.h
navview.obj:	stdafx.h mmcbase.h afxtempl.h resource.h NavWinX.h rpc.h \
		rpcndr.h oaidl.h windows.h ole2.h objidl.h unknwn.h wtypes.h \
		ocidl.h oleidl.h node.h baseC.h namespace.h scopres.h \
		navnode.h nodeview.h navview.h dataobj.h
navwinx.obj:	stdafx.h mmcbase.h afxtempl.h resource.h NavWinX.h rpc.h \
		rpcndr.h oaidl.h windows.h ole2.h objidl.h unknwn.h wtypes.h \
		ocidl.h oleidl.h node.h baseC.h namespace.h scopres.h \
		navnode.h dataobj.h
stdafx.obj:	stdafx.h mmcbase.h afxtempl.h stdafx.h
atl.obj:	stdafx.h resource.h atl.h atl_i.c RegObj.h
regobj.obj:	stdafx.h resource.h atl.h RegObj.h
#ENDUPDATE#
