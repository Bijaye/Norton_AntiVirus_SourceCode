

#if ! Win 32 ...


#%If %Defined(QUAKE_CUSTOM)





TARGET_NAME=S32RSQ$(QUAKE_VER)
# or symrsq.dll

TARGET_TYPE=DLL
PROGRAM_NAME=$(TARGET_NAME).DLL
MODEL=L
.INCLUDE $(QUAKEINC)\PLATFORM.MAK


IMPEXT=DLL



RCBINDOPTS+=-K


#%If ("$(PLATFORM)" == ".WIN")
#    CTL3D_OBJ=r:\ctl3d.3\obj16\dlib\ctl3dd.obj
#%ElseIf ("$(PLATFORM)" == ".W32")
#    CTL3D_OBJ=r:\ctl3d.3\win32s\dlib\ctl3d32d.obj
#%Endif



 

SYMFMT_OBJS = symfmt.obj        \
        for.obj                 \
        fmtdisk.obj             \
        bootcode.obj            \
        system.obj              \
        sysnames.obj            \
        readsys.obj             \
        cvars.obj               \
        locknav.obj     

SYMFAT_OBJS = symfat.obj        \
        fat_fat.obj             \
        fat_dir.obj             \
        fat_lfn.obj             \
        fat_cpr.obj             \
        dirscan.obj             \
        fat_cfat.obj            \
        cdspace.obj             \
        cfreelst.obj            \
        cbfat.obj               \
        imgsave.obj             \
	fat_img.obj
#       cstacker.obj            

        # Not included in W32 version of SYMFAT_OBJS: fat_sft.obj, strings.obj


SYMSYS_OBJS = symsys.obj        \
        realboot.obj            \
        hostdriv.obj            \
        stack.obj               \
        sfaddpth.obj            \
        sfaddprg.obj            \
        sfaddvar.obj            \
        sfbreak.obj             \
        sfbool.obj              \
        sfchkpth.obj            \
        sfdata.obj              \
        sfdos6.obj              \
        sffile.obj              \
        sffind.obj              \
        sffirst.obj             \
        sfinit.obj              \
        sfinivar.obj            \
        sfinsdel.obj            \
        sfinstdv.obj            \
        sfparse.obj             \
        sfpath.obj              \
        sfrem.obj               \
        sfsectn.obj             \
        sfstat.obj              \
        sfstring.obj            \
        sfswap.obj              \
        sfvmm.obj               \
        sfwindev.obj           






#.INCLUDE $(QUAKEINC)\ML.MAK



#PROGRAM_NAME=$(Q_SYMKRNL).DLL
#KRNL_PATH=$(QUAKEPRE)\SYMKRNL\SRC$(QUAKE)

#KRNL=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
#UTIL=$(QUAKELANGPRE)\SYMUTIL\SRC$(QUAKE)
#GUI=$(QUAKELANGPRE)\SYMGUI\SRC$(QUAKE)
#NPT=$(QUAKELANGPRE)\NPT\SRC$(QUAKE)


# Generated Paths of Includes / Headers
PATH_SYMFMT=$(QUAKEPRE)\SYMFMT\SRC$(QUAKE)
PATH_SYMFAT=$(QUAKEPRE)\SYMFAT\SRC$(QUAKE)
PATH_SYMSYS=$(QUAKEPRE)\SYMSYS\SRC$(QUAKE)


# Generated Paths of Resources (Language-Specific)
RESLOC_SYMFMT=$(QUAKELANGPRE)\SYMFMT\SRC$(QUAKE)
RESLOC_SYMFAT=$(QUAKELANGPRE)\SYMFAT\SRC$(QUAKE)
RESLOC_SYMSYS=$(QUAKELANGPRE)\SYMSYS\SRC$(QUAKE)


# HardCoded Paths of Resources (Language-Specific)


#%If ("$(PLATFORM)" == ".WIN")
#    XOPTS=/I$(KRNL_PATH)
#    RCOPTS=/i$(KRNL) /i$(GUI) /i$(UTIL)
#%ElseIf ("$(PLATFORM)" == ".W32")
#    XOPTS=/DQCUSTNOX
#    RCOPTS=/i$(KRNL) /i$(GUI) /i$(UTIL) /i$(NPT) /i /i
#%Endif


$(PROGRAM_NAME): $(STUB)             \
        symrsq.obj              \
        $(SYMFMT_OBJS)          \
        $(SYMFAT_OBJS)          \
        $(SYMSYS_OBJS)          \
        $(Q_SYMKRNL).lib        \
        $(Q_SYMUTIL).lib        \
        $(Q_SYMGUI).lib         \
        symrsq.res              
#       symrsq.d32


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------


#UPDATE#
# All dependency lists are a combination of the dependencies listed in the actual source files IN 
# ADDITION TO the (possibly unused) dependencies displayed in the SYMFMT, SYMFAT, etc. make file.


# ----------------------------------------------------------------------------


# Dependencies for SYMRSQ original components
symrsq.obj:     symrsq.cpp platform.h version.h format.h symfmt.h symsys.h ngui.h symrsq.h


# ----------------------------------------------------------------------------


