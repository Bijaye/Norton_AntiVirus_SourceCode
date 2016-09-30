include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_UPPER                           1/2/93                          CAG
;
;       Convert a ASCIIZ string to uppercase.
;
; VOID WINAPI STR4_UPPER (LPSTR);
;
; Input:
;       1)  ASCIIZ string to be converted to uppercase.
; Output:
;       2)  String converted.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_UPPER PROC USES SDS XSI,
        MyString:LPSTR

        LOCAL   MyChar:BYTE
        
        LDS_SI  MyString
        cld

more_upper:
        lodsb                           ; Load AL from DS:SI, INC SI
        cmp     al,0
        je      upper_done

        mov     MyChar, al
;        cmp     al,'z'
;        ja      no_upper1
;        cmp     al,'a'
;        jb      no_upper1

        INVOKE  CHR4_TO_ALT_CASE, MyChar, CHR_UPPER
;        sub     al,20h
        mov     [XSI-1],al
no_upper1:
        jmp     more_upper
upper_done:

        ret

STR4_UPPER ENDP
;-----------------------------------------------------------------------------
END
