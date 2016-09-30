; Copyright 1994 Symantec, Peter Norton Product Group
;****************************************************************************
;
; $Header:   N:/SYMKRNL/VCS/INTL.ASv   1.0   26 Jan 1996 20:23:24   JREARDON  $
;
; Description:
;       This file contains VSYMKRNL's international stuff.
;
;****************************************************************************
; $Log:   N:/SYMKRNL/VCS/INTL.ASv  $
;  
;     Rev 1.0   26 Jan 1996 20:23:24   JREARDON
;  Initial revision.
;  
;     Rev 1.0   09 Dec 1994 17:09:22   BRAD
;  Initial revision.
;  
;     Rev 1.3   25 Oct 1994 14:53:30   RSTANEV
;  AnsiToOem table not needed anymore.  IFSMgr will be used for character
;  set conversions.
;
;     Rev 1.2   30 Sep 1994 08:08:34   RSTANEV
;  Added a lot of AnsiToOem tables.
;
;     Rev 1.1   29 Sep 1994 18:59:44   RSTANEV
;  Added ANSI2OEMTable.
;
;     Rev 1.0   12 Sep 1994 11:52:14   RSTANEV
;  Initial revision.
;
;****************************************************************************

;----------------------------------------------------------------------------
;
; DefaultKeyboardCode should be set to one of the following values:
;
;       1       for English
;
;----------------------------------------------------------------------------
DefaultKeyboardCode = 1

        .XLIST
        INCLUDE platform.inc
        .LIST

        PUBLIC  OEMtoScanDefaultTable

VXD_LOCKED_DATA_SEG

;****************************************************************************
;
; Do not modify the following tables
;
;****************************************************************************

;----------------------------------------------------------------------------
;
; Scan code table.
;
;----------------------------------------------------------------------------
OEMtoScanDefaultTable  LABEL  BYTE

IFE DefaultKeyboardCode - 1
                                        ; English keyboards
  DB  0FFh,01Eh,030h,02Eh,020h,012h,021h,022h
  DB  00Eh,00Fh,01Ch,025h,026h,01Ch,031h,018h
  DB  019h,010h,013h,01Fh,014h,016h,02Fh,011h
  DB  02Dh,015h,02Ch,001h,02Bh,01Bh,0FFh,0FFh
  DB  039h,002h,028h,004h,005h,006h,008h,028h
  DB  00Ah,00Bh,009h,00Dh,033h,00Ch,034h,035h
  DB  00Bh,002h,003h,004h,005h,006h,007h,008h
  DB  009h,00Ah,027h,027h,033h,00Dh,034h,035h
  DB  003h,01Eh,030h,02Eh,020h,012h,021h,022h
  DB  023h,017h,024h,025h,026h,032h,031h,018h
  DB  019h,010h,013h,01Fh,014h,016h,02Fh,011h
  DB  02Dh,015h,02Ch,01Ah,02Bh,01Bh,007h,00Ch
  DB  029h,01Eh,030h,02Eh,020h,012h,021h,022h
  DB  023h,017h,024h,025h,026h,032h,031h,018h
  DB  019h,010h,013h,01Fh,014h,016h,02Fh,011h
  DB  02Dh,015h,02Ch,01Ah,02Bh,01Bh,029h,00Eh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
  DB  0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh
ELSE
  %OUT  OEMtoScanDefaultTable not defined.
  .err
ENDIF

VXD_LOCKED_DATA_ENDS

        END