# Dependencies for SYMFMT components
symfmt.obj:     symfmt.cpp platform.h version.h format.h symfmt.h symsys.h ngui.h disk.h fillbar.h
for.obj:        for.cpp platform.h xapi.h undoc.h symfmt.h format.h ngui.h formatst.h msgdisp.h file.h disk.h tsr.h fillbar.h
fmtdisk.obj:    fmtdisk.cpp platform.h xapi.h undoc.h disk.h file.h ngui.h symfmt.h formatst.h format.h msgdisp.h fillbar.h
bootcode.obj:   bootcode.asm format.h disk.h fillbar.h ngui.h symfmt.h
system.obj:     system.cpp platform.h xapi.h disk.h file.h msgdisp.h formatst.h symfmt.h format.h ngui.h fillbar.h
sysnames.obj:   sysnames.cpp platform.h format.h disk.h fillbar.h ngui.h symfmt.h
readsys.obj:    readsys.cpp platform.h xapi.h undoc.h disk.h file.h msgdisp.h symsys.h symfmt.h format.h ngui.h formatst.h fillbar.h
cvars.obj:      cvars.cpp platform.h symfmt.h format.h disk.h fillbar.h ngui.h
locknav.obj:    locknav.asm format.h disk.h fillbar.h ngui.h symfmt.h


# ----------------------------------------------------------------------------


# Dependencies for SYMFAT components
# fat_sft.obj, strings.obj are not used in W32
symfat.obj:     symfat.c platform.h version.h platnlm.h dosvmm.h universl.h quakever.h
fat_fat.obj:    fat_fat.c platform.h disk.h file.h xapi.h platnlm.h dosvmm.h
fat_dir.obj:    fat_dir.c platform.h disk.h file.h xapi.h symevent.h platnlm.h dosvmm.h universl.h 
fat_lfn.obj:    fat_lfn.c platform.h stddos.h xapi.h disk.h file.h dosvmm.h universl.h 
dirscan.obj:    dirscan.c platform.h stddos.h xapi.h disk.h file.h dbcs.h platnlm.h dosvmm.h symrsq.h
fat_cfat.obj:   fat_cfat.c platform.h xapi.h stddos.h file.h nlibcfg.h disk.h cdisk.h platnlm.h dosvmm.h
cstacker.obj:   cstacker.c platform.h xapi.h disk.h cdisk.h platnlm.h dosvmm.h
cdspace.obj:    cdspace.c platform.h xapi.h stddos.h file.h nlibcfg.h disk.h cdisk.h platnlm.h dosvmm.h
cfreelst.obj:   cfreelst.c platform.h xapi.h stddos.h file.h nlibcfg.h disk.h cdisk.h platnlm.h dosvmm.h
cbfat.obj:      cbfat.c platform.h xapi.h stddos.h file.h nlibcfg.h disk.h cdisk.h platnlm.h dosvmm.h
imgsave.obj:    imgsave.c platform.h xapi.h disk.h file.h dosnet.h stddos.h platnlm.h dosvmm.h universl.h

# Are these even used in the build?
fat_sft.obj:    fat_sft.c platform.h stddos.h disk.h file.h xapi.h stdwin.h symevent.h dosvmm.h universl.h 
strings.obj:
csstor.obj:     platform.h platnlm.h dosvmm.h xapi.h stddos.h file.h nlibcfg.h disk.h cdisk.h
afxolecl.res:   afxres.h winres.h
afxprint.res:   afxres.h winres.h
afxres.res:     afxres.h winres.h
afxolesv.res:   afxres.h winres.h


# ----------------------------------------------------------------------------


# Dependencies for SYMSYS components

symsys.obj:     symsys.cpp platform.h version.h symsys.h xapi.h
realboot.obj:   realboot.cpp platform.h xapi.h disk.h file.h symsys.h
hostdriv.obj:   hostdriv.cpp platform.h xapi.h disk.h stackerw.h symsys.h
stack.obj:      stack.cpp platform.h xapi.h stackerw.h symsys.h
sfaddpth.obj:   sfaddpth.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfaddprg.obj:   sfaddprg.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfaddvar.obj:   sfaddvar.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfbreak.obj:    sfbreak.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfbool.obj:     sfbool.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfchkpth.obj:   sfchkpth.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfdata.obj:     sfdata.cpp platform.h xapi.h symsys.h
sfdos6.obj:     sfdos6.cpp platform.h xapi.h file.h symsys.h symsysid.h
sffile.obj:     sffile.cpp platform.h xapi.h file.h disk.h symcfg.h symsys.h symsysid.h
sffind.obj:     sffind.cpp platform.h xapi.h file.h symsys.h symsysid.h
sffirst.obj:    sffirst.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfinit.obj:     sfinit.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfinivar.obj:   sfinivar.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfinsdel.obj:   sfinsdel.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfinstdv.obj:   sfinstdv.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfparse.obj:    sfparse.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfpath.obj:     sfpath.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfrem.obj:      sfrem.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfsectn.obj:    sfsectn.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfstat.obj:     sfstat.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfstring.obj:   sfstring.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfswap.obj:     sfswap.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfvmm.obj:      sfvmm.cpp platform.h xapi.h file.h symsys.h symsysid.h
sfwindev.obj:   sfwindev.cpp platform.h xapi.h file.h symsys.h symsysid.h


# ----------------------------------------------------------------------------


# Dependencies for all resources
symrsq.res: symrsq.rc \
        $(RESLOC_SYMFMT)\symfmt.rc \
                platform.h \
                format.h \
                formatst.h \
                $(RESLOC_SYMFMT)\symfmt.stb \
                ngui.h \
                ndrvlist.h \
                ver.h \
                quake.ver \
        $(RESLOC_SYMFAT)\symfat.rc \
                quake.ver \
                platform.h \
                disk.h \
        $(RESLOC_SYMSYS)\symsys.rc \
                quake.ver \
                symsysid.h \
        $(RESLOC_SYMSYS)\symsysst.str 


# ----------------------------------------------------------------------------


#ENDUPDATE#

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------






