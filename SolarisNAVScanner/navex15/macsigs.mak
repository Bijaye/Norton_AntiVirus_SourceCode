#############################################################################
#  Copyright (C) 1997 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/macsigs.mav   1.11   26 Oct 1998 13:32:20   DCHI  $
#
#  Description:
#       This is a makefile for the signature source files.  Compiling
#       only PASSED type defs.  Does not compile HEU sigs.
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/macsigs.mav  $
#  
#     Rev 1.11   26 Oct 1998 13:32:20   DCHI
#  Changes to support MKMCRSIG multiple part data file generation.
#  
#     Rev 1.10   09 Oct 1998 12:17:32   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.9   18 May 1998 11:30:38   DCHI
#  Added A2 and A97 sections.
#  
#     Rev 1.8   23 Apr 1998 14:42:58   DCHI
#  Added XL95 and XL4 sections.
#  
#     Rev 1.7   21 Apr 1998 18:20:44   tkhanpa
#  No change.
#  
#     Rev 1.6   16 Apr 1998 21:15:00   RELNITI
#  CHANGE XLVARSIG.WDF to XL97VAR.WDF for XL97SIGS
#  
#     Rev 1.5   15 Apr 1998 11:44:30   RELNITI
#  ADD XLNAMSIG.WDF and CHANGE WDVARSIG.WDF to XLVARSIG.WDF in XL97SIGS
#  
#     Rev 1.4   30 Mar 1998 13:51:02   AOONWAL
#  Added wd7sigs6.wdf
#  
#     Rev 1.3   11 Feb 1998 12:01:32   JWILBER
#  Added new *.wdf files for foreign offices.
#
#     Rev 1.2   31 Oct 1997 10:31:08   DCHI
#  Changed mkwrdsig to mkmcrsig.
#
#     Rev 1.1   02 Oct 1997 17:08:34   AOONWAL
#  Added WD7SIGS5.WDF support
#
#     Rev 1.0   24 Jul 1997 17:02:00   DCHI
#  Initial revision.
#
#############################################################################

macsigs: wddecsig.cpp \
         wd7sigs0.cpp \
         wd7sigs1.cpp \
         wd7sigs2.cpp \
         wd7sigs3.cpp \
         wd8sigs0.cpp \
         wd8sigs1.cpp \
         wd8sigs2.cpp \
         wd8sigs3.cpp \
         x97sigs0.cpp \
         x97sigs1.cpp \
         x97sigs2.cpp \
         x97sigs3.cpp \
         x95sigs0.cpp \
         x95sigs1.cpp \
         x95sigs2.cpp \
         x95sigs3.cpp \
         xl4sigs0.cpp \
         xl4sigs1.cpp \
         xl4sigs2.cpp \
         xl4sigs3.cpp \
         a2sigs0.cpp  \
         a2sigs1.cpp  \
         a2sigs2.cpp  \
         a2sigs3.cpp  \
         a97sigs0.cpp \
         a97sigs1.cpp \
         a97sigs2.cpp \
         a97sigs3.cpp

wddecsig.cpp: wddecsig.ddf
             if exist wddecsgf.err del wddecsgf.err
             if not exist wddecsig.ddf copy %proj%\src\wddecsig.ddf > wddecsig.cpy
             Echo mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig 컴컴> wddecsgf.err
             @echo mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig
             mkdecsig.exe -qaPASSED wddecsig.ddf wddecsig > wddecsgf.err
             if errorlevel ne 0 set Beep=E
             if errorlevel ne 0 set Chk=E
             if errorlevel ne 0 Echo Error in processing last step.>>wddecsgf.err
             if errorlevel ne 0 type wddecsgf.err
             if errorlevel eq 0 del wddecsgf.err > nul
             if exist wddecsig.cpy del wddecsig.ddf wddecsig.cpy

wddecsig.ddf:


