include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_WRITEL_PHYSICAL                 1/7/93                          CAG
;
;       Write long a PHYSICAL sector to the drive.  (INT 13h)
;
; UINT WINAPI DIS4_WRITEL_PHYSICAL(LPSTR Buffer,BYTE Count,WORD Cylinder,
;                                BYTE Sector,BYTE Head,BYTE Drive);	
;
; Input:
;       1)  Buffer to write sectors from, must be allocated in DOS memory
;       2)  Count, number of sectors to write.
;       3)  Cylinder to write
;       4)  Sector to write
;       5)  Head to write
;       6)  Drive to write
; Output:
;       1)  PASSED (0)
;       2)  ERROR (?) BIOS Error message
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       If Windows is to call this function, the LPSTR pointer must
;       be allocate in the DOS memory, the pointer passed in must
;       also be a SEGMENT.  Then Windows must use the SELECTOR
;       to access the buffer after read.
;
;-----------------------------------------------------------------------------
DIS4_WRITEL_PHYSICAL PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        byCount:BYTE,
        uCylinder:WORD,
        bySector:BYTE,
        byHead:BYTE,
        byDrive:BYTE

        LOCAL   MyRegs:TSR_REGS

        mov     ax,uCylinder
        mov     cl,8
        shl     ax,cl                   ; Place cylinder in high
        mov     cx,ax
        mov     ax,uCylinder
        xor     al,al
        shr     ax,1
        shr     ax,1                    ; Place two bits in low al
        or      al,bySector
        mov     cl,al                   ; CX = Cyl/Sector

        mov     al,byCount              ; Sectors to write
        mov     dh,byHead               ; Head to write to
        mov     dl,byDrive              ; Drive to write to
        mov     ah,0Bh                  ; Do a write long

        mov     MyRegs.r_AX,ax
        mov     MyRegs.r_CX,cx
        mov     MyRegs.r_DX,dx

        mov     ax,word ptr lpBuffer+2
        mov     MyRegs.r_ES,ax
        mov     ax,word ptr lpBuffer
        mov     MyRegs.r_BX,ax

        INVOKE  DIS4_INT13,ADDR MyRegs
        or      ax,ax
        jz      J_Error
        xor     ax,ax
        jmp     IsDone
J_Error:
        mov     ax,MyRegs.r_AX          ; Error in AX
IsDone:
        ret

DIS4_WRITEL_PHYSICAL ENDP
;-----------------------------------------------------------------------------
END

