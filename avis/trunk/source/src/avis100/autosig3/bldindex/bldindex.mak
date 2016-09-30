MO=/DWIN32 /Zp /W4
!ifdef BLDDEBUG
OT=/Od /MTd
CVC=/Zi
CVL=/DEBUG
!else
OT=/Oxw /MT
CVC=
CVL=
!endif

COPTS=$(MO) $(CVC) $(INCPATHS)

OBJLIST= \
$(OBJD)\bldindex.obj

depend:
  if not exist $(OBJD) mkdir $(OBJD)
  if not exist $(OUTD) mkdir $(OUTD)
  if not exist $(LIBD) mkdir $(LIBD)

compile:  $(OBJLIST)

exedll:   $(OUTD)\bldindex.exe

library:

all:      depend compile exedll library

!ifdef BCHECKER
CPP=$(BCHECKERDIR)\nmcl
LINK=$(BCHECKERDIR)\nmlink
!else
CPP=$(MSDEVDIR)\bin\cl
LINK=$(MSDEVDIR)\bin\link
!endif
CPPOPTS=/c $(COPTS) $(OT)

$(OBJD)\bldindex.obj:     $(SRCD)\bldindex.c $(SRCD)\triedb.h
  $(CPP) $(CPPOPTS) /Fo$@ $(SRCD)\bldindex.c

$(OUTD)\bldindex.exe:     $(OBJD)\bldindex.obj
   $(LINK) $(CVL) @<<
$(OBJD)\bldindex.obj
/OUT:$(OUTD)\bldindex.exe
/WARN:3
/MAP
/OPT:REF
/RELEASE
<<
