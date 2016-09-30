# Copyright 1992 Symantec Corporation                                   #
#########################################################################
#                                                                       #
# $Header:   S:/SYMNAV/VCS/symnav.mav   1.10   22 Feb 1996 10:52:14   YVANG
#                                                                       #
# Description:                                                          #
#                                                                       #
#########################################################################
# $Log:   S:/SYMNAV/VCS/symnav.mav  $ #
#  
#     Rev 1.27   11 Nov 1998 19:18:32   RJACKSO
#  Putting back symreg references.
#  
#     Rev 1.25   15 Jul 1997 17:09:04   DBUCHES
#  Added Ctl3D Support for DEC Alpha
#
#     Rev 1.24   19 May 1997 19:56:20   ksackin
#  Added dksfat32.obj to the dependency list.
#
#     Rev 1.23   07 May 1997 19:56:30   BILL
#  Alpha def file changes
#
#     Rev 1.22   07 May 1997 18:17:34   bsobel
#  Added def file for Alpha platform
#
#     Rev 1.21   07 May 1997 16:02:58   BILL
#  Removed dependency on quakever.h as this is always changing.  CM builds won't check the depenedenc
#  y
#  and will always build.
#
#     Rev 1.20   30 Apr 1997 22:08:56   JBRENNA
#  Use CORE_WIN16 for .WIN platform. Now using WDA and WRA platforms.
#
#     Rev 1.19   29 Apr 1997 16:51:16   BILL
#  Alpha ctl3d and def file changes
#
#     Rev 1.18   29 Apr 1997 10:01:08   JBRENNA
#  Ops ... CORE_WIN16 is not defined for .WIN platforms.
#
#     Rev 1.17   29 Apr 1997 08:52:54   JBRENNA
#  Fix another incorrect platform compare
#
#     Rev 1.16   28 Apr 1997 19:11:56   bsobel
#  Fixed wrong platform compare
#
#     Rev 1.15   28 Apr 1997 18:52:32   bsobel
#  Build system changes
#
#     Rev 1.14   12 Oct 1996 21:19:16   GWESTER
#  Added NO_JAPANESE_RESOURCES to the RCOPTS.
#
#     Rev 1.13   10 Oct 1996 11:12:00   GWESTER
#  Added #define NO_JAPANESE_RESOURCES, which will prevent the old Japanese
#  dialogs in CORE from showing up in the custom DLL.
#
#     Rev 1.12   20 Sep 1996 17:47:14   DALLEE
#  Added IMM32.LIB for SYM_WIN32 to match CORE SYMKRNL changes.
#
#     Rev 1.11   05 Sep 1996 20:00:36   GPATTER
#  Fixed munged fonts from SYMGUI. SYMNAV.MAK was overwriting RCOPTS.
#
#     Rev 1.10   22 Feb 1996 10:52:14   YVANG
#  Replaced 'navwhlp.obj' with 'stdhelp.obj' since the NAV StdHelp functons are
#  merged into Quake.
#
#     Rev 1.9   09 Jan 1996 16:36:30   JWORDEN
#  Add to OBJS - dyna, emsg, msgdisp, msgsz, navwhlp, passwd, password, nginit,
#                miscinit, vlinit, vlist, vlscroll, vlsetcur, vlrare, and fop_win
#
#     Rev 1.8.1.1   22 Sep 1995 20:36:38   YVANG
#  Added the following objects under the 'OBJS' section: dyna.obj, emsg.obj,
#  msgdisp.obj, msgrsz.obj, navwhlp.obj, passwd.obj, and password.obj.
#
#     Rev 1.8   30 Jun 1995 16:18:20   JREARDON
#  Fixed RC problem - command line too long
#
#     Rev 1.7   24 Jun 1995 12:56:30   JREARDON
#  Added DRAWMARK.NBC as a dependency for DRAWMARK.NBB
#
#     Rev 1.6   24 Jun 1995 12:11:24   MARKL
#  Fixed drawmark.nbb dependencies.
#
#     Rev 1.5   23 Jun 1995 11:33:16   MARKL
#  Repaired a $(GUI)\ reference.
#  Updated nblob dependencies.
#  IMPORTANT NOTE:  THE DEPENDENCIES IN THE MAKE FILE FOR NBB FILES *MUST*
#  *EXACTLY* MATCH THAT OF THE NBC FILE OR NBOB COMPILER WILL FAIL.
#
#
#     Rev 1.4   03 May 1995 12:10:06   MFALLEN
#  I think all the quirks are worked out.
#
#     Rev 1.3   03 May 1995 10:11:40   MFALLEN
#  Now compiling navwhelp.cpp and not stdhelp.cpp
#
#
#     Rev 1.2   02 May 1995 13:28:00   MFALLEN
#  Added shelllnk.cpp
#
#     Rev 1.1   04 Apr 1995 11:28:18   SCOTTP
#  REMOVED THE SET OF QUAKE_CUSTOM TO SYMNAV
#  BECAUSE BRIAN ONLY CHECKS IF(DEFINED)
#  NOT IF "NAV"
#
#  I now check for quake_custom set to nav/pca/??? whatever
#  in quakever.mak.  This is so we can do bunches of
#  these thingys
#
#     Rev 1.0   24 Mar 1995 11:04:24   MFALLEN
#  Initial revision.
#
#########################################################################

# NO_JAPANESE_RESOURCES is defined below to keep the old Japanese dialogs in Core from
# compiling into the DLL.

%If %Defined(QUAKE_CUSTOM)

TARGET_NAME=SYMNAV
TARGET_TYPE=DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK

PROGRAM_NAME=$(Q_SYMKRNL).DLL
KRNL_PATH=$(QUAKEPRE)\SYMKRNL\SRC$(QUAKE)

