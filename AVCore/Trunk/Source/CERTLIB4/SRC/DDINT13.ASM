include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_INT13                           1/7/93                          CAG
;
;       Low level function that processes an interrupt 13h.
;
; BOOL WINAPI DIS4_INT13(TSR_REGS FAR *Registers);
;
; Input:
;       1)  Registers to be used by INT 13h
; Output:
;       1)  Registers returned in structure.
;       2)  TRUE - Passed 
;       3)  FALSE - Failed, error in TSR_REGS.r_AX
;
; LIBRARY: DOS16.LIB
;          DOS32.LIB
;
;-----------------------------------------------------------------------------
DIS4_INT13 PROC USES XBX XCX XDX SDS SES XDI XSI,
        RegsRegs:LPSTR

        LDS_SI  RegsRegs

        mov     es,[XSI].TSR_REGS.r_ES
        mov     bx,[XSI].TSR_REGS.r_BX
        mov     dx,[XSI].TSR_REGS.r_DX
        mov     cx,[XSI].TSR_REGS.r_CX
        mov     ax,[XSI].TSR_REGS.r_AX
        int     13h

        mov     [XSI].TSR_REGS.r_BX,bx
        mov     [XSI].TSR_REGS.r_DX,dx
        mov     [XSI].TSR_REGS.r_CX,cx
        mov     [XSI].TSR_REGS.r_AX,ax
        mov     [XSI].TSR_REGS.r_DI,di

        jc      IsError
        mov     ax,TRUE
        jmp     IsDone
IsError:
        xor     ax,ax
IsDone:
        ret

DIS4_INT13 ENDP
;-----------------------------------------------------------------------------
END


