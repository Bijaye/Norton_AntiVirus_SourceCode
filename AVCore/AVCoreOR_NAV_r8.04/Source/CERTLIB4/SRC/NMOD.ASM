include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NUM4_MODULUS                         12/31/92                        CAG
;
;       Return the MODULUS of a number.
;
; DWORD WINAPI NUM4_MODULUS(DWORD Number,DWORD Divider);
;
; Input:
;       1)  Number - the large number to get modulus of
;       2)  Divider - small number to use to get modulus
; Output:
;       1)  UINT = Number % Divider
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\nmod.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:06:02   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:15:06   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:24:06   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.1.0   03 Sep 1993 19:27:14   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   26 Aug 1993 18:55:36   DALLEE
;  Check for divide by zero.
;  
;     Rev 1.1   18 Apr 1993 16:49:02   DALLEE
;  Variable names Divider and Number were switched.
;-----------------------------------------------------------------------------
NUM4_MODULUS PROC USES XBX,
        Number:DWORD,
        Divider:DWORD

        mov     ax,word ptr Divider     ; Check divide by 0
        or      ax,word ptr Divider+2
        jnz     J_Start

        mov     dx,ax                   ; Zero DX:AX
        jmp     J_5

J_Start:
        mov     ax,word ptr Divider+2
        or      ax,ax
        jnz     J_1
        mov     cx,word ptr Divider
        mov     ax,word ptr Number+2
        xor     dx,dx
        div     cx
        mov     ax,word ptr Number
        div     cx
        mov     ax,dx
        xor     dx,dx
        jmp     J_5
J_1:
        mov     cx,ax
        mov     bx,word ptr Divider
        mov     dx,word ptr Number+2
        mov     ax,word ptr Number
J_2:
        shr     cx,1
        rcr     bx,1
        shr     dx,1
        rcr     ax,1
        or      cx,cx
        jnz     J_2
        div     bx
        mov     cx,ax
        mul     word ptr Divider+2
        xchg    ax,cx
        mul     word ptr Divider
        add     dx,cx
        jc      J_3
        cmp     dx,word ptr Number+2
        ja      J_3
        jc      J_4
        cmp     ax,word ptr Number
        jbe     J_4
J_3:
        sub     ax,word ptr Divider
        sbb     dx,word ptr Divider+2

J_4:
        sub     ax,word ptr Number
        sbb     dx,word ptr Number+2
        neg     dx
        neg     ax
        sbb     dx,0

J_5:
        ret

NUM4_MODULUS ENDP
;-----------------------------------------------------------------------------
END
