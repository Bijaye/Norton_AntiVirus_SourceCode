include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_QEMM_COPY_STEALTH_ROM                    9/6/93                        bAd
;
;       Copies QEMM's stealth ROM.
;
; BYTE WINAPI SYS4_QEMM_COPY_STEALTH_ROM(LPSTR lpDest,LPSTR lpSource,
;                                        DWORD cNum);
;
; Input:
;       1)  Memory to copy into.
;       2)  Memory to copy from.
;       3)  Number of bytes to copy.
;
; Output:
;       1)  (-1) error, 0 on success
;
;
; LIBRARY:{INTEL}.LIB
;
; NOTE: some/most/none of this code taken from WRSCQEMM.ASM.
;
;-----------------------------------------------------------------------------
SYS4_QEMM_COPY_STEALTH_ROM PROC USES XCX XSI XDI SDS SES,
        lpDest:LPSTR,
        lpSource:LPSTR,
        cNum:DWORD

        LOCAL   lpfnQPI:DWORD

        INVOKE  SYS4_FIND_QEMM,ADDR lpfnQPI
        or      ax, ax
        jnz     Err

        LDS_SI  lpSource
        LES_DI  lpDest

        .386
        mov     ECX, cNum
        .8086

        mov     ax, 33 * 100h + 0
        call    dword ptr lpfnQPI
        jc      Err
        xor     al, al
        jmp     Done

Err:
        mov     al, -1
Done:
        ret

SYS4_QEMM_COPY_STEALTH_ROM ENDP
;----------------------------------------------------------------------------
END