KRNL=$(QUAKELANGPRE)\SYMKRNL\SRC$(QUAKE)
UTIL=$(QUAKELANGPRE)\SYMUTIL\SRC$(QUAKE)
GUI=$(QUAKELANGPRE)\SYMGUI\SRC$(QUAKE)
NPT=$(QUAKELANGPRE)\NPT\SRC$(QUAKE)

%If ("$(CORE_WIN16)" != "")
    XOPTS=/I$(KRNL_PATH)
    CTL3D_OBJ=r:\ctl3d.3\obj16\dlib\ctl3dd.obj
%ElseIf ("$(CORE_WIN32)" != "")
    XOPTS=/DQCUSTNOX /DNO_JAPANESE_RESOURCES
    RCOPTS+= /i$(KRNL) /i$(GUI) /i$(UTIL) /i$(NPT) /i /i /DNO_JAPANESE_RESOURCES
%If ("$(CORE_PLAT)" == "I")
    DEFFILE=quake.d32
    CTL3D_OBJ=r:\ctl3d.3\win32s\dlib\ctl3d32d.obj
%ElseIf ("$(CORE_PLAT)" == "A")
    DEF_EXT=.AXP
    DEFFILE=quake.axp
    LIBS += CTL3D32.LIB
%Endif
%Endif

%If ("$(CORE_WIN16)" != "")

$(PROGRAM_NAME): $(STUB) \
 badchar.obj \
 bufferio.obj \
 cfg_bloc.obj \
 cfg_ini.obj \
 cfg_reg.obj \
 cfg_swit.obj \
 cfg_text.obj \
 colorbox.obj \
 cpalette.obj \
 cvtft.obj \
 dbcs.obj \
 drawmark.obj \
 dsk_abs.obj \
 dsk_accs.obj \
 dsk_bloc.obj \
 dsk_bpb.obj \
 dsk_cach.obj \
 dsk_drvr.obj \
 dsk_gen.obj \
 dsk_int.obj \
 dsk_misc.obj \
 dsk_stac.obj \
 dsk_type.obj \
 dyna.obj     \
 emsg.obj     \
 filetime.obj \
 fillbar.obj \
 findfirs.obj \
 fop_dir.obj \
 fop_dir2.obj \
 fop_disk.obj \
 fop_dos.obj \
 fop_file.obj \
 fop_win.obj \
 hstring.obj \
 hugeoem.obj \
 int13win.obj \
 msgdisp.obj  \
 msgrsz.obj   \
 name.obj \
 name_win.obj \
 nblob.obj \
 ndrvlist.obj \
 nec.obj \
 ngbitmap.obj \
 ngbutnbr.obj \
 ngfont.obj \
 ngprop.obj \
 ngspsrch.obj \
 ngstatus.obj \
 ngtable.obj \
 ngtray.obj \
 ngvwrtbl.obj \
 normtext.obj \
 passwd.obj   \
 password.obj \
 prm_crpt.obj \
 prm_date.obj \
 prm_dos.obj \
 prm_err.obj \
 prm_hard.obj \
 prm_lstr.obj \
 prm_mach.obj \
 prm_mem.obj \
 prm_misc.obj \
 prm_sft.obj \
 prm_str.obj \
 prm_szz.obj \
 prm_win.obj \
 quake.obj \
 romcopy.obj \
 slider.obj \
 spn.obj \
 spnmsgs.obj \
 spnscrl.obj \
 stockbmp.obj \
 stockbrs.obj \
 symcfg.obj \
 symgui.obj \
 nginit.obj \
 miscinit.obj \
 vlinit.obj   \
 vlist.obj    \
 vlscroll.obj \
 vlsetcur.obj \
 vlrare.obj   \
 stdhelp.obj \
 symintl.obj \
 symkrnl.obj \
 symutil.obj \
 tnl.obj \
 tsr.obj \
 tsr_asm.obj \
 vcrbttn.obj \
 version.obj \
 wildcard.obj \
 $(Q_SYMDEBUG).LIB \
 quake.res \
 quake.def \
 stackerw.lib \
 $(CTL3D_OBJ)

%Else


