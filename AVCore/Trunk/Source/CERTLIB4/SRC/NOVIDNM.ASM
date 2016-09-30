include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_IDTONAME                5/20/93                         bAd
;
;   Converts an object ID to a NAME.
;
; UINT WINAPI NOV4_IDTONAME(DWORD dwID,LPSTR lpUserName);
;
; Input:
;       1) Object ID
;       2) returns Object's name 
; Output:
;       1) 0 successful, ?? error
;       2) object's name
;
;-----------------------------------------------------------------------------
NOV4_IDTONAME PROC USES XCX XSI XDI SDS SES,
        dwID:DWORD,
        lpUserName:LPSTR

        LDS_SI  lpUserName
        mov     byte ptr [XSI+0], 07h
        mov     byte ptr [XSI+1], 00h
        mov     byte ptr [XSI+2], 36h
        mov     ax, word ptr dwID+2
        mov     [XSI+3], ax
        mov     ax, word ptr dwID
        mov     [XSI+5], ax

        LES_DI  lpUserName
        add     XDI, 7                  ; size of request buffer is 7 bytes
        mov     word ptr XES[XDI], 56

        mov     ah, 0e3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        or      al, al
        jnz     Done

        LES_DI  lpUserName
        LDS_SI  lpUserName
        add     XSI, 7+8            ; 7 bytes of request buff + 8 bytes into reply
        mov     cx, 48
        cld
        rep     movsb
Done:
        ret

NOV4_IDTONAME ENDP
;-----------------------------------------------------------------------------
END
