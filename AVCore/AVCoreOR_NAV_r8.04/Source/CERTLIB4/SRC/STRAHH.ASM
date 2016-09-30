include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_ASCIIHEX_TO_HEX                 1/8/93                          CAG
;
;       Convert an ASCII character string to a HEX string.
;
; UINT WINAPI STR4_ASCIIHEX_TO_HEX(LPSTR lpFrom, LPSTR lpTo);
;
; Input:
;       1)  lpFrom this string
;       2)  lpTo this string
; Output:
;       1)  Length of new string (0 if out of balance)
;       2)  lpTo filled in
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       Converts "12 34 56 78 AB CD EF" or
;                "12345678ABCDEF"
;       to       0x12345678ABCDEF
;
;       Balanced   = "11 22 33", length will equal 3
;       Unbalanced = "11 22 3",  length will equal 0
;
;-----------------------------------------------------------------------------
STR4_ASCIIHEX_TO_HEX PROC USES XCX XDX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR


        LDS_SI  lpFrom
        LES_DI  lpTo
        cld

        xor     dx,dx
        xor     cx,cx
MoreToString:
        cmp     [XSI],byte ptr 0        ; End of line
        je      EndOfLine
        cmp     [XSI+1],byte ptr 0
        je      EndOfLine1

        cmp     [XSI],byte ptr ' '
        je      HasSpace1
        cmp     [XSI+1],byte ptr ' '
        jne     NoSpace

        lodsb
        xor     ah,ah
        mov     cx,-1
        jmp     Continue
HasSpace1:
        inc     XSI
        jmp     MoreToString

NoSpace:
        lodsw
Continue:
        cmp     ah,'A'
        jle     NotLetter1
        sub     ah,'A'
        add     ah,0Ah
        jmp     Got1
NotLetter1:
        sub     ah,30h
Got1:
        cmp     al,'A'
        jle     NotLetter2
        sub     al,'A'
        add     al,'A'
        jmp     Got2
NotLetter2:
        sub     al,30h
Got2:
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        inc     dx
        stosb
        jmp     MoreToString
EndOfLine1:
        xor     dx,dx

EndOfLine:
        cmp     cx,-1
        jne     Done
        xor     dx,dx
Done:
        xor     al,al
        stosb                           ; Put NULL at the end
        mov     ax,dx

        ret


STR4_ASCIIHEX_TO_HEX ENDP
;-----------------------------------------------------------------------------
END
