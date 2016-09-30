include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_GETDRIVEFROMNAME                3/1/93                          CAG
;
;       Return the DRIVE letter that is found in a FILENAME.
;       If not there, we'll get current drive.
;
; BYTE WINAPI FIL4_GETDRIVEFROMNAME(LPSTR Filename);
;
; Input:
;       1)  Filename to get drive.
; Output:
;       1)  Byte value of drive (ASCII) 'A' 'B' 'C'
;       2)  ERROR 0
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
; LIBRARY:WIN16.LIB
; LIBRARY:WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_GETDRIVEFROMNAME PROC USES SDS XSI,
        lpFilename:LPSTR

        LDS_SI  lpFilename

        cmp     [XSI+1],byte ptr ':'    ; Drive specifier?
        jne     NoDrive
        mov     al,[XSI]
        and     al,5Fh
        jmp     J_Done
NoDrive:
        mov     ah,19h                  ; Get current drive
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        add     al,'A'
J_Done:
        ret

FIL4_GETDRIVEFROMNAME ENDP
;-----------------------------------------------------------------------------
END
