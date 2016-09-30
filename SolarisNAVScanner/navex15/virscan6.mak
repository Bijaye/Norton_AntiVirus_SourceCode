#############################################################################
#  Copyright (C) 1997 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/VIRSCAN6.MAv   1.22   15 Dec 1998 12:11:20   DCHI  $
#
#  Description:
#       This is a make file for VIRSCAN6.DAT (macro scanning data).
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/VIRSCAN6.MAv  $
#  
#     Rev 1.22   15 Dec 1998 12:11:20   DCHI
#  Added P97 section.
#  
#     Rev 1.21   09 Oct 1998 12:17:50   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.20   21 Sep 1998 18:01:32   JWILBER
#  Added support for foreign Word macro sig files.
#
#     Rev 1.19   04 Aug 1998 14:05:54   DCHI
#  Changes for Excel heuristics.
#
#     Rev 1.18   10 Jun 1998 12:59:20   DCHI
#  Added W7H2 sections.
#
#     Rev 1.17   12 May 1998 11:18:28   DCHI
#  Update for Access sections.
#
#     Rev 1.16   11 May 1998 18:23:16   DCHI
#  Added Access 2.0 (A2) and Access 97 (A97) sections.
#
#     Rev 1.15   22 Apr 1998 15:09:10   DCHI
#  Updated for austrvid.h, europvid.h, and japanvid.h.
#
#     Rev 1.14   16 Apr 1998 21:14:56   RELNITI
#  CHANGE XLVARSIG.WDF to XL97VAR.WDF for XL97SIGS
#
#     Rev 1.13   15 Apr 1998 17:51:50   DCHI
#
#     Rev 1.12   15 Apr 1998 17:24:22   DCHI
#  Modifications for new XL95 and XL5 engines.
#
#     Rev 1.11   15 Apr 1998 10:57:50   RELNITI
#  ADD XLNAMSIG.WDF to XL97SIGS build
#  CHANGE WDVARSIG.WDF to XLVARSIG.WDF in XL97SIGS build
#
#     Rev 1.10   30 Mar 1998 13:50:30   AOONWAL
#  Added wd7sigs6.wdf
#
#     Rev 1.9   02 Mar 1998 21:54:42   JWILBER
#  Fixed up typos.
#
#     Rev 1.8   02 Mar 1998 21:53:10   JWILBER
#  Removed references to files for foreign offices.
#
#     Rev 1.7   10 Feb 1998 21:18:12   JWILBER
#  Added name sig files for foreign offices.
#
#     Rev 1.6   09 Feb 1998 19:57:38   JWILBER
#  Made changes due to additions of *.wdf and *.h files for foreign
#  offices.
#
#     Rev 1.5   31 Oct 1997 10:30:54   DCHI
#  Changed mkwrdsig to mkmcrsig.
#
#     Rev 1.4   15 Oct 1997 18:14:44   DCHI
#  Added heuristics exclusion sections.
#
#     Rev 1.3   02 Oct 1997 17:08:16   AOONWAL
#  Added WD7SIGS5.WDF support
#
#     Rev 1.2   24 Jul 1997 17:07:28   DCHI
#  Added -qaHEU to compilation of WD7 signatures.
#
#     Rev 1.1   14 Jul 1997 15:01:56   DCHI
#  Switched order of *.crc and *.dat to fix build weirdness.
#
#     Rev 1.0   08 Jul 1997 15:07:02   DCHI
#  Initial revision.
#
#############################################################################

