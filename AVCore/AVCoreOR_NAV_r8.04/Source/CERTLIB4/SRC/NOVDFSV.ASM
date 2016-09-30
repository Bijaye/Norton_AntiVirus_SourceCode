include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_DEFAULTSERVER                  5/12/93                      bAd
;
;       Gets the default server.                      
;
; BYTE WINAPI NOV4_DEFAULTSERVER(VOID);
;
; Input:
;       1)  None
; Output:
;       1)  Server ID
;
;-----------------------------------------------------------------------------
NOV4_DEFAULTSERVER PROC,

        mov     ax, 0f002h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        ret

NOV4_DEFAULTSERVER ENDP
;-----------------------------------------------------------------------------
END