$(PROGRAM_NAME): $(STUB) \
 symkrnl.obj  \
 symutil.obj  \
 symgui.obj   \
 nginit.obj   \
 miscinit.obj   \
 vlinit.obj   \
 vlist.obj    \
 vlscroll.obj \
 vlsetcur.obj \
 vlrare.obj   \
 nptlm.obj    \
 symcfg.obj   \
 badchar.obj  \
 cfg_bloc.obj \
 cfg_ini.obj  \
 cfg_reg.obj  \
 cfg_swit.obj \
 cfg_text.obj \
 dbcs.obj     \
 prm_date.obj \
 prm_dos.obj  \
 prm_mem.obj  \
 prm_misc.obj \
 prm_str.obj  \
 prm_szz.obj  \
 prm_crpt.obj \
 dsk_accs.obj \
 dsk_bpb.obj  \
 dsk_cach.obj \
 dsk_misc.obj \
 dsk_type.obj \
 dskfat32.obj \
 bufferio.obj \
 fop_dir.obj  \
 fop_dir2.obj \
 fop_disk.obj \
 fop_file.obj \
 fop_win.obj \
 findfirs.obj \
 fop_dos.obj  \
 symintl.obj  \
 version.obj  \
 name.obj     \
 name_win.obj \
 wildcard.obj \
 hstring.obj  \
 prm_hard.obj \
 dsk_abs.obj  \
 dsk_bloc.obj \
 dsk_int.obj  \
 dsk_gen.obj  \
 filetime.obj \
 nec.obj      \
 dsk_drvr.obj \
 dsk_stac.obj \
 vxdload.obj  \
 prm_win.obj  \
 hugeoem.obj  \
 dirsel.obj   \
 dyna.obj     \
 emsg.obj     \
 exterr.obj   \
 msgdisp.obj  \
 msgrsz.obj   \
 nblob.obj    \
 ndos4dos.obj \
 verinfo.obj  \
 winini.obj   \
 passwd.obj   \
 nreginfo.obj \
 nll.obj      \
 symreg.obj   \
 mmsound.obj  \
 symtray.obj  \
 colorbox.obj \
 cpalette.obj \
 draglist.obj \
 editfltr.obj \
 fillbar.obj  \
 slider.obj   \
 ndrvlist.obj \
 drawmark.obj \
 ngbitmap.obj \
 ngbutnbr.obj \
 ngfont.obj   \
 ngprop.obj   \
 ngstatus.obj \
 ngspsrch.obj \
 ngtable.obj  \
 ngtray.obj   \
 normtext.obj \
 password.obj \
 shelllnk.obj \
 spn.obj      \
 spnmsgs.obj  \
 spnscrl.obj  \
 tnl.obj      \
 toolres.obj  \
 vcrbttn.obj  \
 ctrlpanl.obj \
 dynamic.obj  \
 builtin.obj  \
 stdabout.obj \
 stdguilt.obj \
 stdhelp.obj  \
 stockbmp.obj \
 stockbrs.obj \
 ngvwrtbl.obj \
 symarray.obj \
 treeobj.obj  \
 treenode.obj \
 treectrl.obj \
 treedrag.obj \
 treedraw.obj \
 treeline.obj \
 treedata.obj \
 treeenum.obj \
 treereg.obj  \
 treehand.obj \
 browsdlg.obj \
 npt.obj      \
 nptabort.obj \
 npterr.obj   \
 nptinit.obj  \
 nptpjobs.obj \
 nptprtdc.obj \
 nptwini.obj  \
 nptpbuf.obj  \
 nptpfile.obj \
 nptsel.obj   \
 nptdlg.obj   \
 quake.obj    \
 IMM32.LIB    \
 COMCTL32.LIB \
 shell32.lib  \
 ole32.lib    \
 uuid.lib     \
 $(Q_SYMDEBUG).LIB \
 quake.res \
 $(DEFFILE) \
 $(CTL3D_OBJ)
%Endif

quake.res: quake.rc \
 $(KRNL)\symkrnl.rc dialogs.h $(KRNL)\dialogs.dlg symkrnli.h \
 $(UTIL)\symutil.rc $(UTIL)\dirsel.dlg quake.ver \
 $(UTIL)\msgstr.stb $(UTIL)\stdhelps.stb $(UTIL)\verinfo.dlg \
%If ("$(CORE_WIN32)" != "")
 $(NPT)\npt.rc ver.h nptstr.h $(NPT)\nptstr.stb nptres.h \
 $(NPT)\nptres.dlg   nlook.h  nwres.h ngui.h stddlg.h \
%Endif
 $(GUI)\symgui.rc cpdef.h ngui.h ver.h spnres.h tnlres.h $(GUI)\ngui.stb \
 $(GUI)\ngui.dlg $(GUI)\tblsort.dlg $(GUI)\spnres.dlg $(GUI)\tnlres.dlg nblob.h \
 $(GUI)\ndrvlist.dlg ndrvres.h ndrvlist.nbb dmres.h drawmark.nbb \
 $(GUI)\spndis.bmp $(GUI)\spndwn.bmp $(GUI)\spnnml.bmp $(GUI)\spnup.bmp $(GUI)\slider.dlg \
 $(GUI)\tnlres.dlg $(GUI)\tnldis.bmp $(GUI)\tnldwn.bmp $(GUI)\tnlnml.bmp $(GUI)\vcrbttn.bmp \
 nguistb.h passwres.h $(GUI)\password.dlg $(GUI)\ctrlpanl.dlg $(GUI)\stdabout.dlg \
 stdres.h quake.ver $(GUI)\stdguilt.dlg $(GUI)\ngtable.cur $(GUI)\nodrop.cur \
 $(GUI)\stockbmp.dlg $(GUI)\browsdlg.dlg $(GUI)\toolbar.ico stockbmp.nbb \
 $(GUI)\ngopen.bmp $(GUI)\ngsave.bmp $(GUI)\ngclose.bmp $(GUI)\ngnew.bmp $(GUI)\ngprint.bmp \
 $(GUI)\ngcut.bmp $(GUI)\ngcopy.bmp $(GUI)\ngpaste.bmp $(GUI)\ngdelete.bmp $(GUI)\ngundo.bmp \
 $(GUI)\ngfind.bmp $(GUI)\ngfindp.bmp $(GUI)\ngfindn.bmp $(GUI)\ngfont.bmp $(GUI)\nghelp.bmp \
 $(GUI)\ngrun.bmp $(GUI)\ngcmpile.bmp $(GUI)\ngrplace.bmp $(GUI)\ngexit.bmp \
 3dcheck.bmp $(GUI)\ngfind2.bmp $(GUI)\ngdlgedt.bmp $(GUI)\ngfilter.bmp \
 $(GUI)\ngflsel.bmp $(GUI)\ngmove.bmp $(GUI)\ngrefrsh.bmp $(GUI)\ngcntrl.bmp \
 $(GUI)\ngsrtnam.bmp $(GUI)\ngsrttyp.bmp $(GUI)\ngtrpane.bmp $(GUI)\ngview.bmp \
 $(GUI)\ngmkdir.bmp $(GUI)\ngdos.bmp $(GUI)\ngqa.bmp $(GUI)\ngdrvwnd.bmp $(GUI)\ngflpane.bmp \
 $(GUI)\ngvwpane.bmp $(GUI)\ngabort.bmp $(GUI)\ngstyle.bmp $(GUI)\ngtest.bmp \
 quake.ver $(GUI)\slnull.cur \
 $(GUI)\sldown.bmp $(GUI)\sldownf.bmp $(GUI)\slleft.bmp $(GUI)\slleftf.bmp $(GUI)\slright.bmp \
 $(GUI)\slrightf.bmp $(GUI)\slup.bmp $(GUI)\slupf.bmp treeres.h $(GUI)\file.bmp \
 $(GUI)\filemask.bmp $(GUI)\fldrmask.bmp $(GUI)\folder.bmp $(GUI)\ropnmask.bmp \
 $(GUI)\minumask.bmp $(GUI)\minus.bmp $(GUI)\openfldr.bmp $(GUI)\opfldmsk.bmp \
 $(GUI)\plus.bmp $(GUI)\plusmask.bmp $(GUI)\rclsmask.bmp $(GUI)\rootclos.bmp \
 $(GUI)\rootopen.bmp \
 treeres.h $(GUI)\file.bmp $(GUI)\filemask.bmp $(GUI)\fldrmask.bmp $(GUI)\folder.bmp \
 $(GUI)\ropnmask.bmp $(GUI)\minumask.bmp $(GUI)\minus.bmp $(GUI)\openfldr.bmp $(GUI)\opfldmsk.bmp \
 $(GUI)\plus.bmp $(GUI)\plusmask.bmp $(GUI)\rclsmask.bmp $(GUI)\rootclos.bmp $(GUI)\rootopen.bmp \
 $(GUI)\singlnod.cur $(GUI)\insnode.cur $(GUI)\singlsel.cur $(GUI)\insleaf.cur $(GUI)\multisel.cur

