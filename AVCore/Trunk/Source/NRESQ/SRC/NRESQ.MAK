
MODEL=S

TARGET_TYPE=EXE
PROGRAM_NAME=NRESQ32.EXE

DEF += /DRQ_IS_NAV=1 

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

OPT_CV=-Os
OPT_NOCV=-Os /Z7

OBJS= rescuew.obj  \
      RescueSv.obj \
      Rescuelb.obj \
      wrscodrw.obj \
      wrsccmd.obj  \
      wrsccopy.obj \
      wrscsys.obj  \
      wrscSpec.obj \
      wrscmisc.obj \
      wrscfmt.obj  \
      wrscini.obj  \
      wrscswit.obj \
      drvemap.obj  \
      wrscopt.obj  


nresq32.exe :  $(OBJS)           \
	       rescuew.res       \
	       rescuew.def       \
	       comctl32.lib      \
	       $(Q_SYMGUI).LIB   \
	       $(Q_SYMKRNL).LIB  \
	       $(Q_SYMUTIL).LIB  \
	       $(Q_SYMSYS).LIB   \
	       $(Q_SYMFMT).LIB   \
	       $(Q_SYMFAT).LIB   \
	       $(Q_SYMSTAT).LIB  \
	       defutils.lib      \
	       chan32i.lib       
#              numisc.lib        \

# Differences from rescuew32.exe :
# GUI, KRNL, and UTIL all map to S32NAV8.dll
# FMT, FAT, and SYS all map to S32RSQ.dll
# numisc.lib is dropped





# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------


#UPDATE#
# All dependency lists are a combination of the dependencies listed in the actual source files IN 
# ADDITION TO the (possibly unused) dependencies displayed in the original RESCUEW make file.


# ----------------------------------------------------------------------------


wrscopt.obj:    wrscopt.cpp quakever.h wndclass.h passwd.h stddos.h symsys.h stddir.h rescuewi.h \
	version.h platform.h windowsx.h xapi.h file.h disk.h symcfg.h rescuew.h rwhlp.h \
	ngui.h ctl3d.h ndrvlist.h msgdisp.h ctrlpanl.h cpl_res.h stdhelp.h winini.h
wrsccopy.obj:   wrsccopy.cpp platform.h ndrvlist.h disk.h ngui.h version.h quakever.h wndclass.h \
	passwd.h stddos.h symsys.h xapi.h file.h msgdisp.h stddir.h rescuew.h rescuewi.h \
	commctrl.h
wrscmisc.obj:   wrscmisc.cpp xapi.h symcfg.h symsys.h msgdisp.h ngui.h version.h quakever.h \
	wndclass.h passwd.h rescuew.h rescuewi.h stdhelp.h help_rd.h platform.h ndrvlist.h \
	file.h disk.h commdlg.h symsys.h commctrl.h symstat.h rwhlp.h
rescuew.obj:    rescuew.cpp platform.h ndrvlist.h disk.h ngui.h version.h quakever.h wndclass.h \
	passwd.h stddos.h symsys.h xapi.h file.h msgdisp.h siunivsl.h rescuew.h rescuewi.h \
	stdhelp.h help_rd.h stdio.h symcfg.h symstat.h symfmt.h rwhlp.h helpbtn.h commctrl.h \
	ctl3d.h numisc.h sirginfo.h wrscreg.h
wrscspec.obj:   wrscspec.cpp platform.h ndrvlist.h xapi.h symsys.h rescuew.h 
wrscodrw.obj:   wrscodrw.cpp platform.h file.h disk.h xapi.h ndrvlist.h symsys.h rescuew.h 
rescuesv.obj:   rescuesv.cpp platform.h ndrvlist.h xapi.h symsys.h disk.h file.h rescuew.h 
wrscsys.obj:    wrscsys.cpp platform.h ndrvlist.h xapi.h symsys.h msgdisp.h disk.h rescuew.h 
wrsccmd.obj:    wrsccmd.cpp platform.h ctl3d.h ngui.h version.h quakever.h wndclass.h passwd.h \
	file.h disk.h xapi.h ndrvlist.h symsys.h symcfg.h rescuew.h stdhelp.h help_rd.h
wrscini.obj:    wrscini.cpp platform.h ndrvlist.h msgdisp.h symsys.h symfmt.h disk.H ngui.h \
	version.h quakever.h wndclass.h passwd.h xapi.h rescuew.h rwhlp.h commctrl.h
rescuelb.obj:   rescuelb.cpp platform.h ndrvlist.h xapi.h symsys.h disk.h file.h msgdisp.h rescuew.h 
wrscfmt.obj:    wrscfmt.cpp platform.h ndrvlist.h xapi.h symfmt.h disk.h rescuew.h format.h
wrscswit.obj:   wrscswit.cpp platform.h symcfg.h siunivsl.h 
wrscthk.obj:    wrscthk.c platform.h symcfg.h siunivsl.h xapi.h symfmt.h
drvemap.obj:    platform.h stddos.h stddlg.h disk.h
rescuew.res:    rescuew.rc ver.h quake.ver ndrvlist.h rescuew.h rescuew.stb ngui.h version.h \
	quakever.h wndclass.h passwd.h commctrl.h always.bmp check.bmp gray.bmp wrsc16.bmp \
	wrsc256.bmp rescuew.ico replace.ico


# ----------------------------------------------------------------------------


#ENDUPDATE#


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------


