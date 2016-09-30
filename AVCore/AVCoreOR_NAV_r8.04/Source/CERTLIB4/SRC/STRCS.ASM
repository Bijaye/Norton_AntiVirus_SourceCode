include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_COUNT_STRINGS                   2/10/93                         CAG
;
;       Count the number of strings in a string table.
;
; UINT WINAPI STR4_COUNT_STRINGS(LPSTR lpStringTable);
;
; Input:
;       1)  String table - String,NULL,String,NULL,String,NULL,NULL
; Output:
;       1)  Number of strings found in table
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_COUNT_STRINGS PROC USES SDS XSI,
        lpStringTable:LPSTR


        cld
        LDS_SI  lpStringTable
        xor     XAX,XAX

        cmp     [XSI],byte ptr 0
        je      IsDone
MoreCount:
        cmp     [XSI],byte ptr 0
        je      IsCount
        inc     XSI
        jmp     MoreCount
IsCount:
        inc     XAX
        inc     XSI
        cmp     [XSI],byte ptr 0
        jne     MoreCount
IsDone:
        ret

STR4_COUNT_STRINGS ENDP
;-----------------------------------------------------------------------------
END
