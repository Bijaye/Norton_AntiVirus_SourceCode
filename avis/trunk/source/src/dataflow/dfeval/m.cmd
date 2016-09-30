: SCC: @(#) 00 10/9/98 15:30:56 AVNADMIN32 1.1 @(#)
: Syntax:
:     m depend  [..]
:     m compile [..]
:     m help    [..]
:     m library [..]
:     m exedll  [..]
:     m all     [..]
:     m clean   [..]
:
:     m [-a] debug/release

SET   THIS_PROJECT_NAME=DFEval
SET   THIS_LIBRARY_NAME=avisdfev

call %UTILD%\iunimake.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9

SET   THIS_PROJECT_NAME=
SET   THIS_LIBRARY_NAME=
