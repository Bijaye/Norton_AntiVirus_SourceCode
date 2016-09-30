include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_QEMM_VERSION                    9/6/93                        bAd
;
;       Returns version number of QEMM.       
;
; WORD WINAPI SYS4_QEMM_VERSION(VOID);
;
; Input:
;
; Output:
;       1)  (-1) on error, 
;           version on success
;
; LIBRARY:{INTEL}.LIB
;
; NOTE: some/most/none of this code taken from WRSCQEMM.ASM.
;
;-----------------------------------------------------------------------------
SYS4_QEMM_VERSION PROC USES XCX XSI XDI SDS SES

        LOCAL   lpfnQPI:DWORD

        INVOKE  SYS4_FIND_QEMM,ADDR lpfnQPI
        or      ax, ax
        jnz     Err

        mov     ah, 03h
        call    dword ptr lpfnQPI
        mov     ax, bx
        jmp     Done

Err:
        mov     ax, -1
Done:
        ret

SYS4_QEMM_VERSION ENDP
;----------------------------------------------------------------------------
END
