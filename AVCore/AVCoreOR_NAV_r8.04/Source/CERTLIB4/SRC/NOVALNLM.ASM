include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_ALERT_NLM                  9/3/92                        bAd
;
;       Alerts NAV NLM with message.
;
; BYTE WINAPI NOV4_ALERT_NLM(LPSTR lpszNetMessage,LPSTR lpWorkBuffer,
;                            LPSTR lpSmallBuffer);
;
; Input:
;       1)   Network Message
;       2)   Work buffer of at least 210 bytes.
;       3)   Work buffer of at least 150 bytes.
;
; Output:
;       1)   0 on success. (-1) not found.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_ALERT_NLM  PROC USES XCX XSI SES,
        lpszNetMessage:LPSTR,
        lpWorkBuffer:LPSTR,
        lpSmallBuffer:LPSTR

        LOCAL   fStatus:BYTE
        LOCAL   szServer:LPSTR

        mov     fStatus, 0ffh

        mov     ax, word ptr lpSmallBuffer+2
        mov     word ptr szServer+2, ax
        mov     ax, word ptr lpSmallBuffer
        add     ax, 100
        mov     word ptr szServer+2, ax

        ; get file server name table
        mov     ax, 0ef04h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif

        ; ES:SI <- table
        mov     cx, 01h                 ; 8 entries, 48 bytes/entry NULL terminated
J_0:
        cmp     byte ptr XES[XSI], 00h  ; skip empty entry
        jz      Skip

        INVOKE  NOV4_SCAN_BINDERY,0FFFFFFFFh,NAV_NLM,vSES XSI,lpWorkBuffer
        or      al, al
        jnz     Skip                    ; skip if not NLM

        INVOKE  NOV4_NOTIFY_NLM,vSES XSI,lpszNetMessage,lpWorkBuffer,lpSmallBuffer
        mov     fStatus, al
        jmp     WasNotified
Skip:
        add     XSI, 48
        inc     cx
        cmp     cx, 8
        jbe     J_0

WasNotified:
        mov     al, fStatus
        ret

NOV4_ALERT_NLM  ENDP
;-----------------------------------------------------------------------------
END

