include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_READ_PHYSICAL                   1/7/93                          CAG
;
;       Read a PHYSICAL sector from a drive.  (INT 13h)
;
; UINT WINAPI DIS4_READ_PHYSICAL(LPSTR Buffer,BYTE Count,WORD Cylinder,
;                                BYTE Sector,BYTE Head,BYTE Drive);	
;
; Input:
;       1)  Buffer to store sectors in, must be allocated in DOS memory.
;       2)  Count, number of sectors to read.
;       3)  Cylinder to read
;       4)  Sector to read
;       5)  Head, to read
;       6)  Drive to read
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
DIS4_READ_PHYSICAL PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        byCount:BYTE,
        uCylinder:WORD,
        bySector:BYTE,
        byHead:BYTE,
        byDrive:BYTE

        LOCAL   byTry:BYTE
        LOCAL   MyRegs:TSR_REGS
;--------------------------------------------------------
        mov     byTry, 03               ;try three times
TryAgain:
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

        mov     al,byCount              ; Sectors to read
        mov     dh,byHead               ; Head to read from
        mov     dl,byDrive              ; Drive to read from
        mov     ah,02h                  ; Do a READ

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
        sub     byTry, +1               ;
        jnz     TryAgain
        mov     ax,MyRegs.r_AX          ; Error in AX
IsDone:
        ret

DIS4_READ_PHYSICAL ENDP
;-----------------------------------------------------------------------------
END
