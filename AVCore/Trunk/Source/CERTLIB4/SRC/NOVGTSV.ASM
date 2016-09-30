include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETSERVER                  5/12/93                      bAd
;
;       Gets the server offset in server table.
;
; BYTE WINAPI NOV4_GETSERVER(LPSTR szServer);
;
; Input:
;       1)  Server to get index.
; Output:
;       1)  Server ID
;
;-----------------------------------------------------------------------------
NOV4_GETSERVER PROC USES XCX XSI SES,
        szServer:LPSTR

        mov     ax, 0ef04h              ; get server names table
ifndef WIN16
        int     21h                     ; ES:SI <- server table
else
        INVOKE  DPMI21h
        INVOKE  MEM4_SEGTOSEL,bx
        mov     es,ax
endif
           
        mov     cx, 01h                 ; 8 entries, 48 bytes/entry
J_0:
        INVOKE  STR4_COMPARE,szServer,vSES XSI
        or      ax, ax
        jnz     FoundIt
        add     XSI, 48
        inc     cx                      ; move to next entry
        cmp     cx, 8   
        jbe     J_0
        jmp     NotFound

FoundIt:
        mov     ax, cx
        jmp     Done

NotFound:
        mov     ax, -1
Done:
        ret

NOV4_GETSERVER ENDP
;-----------------------------------------------------------------------------
END

