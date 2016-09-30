include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_READ_ROOT                  3/3/93                          bAd
;       
;       Reads the root directory from the specified drive into buffer.
;
;
; WORD WINAPI DIS4_READ_ROOT(BYTE Drive,LPSTR RootBuffer);
;
; Input:
;       1)  Drive to read root directory from.
;       2)  Buffer to place root directory.
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;       3)  Buffer to place root directory.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_READ_ROOT PROC USES XCX XDX,
        Drive:BYTE,
        Buffer:LPSTR
        
        LOCAL   StartSector:DWORD
        LOCAL   AltDrive:BYTE
        LOCAL   BPB:BPB_struc

;----------------------------
; convert 80h to 2, and 81h
; to 3
;
        mov     al,Drive
        shl     al,1
        add     al,2
        mov     AltDrive,al

;----------------------------
; need the bios parameter block
; to determine root directory
; specs
;
        INVOKE  DIS4_GET_BPB,Drive,addr BPB,Buffer
        cmp     ax,-1
        jz      Err

;----------------------------
; calculate number of sectors 
; to read
;
        xor     dx,dx
        mov     ax,BPB.BytesPerSec
        mov     bx,32
        div     bx
        mov     bx,ax
        mov     ax,BPB.RootDirEnts
        div     bx
        mov     cx,ax

;----------------------------
; find where root directory begins
;
        xor     ah,ah
        mov     al,BPB.FATs
        mul     BPB.FATsecs
        inc     ax
        mov     word ptr StartSector+2,dx
        mov     word ptr StartSector,ax

        INVOKE  DIS4_READ_LOGICAL,Buffer,AltDrive,StartSector,cx
        or      ax,ax
        jz      Done
Err:
        mov     ax,-1
Done:
        ret

DIS4_READ_ROOT   ENDP
;-----------------------------------------------------------------------------
END
