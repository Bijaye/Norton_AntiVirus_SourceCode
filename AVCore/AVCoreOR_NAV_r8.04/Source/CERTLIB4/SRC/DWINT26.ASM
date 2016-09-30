include CTS4.INC

GlobalDOSAlloc  PROTO FAR PASCAL,:DWORD
GlobalDOSFree   PROTO FAR PASCAL,:WORD

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_INT26                           1/7/93                          CAG
;
;       Low level routine that processes an interrupt 26h (WRITE).
;       This function will fix the stack.
;
; void WINAPI DIS4_INT26(LPSTR Buffer, DWORD Sector);
;
; Input:
;       1)  This function works on register level, the registers
;           must be setup for an interrupt 26h to work.
;       2)  Buffer
;       3)  Sector, DWORD for large media
; Output:
;       What interrupt 26h would give you.
;       Carry on error and (-1) on error.
;
; LIBRARY: WIN16.LIB
;          WIN32.LIB
;
;-----------------------------------------------------------------------------
DIS4_INT26 PROC USES XDX SDS,
        lpBuffer:LPSTR,
        uSector:DWORD

        LOCAL   uSelector:WORD
        LOCAL   lpExtWrite:LPSTR
        LOCAL   stRealMode:REALMODE
        LOCAL   Reg_AL:BYTE
        LOCAL   Reg_CX:WORD

IFDEF MIRRORS
; &? This requires OS/2 specific code to become functional

        cmp     ax,-1
        stc
ELSE
        mov     Reg_AL, al
        mov     Reg_CX, cx

        INVOKE  GlobalDOSAlloc, 15
        or      ax, ax
        jz      J_AllocError
        mov     uSelector, ax
        mov     word ptr lpExtWrite+2, dx
        mov     word ptr lpExtWrite, 00h
        
        INVOKE  DIS4_SET_IO,lpExtWrite,uSector,Reg_CX
        cmp     ax,-1
        jne     NoCritical
        stc
        jmp     J_Error

NoCritical:
        push    ss
        pop     es
        lea     XDI,stRealMode                  ; ES:DI <- stRealMode

        or      ax,ax
        jz      NoAdjust
        mov     cx,-1         ; Do extended read

        mov     ax,word ptr lpExtWrite+2
        mov     XES[XDI].REALMODE.RM_DS,ax
        mov     ax,word ptr lpExtWrite
        mov     XES[XDI].REALMODE.RM_BX,ax

        push    SDS
        push    XSI
        mov     ax, word ptr lpExtWrite+2
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      J_Error
        mov     SDS, ax
        mov     XSI, word ptr lpExtWrite
        mov     ax, word ptr lpBuffer+2
        mov     word ptr XDS[XSI+8], ax
        mov     ax, word ptr lpBuffer
        mov     word ptr XDS[XSI+6], ax
        pop     XSI
        pop     SDS
        jmp     DoRead

NoAdjust:
        mov     ax,word ptr lpBuffer+2
        mov     XES[XDI].REALMODE.RM_DS,ax
        mov     ax,word ptr lpBuffer
        mov     XES[XDI].REALMODE.RM_BX,ax
DoRead:
        xor     ah, ah
        mov     al,Reg_AL
        mov     XES[XDI].REALMODE.RM_AX,ax
        mov     XES[XDI].REALMODE.RM_SS,0
        mov     XES[XDI].REALMODE.RM_SP,0
        mov     XES[XDI].REALMODE.RM_BP,bp
        mov     XES[XDI].REALMODE.RM_CX,cx
        mov     XES[XDI].REALMODE.RM_DX,dx
        mov     XES[XDI].REALMODE.RM_ES,ax

        mov     XES[XDI].REALMODE.RM_FL,0

        mov     ax,0300h
        mov     bx,0026h
        xor     cx,cx
        int     31h

        mov     ax,XES[XDI].REALMODE.RM_FL
        push    ax
        popf
        
        mov     ax,XES[XDI].REALMODE.RM_AX

J_Error:                                       
        push    ax
        INVOKE  GlobalDOSFree, uSelector
        pop     ax

J_AllocError:
ENDIF
        ret

DIS4_INT26 ENDP
;-----------------------------------------------------------------------------
END            
