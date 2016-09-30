include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_SETSERVER                  5/12/93                      bAd
;
;       Sets the server.                      
;
; BYTE WINAPI NOV4_SETSERVER(BYTE bServer);
;
; Input:
;       1)  ID of server to connect to.
; Output:
;       1)  Server ID
;
;-----------------------------------------------------------------------------
NOV4_SETSERVER PROC USES XDX,
        bServer:BYTE

        mov     ax, 0f000h
        mov     dl, bServer
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        ret

NOV4_SETSERVER ENDP
;-----------------------------------------------------------------------------
END

