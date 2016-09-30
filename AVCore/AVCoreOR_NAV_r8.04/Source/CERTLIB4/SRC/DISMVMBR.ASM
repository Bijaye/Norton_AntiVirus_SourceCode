include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_MOVE_MBR                  3/1/93                          bAd
;
;      Moves master boot record to Cyl, Sec, Side, DestDrive.
;
; DWORD WINAPI DIS4_MOVE_MBR(BYTE Drive,WORD Cyl,BYTE Side,BYTE Sector,BYTE DestDrive,LPSTR Buffer);
;
; Input:
;       1)  Drive with MBR (80h or 81h)
;       2)  dest Cyl 
;       3)  dest Side 
;       4)  dest Sector 
;       5)  dest Drive
;       6)  Work buffer of at least 512 bytes.
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;       3)  Work Buffer will contain the MBR
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_MOVE_MBR PROC USES XBX,
        Drive:BYTE,
        Cyl:WORD,
        Side:BYTE,
        Sector:BYTE,
        DestDrive:BYTE,
        Buffer:LPSTR
     
;--------------------------
; read the MBR
;
        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,0,1,0,Drive
        or      ax,ax
        jnz     Err

;--------------------------
; write it to Cyl,Sid,Sec,Drive
;
        INVOKE  DIS4_WRITE_PHYSICAL,Buffer,1,Cyl,Sector,Side,DestDrive
        or      ax,ax
        jnz     Err
        jmp     Done
Err:
        mov     ax,-1   
Done:
        ret

DIS4_MOVE_MBR   ENDP
;-----------------------------------------------------------------------------
END
