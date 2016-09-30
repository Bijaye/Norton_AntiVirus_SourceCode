;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBOPCRE.ASv   1.3   08 Mar 1996 14:56:34   DALLEE  $
;;
;; Description: Defines DBC4_OPEN_CREATE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBOPCRE.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:34   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   17 Aug 1994 13:25:38   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   30 May 1994 11:09:42   RSTANEV
;  Added file header.
;
;     Rev 1.0   30 May 1994 11:08:00   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_OPEN_CREATE
;
; Open a HASHED database, create if not found.
;
; BOOL WINAPI DBC4_OPEN_CREATE ( LPSTR Filename, DWORD IndexSize,
;                                WORD ElemSize,
;                                FARPROC CompareFunction (void FAR *, void FAR *),
;                                FARPROC HashFunction (void FAR *),
;                                FARPROC QuickFunction (void FAR *),
;                                DBC4_HEADER FAR *Info );
; Input:
;       1)  Filename - Name of database to open.
;       2)  IndexSize - size of its index       - Not used
;       3)  ElemSize - size of a single record  - Not used
;       4)  CompareFunction - Routine that will compare the two items
;                             and return if they are the same.
;       5)  HashFunction - Returns the Hash value of the inputed string.
;                          Example - CRC.
;       6)  QuickFunction - Returns a WORD that will be placed in the
;                           quick look up table.
;       7)  DBC4_HEADER structure for database header.
; Output:
;       1)  BOOL = FALSE(0) Database not found or error.
;                  TRUE(1)  Database found
;----------------------------------------------------------------------------
DBC4_OPEN_CREATE  PROC  CODEDISTANCE  USES SDS XSI,
        Filename:LPSTR,
        IndexSize:DWORD,
        ElemSize:WORD,
        CompareFunction:LPBYTE,
        HashFunction:LPBYTE,
        QuickFunction:LPBYTE,
        Info:LPBYTE

        LOCAL Attrib:UINT

                                        ; Check if exists
        INVOKE  FIL4_ATTR,Filename,Attrib,GET_INFO
        cmp     XAX,-1                  ; -1 is "doesn't exist"
        jne     J_Exists

                                        ; If doesn't exist, create
        INVOKE  DBC4_CREATE,Filename,IndexSize,ElemSize,CompareFunction,
                          HashFunction,QUickFunction,Info
        jmp     SHORT J_Done

J_Exists:
        mov     Attrib,XAX

                                        ; Check if write protected
        INVOKE  FIL4_ATTR,Filename,Attrib,SET_INFO
        inc     XAX
        jz      J_Done                  ; Can't write

                                        ; If not write protected and exists,
                                        ; open for R/W
        INVOKE  DBC4_OPEN,Filename,IndexSize,ElemSize,CompareFunction,
                          HashFunction,QuickFunction,Info,TRUE

J_Done:

                                        ; Reset INT 24 vector, DOS only
IFDEF  DOS16
_CheckErrorFar  PROTO  FAR  C
        push    ax
        INVOKE  _CheckErrorFar
        pop     ax
ENDIF

        ret

DBC4_OPEN_CREATE  ENDP

UniqueSegmentEnd
END
