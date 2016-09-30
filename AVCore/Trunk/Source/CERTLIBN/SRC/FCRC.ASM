include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/FCRC.ASv   1.6   08 Mar 1996 14:56:50   DALLEE  $
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/FCRC.ASv  $
;  
;     Rev 1.6   08 Mar 1996 14:56:50   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;-----------------------------------------------------------------------------
; FIL4_CRC                             12/23/92                        CAG
;
;       Create a CRC for a handle.
;
; WORD WINAPI FIL4_CRC(UINT Handle,WORD PrevCRC,
;                      DWORD NumbBytes,LPBYTE Buffer,WORD BuffSize)
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
        Handle:UINT,
        crc:WORD,
        Count:DWORD,
        Buffer:LPBYTE,
        sBuffer:WORD


J_1:
        movuw   XAX,sBuffer
        INVOKE  FIL4_READ,Handle,Buffer,XAX
        or      XAX,XAX
        jz      J_0
        cmp     XAX,-1
        je      J_ERROR

        mov     XCX,XAX                   ; Store bytes read

        INVOKE  MEM4_CRC,crc,Buffer,XAX
        mov     crc,ax

IF OS_16
        cmp     word ptr Count+2,0
        jnz     J_2

        cmp     word ptr Count,cx
        jle     J_0
J_2:
        sub     word ptr Count,cx
        sbb     word ptr Count+2,0
ELSE
        cmp     Count,ecx
        jle     J_0

        sub     Count,ecx
ENDIF

        jmp     J_1

J_ERROR:
        mov     crc,0
J_0:
        mov     ax,crc

        ret

FIL4_CRC ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
