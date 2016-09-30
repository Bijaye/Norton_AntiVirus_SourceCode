include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_VECTOR                          1/12/93                         CAG
;
;       Change the vector requested to a new location.
;
; VOID WINAPI SYS4_VECTOR(BYTE Vector,LPSTR SetTo, LPSTR StoreHere);
;
; Input:
;       1)  Vector number to change
;       2)  Address of new handler.
;       3)  Address to store old value at.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_VECTOR PROC USES XDX XBX SDS SES XSI,
        byVector:BYTE,
        lpHandler:LPSTR,
        lpStore:LPSTR

        mov     ah,35h
        mov     al,byVector

IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h                     ; Get the current vector
ENDIF

        LDS_SI  lpStore
        mov     [XSI],bx
        mov     [XSI+2],es

        mov     ah,25h
        mov     al,byVector
        LDS_DX  lpHandler

IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

        ret

SYS4_VECTOR ENDP
;-----------------------------------------------------------------------------
END
