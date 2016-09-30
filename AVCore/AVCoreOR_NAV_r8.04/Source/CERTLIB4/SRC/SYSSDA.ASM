include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_SDA                         1/19/93                         CAG
;
;       Fill the SDA structure needed for TSR operation.
;
; BOOL WINAPI SYS4_GET_SDA(SDA FAR *Structure);
; 
; Input:
;       1)  Pointer to SDA structure
; Output:
;       1)  Structure filled (TRUE(1))
;       2)  ERROR (FALSE(0))
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_GET_SDA    PROC USES XBX XCX XDX SDS SES XSI XDI,
        fpSDA:LPSTR

        LES_DI  fpSDA

        mov     ax,5D06h
        int     21h                     ; Get SDA
        jc      J_Error

        mov     word ptr XES[XDI].SDA.lpFrom+2,ds
        mov     word ptr XES[XDI].SDA.lpFrom,si
        mov     word ptr XES[XDI].SDA.uSmall,dx
        mov     word ptr XES[XDI].SDA.uLarge,cx

        mov     ah,34h
        int     21h                     ; Get InDOS flag
        
        LDS_SI  fpSDA
        mov     word ptr [XSI].SDA.lpINDOS+2,es
        mov     word ptr [XSI].SDA.lpINDOS,bx

        mov     ax,TRUE
        jmp     J_Done
J_Error:
        mov     ax,FALSE
J_Done:
        ret

SYS4_GET_SDA    ENDP
;-----------------------------------------------------------------------------
END
