include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_QEMM_INSTALLED                    9/6/93                        bAd
;
;       Returns 1 if QEMM is installed.
;
; BYTE WINAPI SYS4_QEMM_INSTALLED(VOID);
;
; Input:
;       1) NONE
;
; Output:
;       1) LOBYTE - 0 Not installed
;       2) LOBYTE - 1 Installed, HIBYTE = ID of QEMM
;
; LIBRARY:{INTEL}.LIB
;
; NOTE: some/most/none of this code taken from WRSCQEMM.ASM.
;
;-----------------------------------------------------------------------------
SYS4_QEMM_INSTALLED PROC USES XBX XCX XDX XSI XDI SDS SES

        mov     ax, 0d200h              ; first possible QEMM ID
FindLoop:
        mov     bx, 'QD'
        mov     cx, 'ME'
        mov     dx, 'M0'
        push    ax
        int     2fh
        mov     XDI, ax
        pop     ax
        and     XDI, 00ffh
        jz      NotFound
        cmp     XDI, 00ffh
        jne     TryNext
        cmp     bx, 'ME'
        jne     TryNext
        cmp     cx, 'MD'
        jne     TryNext
        cmp     dx, 'VR'
        je      FoundIt

TryNext:	
        inc     ah
        jnz     CheckForWrap
        mov     ah, 0c0h

CheckForWrap:	
        cmp     ah, 0d2h
        jne     FindLoop

NotFound:	
        xor     ax,ax
        jmp     Done

FoundIt:
        mov     al, 01h
Done:
        ret

SYS4_QEMM_INSTALLED ENDP
;----------------------------------------------------------------------------
END