wd7sigs0.cpp: wd7sigs.lst  \
              wdnamsig.wdf \
              wd7sigs2.wdf \
              wd7sigs3.wdf \
              wd7sigs4.wdf \
              wd7sigs5.wdf \
              wd7sigs6.wdf \
              wd7sigs7.wdf \
              wdvarsig.wdf \
              wd7sigs.wdf
              if exist wd7sigsf.err del wd7sigsf.err
              if not exist wd7sigs.lst copy %proj%\src\wd7sigs.lst > wd7sigs0.cpy
              if not exist wdnamsig.wdf copy %proj%\src\wdnamsig.wdf > wd7sigs1.cpy
              if not exist wd7sigs2.wdf copy %proj%\src\wd7sigs2.wdf > wd7sigs2.cpy
              if not exist wd7sigs3.wdf copy %proj%\src\wd7sigs3.wdf > wd7sigs3.cpy
              if not exist wdvarsig.wdf copy %proj%\src\wdvarsig.wdf > wd7sigs4.cpy
              if not exist wd7sigs.wdf copy %proj%\src\wd7sigs.wdf > wd7sigs5.cpy
              if not exist wd7sigs4.wdf copy %proj%\src\wd7sigs4.wdf > wd7sigs6.cpy
              if not exist wd7sigs5.wdf copy %proj%\src\wd7sigs5.wdf > wd7sigs7.cpy
              if not exist wd7sigs6.wdf copy %proj%\src\wd7sigs6.wdf > wd7sigs8.cpy
              if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigs9.cpy
              if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wdjapnam.cpy
              if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wdausnam.cpy
              if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wdeurnam.cpy
              if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
              if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
              if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
              Echo mkmcrsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
              @echo mkmcrsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs
              mkmcrsig.exe -qaPASSED -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
              if errorlevel ne 0 type wd7sigsf.err
              if errorlevel eq 0 del wd7sigsf.err > nul
              if exist wd7sigs0.cpy del wd7sigs.lst wd7sigs0.cpy
              if exist wd7sigs1.cpy del wdnamsig.wdf wd7sigs1.cpy
              if exist wd7sigs2.cpy del wd7sigs2.wdf wd7sigs2.cpy
              if exist wd7sigs3.cpy del wd7sigs3.wdf wd7sigs3.cpy
              if exist wd7sigs4.cpy del wdvarsig.wdf wd7sigs4.cpy
              if exist wd7sigs5.cpy del wd7sigs.wdf wd7sigs5.cpy
              if exist wd7sigs6.cpy del wd7sigs4.wdf wd7sigs6.cpy
              if exist wd7sigs7.cpy del wd7sigs5.wdf wd7sigs7.cpy
              if exist wd7sigs8.cpy del wd7sigs6.wdf wd7sigs8.cpy
              if exist wd7sigs9.cpy del wd7sigs7.wdf wd7sigs9.cpy
              if exist wdjapsig.cpy del wdjapnam.wdf wdjapnam.cpy
              if exist wdaussig.cpy del wdausnam.wdf wdausnam.cpy
              if exist wdeursig.cpy del wdeurnam.wdf wdeurnam.cpy
              if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
              if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
              if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy

wd7sigs1.cpp: wd7sigs0.cpp
wd7sigs2.cpp: wd7sigs0.cpp
wd7sigs3.cpp: wd7sigs0.cpp

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdnamsig.wdf:
wdvarsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:

wd8sigs0.cpp: wd8sigs.lst  \
              wdnamsig.wdf \
              wd7sigs.wdf  \
              wd7sigs2.wdf \
              wd7sigs3.wdf \
              wd7sigs4.wdf \
              wd7sigs5.wdf \
              wd7sigs6.wdf \
              wd7sigs7.wdf \
              wd8sigs.wdf  \
              wdvarsig.wdf
              if exist wd8sigsf.err del wd8sigsf.err
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
              if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd8sigsA.cpy
              if not exist wdjapnam.wdf copy %proj%\src\wdjapnam.wdf > wdjapnam.cpy
              if not exist wdausnam.wdf copy %proj%\src\wdausnam.wdf > wdausnam.cpy
              if not exist wdeurnam.wdf copy %proj%\src\wdeurnam.wdf > wdeurnam.cpy
              if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
              if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
              if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
              Echo mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
              @echo mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs
              mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs > wd8sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
              if errorlevel ne 0 type wd8sigsf.err
              if errorlevel eq 0 del wd8sigsf.err > nul
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
              if exist wd8sigsA.cpy del wd7sigs7.wdf wd8sigsA.cpy
              if exist wdjapsig.cpy del wdjapnam.wdf wdjapnam.cpy
              if exist wdaussig.cpy del wdausnam.wdf wdausnam.cpy
              if exist wdeursig.cpy del wdeurnam.wdf wdeurnam.cpy
              if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
              if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
              if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy

