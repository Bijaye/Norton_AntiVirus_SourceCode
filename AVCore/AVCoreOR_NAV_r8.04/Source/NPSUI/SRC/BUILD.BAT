: Copyright 1993 Symantec Corporation                                   :
:-----------------------------------------------------------------------:
:                                                                       :
: $Header:   S:/NPSUI/VCS/build.bav   1.0   23 May 1997 17:50:40   KSACKIN  $ :
:                                                                       :
: Description:                                                          :
:                                                                       :
:-----------------------------------------------------------------------:
: $Log:   S:/NPSUI/VCS/build.bav  $ :
:  
:     Rev 1.0   23 May 1997 17:50:40   KSACKIN
:  Initial revision.
:
:-----------------------------------------------------------------------:

call mk npsui %1
if exist make.err copy build.err+make.err build.err


:
: Copy all source files to a lib.bin directory and remove read-only
: attribute to provide Install Shield with writable source files.
:
IFF NOT EXIST %PROJ%\LIB%PROJVERS%.BIN\NUL  THEN
    md %PROJ%\LIB%PROJVERS%.BIN
ENDIFF

copy %PROJ%\SRC%PROJVERS%\*.* %PROJ%\LIB%PROJVERS%.BIN
attrib -r %PROJ%\LIB%PROJVERS%.BIN\*.*

