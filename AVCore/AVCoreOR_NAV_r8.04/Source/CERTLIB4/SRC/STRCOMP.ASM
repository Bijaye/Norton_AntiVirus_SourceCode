include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_COMPARE                         1/22/93                         CAG
;
;       Compare two string for an exact match.
;
; BOOL WINAPI STR4_COMPARE(LPSTR String1, LPSTR String2);
;
; Input:
;       1)  String to compare
;       2)  String to compare to
; Output:
;       1)  TRUE (1)  Match
;       2)  FALSE (0) Not match
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_COMPARE PROC USES SDS SES XSI XDI,
        lpString1:LPSTR,
        lpString2:LPSTR

        cld
        xor     ax,ax
        LDS_SI  lpString1
        LES_DI  lpString2

more_test_compair1055:
        cmpsb
        jne     didnotmatch
        cmp     [XSI-1],byte ptr 0
        jnz     more_test_compair1055
        mov     ax,TRUE

didnotmatch:

        ret

STR4_COMPARE ENDP
;-----------------------------------------------------------------------------
END