stockbmp.nbb:   $(GUI)\stockbmp.nbc \
    $(GUI)\ngdummy.bmp  $(GUI)\ngdummym.bmp $(GUI)\ngabort.bmp  $(GUI)\ngabortm.bmp \
    $(GUI)\ngarrng.bmp  $(GUI)\ngarrngm.bmp $(GUI)\ngascndg.bmp $(GUI)\ngascndm.bmp \
    $(GUI)\ngbackup.bmp $(GUI)\ngbackum.bmp $(GUI)\ngbydate.bmp $(GUI)\ngbydatm.bmp \
    $(GUI)\ngbysze.bmp  $(GUI)\ngbyszem.bmp $(GUI)\ngcalcf.bmp  $(GUI)\ngcalcfm.bmp \
    $(GUI)\ngcalcs.bmp  $(GUI)\ngcalcsm.bmp $(GUI)\ngcalct.bmp  $(GUI)\ngcalctm.bmp \
    $(GUI)\ngchgvu.bmp  $(GUI)\ngchgvum.bmp $(GUI)\ngclose.bmp  $(GUI)\ngclosem.bmp \
    $(GUI)\ngclpsal.bmp $(GUI)\ngclpsam.bmp $(GUI)\ngclpsbr.bmp $(GUI)\ngclpsbm.bmp \
    $(GUI)\ngclpstr.bmp $(GUI)\ngclpstm.bmp $(GUI)\ngclsal.bmp  $(GUI)\ngclsalm.bmp \
    $(GUI)\ngcmpile.bmp $(GUI)\ngcmpilm.bmp $(GUI)\ngcmprs.bmp  $(GUI)\ngcmprsm.bmp \
    $(GUI)\ngcntrl.bmp  $(GUI)\ngcntrlm.bmp $(GUI)\ngconnec.bmp $(GUI)\ngconnem.bmp \
    $(GUI)\ngcopy.bmp   $(GUI)\ngcopym.bmp  $(GUI)\ngcscde.bmp  $(GUI)\ngcscdem.bmp \
    $(GUI)\ngcut.bmp    $(GUI)\ngcutm.bmp   $(GUI)\ngdaypln.bmp $(GUI)\ngdaypnm.bmp \
    $(GUI)\ngdeall.bmp  $(GUI)\ngdeallm.bmp $(GUI)\ngdeinv.bmp  $(GUI)\ngdeinvm.bmp \
    $(GUI)\ngdelete.bmp $(GUI)\ngdeletm.bmp $(GUI)\ngdescdn.bmp $(GUI)\ngdescdm.bmp \
    $(GUI)\ngdesom.bmp  $(GUI)\ngdesomm.bmp $(GUI)\ngdetls.bmp  $(GUI)\ngdetlsm.bmp \
    $(GUI)\ngdlgedt.bmp $(GUI)\ngdlgedm.bmp $(GUI)\ngdos.bmp    $(GUI)\ngdosm.bmp   \
    $(GUI)\ngdrvicn.bmp $(GUI)\ngdrvinm.bmp $(GUI)\ngdrvwnd.bmp $(GUI)\ngdrvwnm.bmp \
    $(GUI)\ngdsconn.bmp $(GUI)\ngdscnnm.bmp $(GUI)\ngdskcpy.bmp $(GUI)\ngdskcpm.bmp \
    $(GUI)\ngexit.bmp   $(GUI)\ngexitm.bmp  $(GUI)\ngfiltr.bmp  $(GUI)\ngfiltrm.bmp \
    $(GUI)\ngfind.bmp   $(GUI)\ngfindm.bmp  $(GUI)\ngfindn.bmp  $(GUI)\ngfindnm.bmp \
    $(GUI)\ngfindp.bmp  $(GUI)\ngfindpm.bmp $(GUI)\ngflall.bmp  $(GUI)\ngflallm.bmp \
    $(GUI)\ngflinv.bmp  $(GUI)\ngflinvm.bmp $(GUI)\ngflpane.bmp $(GUI)\ngflpanm.bmp \
    $(GUI)\ngflsel.bmp  $(GUI)\ngflselm.bmp $(GUI)\ngfont.bmp   $(GUI)\ngfontm.bmp  \
    $(GUI)\ngformat.bmp $(GUI)\ngformtm.bmp $(GUI)\nggoto.bmp   $(GUI)\nggotom.bmp  \
    $(GUI)\nggrpcpy.bmp $(GUI)\nggrpcym.bmp $(GUI)\nggrpdel.bmp $(GUI)\nggrpdem.bmp \
    $(GUI)\nggrpitm.bmp $(GUI)\nggrptmm.bmp $(GUI)\nggrpmv.bmp  $(GUI)\nggrpmvm.bmp \
    $(GUI)\nggrpnew.bmp $(GUI)\nggrpnem.bmp $(GUI)\nggrpopn.bmp $(GUI)\nggrpopm.bmp \
    $(GUI)\nghelp.bmp   $(GUI)\nghelpm.bmp  $(GUI)\nghlpabt.bmp $(GUI)\nghlpabm.bmp \
    $(GUI)\ngicnedt.bmp $(GUI)\ngicnedm.bmp $(GUI)\ngkeyfdr.bmp $(GUI)\ngkeyfdm.bmp \
    $(GUI)\nglabel.bmp  $(GUI)\nglabelm.bmp $(GUI)\ngmail.bmp   $(GUI)\ngmailm.bmp  \
    $(GUI)\ngmkdir.bmp  $(GUI)\ngmkdirm.bmp $(GUI)\ngmove.bmp   $(GUI)\ngmovem.bmp  \
    $(GUI)\ngnav.bmp    $(GUI)\ngnavm.bmp   $(GUI)\ngndd.bmp    $(GUI)\ngnddm.bmp   \
    $(GUI)\ngnew.bmp    $(GUI)\ngnewm.bmp   $(GUI)\ngoem.bmp    $(GUI)\ngoemm.bmp   \
    $(GUI)\ngopen.bmp   $(GUI)\ngopenm.bmp  $(GUI)\ngpaste.bmp  $(GUI)\ngpastem.bmp \
    $(GUI)\ngprgvw.bmp  $(GUI)\ngprgvwm.bmp $(GUI)\ngprint.bmp  $(GUI)\ngprnt3m.bmp \
    $(GUI)\ngqa.bmp     $(GUI)\ngqam.bmp    $(GUI)\ngreadme.bmp $(GUI)\ngreadmm.bmp \
    $(GUI)\ngref.bmp    $(GUI)\ngrefm.bmp   $(GUI)\ngrefrsh.bmp $(GUI)\ngrefrsm.bmp \
    $(GUI)\ngrename.bmp $(GUI)\ngrenamm.bmp $(GUI)\ngrplace.bmp $(GUI)\ngrplacm.bmp \
    $(GUI)\ngrun.bmp    $(GUI)\ngrunm.bmp   $(GUI)\ngsavcfg.bmp $(GUI)\ngsavcfm.bmp \
    $(GUI)\ngsave.bmp   $(GUI)\ngsavem.bmp  $(GUI)\ngscdadd.bmp $(GUI)\ngscdadm.bmp \
    $(GUI)\ngscdcpy.bmp $(GUI)\ngscdcpm.bmp $(GUI)\ngscddel.bmp $(GUI)\ngscddem.bmp \
    $(GUI)\ngscdedt.bmp $(GUI)\ngscdedm.bmp $(GUI)\ngsched.bmp  $(GUI)\ngschedm.bmp \
    $(GUI)\ngscptmk.bmp $(GUI)\ngscptmm.bmp $(GUI)\ngscrnsv.bmp $(GUI)\ngscrnsm.bmp \
    $(GUI)\ngshare.bmp  $(GUI)\ngsharm.bmp  $(GUI)\ngshrdr.bmp  $(GUI)\ngshrdrm.bmp \
    $(GUI)\ngsmrte.bmp  $(GUI)\ngsmrtem.bmp $(GUI)\ngspddsk.bmp $(GUI)\ngspddsm.bmp \
    $(GUI)\ngsprfnd.bmp $(GUI)\ngsprfnm.bmp $(GUI)\ngsrtnam.bmp $(GUI)\ngsrtnmm.bmp \
    $(GUI)\ngsrttyp.bmp $(GUI)\ngsrttym.bmp $(GUI)\ngstopsh.bmp $(GUI)\ngstopsm.bmp \
    $(GUI)\ngstyle.bmp  $(GUI)\ngstylem.bmp $(GUI)\ngsysnfo.bmp $(GUI)\ngsysnfm.bmp \
    $(GUI)\ngtest.bmp   $(GUI)\ngtestm.bmp  $(GUI)\ngtile.bmp   $(GUI)\ngtilem.bmp  \
    $(GUI)\ngtitle.bmp  $(GUI)\ngtitlem.bmp $(GUI)\ngtoolbr.bmp $(GUI)\ngtoolbm.bmp \
    $(GUI)\ngtrpane.bmp $(GUI)\ngtrpanm.bmp $(GUI)\ngtxtedt.bmp $(GUI)\ngtxtedm.bmp \
    $(GUI)\ngundo.bmp   $(GUI)\ngundo5m.bmp $(GUI)\ngunsrt.bmp  $(GUI)\ngunsrtm.bmp \
    $(GUI)\ngview.bmp   $(GUI)\ngviewm.bmp  $(GUI)\ngvwpane.bmp $(GUI)\ngvwpanm.bmp \
    $(GUI)\ngxpnd1.bmp  $(GUI)\ngxpnd1m.bmp $(GUI)\ngxpndal.bmp $(GUI)\ngxpndam.bmp \
    $(GUI)\ngxpndbr.bmp $(GUI)\ngxpndbm.bmp $(GUI)\ngcancel.bmp $(GUI)\ngcancem.bmp \
    $(GUI)\ngcmdir.bmp  $(GUI)\ngcmdirm.bmp $(GUI)\nggrparr.bmp $(GUI)\nggrparm.bmp \
    $(GUI)\ngok.bmp     $(GUI)\ngokm.bmp    $(GUI)\nghideal.bmp $(GUI)\nghideam.bmp \
    $(GUI)\ngshowal.bmp $(GUI)\ngshowam.bmp $(GUI)\ngtools.bmp  $(GUI)\ngtoolsm.bmp \
    $(GUI)\ngedit.bmp   $(GUI)\ngeditm.bmp  $(GUI)\ngprop.bmp   $(GUI)\ngpropm.bmp  \
    $(GUI)\ngassoc.bmp  $(GUI)\ngassocm.bmp $(GUI)\ngzips.bmp   $(GUI)\ngzipsm.bmp  \
    $(GUI)\ngdcp.bmp    $(GUI)\ngdcpm.bmp

