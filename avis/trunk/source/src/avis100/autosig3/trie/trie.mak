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
$(OBJD)\fzmalloc.obj \
$(OBJD)\trie.obj \

depend:
  if not exist $(OBJD) mkdir $(OBJD)
  if not exist $(OUTD) mkdir $(OUTD)
  if not exist $(LIBD) mkdir $(LIBD)

compile:  $(OBJLIST)

exedll:   $(OUTD)\trie.exe

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

$(OBJD)\fzmalloc.obj:     $(SRCD)\fzmalloc.c $(SRCD)\fzmalloc.h
  $(CPP) $(CPPOPTS) /Fo$@ $(SRCD)\fzmalloc.c

$(OBJD)\dbutil.obj:     $(SRCD)\dbutil.c $(SRCD)\dbutil.h $(SRCD)\triedb.h
  $(CPP) $(CPPOPTS) /Fo$@ $(SRCD)\dbutil.c

$(OBJD)\trie.obj:     $(SRCD)\trie.c $(SRCD)\triedb.h $(SRCD)\dbutil.h
  $(CPP) $(CPPOPTS) /Fo$@ $(SRCD)\trie.c

$(OUTD)\trie.exe:     $(OBJD)\trie.obj $(OBJD)\dbutil.obj $(OBJD)\fzmalloc.obj
   $(LINK) $(CVL) @<<
$(OBJD)\trie.obj $(OBJD)\dbutil.obj $(OBJD)\fzmalloc.obj
/OUT:$(OUTD)\trie.exe
/WARN:3
/MAP
/OPT:REF
/RELEASE
<<

