include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_LOWER                           1/2/93                          CAG
;
;       Convert a ASCIIZ string to lowercase.
;
; VOID WINAPI STR4_LOWER (LPSTR);
;
; Input:
;       1)  ASCIIZ string to be converted to lowercase.
; Output:
;       2)  String converted.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_LOWER PROC USES SDS XSI,
        MyString:LPSTR

        LOCAL   MyChar:BYTE

        LDS_SI  MyString
        cld

more_LOWER:
        lodsb                           ; Load AL from DS:SI, INC SI
        cmp     al,0
        je      upper_done
        mov     MyChar, al

;        cmp     al,'z'
;        ja      no_LOWER1
;        cmp     al,'a'
;        jb      no_LOWER1
;        add     al,20h
        INVOKE  CHR4_TO_ALT_CASE, MyChar, CHR_LOWER
        mov     [XSI-1],al
no_LOWER1:
        jmp     more_LOWER
upper_done:

        ret

STR4_LOWER ENDP
;-----------------------------------------------------------------------------
END