wd8sigs1.cpp: wd8sigs0.cpp
wd8sigs2.cpp: wd8sigs0.cpp
wd8sigs3.cpp: wd8sigs0.cpp

wdnamsig.wdf:
wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wd8sigs.wdf:
wdvarsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:

x97sigs0.cpp: xl97sigs.lst \
              xlnamsig.wdf \
              xljapsig.wdf \
              xlaussig.wdf \
              xleursig.wdf \
              xl97var.wdf  \
              xl97sigs.wdf
              if exist xl97sgsf.err del xl97sgsf.err
              if not exist xl97sigs.lst copy %proj%\src\xl97sigs.lst > xl97sgs0.cpy
              if not exist xl97sigs.wdf copy %proj%\src\xl97sigs.wdf > xl97sgs1.cpy
              if not exist xljapsig.wdf copy %proj%\src\xljapsig.wdf > xljapsig.cpy
              if not exist xlaussig.wdf copy %proj%\src\xlaussig.wdf > xlaussig.cpy
              if not exist xleursig.wdf copy %proj%\src\xleursig.wdf > xleursig.cpy
              if not exist XL97VAR.WDF copy %proj%\src\XL97VAR.WDF > xl97sgs2.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xl97sgs3.cpy
              Echo mkmcrsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst x97sigs 컴컴> xl97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst x97sigs
              mkmcrsig.exe -qaPASSED -tyXL97 -lxl97sigs.lst x97sigs > xl97sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl97sgsf.err
              if errorlevel ne 0 type xl97sgsf.err
              if errorlevel eq 0 del xl97sgsf.err > nul
              if exist xl97sgs0.cpy del xl97sigs.lst xl97sgs0.cpy
              if exist xl97sgs1.cpy del xl97sigs.wdf xl97sgs1.cpy
              if exist xl97sgs2.cpy del XL97VAR.WDF xl97sgs2.cpy
              if exist xl97sgs3.cpy del xlnamsig.wdf xl97sgs3.cpy
              if exist xljapsig.cpy del xljapsig.wdf xljapsig.cpy
              if exist xlaussig.cpy del xlaussig.wdf xlaussig.cpy
              if exist xleursig.cpy del xleursig.wdf xleursig.cpy

x97sigs1.cpp: x97sigs0.cpp
x97sigs2.cpp: x97sigs0.cpp
x97sigs3.cpp: x97sigs0.cpp

xlnamsig.wdf:
xl97sigs.wdf:
xljapsig.wdf:
xlaussig.wdf:
xleursig.wdf:
XL97VAR.WDF:

x95sigs0.cpp: xl95sigs.lst \
              xlnamsig.wdf \
              xl95sigs.wdf \
              xljapsig.wdf \
              xlaussig.wdf \
              xleursig.wdf \
              xlvarsig.wdf
              if exist xl95sgsf.err del xl95sgsf.err
              if not exist xl95sigs.lst copy %proj%\src\xl95sigs.lst > xl95sgs0.cpy
              if not exist xlnamsig.wdf copy %proj%\src\xlnamsig.wdf > xlnamsig.cpy
              if not exist xl95sigs.wdf copy %proj%\src\xl95sigs.wdf > xl95sgs1.cpy
              if not exist xljapsig.wdf copy %proj%\src\xljapsig.wdf > xljapsig.cpy
              if not exist xlaussig.wdf copy %proj%\src\xlaussig.wdf > xlaussig.cpy
              if not exist xleursig.wdf copy %proj%\src\xleursig.wdf > xleursig.cpy
              if not exist xlvarsig.wdf copy %proj%\src\xlvarsig.wdf > xl95sgs2.cpy
              Echo mkmcrsig.exe -qaPASSED -tyXL95 -lxl95sigs.lst x95sigs 컴컴> xl95sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyXL95 -lxl95sigs.lst x95sigs
              mkmcrsig.exe -qaPASSED -tyXL95 -lxl95sigs.lst x95sigs > xl95sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl95sgsf.err
              if errorlevel ne 0 type xl95sgsf.err
              if errorlevel eq 0 del xl95sgsf.err > nul
              if exist xl95sgs0.cpy del xl95sigs.lst xl95sgs0.cpy
              if exist xlnamsig.cpy del xlnamsig.wdf xlnamsig.cpy
              if exist xl95sgs1.cpy del xl95sigs.wdf xl95sgs1.cpy
              if exist xl95sgs2.cpy del xlvarsig.wdf xl95sgs2.cpy
              if exist xljapsig.cpy del xljapsig.wdf xljapsig.cpy
              if exist xlaussig.cpy del xlaussig.wdf xlaussig.cpy
              if exist xleursig.cpy del xleursig.wdf xleursig.cpy

