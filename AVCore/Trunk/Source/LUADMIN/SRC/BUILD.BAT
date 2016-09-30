: Copyright 1993 Symantec Corporation                                   :
:-----------------------------------------------------------------------:
:                                                                       :
: $Header:   S:/LUADMIN/VCS/build.bav   1.0   13 Jun 1997 13:51:48   KSACKIN  $ :
:                                                                       :
: Description:                                                          :
:                                                                       :
:-----------------------------------------------------------------------:
: $Log:   S:/LUADMIN/VCS/build.bav  $ :
:  
:     Rev 1.0   13 Jun 1997 13:51:48   KSACKIN
:  Initial revision.
:
:     Rev 1.0   23 May 1997 17:50:40   KSACKIN
:  Initial revision.
:
:-----------------------------------------------------------------------:

: Copy all source files to a lib.bin directory and remove read-only
: attribute to provide Install Shield with writable source files.
:
IFF NOT EXIST %PROJ%\LIB%PROJVERS%.BIN\NUL  THEN
    md %PROJ%\LIB%PROJVERS%.BIN
ENDIFF

copy %PROJ%\SRC%PROJVERS%\*.* %PROJ%\LIB%PROJVERS%.BIN
attrib -r %PROJ%\LIB%PROJVERS%.BIN\*.*

