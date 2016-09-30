include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_COPY                            1/14/93                         CAG
;
;       Copy a memory block.
;
; VOID WINAPI MEM4_COPY(LPSTR From,LPSTR To,UINT Count);	
;
; Input:
;       1)  From Buffer
;       2)  To Buffer
;       3)  Number of bytes
;
; LIBRARY: {INTEL}.LIB
;       
;-----------------------------------------------------------------------------
MEM4_COPY PROC USES XCX SDS SES XSI XDI,
        lpFrom:LPSTR,
        lpTo:LPSTR,
        uCount:UINT

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        mov     XCX,uCount
        rep     movsb

        ret

MEM4_COPY ENDP
;-----------------------------------------------------------------------------
END
