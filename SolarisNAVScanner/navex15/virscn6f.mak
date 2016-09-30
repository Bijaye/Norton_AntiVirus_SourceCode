#############################################################################
#  Copyright (C) 1998 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/virscn6f.mav   1.4   09 Oct 1998 12:17:56   relniti  $
#
#  Description:
#       This is a make file for VIRSCN6F.DAT (macro scanning data for
#  foreign offices).
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/virscn6f.mav  $
#  
#     Rev 1.4   09 Oct 1998 12:17:56   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.3   16 Apr 1998 21:14:58   RELNITI
#  CHANGE XLVARSIG.WDF to XL97VAR.WDF for XL97SIGS
#  
#     Rev 1.2   15 Apr 1998 11:44:26   RELNITI
#  ADD XLNAMSIG.WDF and CHANGE WDVARSIG.WDF to XLVARSIG.WDF in XL97SIGS
#  
#     Rev 1.1   30 Mar 1998 13:51:06   AOONWAL
#  Added wd7sigs6.wdf
#  
#     Rev 1.0   02 Mar 1998 21:59:10   JWILBER
#  Initial revision.
#
#############################################################################

VIRSCAN6.DAT: wd7sigs.inf  \
              wd7sigs.dat  \
              wd7sigs.crc  \
              wd8sigs.inf  \
              wd8sigs.dat  \
              wd8sigs.crc  \
              xl97sigs.inf \
              xl97sigs.dat \
              xl97sigs.crc \
              wd7xclsg.dat \
              wd8xclsg.dat \
              virscan6.def \
              version.def
             del master.err VIRSCAN6.DAT
             if not exist virscan6.def copy %proj%\src\virscan6.def > virscan6.cpy
             if not exist version.def copy %proj%\src\version.def > version.cpy
             BUILDDAT   VIRSCAN6.DEF VIRSCAN6.DAT version.def > error.tmp
             if errorlevel ne 0 echo error error error >> master.err
             if errorlevel ne 0 copy master.err+error.tmp master.err
             if exist virscan6.cpy del virscan6.cpy virscan6.def
             if exist version.cpy del version.cpy version.def
             del error.tmp
             copy make.err+*.err make.err

wd7sigs.inf:
wd7sigs.crc:
wd7sigs.dat: macrovid.h   \
             japanvid.h   \
             austrvid.h   \
             europvid.h   \
             wd7sigs.lst  \
             wdnamsig.wdf \
             wdjapnam.wdf \
             wdausnam.wdf \
             wdeurnam.wdf \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wd7sigs4.wdf \
             wd7sigs5.wdf \
             wd7sigs6.wdf \
             wd7sigs7.wdf \
             wdjapsig.wdf \
             wdaussig.wdf \
             wdeursig.wdf \
             wdvarsig.wdf \
             wd7sigs.wdf
             if exist wd7sigsf.err del wd7sigsf.err
             if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
             if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
             if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
             if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
             if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
             if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
             if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd7sigs8.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd7sigs9.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd7sigsa.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigsb.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsC.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
             Echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
             @echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs
             mkmcrsg2.exe -qaPASSED -qaHEU -tyWD7 -datafile -lwd7sigs.lst wd7sigs > wd7sigsf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
             if errorlevel ne 0 type wd7sigsf.err
             if errorlevel eq 0 del wd7sigsf.err > nul
             if exist macrovid.cpy del macrovid.h macrovid.cpy
             if exist japanvid.cpy del japanvid.h japanvid.cpy
             if exist austrvid.cpy del austrvid.h austrvid.cpy
             if exist europvid.cpy del europvid.h europvid.cpy
             if exist wd7sigs0.cpy del wd7sigs.lst wd7sigs0.cpy
             if exist wd7sigs1.cpy del wdnamsig.wdf wd7sigs1.cpy
             if exist wd7sigs2.cpy del wd7sigs2.wdf wd7sigs2.cpy
             if exist wd7sigs3.cpy del wd7sigs3.wdf wd7sigs3.cpy
             if exist wd7sigs4.cpy del wdvarsig.wdf wd7sigs4.cpy
             if exist wd7sigs5.cpy del wd7sigs.wdf wd7sigs5.cpy
             if exist wd7sigs6.cpy del wd7sigs4.wdf wd7sigs6.cpy
             if exist wd7sigs7.cpy del wd7sigs5.wdf wd7sigs7.cpy
             if exist wd7sigs8.cpy del wdjapnam.wdf wd7sigs8.cpy
             if exist wd7sigs9.cpy del wdausnam.wdf wd7sigs9.cpy
             if exist wd7sigsa.cpy del wdeurnam.wdf wd7sigsa.cpy
             if exist wd7sigsb.cpy del wd7sigs6.wdf wd7sigsb.cpy
             if exist wd7sigsC.cpy del wd7sigs7.wdf wd7sigsC.cpy
             if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
             if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
             if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy


wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdnamsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdvarsig.wdf:

