include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_SYS_NORM                     1/21/93                             JOE
;
;     Does repair on Norm SYS infections (INVOL type).
;
; UINT WINAPI REP4_SYS(WORD HostHandle, WORD uBuffer,LPSTR lpBuffer);
;
; Input:
;       1) Handle
;       2) Buffer, filled with data
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
REP4_SYS_NORM   PROC USES XBX XCX XDX SDS SES XSI XDI,
                HostHandle:WORD,        ; Handle of infected file
                uBuffer:WORD,           ; Size of work buffer
                lpBuffer:LPSTR          ; Ptr to my work buffer
                
                LOCAL dwVSIZE:DWORD

;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpBuffer

        xor     ax,ax
        ret   

REP4_SYS_NORM   ENDP
;--------------------------------------------------------------------------
END

