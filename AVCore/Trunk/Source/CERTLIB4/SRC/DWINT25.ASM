include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_INT25                           1/7/93                          CAG
;
;       Low level routine that processes an interrupt 25h (READ).
;       This function will fix the stack.
;
; void WINAPI DIS4_INT25(LPSTR Buffer, DWORD Sector);
;
; Input:
;       1)  This function works on register level, the registers
;           must be setup for an interrupt 25h to work.
;       2)  Buffer
;       3)  Sector, DWORD for large media
; Output:
;       What interrupt 25h would give you.
;
; LIBRARY: WIN16.LIB
;          WIN32.LIB
;
;-----------------------------------------------------------------------------
DIS4_INT25 PROC USES XDX SDS,
        lpBuffer:LPSTR,
        uSector:DWORD

        LOCAL stRealMode:REALMODE
        LOCAL uSelector:WORD
        LOCAL Reg_AL:BYTE

IFDEF MIRRORS
; &? OS/2 Specific code to be inserted here
        mov     ax,-1
ELSE
        mov     Reg_AL,al

        push    ax
        push    bx
        push    cx
        push    dx
        mov     bx,word ptr lpBuffer+2
        mov     ax,0002h                ; Convert SEG to SEL
        int     31h
        mov     uSelector,ax
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        jnc     GotSEL
        mov     ax,-1                   ; Bad error
        jmp     J_Error
GotSEL:

        INVOKE  DIS4_SET_IO,lpBuffer,uSector,cx
        cmp     ax,-1
        jne     NoCritical
        stc
        jmp     J_Error
NoCritical:
        or      ax,ax
        jz      NoAdjust
        mov     cx,-1                   ; Do extended read
NoAdjust:

        push    ss
        pop     es
        lea     XDI,stRealMode

        mov     al,Reg_AL

        mov     XES[XDI].REALMODE.RM_SS,0
        mov     XES[XDI].REALMODE.RM_SP,0
        mov     XES[XDI].REALMODE.RM_BP,bp

        mov     XES[XDI].REALMODE.RM_AX,ax
        mov     bx,word ptr lpBuffer
        mov     XES[XDI].REALMODE.RM_BX,bx
        mov     XES[XDI].REALMODE.RM_CX,cx
        mov     XES[XDI].REALMODE.RM_DX,dx

        mov     ax,word ptr lpBuffer+2
        mov     XES[XDI].REALMODE.RM_DS,ax
        mov     XES[XDI].REALMODE.RM_ES,ax
        
        mov     XES[XDI].REALMODE.RM_FL,0

        mov     ax,0300h
        mov     bx,0025h
        xor     cx,cx
        int     31h

        mov     ax,XES[XDI].REALMODE.RM_FL
        push    ax
        popf
        
        mov     ax,XES[XDI].REALMODE.RM_AX
J_Error:

ENDIF

        ret

DIS4_INT25 ENDP
;-----------------------------------------------------------------------------
END