ndrvlist.nbb: $(GUI)\NDRVLIST.NBC \
    $(GUI)\dvall.bmp   $(GUI)\dvallm.bmp      $(GUI)\dvcdrom.bmp     $(GUI)\dvcdromm.bmp \
    $(GUI)\dvcomp.bmp  $(GUI)\dvcompm.bmp     $(GUI)\dvcurr.bmp      $(GUI)\dvcurrm.bmp \
    $(GUI)\dvflop.bmp  $(GUI)\dvflopm.bmp     $(GUI)\dvflops.bmp     $(GUI)\dvflopsm.bmp \
    $(GUI)\dvhards.bmp $(GUI)\dvhardsm.bmp    $(GUI)\dvhd.bmp        $(GUI)\dvhdm.bmp \
    $(GUI)\dvlock.bmp  $(GUI)\dvlockm.bmp     $(GUI)\dvnet.bmp       $(GUI)\dvnetm.bmp \
    $(GUI)\dvnets.bmp  $(GUI)\dvnetsm.bmp     $(GUI)\dvram.bmp       $(GUI)\dvramm.bmp \
    $(GUI)\dvsubst.bmp $(GUI)\dvsubstm.bmp    $(GUI)\dvunlck.bmp     $(GUI)\dvunlckm.bmp

