# Copyright 1992-1993 Symantec, Peter Norton Product Group
#########################################################################
#
# $Header:   S:/AVENGE/VCS/avenge.mav   1.4   21 Oct 1998 13:06:00   MKEATIN  $
#
# Description:
#  Make file for AVENGE library.
#
#########################################################################
#
# $Log:   S:/AVENGE/VCS/avenge.mav  $
#  
#     Rev 1.4   21 Oct 1998 13:06:00   MKEATIN
#  Added the -ALfw switch for DX, DVX, NCX and DOS.  This make sure the
#  compiler does not assume DS == SS.
#  
#     Rev 1.3   11 Aug 1998 13:21:30   MKEATIN
#  Added the /ALfw
#  
#     Rev 1.2   21 Jul 1998 14:47:40   MKEATIN
#  Use large model on WIN16.
#  
#     Rev 1.1   20 May 1997 13:18:54   CNACHEN
#  Added support for virus list functions.
#  
#     Rev 1.0   18 Apr 1997 13:39:12   MKEATIN
#  Initial revision.
#  
#     Rev 1.5   25 Oct 1996 13:08:44   CNACHEN
#  Removed mutex.h and symsync.h
#  
#     Rev 1.4   04 Jun 1996 17:42:34   DCHI
#  Changes conforming to modifications in include structure.
#  
#     Rev 1.3   03 Jun 1996 11:38:10   DCHI
#  Added engcopyf.obj to list.
#  
#     Rev 1.2   17 May 1996 14:13:50   RAY
#  added bootrep.obj
#  
#     Rev 1.1   16 May 1996 14:14:14   CNACHEN
#  
#     Rev 1.0   13 May 1996 16:49:14   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=AVENGE
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

%If ("$[u,$(PLATFORM)]" == ".DOS") || \
    ("$[u,$(PLATFORM)]" == ".NCX") || \
    ("$[u,$(PLATFORM)]" == ".DVX") || \
    ("$[u,$(PLATFORM)]" == ".DX")  || \
    ("$(CORE_WIN16)"    != "")
    MODEL = L
    XOPTS += -ALfw
%Endif

$(PROGRAM_NAME):        \
        algload.obj     \
        algscan.obj     \
        avcache.obj     \
        avenge.obj      \
        booload.obj     \
        bootrep.obj     \
        crcload.obj     \
        crcscan.obj     \
        delentry.obj    \
        engcopyf.obj    \
        filerep.obj     \
        fstart.obj      \
        getinfo.obj     \
        infoaux.obj     \
        memload.obj     \
        memscan.obj     \
        reparaux.obj    \
        namec.obj       \
        infc.obj        \
        getbulk.obj     \
        vdatfile.obj

#UPDATE#

namec.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
infc.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
getbulk.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
algload.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
algscan.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
avcache.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
avenge.obj:  avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
booload.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
crcload.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
crcscan.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
delentry.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
engcopyf.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
filerep.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
fstart.obj:  avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
getinfo.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
infoaux.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
memload.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
memscan.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
reparaux.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
vdatfile.obj: avenge.h avengel.h avcb.h avendian.h avobject.h avtypes.h
bootrep.obj: avenge.h avengel.h avcb.h avobject.h avtypes.h bootrep.h


#ENDUPDATE#

