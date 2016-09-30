include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_SRCH_CHAR                       2/7/93                          CAG
;
;       Search for a matching character in a string.
;
; WORD WINAPI STR4_SRCH_CHAR(LPSTR String,BYTE Char);
;
; Input:
;       1)  String to look in
;       2)  Look for this character
; Output:
;       1)  Offset where found
;       2)  Character not found (FALSE 0)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_SRCH_CHAR PROC USES SDS XSI,
        lpString:LPSTR,
        byChar:BYTE

        LDS_SI  lpString

MoreString:
        cmp     [XSI],byte ptr 0        ; Not found
        je      NotFound
        INVOKE  DBCS_DBCSIsLeadByte, [XSI] ; Test if char is a DBCS char
        cmp     XAX,TRUE
        je      @F                      ; If yes, do not check for match
        mov     al,byChar
        cmp     [XSI],al
        je      Found
        inc     XSI
        jmp     MoreString
@@:
        inc     XSI                     ; Advance ptr to next char
        inc     XSI
        jmp     MoreString
Found:
        mov     XAX,XSI
        jmp     J_Done
NotFound:
        mov     XAX,FALSE
J_Done:
        ret

STR4_SRCH_CHAR ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
