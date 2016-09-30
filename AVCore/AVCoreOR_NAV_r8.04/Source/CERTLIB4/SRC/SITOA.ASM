include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_INT_TO_ASCII                    1/2/93                          CAG
;
;       Convert a INT to an ASCIIZ string.
;
; VOID WINAPI STR4_INT_TO_ASCII(WORD,LPSTR);
;
; Input:
;       1)  INT to convert (SIGNED)
;       2)  Where to store
; Output:
;       1)  LPSTR Filled
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_INT_TO_ASCII PROC USES SDS XSI,
        MyWord:WORD,
        Storage:LPSTR

        LDS_SI  Storage

        mov     ax,MyWord
        or      ax, ax
        jns     J_0
        mov     byte ptr [XSI], '-'
        inc     XSI
        neg     ax
J_0:
        INVOKE  STR4_WORD_TO_ASCII,ax,vSDS XSI

        ret

STR4_INT_TO_ASCII ENDP
;-----------------------------------------------------------------------------
END

