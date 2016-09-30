include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_INIT_IPX             9/2/92                        bAd
;
;       Gets the address of the IPX routine to call.
;
; DWORD WINAPI NOV4_INIT_IPX(VOID);
;
; Input:
;       1)  None
;
; Output:
;       1)  0 HIWORD on failure.
;       2)  DWORD - address of IPX routine
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_INIT_IPX  PROC USES XDI SES,

        mov     ax, 7a00h
        int     2fh
        cmp     al, 0ffh
        jnz     NoIPX
        mov     ax, XDI
        mov     dx, SES
        jmp     Done

NoIPX:  
        xor     dx, dx
Done:
        ret

NOV4_INIT_IPX  ENDP
;-----------------------------------------------------------------------------
END
