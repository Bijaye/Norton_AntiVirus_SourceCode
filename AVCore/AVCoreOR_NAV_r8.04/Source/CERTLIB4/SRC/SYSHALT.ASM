include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_HALT                            1/20/93                         CAG
;
;       Halt CPU processor.
;
; VOID WINAPI SYS4_HALT(VOID);
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_HALT PROC

        mov     ah,0Dh                  ; Flush buffers before we halt
        int     21h

        cli
;        hlt
; Not allowed in Windows or DOS box.

J_0:
        jmp     J_0


SYS4_HALT ENDP
;-----------------------------------------------------------------------------
END

