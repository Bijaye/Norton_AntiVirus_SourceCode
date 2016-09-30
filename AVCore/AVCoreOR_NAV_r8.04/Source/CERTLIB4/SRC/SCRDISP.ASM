include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SCR4_STRING                          1/26/93                         CAG
;
;       Display string on screen using the BIOS as output.
;
; VOID WINAPI SCR4_STRING(LPSTR String);
;
; Input:
;       1)  String to display, null terminated
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SCR4_STRING PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpString:LPSTR

        INVOKE  SCR4_GETPAGE
        mov     bh,al
        INVOKE  SCR4_GETATTR
        mov     bl,al
        INVOKE  SCR4_GETCURR
        mov     dl,al
        mov     dh,ah
        INVOKE  STR4_LENGTH,lpString
        mov     cx,ax

        push    bp
        LES_DI  lpString
        mov     bp,di
        mov     ax,1301h
        int     10h
        pop     bp

        ret

SCR4_STRING ENDP
;-----------------------------------------------------------------------------
END
