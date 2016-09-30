include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOGICAL_TO_ABSOLUTE             3/24/93                          bAd
;
;       Convert a LOGICAL SECTOR NUMBER to a ABSOLUTE SECTOR NUMBER
;
; DWORD WINAPI DIS4_LOGICAL_TO_ABSOLUTE(DWORD Sector,BYTE byDrive,WORD 
;                                       uSectors,WORD uSides);
;
; Input:
;       1)  Long sector number 
;       2)  Drive (A: = 00, B: = 01, C: = 80,...)
;       2)  Greatest Sector number per side (e.g. 63 if Sectors = 1-63)
;       3)  Greatest Side number (e.g. 8 if Sides = 0-7)
; Output:
;       1)  ERROR (0)
;       2)  HIWORD (DX)         = Cylinder 
;           LOWORD(HIBYTE) (AH) = Side
;           LOWORD(LOBYTE) (AL) = Sector
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Note:
;       ASB = CYL/SEC/HEAD
;       Fixed disks do not start logically numbering sectors until side 1 of
;       cylinder 0, while floppy drives start logically numbering immediately
;       at side 0 of cylinder 0.
;
;-----------------------------------------------------------------------------
DIS4_LOGICAL_TO_ABSOLUTE PROC USES XBX,
        dwSector:DWORD,
        byDrive:BYTE,
        uGreatestSector:WORD,
        uGreatestSide:WORD

        LOCAL   dwStatus:DWORD

        mov     word ptr dwStatus+2,0
        mov     word ptr dwStatus,0

;------------------------------------------
; Since we use the whole number and the
; remainder generated from (LSN / GREATEST_
; SECTOR), store those calculations for use
; in all our equations.
;
; WHOLE     (AX) = LSN  /  GREATEST_SECTOR
; REMAINDER (DX) = LSN mod GREATEST_SECTOR
;------------------------------------------
        mov     ax, word ptr dwSector
        mov     dx, word ptr dwSector + 2
        mov     bx, uGreatestSector
        div     bx

;** FIXED DRIVE:  & FLOPPY DRIVE: ********
;------------------------------------------
; SECTOR = (LSN mod GREATEST_SECTOR) + 1         
;------------------------------------------
        inc     dx
        mov     byte ptr dwStatus + 2, dl

;*********** FIXED DRIVE: ***************
;------------------------------------------
; CYLINDER = (LSN / GREATEST_SECTOR) + 1
; _______________________________________
;           GREATEST_SIDE
;
;------------------------------------------
;*********** FLOPPY DRIVE: ***************
;------------------------------------------
; CYLINDER = (LSN / GREATEST_SECTOR)
; _______________________________________
;           GREATEST_SIDE
;
;------------------------------------------
        test    byDrive, 80h
        jz      J_IsFloppy
        inc     ax
J_IsFloppy:
        mov     bx, uGreatestSide
        xor     dx, dx
        div     bx
        mov     word ptr dwStatus, ax

;*********** FIXED DRIVE: ***************
;------------------------------------------
; SIDE = (LSN / GREATEST_SECTOR) + 1) 
;                mod 
;         GREATEST_SIDE
;------------------------------------------
;*********** FLOPPY DRIVE: ***************
;------------------------------------------
; SIDE = (LSN / GREATEST_SECTOR) 
;                mod 
;         GREATEST_SIDE
;------------------------------------------
        mov     byte ptr dwStatus + 3, dl

        mov     ax, word ptr dwStatus+2
        mov     dx, word ptr dwStatus

        jmp     J_Done

J_ERROR:
        xor     ax,ax
        xor     dx,dx
J_Done:
        ret

DIS4_LOGICAL_TO_ABSOLUTE ENDP
;-----------------------------------------------------------------------------
END
