include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_CRC                             12/23/92                        CAG
;
;       Create a CRC for a handle.
;
; UINT WINAPI FIL4_CRC(UINT Handle,WORD PrevCRC,
;                     DWORD NumbBytes,LPSTR Buffer,UINT BuffSize)
;
; Input:
;       1)  Handle of opened file.
;       2)  PrevCRC (Seed)
;       3)  Number of bytes to CRC (-1 for entire file.
;       4)  Buffer to use
;       5)  Size of buffer
; Output:
;       1)  FAILED (0)
;       2)  CRC any other number
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_CRC PROC USES XCX,
        Handle:WORD,
        crc:WORD,
        Count:DWORD,
        Buffer:LPSTR,
        sBuffer:WORD

J_1:
        INVOKE  FIL4_READ,Handle,Buffer,sBuffer
        or      ax,ax
        jz      J_0
        cmp     ax,-1
        je      J_ERROR

        mov     cx,ax                   ; Store bytes read

        INVOKE  MEM4_CRC,crc,Buffer,ax
        mov     crc,ax

        cmp     word ptr Count+2,0
        jnz     J_2

        cmp     word ptr Count,cx
        jle     J_0
J_2:
        sub     word ptr Count,cx
        sbb     word ptr Count+2,0
        jmp     J_1
J_ERROR:
        mov     crc,0
J_0:
        mov     ax,crc

        ret

FIL4_CRC ENDP
;-----------------------------------------------------------------------------
END
