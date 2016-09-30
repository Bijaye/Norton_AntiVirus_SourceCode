include CTS4.INC

IFDEF WIN16
extrn DOS3CALL:far
ENDIF

UniqueSegment
;-----------------------------------------------------------------------------
; BAN4_TEST                            1/29/93                         CAG
;
;       Test to see if the BANYAN network is installed.
;
; BOOL WINAPI BAN4_TEST(VOID);
;
; Output:
;       1)  TRUE (1)  BANYAN found in memory.
;       2)  FALSE (0) No BANYAN found.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
BAN4_TEST PROC USES XBX XCX XDX SDS SES XSI XDI

        mov     ax,0D701h
        int     2Fh
        or      ax,ax
        jz      IsBANYAN

; We didn't find it, but may be older version, try the vector approach
        mov     dx,0060h
        mov     cx,6

MoreLookUp:
        mov     ax,dx
        mov     ah,35h
IFDEF WIN16
        call    DOS3CALL
ELSE
        int     21h                     ; Get vector
ENDIF
        ; Make sure that the values we check next are not around bad
        ; segment boundaries...
        cmp     bx,4
        jb      NotThisOne

        cmp     XES[XBX-4],word ptr 'AB'
        jne     NotThisOne
        cmp     XES[XBX-2],word ptr 'VN'
        jne     NotThisOne
        jmp     IsBANYAN
NotThisOne:
        inc     dx
        loop    MoreLookUp

        mov     XAX,FALSE
        jmp     J_Done
IsBANYAN:
        mov     XAX,TRUE
J_Done:
        ret

BAN4_TEST ENDP
;-----------------------------------------------------------------------------
END