drawmark.nbb: $(GUI)\DRAWMARK.NBC \
    $(GUI)\CHECK.BMP    $(GUI)\CHECKM.BMP \
    $(GUI)\UNCHECK.BMP  $(GUI)\UNCHECKM.BMP

badchar.obj:    badchar.cpp
symkrnl.obj:    symkrnl.c
symutil.obj:    symutil.cpp
symgui.obj:     symgui.cpp
nginit.obj:     platform.h ngui.h version.h wndclass.h passwd.h \
                stockbmp.h fillbari.h spnint.h tnlint.h ndrvlist.h cpdef.h \
                normtext.h vcrbttn.h symguii.h
miscinit.obj:   platform.h ngui.h version.h wndclass.h passwd.h \
                stockbmp.h fillbari.h spnint.h tnlint.h ndrvlist.h cpdef.h \
                normtext.h vcrbttn.h symguii.h
nptlm.obj:      nptlm.cpp
vxdload.obj:    vxdload.c platform.h vxdload.h
vxdwraps.obj:   vxdwraps.asm platform.inc ios.inc ifsmgr.inc
vxdstat.obj:    vxdstat.cpp platform.h xapi.h
vsymkrnl.obj:   vsymkrnl.asm platform.inc vsymkrnl.inc
services.obj:   service.asm platform.inc ios.inc blockdev.inc vsymkrnl.inc
iossrv.obj:     iossrv.asm platform.inc blockdev.inc ior.inc iop.inc aep.inc isp.inc \
                ios.inc drp.inc ilb.inc dcb.inc sgd.inc ida.inc
rtl32.obj:      rtl32.cpp platform.h
bufferio.obj:   bufferio.c platform.h stddos.h ibmpc.h file.h
cfg_bloc.obj:   cfg_bloc.c platform.h file.h xapi.h symcfg.h
cfg_ini.obj:    cfg_ini.c platform.h file.h xapi.h symcfg.h
cfg_reg.obj:    cfg_reg.cpp platform.h file.h xapi.h symcfg.h
cfg_swit.obj:   cfg_swit.c platform.h file.h xapi.h symcfg.h
cfg_text.obj:   cfg_text.c platform.h file.h xapi.h symcfg.h
nec.obj:        nec.c platform.h xapi.h
version.obj:    version.c platform.h version.h quakever.h
dbcs.obj:       dbcs.c platform.h file.h xapi.h dbcs.h
dsk_abs.obj:    dsk_abs.c platform.h disk.h xapi.h thunk.h
dsk_accs.obj:   dsk_accs.c platform.h disk.h xapi.h file.h dos.h
dsk_bloc.obj:   dsk_bloc.c platform.h disk.h xapi.h symcfg.h thunk.h
dsk_bpb.obj:    dsk_bpb.c platform.h disk.h xapi.h
dsk_cach.obj:   dsk_cach.c platform.h xapi.h disk.h
dsk_drvr.obj:   dsk_drvr.c platform.h undoc.h file.h disk.h xapi.h
dsk_stac.obj:   dsk_stac.c platform.h undoc.h file.h disk.h xapi.h
dsk_gen.obj:    dsk_gen.c platform.h file.h disk.h xapi.h
dsk_int.obj:    dsk_int.c platform.h disk.h xapi.h thunk.h
dsk_misc.obj:   dsk_misc.c platform.h xapi.h thunk.h disk.h file.h
dsk_type.obj:   dsk_type.c platform.h xapi.h symcfg.h file.h disk.h dbcs.h
filetime.obj:   filetime.c platform.h xapi.h file.h symkrnl.h
findfirs.obj:   findfirs.cpp platform.h xapi.h file.h undoc.h symkrnl.h
fop_dir.obj:    fop_dir.c platform.h undoc.h file.h disk.h xapi.h
fop_dir2.obj:   fop_dir2.c platform.h file.h xapi.h
fop_disk.obj:   fop_disk.c platform.h disk.h undoc.h file.h xapi.h
fop_dos.obj:    fop_dos.c platform.h file.h xapi.h
fop_win.obj:    platform.h file.h
fop_file.obj:   fop_file.c platform.h xapi.h file.h disk.h symkrnl.h undoc.h
hstring.obj:    hstring.c platform.h undoc.h hstring.h
hugeoem.obj:    hugeoem.c platform.h hugeoem.h
name.obj:       name.c platform.h xapi.h file.h disk.h symkrnl.h dbcs.h
name_win.obj:   name_win.c platform.h file.h disk.h xapi.h symkrnl.h dbcs.h
prm_crpt.obj:   prm_crpt.c platform.h xapi.h
prm_date.obj:   prm_date.c platform.h undoc.h xapi.h
prm_dos.obj:    prm_dos.c platform.h xapi.h file.h
prm_hard.obj:   prm_hard.c platform.h undoc.h disk.h xapi.h symevent.h symevnt.ver \
                version.h dialogs.h thunk.h
