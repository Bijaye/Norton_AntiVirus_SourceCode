include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_COPY_TO_CHAR                    1/14/93                         CAG
;
;       Copy from a buffer to a buffer, upto, not include character.	
;
; UINT WINAPI MEM4_COPY_TO_CHAR(LPSTR From,LPSTR To,UINT Count,BYTE Char);
;
; Input:
;       1)  From buffer
;       2)  To buffer
;       3)  Max number of bytes to copy
;       4)  Copy till this character found, or count = 0
; Output:
;       1)  Number of characters copied.
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_COPY_TO_CHAR PROC USES XBX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR,
        uCount:UINT,
        byChar:BYTE

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        mov     XCX,uCount
        xor     XBX,XBX

MoreDo:
        lodsb
        cmp     al,byChar
        je      FoundEnd
        inc     XBX
        stosb
        loop    MoreDo
FoundEnd:
        mov     XAX,XBX
        ret

MEM4_COPY_TO_CHAR ENDP
;-----------------------------------------------------------------------------
END
