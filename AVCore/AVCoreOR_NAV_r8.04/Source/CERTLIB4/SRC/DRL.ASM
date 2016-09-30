include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_READ_LOGICAL                    1/7/93                          CAG
;
;       Read a logical sector from the drive.
;
; UINT WINAPI DIS4_READ_LOGICAL(LPSTR Buffer,BYTE Drive,
;               DWORD Sector,WORD Count);
;
; Input:
;       1)  Buffer to write to, must be allocated in DOS memory
;               and passed in as SEGMENT:OFFSET
;       2)  Drive to read from 0=A, 1=B, etc
;       3)  Sector to read, DWORD for large media.
;       4)  Number of sectors to read.
; Output:
;       1)  PASSED (0)
;       2)  DOS Error number (?)
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
DIS4_READ_LOGICAL PROC USES XCX ,
        lpBuffer:LPSTR,
        byDrive:BYTE,
        uSector:DWORD,
        uCount:WORD

        mov     al,byDrive
        mov     cx,uCount
        INVOKE  DIS4_INT25,lpBuffer,uSector
        jc      J_Error
        xor     ax,ax
J_Error:
        ret

DIS4_READ_LOGICAL ENDP
;-----------------------------------------------------------------------------
END
