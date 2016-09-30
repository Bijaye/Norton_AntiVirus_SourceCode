#############################################################################
#  Copyright (C) 1996 Symantec Corporation
#############################################################################
#  $Header:   S:/NAVEX/VCS/wd.mav   1.7   26 Oct 1998 13:30:28   DCHI  $
#
#  Description:
#       This is a make file for compiling WD7 signature w/o compiling NAVEX
#
#############################################################################
#  $Log:   S:/NAVEX/VCS/wd.mav  $
#  
#     Rev 1.7   26 Oct 1998 13:30:28   DCHI
#  Changes to support MKMCRSIG multiple part data file generation.
#  
#     Rev 1.6   09 Oct 1998 12:17:52   relniti
#  ADD wd7sigs7.wdf
#  
#     Rev 1.5   30 Mar 1998 13:51:10   AOONWAL
#  Added wd7sigs6.wdf
#  
#     Rev 1.4   11 Feb 1998 12:00:58   JWILBER
#  Added new *.wdf files for foreign offices.
#
#     Rev 1.3   31 Oct 1997 10:31:10   DCHI
#  Changed mkwrdsig to mkmcrsig.
#
#     Rev 1.2   02 Oct 1997 17:08:36   AOONWAL
#  Added WD7SIGS5.WDF support
#
#     Rev 1.1   16 Sep 1997 14:10:44   RELNITI
#  ADD WD8
#
#     Rev 1.0   16 Sep 1997 13:53:14   RELNITI
#  Initial revision.
#############################################################################

wd7sigs0.cpp: wd7sigs.lst  \
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
              wdvarsig.wdf \
              wd7sigs.wdf  \
              wdjapsig.wdf \
              wdaussig.wdf \
              wdeursig.wdf
              if exist wd7sigsf.err del wd7sigsf.err
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
              if not exist wd7sigs7.wdf copy %proj%\src\wd7sigs7.wdf > wd7sigsc.cpy
              if not exist wdjapsig.wdf copy %proj%\src\wdjapsig.wdf > wdjapsig.cpy
              if not exist wdaussig.wdf copy %proj%\src\wdaussig.wdf > wdaussig.cpy
              if not exist wdeursig.wdf copy %proj%\src\wdeursig.wdf > wdeursig.cpy
              Echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs 컴컴> wd7sigsf.err
              @echo mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs
              mkmcrsig.exe -qaPASSED -qaHEU -tyWD7 -lwd7sigs.lst wd7sigs > wd7sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd7sigsf.err
              if errorlevel ne 0 type wd7sigsf.err
              if errorlevel eq 0 del wd7sigsf.err > nul
              if exist wd8sigsf.err del wd8sigsf.err
              if not exist wd8sigs.lst copy %proj%\src\wd8sigs.lst > wd8sigs0.cpy
              if not exist wd8sigs.wdf copy %proj%\src\wd8sigs.wdf > wd8sigs5.cpy
              Echo mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs 컴컴> wd8sigsf.err
              @echo mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs
              mkmcrsig.exe -qaPASSED -tyWD8 -lwd8sigs.lst wd8sigs > wd8sigsf.err
              if errorlevel ne 0 set Beep=E
              if errorlevel ne 0 set Chk=E
              if errorlevel ne 0 Echo Error in processing last step.>>wd8sigsf.err
              if errorlevel ne 0 type wd8sigsf.err
              if errorlevel eq 0 del wd8sigsf.err > nul
              if exist wd8sigs0.cpy del wd8sigs.lst wd8sigs0.cpy
              if exist wd8sigs5.cpy del wd8sigs.wdf wd8sigs5.cpy
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
              if exist wd7sigsc.cpy del wd7sigs7.wdf wd7sigsc.cpy
              if exist wdjapsig.cpy del wdjapsig.wdf wdjapsig.cpy
              if exist wdaussig.cpy del wdaussig.wdf wdaussig.cpy
              if exist wdeursig.cpy del wdeursig.wdf wdeursig.cpy

wd7sigs1.cpp: wd7sigs0.cpp
wd7sigs2.cpp: wd7sigs0.cpp
wd7sigs3.cpp: wd7sigs0.cpp

wd8sigs1.cpp: wd8sigs0.cpp
wd8sigs2.cpp: wd8sigs0.cpp
wd8sigs3.cpp: wd8sigs0.cpp

wd7sigs.wdf:
wd7sigs2.wdf:
wd7sigs3.wdf:
wd7sigs4.wdf:
wd7sigs5.wdf:
wd7sigs6.wdf:
wd7sigs7.wdf:
wdnamsig.wdf:
wdjapnam.wdf:
wdausnam.wdf:
wdeurnam.wdf:
wdjapsig.wdf:
wdaussig.wdf:
wdeursig.wdf:
wdvarsig.wdf:
wd8sigs.wdf:
