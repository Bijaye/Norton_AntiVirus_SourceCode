include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_COUNT_CHAR                      2/10/93                         CAG
;
;       Count the number of occurances of a char in a string.
;
; UINT WINAPI STR4_COUNT_CHAR(LPSTR lpString,BYTE Char);
;
; Input:
;       1)  String to look in, null terminated.
;       2)  Character to look for
; Output:
;       1)  Number of times character found before NULL
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_COUNT_CHAR PROC USES XCX SDS XSI,
        lpString:LPSTR,
        byChar:BYTE

        cld
        LDS_SI  lpString
        mov     al,byChar
        xor     XCX,XCX

MoreString:
        cmp     [XSI],byte ptr 0
        je      EndOfString
        cmp     [XSI],al
        jne     NotMatch
        inc     XCX
NotMatch:
        inc     XSI
        jmp     MoreString

EndOfString:
        mov     XAX,XCX
        ret

STR4_COUNT_CHAR ENDP
;-----------------------------------------------------------------------------
END
