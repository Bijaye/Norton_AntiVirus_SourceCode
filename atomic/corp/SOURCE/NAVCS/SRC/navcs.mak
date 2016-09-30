TARGET_TYPE=DLL    ## Win32 DLL (x86)
USE_MFC_DLL=TRUE

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

#MKMK-UPDATE#
LIBS += S:\NOBUILDS.NT\SRC\clusapi$(CORE_PLAT).lib
LIBS += S:\NOBUILDS.NT\SRC\resutil$(CORE_PLAT).lib 

CLOPTS += -GX

ALL:    navcs.dll

navcs.dll:  clusinst.obj \
		clusnav.obj \
		clusnave.obj \
		nhsdebug.obj \
		PropList.obj

#UPDATE#
clusint.obj:  clusint.cpp clus.h
clusnav.obj:  clusnav.cpp clus.h
clusnave.obj: clusnave.cpp clus.h
nhsdebug.obj: nhsdebug.cpp nhsetup.h
PropList.obj: PropList.cpp clus.h
#ENDUPDATE#
#MKMK-ENDUPDATE#
