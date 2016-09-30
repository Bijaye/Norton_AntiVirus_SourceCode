include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_FIND_QEMM                    9/6/93                        bAd
;
;       Returns pointer to QPI.
;
; BYTE WINAPI SYS4_FIND_QEMM(LPSTR lpfnQPI);
;
; Input:                                
;       1)  Pointer to QEMM structure to fill.
;
; Output:
;       1)  0 on success, -1 if not.
;       2)  Filled structure
;
; LIBRARY:{INTEL}.LIB
;
; NOTE: some/most/none of this code taken from WRSCQEMM.ASM.
;
;-----------------------------------------------------------------------------
SYS4_FIND_QEMM PROC USES XBX XCX XDX XSI XDI SDS SES,
        lpfnQPI:LPSTR

        INVOKE  SYS4_QEMM_INSTALLED
        or      ax,ax
        jz      Error

        mov     al, 001h                ; get driver type
        mov     bx, 'QE'
        mov     cx, 'MM'
        mov     dx, '42'
        int     2fh
        cmp     bx, 'OK'
        je      OK

Error:
        mov     al, -1
        jmp     Done
Ok:     
        LDS_SI  lpfnQPI

        mov     [XSI+2],es
        mov     [XSI],di
        xor     al, al
Done:
        ret

SYS4_FIND_QEMM ENDP
;----------------------------------------------------------------------------
END
