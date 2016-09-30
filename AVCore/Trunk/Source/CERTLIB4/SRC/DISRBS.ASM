include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_READ_BOOTSECTOR                  3/2/93                          bAd
;
;       Reads the boot sector and stores in buffer.
;
; WORD WINAPI DIS4_READ_BOOTSECTOR(BYTE Drive,LPSTR Buffer);
;
; Input:
;       1)  Drive to read boot sector
;       2)  Pointer to buffer
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_READ_BOOTSECTOR PROC USES XDX,
        Drive:BYTE,
        Buffer:LPSTR

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

;--------------------------
; find the location of bootsector
;
        INVOKE  DIS4_LOC_BOOTSECTOR,Drive,Buffer
        cmp     ax,-1
        jz      Err
        mov     Cyl,dx
        mov     Side,ah
        mov     Sector,al

;--------------------------
; read the bootsector
;
        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,Cyl,Sector,Side,Drive
        or      ax,ax
        jnz     Err
        jmp     Done
Err:    
        mov     ax,-1
Done:        
        ret

DIS4_READ_BOOTSECTOR ENDP
;-----------------------------------------------------------------------------
END
