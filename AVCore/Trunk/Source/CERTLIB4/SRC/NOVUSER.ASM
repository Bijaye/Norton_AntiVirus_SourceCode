include CTS4.INC

ifdef WIN16
.data
PUBLIC dwBuffer
dwBuffer        dword   0
endif

UniqueSegment
ifdef WIN16
;-----------------------------------------------------------------------------
; DPMI21h
;
;       Process real mode interrupt thru the DPMI.
;
; Input:
;       1)  Registers to be passed to DPMI.
; Output:
;       2)  Registers and flag from DPMI.
;-----------------------------------------------------------------------------
DPMI21h PROC FAR

        LOCAL   stRealMode:REALMODE
        LOCAL   wTempDI:WORD
        LOCAL   wTempES:WORD

        mov     wTempDI,di
        mov     wTempES,es

        lea     XDI,stRealMode

        mov     ss:[XDI].REALMODE.RM_AX,ax
        mov     ss:[XDI].REALMODE.RM_BX,bx
        mov     ss:[XDI].REALMODE.RM_CX,cx
        mov     ss:[XDI].REALMODE.RM_DX,dx

        mov     ss:[XDI].REALMODE.RM_SS,0
        mov     ss:[XDI].REALMODE.RM_SP,0
        mov     ss:[XDI].REALMODE.RM_BP,bp

        mov     ss:[XDI].REALMODE.RM_FL,0

        mov     ss:[XDI].REALMODE.RM_SI,si

        push    ds
        mov     ax,@data
        mov     ds,ax
        mov     ax,word ptr dwBuffer+2
        mov     ss:[XDI].REALMODE.RM_DS,ax
        mov     ax,word ptr dwBuffer+2
        mov     ss:[XDI].REALMODE.RM_ES,ax
        pop     ds

        mov     ax,wTempDI
        mov     ss:[XDI].REALMODE.RM_DI,ax

        mov     ax,ss
        mov     es,ax                   ; ES:DI = Real Mode Structure
                
        mov     ax,0300h
        mov     bx,0021h
        xor     cx,cx
        int     31h

        mov     ax,ss:[XDI].REALMODE.RM_FL
        push    ax
        popf

;        mov     bx,ss:[XDI].REALMODE.RM_BX

; Since NONE of the NOVELL functions uses the return value
; for BX, we return the segment of ES in it this is only
; used by one function:  NOV4_GETSERVER

        mov     bx,ss:[XDI].REALMODE.RM_ES

        mov     ax,ss:[XDI].REALMODE.RM_AX
        mov     cx,ss:[XDI].REALMODE.RM_CX
        mov     dx,ss:[XDI].REALMODE.RM_DX
        mov     si,ss:[XDI].REALMODE.RM_SI
        mov     di,ss:[XDI].REALMODE.RM_DI

        ret

DPMI21h ENDP
;-----------------------------------------------------------------------------
endif
;-----------------------------------------------------------------------------
; NOV4_USER                            3/12/93                         CAG
;
;       Get the USER that is connected to this machine.
;
; BOOL WINAPI NOV4_USER(LPSTR lpBuffer);
;
; Input:
;       1)  lpBuffer to store name, 71 bytes used  (Used to be 64)
; Output:
;       1)  TRUE, Got name
;       2)  FALSE, Invalid name
;
; LIBRARY:DOS16.LIB	
; LIBRARY:DOS32.LIB	
; LIBRARY:WIN16.LIB	
; LIBRARY:WIN32.LIB	
;
;-----------------------------------------------------------------------------
NOV4_USER PROC USES XCX SDS SES XSI XDI,
        lpBuffer:LPSTR

;        mov     ah,0DCh                 ; Get Connection Number
;ifndef WIN16
;        int     21h
;else
;        INVOKE  DPMI21h
;endif
        INVOKE  NOV4_GETCONNID          ; Support has been added for 1000 usr

        or      ax,ax                   ; No ID means no connection
        jz      IsError

        or      ah,ah                   ; Test for 1000 user.
        jz      NotThousand

        LDS_SI  lpBuffer                ; Start of 1000 user code
        mov     byte ptr [XSI],7        ; Length of REQUEST buffer
        mov     byte ptr [XSI+1],0      ; Make length 07 for 1000 user
        mov     byte ptr [XSI+2],1Ch    ; Get ConnInfo for 1000 user
        mov     word ptr [XSI+3],ax     ; Of user to get
        mov     word ptr [XSI+5],0      ; Zero top word of DWORD ConnID

        LES_DI  lpBuffer
        add     XDI,7                   ; Move to the end of this buffer
        mov     byte ptr XES[XDI],63    ; Length of REPLY buffer 
        mov     byte ptr XES[XDI+1],0   ; Make length 063
        mov     ah,0E3h                 ; Get Connection Information
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        or      al,al
        jnz     IsError

        LDS_SI  lpBuffer
        LES_DI  lpBuffer
        add     XSI,15                  ; Offset to name
        mov     XCX,48                  ; But not more then 48 bytes
        jmp     MoreToName              ; Copy the name back into the buffer

NotThousand:
        LDS_SI  lpBuffer
        mov     byte ptr [XSI],4        ; Length of REQUEST buffer
        mov     byte ptr [XSI+1],0      ; Make length 04
        mov     byte ptr [XSI+2],16h    ; Get Connection Number ID
        mov     byte ptr [XSI+3],al     ; Of user to get

        LES_DI  lpBuffer
        add     XDI,4
        mov     byte ptr XES[XDI],63    ; Length of REPLY buffer
        mov     byte ptr XES[XDI+1],0   ; Make length 063
        mov     ah,0E3h                 ; Get Connection Information
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        or      al,al
        jnz     IsError

        LDS_SI  lpBuffer
        LES_DI  lpBuffer
        add     XSI,12                  ; Offset to name
        mov     XCX,48                  ; But not more then 48 bytes
MoreToName:
        lodsb
        stosb
        or      al,al
        loopne  MoreToName
        mov     ax,TRUE
        jmp     Done

IsError:
        mov     ax,FALSE

Done:

        ret

NOV4_USER ENDP
;-----------------------------------------------------------------------------
END
