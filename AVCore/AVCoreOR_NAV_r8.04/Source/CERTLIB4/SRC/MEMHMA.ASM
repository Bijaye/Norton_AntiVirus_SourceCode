include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_HMA                             3/12/93                         CAG
;
;       Process the A20 line for access to the HMA
;
; BOOL WINAPI MEM4_HMA(WORD FLAG);
;
; Input:
;       1)  Flag = HMA_STATUS (Get status of HMA)
;                  HMA_ON     (Turn HMA on)
;                  HMA_OFF    (Turn HMA off)
; Output:
;       1)  STATUS
;               0 = OFF
;               1 = ON
;       2)  ON/OFF
;               0 = FALSE (FAIL)
;               1 = TRUE  (SUCCESS)
;
;-----------------------------------------------------------------------------
MEM4_HMA PROC USES XBX SDS SES XSI XDI,
        uFlag:WORD

        cmp     uFlag,HMA_STATUS        ; Get status of HMA line
        je      IsStatus
        cmp     uFlag,HMA_ON            ; Turn HMA on
        je      IsOn
        cmp     uFlag,HMA_OFF           ; Turn HMA off
        je      IsOff

        mov     ax,FALSE
        jmp     IsError

IsStatus:
        mov     ah,MEM_QUERY_A20
        INVOKE  XMS_CALL
        jmp     IsDone                  ; 1 = On, 0 = Off

IsOn:
        mov     ah,MEM_LOCALENABLE_A20
        INVOKE  XMS_CALL
        jmp     IsDone

IsOff:
        mov     ah,MEM_LOCALDISABLE_A20
        INVOKE  XMS_CALL
        jmp     IsDone


IsError:
IsDone:

        ret

MEM4_HMA ENDP
;-----------------------------------------------------------------------------
END


