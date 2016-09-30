# Makefile for NAVLUCBK.DLL
# (NAV plugin for LUAll)

TARGET_TYPE=DLL
MODULE_NAME=NAVLUCBK
PROGRAM_NAME=$(MODULE_NAME).DLL

PCH=stdafx
USE_DEFAULT_LIBS=TRUE
USE_MFC_LIB=TRUE
USE_CRT_LIB=TRUE

Patch_LIB=Patch32$(CORE_PLAT).LIB
Channel_LIB=Chan32$(CORE_PLAT).LIB
DefUtils_LIB=DefUtils.LIB

%if "$(CV)" != ""
DEBUG_AFX=YES
DEBUG_CRT=YES
%endif

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

CLOPTS += /EHac- /DNO_SYMDBG /W3 /DSYM_MFC /D_ATL_STATIC_REGISTRY

LIBS += OLE32.LIB OLEAUT32.LIB UUID.LIB COMCTL32.LIB COMSUPP.LIB \
        n32cores.lib npsapi.lib n32call.lib \
        ctaskapi.lib defannty.lib \
        $(Q_SYMKRNL).LIB \
        $(DefUtils_LIB) \
        $(Patch_LIB) \
        $(Channel_LIB)


all : $(PROGRAM_NAME)

$(PROGRAM_NAME): NAVLUCBK.d32 \
                Stdafx.obj \
                luNavCallBack.obj \
                Myutils.obj \
                NavLu.obj \
                navlucallback.idl \
                NAVLUCBK.obj \
                navluprog.obj \
                navluutils.obj \
                progdlg.obj \
                navlucbk.res

navlucbk.res: navlucallback.idl

NavLuCallback_i.c: navlucallback.idl

navlucallback.idl:
    midl navlucallback.idl /tlb NavLuCallback.tlb /h NavLuCallback.h /iid NavLuCallback_i.c

