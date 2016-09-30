#-----------------------------------------------------------------------------
#  $Workfile:   restore.mak  $
#  $Revision:   1.0  $
#
#  Copyright (c) 1996, Symantec Corporation.  All rights reserved.
#
#  LANG          : Microsoft C7.0
#  STATUS        : Development
#
#  Make file for the 16-bit rescue disk apps.
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
#   INCLUDES/LIBS/OBJS
#-----------------------------------------------------------------------------
TOOLROOTDIR=.\Bin16
LIB=.\Lib16

OBJS = $(OUTDIR)\norton.obj $(OUTDIR)\events.obj $(OUTDIR)\startup.obj \
       $(OUTDIR)\ops.obj $(OUTDIR)\util.obj $(OUTDIR)\launch.obj \
       $(OUTDIR)\cmdio.obj $(OUTDIR)\cmdalloc.obj $(OUTDIR)\sort.obj \
       $(OUTDIR)\editlist.obj $(OUTDIR)\config.obj $(OUTDIR)\cvars.obj \
       $(OUTDIR)\nistr.obj

OBJ1 = $(OUTDIR)\norton.obj+$(OUTDIR)\events.obj+$(OUTDIR)\startup.obj+ \
       $(OUTDIR)\ops.obj+$(OUTDIR)\util.obj+ $(OUTDIR)\launch.obj+ \
       $(OUTDIR)\cmdio.obj+ $(OUTDIR)\cmdalloc.obj+ $(OUTDIR)\sort.obj+ \
       $(OUTDIR)\editlist.obj+ $(OUTDIR)\config.obj+ $(OUTDIR)\cvars.obj+ \
       $(OUTDIR)\nistr.obj

LIBS = $(LIB)\llibce.lib $(LIB)\langl.lib $(LIB)\nortonl.lib
CPPS = norton.c events.c startup.c ops.c util.c launch.c cmdio.c cmdalloc.c sort.c \
       editlist.c config.c cvars.c nistr.c

#-----------------------------------------------------------------------------
#   BUILD
#-----------------------------------------------------------------------------
!IF "$(DEBUG)"=="ON"

# debug-only flags
MORELFLAGS = /CO
MORECFLAGS = /f /Zi /G2e /Od /D_DEBUG

!else

# release-only flags
MORELFLAGS = 
MORECFLAGS = /O /Os /G3s

!endif

LFLAGS =  /NOD /MAP /NOI /B /X /QUICKPACK:65

CFLAGS =  /Gs /G2 /Zp /Zl /J /c /W3 /Os /AL /Zi /DEMERGENCY /I.\inc16
EXE_NAME=startup.exe


.c.obj:

#
# BASIC DEPENDENCY
#
ALL: $(OUTDIR)\$(EXE_NAME)

#
# BUILD OBJ FILES
#
#src\nort-hlp.h: src\norton.hs
#       .\bin16\HELPCOMP.EXE src\norton.hs

$(OUTDIR)\norton.obj: src\norton.c src\nort-hlp.h
        del $(OUTDIR)\*.err
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\norton.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\events.obj: src\events.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\events.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\startup.obj: src\startup.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\startup.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\ops.obj: src\ops.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\ops.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\util.obj: src\util.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\util.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\launch.obj: src\launch.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\launch.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\cmdio.obj: src\cmdio.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\cmdio.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\cmdalloc.obj: src\cmdalloc.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\cmdalloc.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\sort.obj: src\sort.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\sort.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\editlist.obj:  src\editlist.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\editlist.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\config.obj:  src\config.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\config.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\cvars.obj:  src\cvars.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\cvars.c > $*.err
        set CL=
        type $*.err

$(OUTDIR)\nistr.obj:  src\nistr.c 
        set CL=$(CFLAGS)
        .\bin16\cl -D$(*B)_c -Fo$*.obj src\nistr.c > $*.err
        set CL=
        type $*.err


$(OUTDIR)\lnk.opt:
       echo /NOD/MAP/NOI/B/X/QUICKPACK:65+LIB16\lcrt0.obj+$(OUTDIR)\norton.obj+$(OUTDIR)\events.obj+$(OUTDIR)\startup.obj+$(OUTDIR)\ops.obj+$(OUTDIR)\util.obj+$(OUTDIR)\launch.obj+$(OUTDIR)\cmdio.obj+$(OUTDIR)\cmdalloc.obj+$(OUTDIR)\sort.obj+$(OUTDIR)\editlist.obj+$(OUTDIR)\config.obj+$(OUTDIR)\cvars.obj+$(OUTDIR)\nistr.obj, > $(OUTDIR)\lnk.opt
       echo startup.exe, >> $(OUTDIR)\lnk.opt
       echo startup.map, >> $(OUTDIR)\lnk.opt
       echo lib16\nulibl+lib16\nortonl+lib16\vmm_l+lib16\llibce.lib+lib16\langl.lib >> $(OUTDIR)\lnk.opt


#
# BUILD EXE FILE
#
$(OUTDIR)\$(EXE_NAME): $(OBJS) $(OUTDIR)\lnk.opt
        .\bin16\olink.exe @$(OUTDIR)\lnk.opt

	copy $(EXE_NAME)       $(OUTDIR)\$(EXE_NAME)
	copy src\emergncy.cmd  $(OUTDIR)
    del  $(EXE_NAME)