x95sigs1.cpp: x95sigs0.cpp
x95sigs2.cpp: x95sigs0.cpp
x95sigs3.cpp: x95sigs0.cpp

xlnamsig.wdf:
xl95sigs.wdf:
xljapsig.wdf:
xlaussig.wdf:
xleursig.wdf:
xlvarsig.wdf:

xl4sigs0.cpp: xl4sigs.lst  \
              xl4sigs.wdf  \
              xljapsig.wdf \
              xlaussig.wdf \
              xleursig.wdf
              if exist xl4sgsf.err del xl4sgsf.err
              if not exist xl4sigs.lst copy %proj%\src\xl4sigs.lst > xl4sgs0.cpy
              if not exist xl4sigs.wdf copy %proj%\src\xl4sigs.wdf > xl4sgs1.cpy
              if not exist xljapsig.wdf copy %proj%\src\xljapsig.wdf > xljapsig.cpy
              if not exist xlaussig.wdf copy %proj%\src\xlaussig.wdf > xlaussig.cpy
              if not exist xleursig.wdf copy %proj%\src\xleursig.wdf > xleursig.cpy
              Echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst xl4sigs 컴컴> xl4sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst xl4sigs
              mkmcrsig.exe -qaPASSED -tyXL4 -lxl4sigs.lst xl4sigs > xl4sgsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>xl4sgsf.err
              if errorlevel ne 0 type xl4sgsf.err
              if errorlevel eq 0 del xl4sgsf.err > nul
              if exist xl4sgs0.cpy del xl4sigs.lst xl4sgs0.cpy
              if exist xl4sgs1.cpy del xl4sigs.wdf xl4sgs1.cpy
              if exist xljapsig.cpy del xljapsig.wdf xljapsig.cpy
              if exist xlaussig.cpy del xlaussig.wdf xlaussig.cpy
              if exist xleursig.cpy del xleursig.wdf xleursig.cpy

xl4sigs1.cpp: xl4sigs0.cpp
xl4sigs2.cpp: xl4sigs0.cpp
xl4sigs3.cpp: xl4sigs0.cpp

xl4sigs.wdf:
xljapsig.wdf:
xlaussig.wdf:
xleursig.wdf:

a2sigs0.cpp:  macrovid.h \
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
              Echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst a2sigs 컴컴> a2sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst a2sigs
              mkmcrsig.exe -qaPASSED -tyA2 -la2sigs.lst a2sigs > a2sgsf.err
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

a2sigs1.cpp:  a2sigs0.cpp
a2sigs2.cpp:  a2sigs0.cpp
a2sigs3.cpp:  a2sigs0.cpp

a2sigs.wdf:

a97sigs0.cpp: macrovid.h \
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
              Echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst a97sigs 컴컴> a97sgsf.err
              @echo mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst a97sigs
              mkmcrsig.exe -qaPASSED -tyA97 -la97sigs.lst a97sigs > a97sgsf.err
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

a97sigs1.cpp: a97sigs0.cpp
a97sigs2.cpp: a97sigs0.cpp
a97sigs3.cpp: a97sigs0.cpp

a97sigs.wdf:


