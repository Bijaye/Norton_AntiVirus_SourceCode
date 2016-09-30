include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_LENGTH_TO_LAST           2/5/93                                  CAG
;
;       Return the length of a string searching to the last occurance
;       of a character.
; 
; UINT WINAPI STR4_LENGTH_TO_LAST(LPSTR lpString,BYTE Char);
;
; Input:
;       1)  The string to look for the character
;       2)  The character to look for
; Output:
;       1)  Length of the string, including character.
;       2)  Character not found 0
; 
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_LENGTH_TO_LAST PROC USES XDX SDS XSI,
        lpString:LPSTR,
        byChar:BYTE

        cld
        INVOKE  STR4_LENGTH,lpString

        LDS_SI  lpString
        add     XSI,XAX
        mov     dl,byChar
LookMore:
        cmp     [XSI],dl
        je      GotIt
        cmp     XAX,0
        jz      NoGo
        dec     XAX
        dec     XSI
        jmp     LookMore
GotIt:
NoGo:
        ret

STR4_LENGTH_TO_LAST ENDP
;-----------------------------------------------------------------------------
END
