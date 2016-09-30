#####################################################################
# File: AtMigrat.mak
#
# Copyright 1992-1997 Symantec, Peter Norton Product Group
####################################################################
#
#  $Header:   S:/ATMIGRAT/VCS/ATMIGRAT.MAv   1.3   12 Feb 1999 17:43:00   MBROWN  $
#
#
#####################################################################
#
#  $Log:   S:/ATMIGRAT/VCS/ATMIGRAT.MAv  $
#  
#     Rev 1.3   12 Feb 1999 17:43:00   MBROWN
#  changed dll name from atmigrat to migrate since migrate proj is now unused and there's no longer a name conflict
#
#     Rev 1.2   28 Sep 1998 11:36:54   MBROWN
#  Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
#
#     Rev 1.1   02 Jun 1998 17:01:16   SEDWARD
#  Changed target to 'ATMIGRAT.DLL' so we won't overwrite 'MIGRATE.DLL'.
#
#     Rev 1.0   26 May 1998 14:41:10   SEDWARD
#  Initial revision.
#
#     Rev 1.0   26 May 1998 14:38:10   SEDWARD
#  Initial revision.
#
#
#####################################################################


TARGET_TYPE=DLL
PROGRAM_NAME = MIGRATE.DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

LIBS += UUID.LIB

CLOPTS += -Zp8
#LINKOPTS += /def:migrate.def

$(PROGRAM_NAME):        CREATMIG.OBJ            \
                        CREGENT.OBJ             \
                        CWSTRING.OBJ            \
                        GLOBAL.OBJ              \
                        MIGRANT.OBJ             \
                        MIGRATE.OBJ             \
                        NAVMIG95.OBJ            \
						SHELLLNK.OBJ			\
                        UTILS.OBJ               \
                        MIGRATE.D32             \
                        MIGRATE.RES


#UPDATE#
global.obj:	global.h Migrant.h CWString.h
navmig95.obj:	global.h Migrant.h CWString.h NavMig95.h options.h \
		navopts.h exclude.h cts4.h ctsn.h callback.h callbk15.h \
		n30type.h avtypes.h platform.h dbcs_str.h dosvmm.h nlm_str.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h quarset.h \
		custom.h navbnhdr.h ApComm.h CRegEnt.h registry.h resource.h \
		Utils.h shelllnk.h global.h Migrant.h CWString.h
cregent.obj:	cregent.h cregent.h
creatmig.obj:	CreatMig.h Migrant.h CWString.h NavMig95.h options.h \
		navopts.h exclude.h cts4.h ctsn.h callback.h callbk15.h \
		n30type.h avtypes.h platform.h dbcs_str.h dosvmm.h nlm_str.h \
		excludel.h netobj.h symnet.h symnd2.h navstart.h quarset.h \
		custom.h navbnhdr.h ApComm.h
shelllnk.obj:	platform.h dbcs_str.h dosvmm.h nlm_str.h cwstring.h \
		shelllnk.h
cwstring.obj:	cwstring.h cwstring.h
migrant.obj:	global.h Migrant.h CWString.h Utils.h global.h
migrate.obj:	setupapi.h global.h Migrant.h CWString.h migrate.h \
		resource.h utils.h CreatMig.h CRegent.h registry.h setupapi.h \
		global.h Migrant.h CWString.h
utils.obj:	resource.h utils.h cwstring.h CRegEnt.h
migrate.res:	resource.h afxres.h winres.h nav.ver
#ENDUPDATE#
