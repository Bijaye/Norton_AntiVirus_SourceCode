include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_ADD_LIST                        2/16/93                         CAG
;
;       Add a string to a string table.
;
; VOID WINAPI STR4_ADD_LIST(LPSTR lpString, LPSTR lpTable);
;
; Input:
;       1)  lpString to be added to table
;       2)  lpTable to be added to
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_ADD_LIST PROC USES SDS XSI,
        lpString:LPSTR,
        lpTable:LPSTR

        LDS_SI  lpTable

        cmp     [XSI],byte ptr 0
        je      FoundEnd2
MoreTable:
        cmp     [XSI],word ptr 0        ; End of table
        je      FoundEnd
        inc     XSI
        jmp     MoreTable
FoundEnd:
        inc     XSI
FoundEnd2:
        INVOKE  STR4_COPY,lpString,vSDS XSI
        INVOKE  STR4_SRCH_END,vSDS XSI
        mov     XSI,XAX
        mov     [XSI],word ptr 0
        ret

STR4_ADD_LIST ENDP
;-----------------------------------------------------------------------------
END