prm_mach.obj:   prm_mach.c platform.h xapi.h stddos.h ibmpc.h hardware.h
prm_mem.obj:    prm_mem.c platform.h xapi.h symkrnl.h file.h thunk.h disk.h \
                dosvmm.h
prm_misc.obj:   prm_misc.c platform.h undoc.h xapi.h
prm_sft.obj:    prm_sft.c platform.h xapi.h disk.h file.h symevent.h symevnt.ver \
                version.h stddos.h ibmpc.h
prm_err.obj:    prm_err.asm platform.inc
romcopy.obj:    romcopy.asm platform.inc
int13win.obj:   int13win.asm platform.inc
prm_lstr.obj:   prm_lstr.asm platform.inc
prm_str.obj:    prm_str.c platform.h xapi.h symintl.h symkrnli.h stddos.h ibmpc.h
prm_szz.obj:    prm_szz.c platform.h xapi.h
prm_win.obj:    prm_win.c platform.h xapi.h disk.h
symcfg.obj:     symcfg.c platform.h file.h xapi.h symcfg.h disk.h undoc.h
symintl.obj:    symintl.c platform.h xapi.h stddos.h ibmpc.h symkrnli.h symintl.h \
                dbcs.h
wildcard.obj:   wildcard.c platform.h
tsr.obj:        tsr.c platform.h tsr.h
#cvtft.obj:      mprec.inc
cvtft.obj:     cvtft.asm mprec.inc
int13dos.obj:   int13dos.asm struct.inc types.inc
seterror.obj:   seterror.asm mixed.inc
tsr_asm.obj:    tsr_asm.asm mixed.inc
dyna.obj:       dyna.cpp platform.h msgdisp.h dyna.h
stdhelp.obj:    stdhelp.cpp platform.h xapi.h undoc.h msgdisp.h file.h \
                stdhelp.h dll.h sumisc.h msgstr.h
emsg.obj:       emsg.cpp platform.h undoc.h msgdisp.h emsgint.h msgstr.h
msgrsz.obj:     msgrsz.cpp platform.h undoc.h msgdisp.h msgstr.h msgint.h
nblob.obj:      nblob.cpp platform.h undoc.h file.h xapi.h msgstr.h sumisc.h nblob.h \
                nblobi.h
ndos4dos.obj:   ndos4dos.c platform.h file.h xapi.h
textfind.obj:   textfind.cpp platform.h file.h xapi.h
msgdisp.obj:    msgdisp.cpp platform.h xapi.h msgdisp.h msgstr.h dyna.h msgint.h
dirsel.obj:     dirsel.cpp platform.h undoc.h stdhelp.h ctl3d.h commdlg.h xapi.h \
                stddir.h file.h disk.h sumisc.h dirsel.h
hist.obj:       hist.cpp platform.h submem.h symcfg.h history.h histstb.h
wldcard.obj:    wldcard.cpp platform.h wildcard.h
winini.obj:     winini.cpp platform.h xapi.h winini.h
passwd.obj:     passwd.c platform.h
verinfo.obj:    verinfo.cpp platform.h xapi.h undoc.h verinfo.h vercmd.h
exterr.obj:     exterr.cpp platform.h xapi.h msgdisp.h msgstr.h
nll.obj:        nll.cpp platform.h xapi.h submem.h
symreg.obj:     symreg.cpp platform.h siunivsl.h sirginfo.h
nreginfo.obj:   nreginfo.cpp platform.h xapi.h nreginfo.h
mmsound.obj:    mmsound.cpp platform.h xapi.h MMSYSTEM.H sound.h
cpalette.obj:   cpalette.cpp platform.h ctl3d.h string.h cpdef.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
ngtable.obj:    ngtable.cpp platform.h xapi.h windowsx.h ctl3d.h memory.h ngui.h \
                version.h wndclass.h passwd.h stockbmp.h symguii.h
stdabout.obj:   stdabout.cpp platform.h xapi.h nguistb.h stdres.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h malloc.h verinfo.h \
                symguii.h
ngstatus.obj:   ngstatus.cpp platform.h cpdef.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
stockbrs.obj:   stockbrs.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h nguistb.h symguii.h
ngbutnbr.obj:   ngbutnbr.cpp platform.h xapi.h nblob.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
ngbitmap.obj:   ngbitmap.cpp platform.h cpdef.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h
ngprop.obj:     ngprop.cpp platform.h string.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h
ngspsrch.obj:   ngspsrch.cpp platform.h string.h cpdef.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
fillbar.obj:    fillbar.cpp platform.h windowsx.h commdlg.h undoc.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h \
                fillbari.h
ndrvlist.obj:   ndrvlist.cpp platform.h ctl3d.h file.h disk.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h nguistb.h nblob.h \
                drawmark.h dmres.h drawmark.nbh ndrvres.h ndrvlist.nbh \
                ndrvlist.h paneuwm.h symguii.h
