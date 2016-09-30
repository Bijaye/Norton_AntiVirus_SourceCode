include platform.inc
include CTSN.INC


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
; LIBRARY: WIN16.LIB
;          WIN32.LIB
;
; NOTE:  TSR_REGS.r_ES MUST be a segment value in DOS.
;
;-----------------------------------------------------------------------------
DIS4_INT13 PROC USES XBX XCX XDX SDS SES XSI XDI,
        RegsRegs:LPSTR

if bSYM_DOS

IFNDEF SYM_DOSX

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

ELSE

        bSYM_WIN16 = TRUE

ENDIF

endif
;-----------------------------------------------------
if bSYM_WIN16

        LOCAL   stRealMode:REALMODE
        LOCAL   uSelector:WORD

IFDEF MIRRORS
; &? OS/2 Specific code to be inserted here
ELSE
        LDS_SI  RegsRegs

        mov     bx,[XSI].TSR_REGS.r_BX
        mov     dx,[XSI].TSR_REGS.r_DX
        mov     cx,[XSI].TSR_REGS.r_CX
        mov     ax,[XSI].TSR_REGS.r_AX
        
        push    ss
        pop     es
        lea     XDI,stRealMode

        mov     XES[XDI].REALMODE.RM_SS,0
        mov     XES[XDI].REALMODE.RM_SP,0
        mov     XES[XDI].REALMODE.RM_BP,bp

        mov     XES[XDI].REALMODE.RM_AX,ax
        mov     XES[XDI].REALMODE.RM_BX,bx
        mov     XES[XDI].REALMODE.RM_CX,cx
        mov     XES[XDI].REALMODE.RM_DX,dx

        mov     ax,[XSI].TSR_REGS.r_ES
        mov     XES[XDI].REALMODE.RM_ES,ax

        mov     XES[XDI].REALMODE.RM_FL,0

        mov     ax,0300h
        mov     bx,0013h
        xor     cx,cx
        int     31h

        mov     ax,XES[XDI].REALMODE.RM_FL
        push    ax
        popf
        mov     ax,XES[XDI].REALMODE.RM_AX
        mov     bx,XES[XDI].REALMODE.RM_BX
        mov     cx,XES[XDI].REALMODE.RM_CX
        mov     dx,XES[XDI].REALMODE.RM_DX
        mov     di,XES[XDI].REALMODE.RM_DI

        mov     [XSI].TSR_REGS.r_BX,bx
        mov     [XSI].TSR_REGS.r_DX,dx
        mov     [XSI].TSR_REGS.r_CX,cx
        mov     [XSI].TSR_REGS.r_AX,ax
        mov     [XSI].TSR_REGS.r_DI,di

        jc      IsError
        mov     ax,TRUE
        jmp     IsDone
ENDIF


endif   ;; if SYM_WIN

;---------------------------------------
IsError:
        xor     ax,ax
IsDone:
        ret

DIS4_INT13 ENDP
;-----------------------------------------------------------------------------

UniqueSegmentEnd
END


