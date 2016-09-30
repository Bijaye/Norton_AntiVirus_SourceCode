include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETCONNID                  5/13/93                      bAd
;
;       Gets the connection number of this terminal.                      
;
; WORD WINAPI NOV4_GETCONNID(VOID);
;
; Input:
;       1)  None
; Output:
;       1)  Connection ID
;
; NOVELL 1000 USER SUPPORT ADDED by DSACKING 12/8/94
;-----------------------------------------------------------------------------
NOV4_GETCONNID PROC USES XCX,

        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     cl, 'X'
        mov     ax,0DC00h

ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif

        cmp     ah, 0DCh
        jne     all_is_well
                                        ; Call failed, clear the 'ah'
                                        ; just in case
        xor     ah, ah
        jmp     GREATER_255

    all_is_well:

        cmp     cl, 'X'
        je      GREATER_255
        xor     ah, ah

    GREATER_255:

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si

        ret

NOV4_GETCONNID ENDP
;-----------------------------------------------------------------------------
END

