TARGET_NAME=NAVAPEL

#-------------------------------------------------------------------------
#
# Check for NTK platform.
#
#-------------------------------------------------------------------------
%If "$(CORE_SOFT)" == "K"

TARGET_TYPE = SYS

RES_FILE =
RES_SRC  =
DVX_FILE =

PLATOBJS = navapent.obj

LIBS = \
 ntoskrnl.lib \
 hal.lib      \
 SYMKRNL.LIB  \
 DRVCOMM.LIB  \
%If "$(CORE_PLAT)" == "I"
 int64.lib
%EndIf

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

LINKOPTSExt1=-MERGE:_PAGE=PAGE -MERGE:_TEXT=.text
LINKOPTSExt2=-MERGE:.rdata=.text
LINKOPTSExt3=-SECTION:INIT,d -OPT:REF
LINKOPTSExt4=-RELEASE -INCREMENTAL:NO -FULLBUILD -FORCE:MULTIPLE
LINKOPTSExt5=-version:3.51 -osversion:3.51 -align:0x20 -subsystem:native,3.51
%If "$(CORE_PLAT)" == "I"
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry@8
%Else
LINKOPTSExt6=-base:0x10000 -entry:DriverEntry
%EndIf

SYSLIBS=

%ElseIf "$(CORE_SOFT)" == "V"

TARGET_TYPE = VXD

RES_FILE = NAVAPEVX.RES
RES_SRC  = NAVAPEVX.RC
DVX_FILE = NAVAPEL.DVX

PLATOBJS = vxdentry.obj \
           navapevx.obj

LIBS = \
 DRVCOMM.LIB \
 SYMKRNL.LIB

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

%EndIf

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)

$(PROGRAM_NAME): $(PLATOBJS)  \
                 navapel.obj  \
                 pel.obj      \
                 $(LIBS)      \
                 $(RES_FILE)  \
                 $(DVX_FILE)