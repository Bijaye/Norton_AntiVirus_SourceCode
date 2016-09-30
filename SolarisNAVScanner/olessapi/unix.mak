# unix.mak
# Unix makefile for the acsapi (Access), msoapi (MS Office), 
# o97api (Office 97), olessapi (OLE Struct. Stg.), ppapi (Power Point), 
# wd7api (Word 97), and xl5api (Excel 5) libraries.

TARGETS = \
libacsapi.a \
libmsoapi.a \
libo97api.a \
libolessapi.a \
libppapi.a \
libwd7api.a \
libxl5api.a

include ../unixmake.inc

CCFLAGS += -DNDEBUG $(CCSHAREDFLAGS)

ACSAPIOBJS = \
acsapi.o \
a97lznt.o \
a2api.o

MSOAPIOBJS = \
msoapi.o

O97APIOBJS = \
o97api.o \
o97lznt.o \
o97mod.o \
o97wdapi.o \
o97xlapi.o \
w97rmtxt.o 
#crc32.lib

OLESSAPIOBJS = \
olessapi.o \
olestrnm.o \
summary.o

PPAPIOBJS = \
ppapi.o

WD7APIOBJS = \
wd7api.o \
wd7encdc.o \
wd7rmtxt.o
#crc32.lib

XL5APIOBJS = \
xl5api.o
#crc32.lib

libacsapi.a: $(ACSAPIOBJS)
	$(AR) $(ARFLAGS) $@ $(ACSAPIOBJS)

libmsoapi.a: $(MSOAPIOBJS)
	$(AR) $(ARFLAGS) $@ $(MSOAPIOBJS)

libo97api.a: $(O97APIOBJS)
	$(AR) $(ARFLAGS) $@ $(O97APIOBJS)

libolessapi.a: $(OLESSAPIOBJS)
	$(AR) $(ARFLAGS) $@ $(OLESSAPIOBJS)

libppapi.a: $(PPAPIOBJS)
	$(AR) $(ARFLAGS) $@ $(PPAPIOBJS)

libwd7api.a: $(WD7APIOBJS)
	$(AR) $(ARFLAGS) $@ $(WD7APIOBJS)

libxl5api.a: $(XL5APIOBJS)
	$(AR) $(ARFLAGS) $@ $(XL5APIOBJS)
