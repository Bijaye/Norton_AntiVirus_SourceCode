include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_ASCII_TO_INT                   1/2/93                          CAG
;
;       Convert a ASCIIZ string to a INT.
;
; INT WINAPI STR4_ASCII_TO_INT(LPSTR);
;
; Input:
;       1)  What to convert.
; Output:
;       1)  INT returned. (SIGNED)
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       '1234' is converted to 1234d, 4D2h.
;
;-----------------------------------------------------------------------------
STR4_ASCII_TO_INT PROC USES XBX XCX XDX SDS XSI,
        Storage:LPSTR

        LOCAL Negitive:WORD

        LDS_SI  Storage
        xor    cx, cx               ; sum of integer value
        mov    Negitive,cx          ; neg flag
        mov    bx, 10               ; base 10 conversion
        cld
ati_1:
        lodsb                       ; skip by white space
        cmp    al, ' '              ;  spaces
        je     ati_1
        cmp    al, 9                ;  or tabs
        je     ati_1

        cmp    al, '+'
        je     ati_2
        cmp    al, '-'
        jne    ati_3
        inc    Negitive             ; Negitive <> 0 if minus
ati_2:
        lodsb
ati_3:
        cbw
        sub    al, '0'
        cmp    al, 9
        ja     ati_5
        xchg   ax, cx
        mul    bx
        or     dx, dx
        jnz    ati_4
        add    cx, ax
        jmp    ati_2
ati_4:
        mov    ax, -1               ; overflow error
        jmp    ati_9

ati_5:
        xchg   ax, cx
        cmp    Negitive,0
        jz     ati_6
        neg    ax
ati_6:
        cmp    cl, -48              ; -48 = null found
        je     ati_ret
ati_9:
        stc
ati_ret:
        ret

STR4_ASCII_TO_INT ENDP
;-----------------------------------------------------------------------------
END

