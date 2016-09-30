: Copyright 1992 Symantec Corporation                                   :
:-----------------------------------------------------------------------:
:                                                                       :
: $Header:   S:\source\certlib4\vcs\nlmupdat.bav   1.2   11 Feb 1994 15:17:56   MARKL  $ :
:                                                                       :
: Description:                                                          :
:                                                                       :
:-----------------------------------------------------------------------:
: $Log:   S:\source\certlib4\vcs\nlmupdat.bav  $ :
:  
:     Rev 1.2   11 Feb 1994 15:17:56   MARKL
:  Qak 2 changes moved to trunk
:  
:     Rev 1.1.1.0   19 Oct 1993 07:15:12   SCOTTP
:  Branch base for version QAK2
:  
:     Rev 1.1   12 Oct 1993 16:24:18   PATRICKM
:  Baseline from QAK1 branch
:  
:     Rev 1.0.2.0   03 Sep 1993 19:27:16   SCOTTP
:  Branch base for version QAK1
:  
:     Rev 1.0   17 Mar 1993 09:15:18   JMILLARD
:  Initial revision.
:  
:     Rev 1.0   07 Jan 1993 23:22:54   JMILLARD
:  Initial revision.
:  
:     Rev 1.3   03 Nov 1992 17:25:18   MARKL
:  DLL goes in BIN.WIN not in LIB.WIN
:  
:     Rev 1.2   30 Oct 1992 09:46:36   JMILLARD
:  Don't copy *.* to s:..\lib.win
:  
:     Rev 1.1   30 Oct 1992 09:29:00   JMILLARD
:  Added windows
:  
:-----------------------------------------------------------------------:

copy *.res %PROJ%\LIB%PLATFORM%
copy *.lib %PROJ%\LIB%PLATFORM%
copy *.dll %PROJ%\LIB%PLATFORM%
copy *.obj %PROJ%\LIB%PLATFORM%
copy *.sym %PROJ%\LIB%PLATFORM%
copy *.lib %PROJPRE%\LIB%PLATFORM%
copy *.dll %PROJPRE%\BIN%PLATFORM%
copy *.sym %PROJPRE%\BIN%PLATFORM%
