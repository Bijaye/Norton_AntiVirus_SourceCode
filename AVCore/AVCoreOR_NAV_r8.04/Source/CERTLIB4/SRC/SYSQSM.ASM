include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_QEMM_STEALTH_MODE                    9/6/93                        bAd
;
;       Returns QEMM stealth mode.
;
; BYTE WINAPI SYS4_QEMM_STEALTH_MODE(VOID);
;
; Input:
;
; Output:
;       1)  (-1) Error
;           'M' or 'F'
;
;
; LIBRARY:{INTEL}.LIB
;
; NOTE: some/most/none of this code taken from WRSCQEMM.ASM.
;
;-----------------------------------------------------------------------------
SYS4_QEMM_STEALTH_MODE PROC USES XCX XSI XDI SDS SES,

        LOCAL   lpfnQPI:DWORD

        INVOKE  SYS4_FIND_QEMM,ADDR lpfnQPI
        or      ax, ax
        jnz     Err

        mov     ax, 30 * 100h + 0
        xor     cl, cl
        call    dword ptr lpfnQPI
        mov     al, cl
        xor     ah,ah
        jmp     Done

Err:
        mov     ax, -1
Done:
        ret

SYS4_QEMM_STEALTH_MODE ENDP
;----------------------------------------------------------------------------
END