wd8sigs.inf:
wd8sigs.crc:
wd8sigs.dat: macrovid.h   \
             japanvid.h   \
             austrvid.h   \
             europvid.h   \
             wd8sigs.lst  \
             wdnamsig.wdf \
             wdjapnam.wdf \
             wdausnam.wdf \
             wdeurnam.wdf \
             wd7sigs.wdf  \
             wd7sigs2.wdf \
             wd7sigs3.wdf \
             wd7sigs4.wdf \
             wd7sigs5.wdf \
             wd7sigs6.wdf \
             wd7sigs7.wdf \
             wdjapsig.wdf \
             wdaussig.wdf \
             wdeursig.wdf \
             wd8sigs.wdf  \
             wdvarsig.wdf
             if exist wd8sigsf.err del wd8sigsf.err
             if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
             if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
             if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
             if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
             if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
             if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd8sigs1.cpy
             if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd8sigs2.cpy
             if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd8sigs3.cpy
             if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd8sigs4.cpy
             if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
             if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd8sigs6.cpy
             if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd8sigs7.cpy
             if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd8sigs8.cpy
             if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wd8sigs9.cpy
             if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wd8sigsa.cpy
             if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wd8sigsb.cpy
             if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd8sigsc.cpy
             if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sigsD.cpy
             if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
             if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
             if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
             Echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
             @echo mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs
             mkmcrsg2.exe -qaPASSED -qaHEU -tyWD8 -datafile -lwd8sigs.lst wd8sigs > wd8sigsf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
             if errorlevel ne 0 type wd8sigsf.err
             if errorlevel eq 0 del wd8sigsf.err > nul
             if exist macrovid.cpy del macrovid.h macrovid.cpy
             if exist japanvid.cpy del japanvid.h japanvid.cpy
             if exist austrvid.cpy del austrvid.h austrvid.cpy
             if exist europvid.cpy del europvid.h europvid.cpy
             if exist wd8sigs0.cpy del wd8sigs.lst wd8sigs0.cpy
             if exist wd8sigs1.cpy del wdnamsig.wdf wd8sigs1.cpy
             if exist wd8sigs2.cpy del wd7sigs.wdf wd8sigs2.cpy
             if exist wd8sigs3.cpy del wd7sigs2.wdf wd8sigs3.cpy
             if exist wd8sigs4.cpy del wd7sigs3.wdf wd8sigs4.cpy
             if exist wd8sigs5.cpy del wd8sigs.wdf wd8sigs5.cpy
             if exist wd8sigs6.cpy del wdvarsig.wdf wd8sigs6.cpy
             if exist wd8sigs7.cpy del wd7sigs4.wdf wd8sigs7.cpy
             if exist wd8sigs8.cpy del wd7sigs5.wdf wd8sigs8.cpy
             if exist wd8sigs9.cpy del wdjapnam.wdf wd8sigs9.cpy
             if exist wd8sigsa.cpy del wdausnam.wdf wd8sigsa.cpy
             if exist wd8sigsb.cpy del wdeurnam.wdf wd8sigsb.cpy
             if exist wd8sigsc.cpy del wd7sigs6.wdf wd8sigsc.cpy
             if exist wd8sigsD.cpy del wd7sigs7.wdf wd8sigsD.cpy
             if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
             if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
             if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy


wdnamsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:
wd8sigs.wdf:
wdvarsig.wdf:

xl97sigs.inf:
xl97sigs.crc:
xl97sigs.dat: macrovid.h   \
              japanvid.h   \
              austrvid.h   \
              europvid.h   \
              xljapsig.wdf \
              xlaussig.wdf \
              xleursig.wdf \
              xl97sigs.lst \
              xlnamsig.wdf \
              xl97var.wdf \
              xl97sigs.wdf
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist macrovid.h copy %proj%\src\macrovid.h > macrovid.cpy
              if not exist japanvid.h copy %proj%\src\japanvid.h > japanvid.cpy
              if not exist austrvid.h copy %proj%\src\austrvid.h > austrvid.cpy
              if not exist europvid.h copy %proj%\src\europvid.h > europvid.cpy
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist xl97var.wdf copy %proj%\src\xl97var.wdf > xl97sgs2.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
              if not exist xljapsig.wdf copy %proj%\src\xljapsig.wdf > xljapsig.cpy
              if not exist xlaussig.wdf copy %proj%\src\xlaussig.wdf > xlaussig.cpy
              if not exist xleursig.wdf copy %proj%\src\xleursig.wdf > xleursig.cpy
              Echo mkmcrsg2.exe -qaPASSED -tyXL97 -datafile -lxl97sigs.lst xl97sigs 컴컴> xl97sgsf.err
              @echo mkmcrsg2.exe -qaPASSED -tyXL97 -datafile -lxl97sigs.lst xl97sigs
              mkmcrsg2.exe -qaPASSED -tyXL97 -datafile -lxl97sigs.lst xl97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist macrovid.cpy del macrovid.h macrovid.cpy
              if exist japanvid.cpy del japanvid.h japanvid.cpy
              if exist austrvid.cpy del austrvid.h austrvid.cpy
              if exist europvid.cpy del europvid.h europvid.cpy
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del xl97var.wdf xl97sgs2.cpy
              if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy

xl97sigs.wdf:
xljapsig.wdf:
xlaussig.wdf:
xleursig.wdf:
xl97var.wdf:
xlnamsig.wdf:

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