tnl.obj:        tnl.cpp platform.h undoc.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h tnlint.h tnlres.h symguii.h
draglist.obj:   draglist.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h symguii.h
normtext.obj:   normtext.cpp platform.h windowsx.h normtext.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
spn.obj:        spn.cpp platform.h undoc.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h xapi.h spnint.h symguii.h
spnscrl.obj:    spnscrl.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h spnres.h symguii.h
ngtray.obj:     ngtray.cpp platform.h xapi.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h symguii.h
ctrlpanl.obj:   ctrlpanl.cpp platform.h stdhelp.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h ctrlpanl.h cpl_res.h symguii.h
dynamic.obj:    dynamic.cpp platform.h undoc.h ctrlpanl.h xapi.h file.h ngui.h \
                version.h wndclass.h passwd.h stockbmp.h symguii.h
symtray.obj:    symtray.cpp platform.h
colorbox.obj:   colorbox.cpp platform.h undoc.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h
stockbmp.obj:   stockbmp.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h symguii.h nguistb.h
password.obj:   password.cpp platform.h undoc.h file.h msgdisp.h nguistb.h passwres.h \
                ngui.h version.h wndclass.h passwd.h stockbmp.h \
                stdhelp.h symguii.h
vcrbttn.obj:    vcrbttn.cpp platform.h undoc.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h symguii.h
ngfont.obj:     ngfont.cpp platform.h cpdef.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h nguistb.h symguii.h
editfltr.obj:   editfltr.cpp platform.h xapi.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h
builtin.obj:    builtin.cpp platform.h ctrlpanl.h cpl_res.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h stdhelp.h
stdguilt.obj:   stdguilt.cpp platform.h xapi.h nguistb.h stdres.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h malloc.h verinfo.h \
                symguii.h
drawmark.obj:   drawmark.cpp platform.h nblob.h dmres.h drawmark.nbh drawmark.h \
                ngui.h version.h wndclass.h passwd.h stockbmp.h \
                symguii.h
spnmsgs.obj:    spnmsgs.cpp platform.h xapi.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h spnint.h
slider.obj:     slider.cpp platform.h windowsx.h file.h undoc.h ngui.h version.h \
                wndclass.h passwd.h stockbmp.h symguii.h
toolres.obj:    toolres.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h
treedrag.obj:   treedrag.cpp platform.h treeabs.hpp treeobj.hpp treectrl.hpp \
                treeres.h
treectrl.obj:   treectrl.cpp platform.h treeabs.hpp treeobj.hpp \
                treectrl.hpp treeres.h
browsdlg.obj:   browsdlg.cpp platform.h
treehand.obj:   treehand.cpp platform.h symarray.hpp treehand.hpp
treedraw.obj:   treedraw.cpp platform.h treeabs.hpp treeobj.hpp treectrl.hpp
treeenum.obj:   treeenum.cpp platform.h treeabs.hpp treeobj.hpp treectrl.hpp
treedata.obj:   treedata.cpp platform.h treeabs.hpp treeobj.hpp treectrl.hpp
treeobj.obj:    treeobj.cpp platform.h symarray.hpp treeabs.hpp treeobj.hpp \
                treenode.hpp
treereg.obj:    treereg.cpp platform.h treectrl.hpp
treeline.obj:   treeline.cpp platform.h treeabs.hpp treeobj.hpp treectrl.hpp
treenode.obj:   treenode.cpp platform.h symarray.hpp treeabs.hpp treeobj.hpp \
                treenode.hpp
guistubs.obj:   guistubs.cpp platform.h ngui.h version.h wndclass.h \
                passwd.h stockbmp.h
treemain.obj:   treemain.cpp platform.h
ngvwrtbl.obj:   ngvwrtbl.cpp platform.h xapi.h windowsx.h vwr.h file.h ngui.h \
                version.h wndclass.h passwd.h stockbmp.h symguii.h
symarray.obj:   symarray.cpp platform.h symarray.hpp
npt.obj:        npt.cpp platform.h stdwin.h fileids.h nptstr.h emsg.h \
                npt.h nptint.h
nptabort.obj:   nptabort.cpp platform.h stdwin.h npt.h nptint.h nptres.h ngui.h \
                nlook.h
npterr.obj:     npterr.cpp platform.h stdwin.h fileids.h nptstr.h emsg.h \
                npt.h nptint.h
nptinit.obj:    nptinit.cpp platform.h stdwin.h npt.h nptint.h
nptpbuf.obj:    nptpbuf.cpp platform.h stdwin.h fileids.h nptstr.h emsg.h \
                ngui.h npt.h nptint.h
nptpfile.obj:   nptpfile.cpp platform.h stdwin.h fileids.h nptstr.h \
                emsg.h ngui.h file.h npt.h nptint.h
nptpjobs.obj:   nptpjobs.cpp platform.h stdwin.h fileids.h nptstr.h \
                emsg.h npt.h nptint.h nptres.h
nptprtdc.obj:   nptprtdc.cpp platform.h stdwin.h fileids.h nptstr.h \
                emsg.h npt.h nptint.h
nptsel.obj:     nptsel.cpp platform.h universl.h stdwin.h fileids.h nptstr.h \
                emsg.h nlook.h npt.h nptint.h nptres.h stddlg.h
nptwini.obj:    nptwini.cpp platform.h stdwin.h npt.h nptint.h
nptdlg.obj:     nptdlg.cpp platform.h xapi.h file.h stdhelp.h nmail.h ngui.h npt.h \
                hstring.h nptstr.h nptres.h
shelllnk.obj:   platform.h xapi.h winini.h
%Else
    %Echo QUAKE_CUSTOM not set.  Aborting the build.
%Endif