VIRSCAN6.DAT:              \
              wd7sigs.inf  \
              wd7sigs.dat  \
              wd7sigs.crc  \
              wd8sigs.inf  \
              wd8sigs.dat  \
              wd8sigs.crc  \
              xl97sigs.inf \
              xl97sigs.dat \
              xl97sigs.crc \
              xl95sigs.inf \
              xl95sigs.dat \
              xl95sigs.crc \
              xl4sigs.inf  \
              xl4sigs.dat  \
              xl4sigs.crc  \
              wd7xclsg.dat \
              x95xclsg.dat \
              wd8xclsg.dat \
              x97xclsg.dat \
              a2sigs.inf   \
              a2sigs.dat   \
              a2sigs.crc   \
              a97sigs.inf  \
              a97sigs.dat  \
              a97sigs.crc  \
              p97sigs.inf  \
              p97sigs.dat  \
              p97sigs.crc  \
              w7h2sigs.inf \
              w7h2sigs.dat \
              version.def
             del master.err VIRSCAN6.DAT
             if not exist w7h2sigs.inf copy %proj%\src\w7h2sigs.inf > w7h2sgsi.cpy
             if not exist w7h2sigs.dat copy %proj%\src\w7h2sigs.dat > w7h2sgsd.cpy
             if not exist virscan6.def copy %proj%\src\virscan6.def > virscan6.cpy
             if not exist version.def copy %proj%\src\version.def > version.cpy
             BUILDDAT   VIRSCAN6.DEF VIRSCAN6.DAT version.def > error.tmp
             if errorlevel ne 0 echo error error error >> master.err
             if errorlevel ne 0 copy master.err+error.tmp master.err
             if exist w7h2sgsi.cpy del w7h2sgsi.cpy w7h2sigs.inf
             if exist w7h2sgsd.cpy del w7h2sgsd.cpy w7h2sigs.dat
             if exist virscan6.cpy del virscan6.cpy virscan6.def
             if exist version.cpy del version.cpy version.def
             del error.tmp
             copy make.err+*.err make.err

wd7sigs.inf:
wd7sigs.crc:
wd7sigs.dat: macrovid.h     \
             austrvid.h     \
             europvid.h     \
             japanvid.h     \
             wd7sigs.lst    \
             wdnamsig.wdf   \
             wdausnam.wdf   \
             wdeurnam.wdf   \
             wdjapnam.wdf   \
             wd7sigs2.wdf   \
             wd7sigs3.wdf   \
             wd7sigs4.wdf   \
             wd7sigs5.wdf   \
             wd7sigs6.wdf   \
             wd7sigs7.wdf   \
             wdaussig.wdf   \
             wdeursig.wdf   \
             wdjapsig.wdf   \
             wdvarsig.wdf   \
             wd7sigs.wdf
             if exist wd7sigsf.err del wd7sigsf.err
             if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
             if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
             if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
             if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
             if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
             if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
             if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigs8.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd7sigs9.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd7sigsa.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd7sigsb.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd7sigsc.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd7sigsd.cpy
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd7sigse.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsF.cpy
             Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
             @echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs
             mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs > wd7sigsf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
             if errorlevel ne 0 type wd7sigsf.err
             if errorlevel eq 0 del wd7sigsf.err > nul
             if exist macrovid.cpy del macrovid.h macrovid.cpy
             if exist austrvid.cpy del austrvid.h austrvid.cpy
             if exist europvid.cpy del europvid.h europvid.cpy
             if exist japanvid.cpy del japanvid.h japanvid.cpy
             if exist wd7sigs0.cpy del wd7sigs.lst wd7sigs0.cpy
             if exist wd7sigs1.cpy del wdnamsig.wdf wd7sigs1.cpy
             if exist wd7sigs2.cpy del wd7sigs2.wdf wd7sigs2.cpy
             if exist wd7sigs3.cpy del wd7sigs3.wdf wd7sigs3.cpy
             if exist wd7sigs4.cpy del wdvarsig.wdf wd7sigs4.cpy
             if exist wd7sigs5.cpy del wd7sigs.wdf wd7sigs5.cpy
             if exist wd7sigs6.cpy del wd7sigs4.wdf wd7sigs6.cpy
             if exist wd7sigs7.cpy del wd7sigs5.wdf wd7sigs7.cpy
             if exist wd7sigs8.cpy del wd7sigs6.wdf wd7sigs8.cpy
             if exist wd7sigs9.cpy del wdaussig.wdf wd7sigs9.cpy
             if exist wd7sigsa.cpy del wdeursig.wdf wd7sigsa.cpy
             if exist wd7sigsb.cpy del wdjapsig.wdf wd7sigsb.cpy
             if exist wd7sigsc.cpy del wdausnam.wdf wd7sigsc.cpy
             if exist wd7sigsd.cpy del wdeurnam.wdf wd7sigsd.cpy
             if exist wd7sigse.cpy del wdjapnam.wdf wd7sigse.cpy
             if exist wd7sigsF.cpy del wd7sigs7.wdf wd7sigsF.cpy

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdjapsig.wdf:
wdnamsig.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapnam.wdf:
wdvarsig.wdf:

wd8sigs.inf:
wd8sigs.crc:
wd8sigs.dat: macrovid.h     \
             austrvid.h     \
             europvid.h     \
             japanvid.h     \
             wd8sigs.lst    \
             wdnamsig.wdf   \
             wdausnam.wdf   \
             wdeurnam.wdf   \
             wdjapnam.wdf   \
             wd7sigs.wdf    \
             wd7sigs2.wdf   \
             wd7sigs3.wdf   \
             wd7sigs4.wdf   \
             wd7sigs5.wdf   \
             wd7sigs6.wdf   \
             wd7sigs7.wdf   \
             wdaussig.wdf   \
             wdeursig.wdf   \
             wdjapsig.wdf   \
             wd8sigs.wdf    \
             wdvarsig.wdf
             if exist wd8sigsf.err del wd8sigsf.err
             if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
             if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
             if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
             if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
             if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd8sigs1.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd8sigs2.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd8sigs3.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd8sigs4.cpy
             if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd8sigs6.cpy
             if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd8sigs7.cpy
             if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd8sigs8.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd8sigs9.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wd8sigsa.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wd8sigsb.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wd8sigsc.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd8sigsd.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd8sigse.cpy
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd8sigsf.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sig10.cpy
             Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
             @echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs
             mkmcrsig.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs > wd8sigsf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
             if errorlevel ne 0 type wd8sigsf.err
             if errorlevel eq 0 del wd8sigsf.err > nul
             if exist macrovid.cpy del macrovid.h macrovid.cpy
             if exist austrvid.cpy del austrvid.h austrvid.cpy
             if exist europvid.cpy del europvid.h europvid.cpy
             if exist japanvid.cpy del japanvid.h japanvid.cpy
             if exist wd8sigs0.cpy del wd8sigs.lst wd8sigs0.cpy
             if exist wd8sigs1.cpy del wdnamsig.wdf wd8sigs1.cpy
             if exist wd8sigs2.cpy del wd7sigs.wdf wd8sigs2.cpy
             if exist wd8sigs3.cpy del wd7sigs2.wdf wd8sigs3.cpy
             if exist wd8sigs4.cpy del wd7sigs3.wdf wd8sigs4.cpy
             if exist wd8sigs5.cpy del wd8sigs.wdf wd8sigs5.cpy
             if exist wd8sigs6.cpy del wdvarsig.wdf wd8sigs6.cpy
             if exist wd8sigs7.cpy del wd7sigs4.wdf wd8sigs7.cpy
             if exist wd8sigs8.cpy del wd7sigs5.wdf wd8sigs8.cpy
             if exist wd8sigs9.cpy del wd7sigs6.wdf wd8sigs9.cpy
             if exist wd8sigsa.cpy del wdaussig.wdf wd8sigsa.cpy
             if exist wd8sigsb.cpy del wdeursig.wdf wd8sigsb.cpy
             if exist wd8sigsc.cpy del wdjapsig.wdf wd8sigsc.cpy
             if exist wd8sigsd.cpy del wdausnam.wdf wd8sigsd.cpy
             if exist wd8sigse.cpy del wdeurnam.wdf wd8sigse.cpy
             if exist wd8sigsf.cpy del wdjapnam.wdf wd8sigsf.cpy
             if exist wd8sig10.cpy del wd7sigs7.wdf wd8sig10.cpy

wdnamsig.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapnam.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdjapsig.wdf:
wd8sigs.wdf:
wdvarsig.wdf:

xl97sigs.inf:
xl97sigs.crc:
xl97sigs.dat: macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              xl97sigs.lst \
              xlnamsig.wdf \
              xl97var.wdf \
              xl97sigs.wdf
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist xl97var.wdf copy %proj%\src\xl97var.wdf > xl97sgs2.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -datafile -lxl97sigs.lst xl97sigs 컴컴> xl97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -datafile -lxl97sigs.lst xl97sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyXL97 -datafile -lxl97sigs.lst xl97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del xl97var.wdf xl97sgs2.cpy
              if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy

xl97sigs.wdf:
xlnamsig.wdf:
xl97var.wdf:

xl95sigs.inf:
xl95sigs.crc:
xl95sigs.dat: macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              xl95sigs.lst \
              xlnamsig.wdf \
              xl95sigs.wdf \
              xlvarsig.wdf
              if exist xl95sgsf.err del xl95sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist xl95sigs.lst copy %proj%\src\xl95sigs.lst > xl95sgs0.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl95sgs1.cpy
              if not exist xl95sigs.wdf copy %proj%\src\xl95sigs.wdf > xl95sgs2.cpy
              if not exist xlvarsig.wdf copy %proj%\src\xlvarsig.wdf > xl95sgs3.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -datafile -lxl95sigs.lst xl95sigs 컴컴> xl95sgsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -datafile -lxl95sigs.lst xl95sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyXL95 -datafile -lxl95sigs.lst xl95sigs > xl95sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl95sgsf.err
              if errorlevel ne 0 type xl95sgsf.err
              if errorlevel eq 0 del xl95sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist xl95sgs0.cpy del xl95sigs.lst xl95sgs0.cpy
              if exist xl95sgs1.cpy del xlnamsig.wdf xl95sgs1.cpy
              if exist xl95sgs2.cpy del xl95sigs.wdf xl95sgs2.cpy
              if exist xl95sgs3.cpy del xlvarsig.wdf xl95sgs3.cpy

xl95sigs.wdf:
xlnamsig.wdf:
xlvarsig.wdf:

xl4sigs.inf:
xl4sigs.crc:
xl4sigs.dat:  macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              xl4sigs.lst \
              xl4sigs.wdf
              if exist xl4sgsf.err del xl4sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist xl4sigs.lst copy %proj%\src\xl4sigs.lst > xl4sgs0.cpy
              if not exist xl4sigs.wdf copy %proj%\src\xl4sigs.wdf > xl4sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyXL4 -datafile -lxl4sigs.lst xl4sigs 컴컴> xl4sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyXL4 -datafile -lxl4sigs.lst xl4sigs
              mkmcrsig.exe -qaPASSED -tyXL4 -datafile -lxl4sigs.lst xl4sigs > xl4sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl4sgsf.err
              if errorlevel ne 0 type xl4sgsf.err
              if errorlevel eq 0 del xl4sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist xl4sgs0.cpy del xl4sigs.lst xl4sgs0.cpy
              if exist xl4sgs1.cpy del xl4sigs.wdf xl4sgs1.cpy

xl4sigs.wdf:

wd7xclsg.dat: wd7xclsg.xdf
              if exist wd7xclsg.err del wd7xclsg.err
              if not exist wd7xclsg.xdf copy %proj%\src\wd7xclsg.xdf > wd7xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD7 -datafile wd7xclsg.xdf wd7xclsg 컴컴> wd7xclsg.err
              @echo mkxclsig.exe -qaPASSED -tyWD7 -datafile wd7xclsg.xdf wd7xclsg
              mkxclsig.exe -qaPASSED -tyWD7 -datafile wd7xclsg.xdf wd7xclsg > wd7xclsg.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7xclsg.err
              if errorlevel ne 0 type wd7xclsg.err
              if errorlevel eq 0 del wd7xclsg.err > nul
              if exist wd7xclsg.cpy del wd7xclsg.xdf wd7xclsg.cpy

wd7xclsg.xdf:

x95xclsg.dat: x95xclsg.xdf
              if exist x95xclsg.err del x95xclsg.err
              if not exist x95xclsg.xdf copy %proj%\src\x95xclsg.xdf > x95xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyXL95 -datafile x95xclsg.xdf x95xclsg 컴컴> x95xclsg.err
              @echo mkxclsig.exe -qaPASSED -tyXL95 -datafile x95xclsg.xdf x95xclsg
              mkxclsig.exe -qaPASSED -tyXL95 -datafile x95xclsg.xdf x95xclsg > x95xclsg.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>x95xclsg.err
              if errorlevel ne 0 type x95xclsg.err
              if errorlevel eq 0 del x95xclsg.err > nul
              if exist x95xclsg.cpy del x95xclsg.xdf x95xclsg.cpy

x95xclsg.xdf:

wd8xclsg.dat: wd8xclsg.xdf
              if exist wd8xclsg.err del wd8xclsg.err
              if not exist wd8xclsg.xdf copy %proj%\src\wd8xclsg.xdf > wd8xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyWD8 -datafile wd8xclsg.xdf wd8xclsg 컴컴> wd8xclsg.err
              @echo mkxclsig.exe -qaPASSED -tyWD8 -datafile wd8xclsg.xdf wd8xclsg
              mkxclsig.exe -qaPASSED -tyWD8 -datafile wd8xclsg.xdf wd8xclsg > wd8xclsg.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8xclsg.err
              if errorlevel ne 0 type wd8xclsg.err
              if errorlevel eq 0 del wd8xclsg.err > nul
              if exist wd8xclsg.cpy del wd8xclsg.xdf wd8xclsg.cpy

