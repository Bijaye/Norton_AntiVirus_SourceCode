include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_APPEND                          2/7/93                          CAG
;
;       Append two strings into one.
;
; VOID WINAPI STR4_APPEND(LPSTR lpOne,LPSTR lpTwo);
;
; Input:
;       1)  String to have lpTwo appended onto.
;       2)  String to append onto lpOne
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
STR4_APPEND PROC USES SDS XSI,
        lpOne:LPSTR,
        lpTwo:LPSTR

        LDS_SI  lpOne
        INVOKE  STR4_SRCH_END,lpOne
        mov     XSI,XAX
        INVOKE  STR4_COPY,lpTwo,vSDS XSI
        ret

STR4_APPEND ENDP
;-----------------------------------------------------------------------------
END
