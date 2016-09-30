::
:: $Header:   S:/help.dx/VCS/update.bav   1.0   06 Feb 1997 20:50:50   RFULLER  $
::
::  Update batch file for HELP.DOS
::
:*************************************************************************
:: $Log:   S:/help.dx/VCS/update.bav  $
:  
:     Rev 1.0   06 Feb 1997 20:50:50   RFULLER
:  Initial revision
:  
:     Rev 1.0   29 Aug 1996 10:40:10   JBRENNA
:  Initial revision.
:  
:     Rev 1.2.2.0   04 Mar 1996 16:59:28   RJACKSO
:  Branch base for version LUG3
:  
:     Rev 1.2   11 Dec 1995 21:52:20   DALLEE
:  Update for PROJVERS.
:  
:     Rev 1.1   07 Jun 1995 12:32:50   BARRY
:  Kludge creation of nethelp.h
:  
:     Rev 1.0   27 Apr 1994 14:51:24   DALLEE
:  Initial revision.
:*************************************************************************

: Kludge -- Many make files need NETHELP.H but we're not making it anymore
:           Fixed it this way to get build 54 going but need to fix all
:           MAK files to not require this anymore.

if exist navhelp.h copy navhelp.h nethelp.h

if exist *.*    copy *.*    %PROJ%\LIB%PROJVERS%%PLATFORM%\%TARGET%\*.*

if exist *.h    copy *.h    %PROJPRE%\INCLUDE\SRC%PROJVERS%\*.*
if exist *.hlp  copy *.hlp  %PROJPRE%\BIN%PROJVERS%%PLATFORM%\*.*

