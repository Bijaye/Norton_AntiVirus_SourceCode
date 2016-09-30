: Copyright 1993 Symantec Corporation                                   :
:-----------------------------------------------------------------------:
:                                                                       :
: $Header:   S:/NPS/VCS/build.bav   1.5   23 May 1997 17:41:18   KSACKIN  $ :
:                                                                       :
: Description:                                                          :
:                                                                       :
:-----------------------------------------------------------------------:
: $Log:   S:/NPS/VCS/build.bav  $ :
:  
:     Rev 1.5   23 May 1997 17:41:18   KSACKIN
:  Updated this file to copy all of the source files to a lib.bin directory
:  and remove the read-only attribute for redistribution through Install Shield.
:
:     Rev 1.4   13 Mar 1995 15:51:06   MFALLEN
:  Removed help building
:
:     Rev 1.3   10 Feb 1994 12:01:42   PGRAVES
:  Add QAK2 changes to trunk.
:
:     Rev 1.2.1.1   01 Dec 1993 19:26:06   SCOTTP
:  PUT BACK IN AUTOBUILD OF HELP
:
:     Rev 1.2.1.0   19 Oct 1993 08:14:46   SCOTTP
:  Branch base for version QAK2
:
:     Rev 1.2   15 Oct 1993 02:20:36   BRUCEH
:  Took out autobuild of help
:
:     Rev 1.1   11 Aug 1993 17:53:26   MARKL
:  Now builds NPS help.
:
:     Rev 1.0   15 Mar 1993 17:54:00   MARKL
:  Initial revision.
:-----------------------------------------------------------------------:

call mk nps %1
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

