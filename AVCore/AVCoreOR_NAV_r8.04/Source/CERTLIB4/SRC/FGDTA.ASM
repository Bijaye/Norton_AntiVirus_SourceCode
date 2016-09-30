include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_GET_DTA                         12/28/92                        CAG
;
;       Get the current DTA and return;
;
; DWORD WINAPI FIL4_GET_DTA(void);
;
; Output:
;       1)  Pointer to DTA.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
FIL4_GET_DTA PROC USES XBX SES

        mov     ah,2Fh                  ; Get Current DTA
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h
ENDIF
        mov     dx,es
        mov     ax,bx
        ret

FIL4_GET_DTA ENDP
;-----------------------------------------------------------------------------
END

