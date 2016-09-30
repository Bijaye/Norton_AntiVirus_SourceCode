include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_CHANGE_DTA                      12/28/92                        CAG
;
;       Change the DTA and return pointer to Old DTA.
;
; DWORD WINAPI FIL4_CHANGE_DTA(LPSTR NewDTA);
;
; Input:
;       1)  Address to change DTA to
; Output:
;       2)  Original DTA before change.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_CHANGE_DTA PROC USES XBX SDS SES,
        NewDTA:LPSTR

        mov     ah,2Fh                  ; Get Current DTA
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF

        LDS_DX  NewDTA                  ; Set to this DTA
        mov     ah,1Ah
        int     21h
        mov     dx,es
        mov     ax,bx
        ret

FIL4_CHANGE_DTA ENDP
;-----------------------------------------------------------------------------
END
