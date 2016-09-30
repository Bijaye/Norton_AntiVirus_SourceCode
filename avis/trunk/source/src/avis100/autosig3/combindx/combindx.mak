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
$(OBJD)\combindx.obj

depend:
  if not exist $(OBJD) mkdir $(OBJD)
  if not exist $(OUTD) mkdir $(OUTD)
  if not exist $(LIBD) mkdir $(LIBD)

compile:  $(OBJLIST)

exedll:   $(OUTD)\combindx.exe

library:

all:      depend compile exedll library

!ifdef BCHECKER
CPP=$(BCHECKERDIR)\nmcl
LINK=$(BCHECKERDIR)\nmlink
!else
CCP=$(MSDEVDIR)\bin\cl
LINK=$(MSDEVDIR)\bin\link
!endif
CPPOPTS=/c $(COPTS) $(OT)

$(OBJD)\combindx.obj:     $(SRCD)\combindx.c $(SRCD)\triedb.h
  $(CPP) $(CPPOPTS) /Fo$@ $(SRCD)\combindx.c

$(OUTD)\combindx.exe:     $(OBJD)\combindx.obj
   $(LINK) $(CVL) @<<
$(OBJD)\combindx.obj
/OUT:$(OUTD)\combindx.exe
/WARN:3
/MAP
/OPT:REF
/RELEASE
<<