wd8xclsg.xdf:

x97xclsg.dat: x97xclsg.xdf
              if exist x97xclsg.err del x97xclsg.err
              if not exist x97xclsg.xdf copy %proj%\src\x97xclsg.xdf > x97xclsg.cpy
              Echo mkxclsig.exe -qaPASSED -tyXL97 -datafile x97xclsg.xdf x97xclsg 컴컴> x97xclsg.err
              @echo mkxclsig.exe -qaPASSED -tyXL97 -datafile x97xclsg.xdf x97xclsg
              mkxclsig.exe -qaPASSED -tyXL97 -datafile x97xclsg.xdf x97xclsg > x97xclsg.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>x97xclsg.err
              if errorlevel ne 0 type x97xclsg.err
              if errorlevel eq 0 del x97xclsg.err > nul
              if exist x97xclsg.cpy del x97xclsg.xdf x97xclsg.cpy

x97xclsg.xdf:

a2sigs.inf:
a2sigs.crc:
a2sigs.dat:   macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              a2sigs.lst \
              a2sigs.wdf
              if exist a2sgsf.err del a2sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist a2sigs.lst copy %proj%\src\a2sigs.lst > a2sgs0.cpy
              if not exist a2sigs.wdf copy %proj%\src\a2sigs.wdf > a2sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyA2 -datafile -la2sigs.lst a2sigs 컴컴> a2sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA2 -datafile -la2sigs.lst a2sigs
              mkmcrsig.exe -qaPASSED -tyA2 -datafile -la2sigs.lst a2sigs > a2sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>a2sgsf.err
              if errorlevel ne 0 type a2sgsf.err
              if errorlevel eq 0 del a2sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist a2sgs0.cpy del a2sigs.lst a2sgs0.cpy
              if exist a2sgs1.cpy del a2sigs.wdf a2sgs1.cpy

a2sigs.wdf:

a97sigs.inf:
a97sigs.crc:
a97sigs.dat:  macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              a97sigs.lst \
              a97sigs.wdf
              if exist a97sgsf.err del a97sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist a97sigs.lst copy %proj%\src\a97sigs.lst > a97sgs0.cpy
              if not exist a97sigs.wdf copy %proj%\src\a97sigs.wdf > a97sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyA97 -datafile -la97sigs.lst a97sigs 컴컴> a97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA97 -datafile -la97sigs.lst a97sigs
              mkmcrsig.exe -qaPASSED -tyA97 -datafile -la97sigs.lst a97sigs > a97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>a97sgsf.err
              if errorlevel ne 0 type a97sgsf.err
              if errorlevel eq 0 del a97sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist a97sgs0.cpy del a97sigs.lst a97sgs0.cpy
              if exist a97sgs1.cpy del a97sigs.wdf a97sgs1.cpy

a97sigs.wdf:

p97sigs.inf:
p97sigs.crc:
p97sigs.dat:  macrovid.h \
              austrvid.h \
              europvid.h \
              japanvid.h \
              p97sigs.lst \
              p97sigs.wdf
              if exist p97sgsf.err del p97sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist p97sigs.lst copy %proj%\src\p97sigs.lst > p97sgs0.cpy
              if not exist p97sigs.wdf copy %proj%\src\p97sigs.wdf > p97sgs1.cpy
              Echo mkmcrsig.exe -qaPASSED -tyP97 -datafile -lp97sigs.lst p97sigs 컴컴> p97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyP97 -datafile -lp97sigs.lst p97sigs
              mkmcrsig.exe -qaPASSED -tyP97 -datafile -lp97sigs.lst p97sigs > p97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>p97sgsf.err
              if errorlevel ne 0 type p97sgsf.err
              if errorlevel eq 0 del p97sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist p97sgs0.cpy del p97sigs.lst p97sgs0.cpy
              if exist p97sgs1.cpy del p97sigs.wdf p97sgs1.cpy

p97sigs.wdf:

w7h2sigs.inf:
w7h2sigs.dat:
              echo nothing > NUL


