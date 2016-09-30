include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_CREATE_RECOVER                  3/2/93                          bAd
;
;      Creates a file that contains the master boot record for both hard
;       drives and the 4 boot sectors per MBR.        
;
; WORD WINAPI DIS4_CREATE_RECOVER(LPSTR Filename,LPSTR Buffer);
;
; Input:
;       1)  File where recover info will be stored.
;       2)  Work buffer of at least 1024 bytes.        
;
; Output:
;       1)  (-1) failed
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_CREATE_RECOVER PROC USES XBX XCX XDX XDI SES,
        Filename:LPSTR,
        Buffer:LPSTR

        LOCAL   Handle:UINT
        LOCAL   Drive:BYTE
        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE
        LOCAL   MaxCyl:WORD
        LOCAL   MaxSide:BYTE
        LOCAL   MaxSector:BYTE
        LOCAL   MyRegs:TSR_REGS


;-----------------------------
; create the file
;
        INVOKE  FIL4_OPEN,Filename,fCREATE
        cmp     ax, -1
        jz      Err
        mov     Handle,ax

        mov     Drive,80h                    ; do drives 80h and 81h
;-----------------------------
; copy the master boot record 
; to the file 
;
J_0:
; get max cyl, sector, and size
        mov     dl, Drive
        xor     dh,dh

        mov     MyRegs.r_AX,0800h
        mov     MyRegs.r_DX,dx
        INVOKE  DIS4_INT13,ADDR MyRegs  ; Read Drive Parameters
        or      ax, ax
        jz      Err
        mov     MaxSide, dh             ; side with max sides
        mov     ah, cl                  ; low 5 bits are max sector
        and     ah, 00111111b
        mov     MaxSector, ah
        mov     dx, cx
        xchg    dh, dl                  
        mov     cl, 6
        shr     dh, cl
        mov     MaxCyl, dx        

        INVOKE  DIS4_LOC_HIDDEN_MBR,Drive,Buffer
        cmp     ax, -1
        jz      J_2
        mov     Side, al
        mov     ax, dx
        and     al, 3fh
        mov     Sector, al
        xchg    dh, dl
        mov     cx, 6
        shr     dh, cl
        mov     Cyl, dx

        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,Cyl,Sector,Side,Drive
        or      ax, ax
        jz      J_3
J_2:
        cmp     Drive,81h               ; the drive just doesn't exist
        jz      J_6                     ; that's still OK
        jmp     Err
J_3:
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL,ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax
        mov     XDI, word ptr Buffer
        INVOKE  FIL4_WRITE,Handle,vSES XDI,512      
        cmp     ax,-1
        jz      Err

;-----------------------------
; copy the 4 boot sectors to 
; the file
;
        mov     cx,4
J_4:
        mov     bh,4                   ; move from 0,1,2,3
        sub     bh,cl
        INVOKE  DIS4_LOC_BOOTSECTOR_NUM,Drive,bh,Buffer ; find bootsector for nth partition
        cmp     ax,-1
        jz      Err

        mov     Cyl,dx
        mov     Side,ah
        mov     Sector,al

;-----------------------------
; make sure these are valid
; values
        cmp     Sector, 0               ; sector begins at 1
        jz      InvalidBoot
        mov     al, MaxSector
        cmp     Sector, al
        ja      InvalidBoot
        mov     al, MaxSide
        cmp     Side, al
        ja      InvalidBoot
        mov     ax, MaxCyl
        cmp     Cyl, ax
        jA      InvalidBoot

        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,Cyl,Sector,Side,Drive ; read boot sector
        or      ax,ax
        jnz     Err
        jmp     ValidBootSector
;----------------------------
; this is not a good boot sector
; so we will mark it such.  This way
; it will not be recovered, but will
; still exist in the critical file
;
InvalidBoot:
        push    XDI                     ; null out the buffer
        push    cx
        xor     al, al
        mov     cx, 512
        cld
        rep     stosb
        pop     cx
        pop     XDI
        mov     word ptr XES[XDI], 4d62h   ; mark the first two words
        mov     word ptr XES[XDI+2], 0072h
ValidBootSector:
        INVOKE  FIL4_WRITE,Handle,vSES XDI,512  ; write it to file    
        cmp     ax,-1
        jz      Err

        dec     cx
        jcxz    LoopDone
        jmp     J_4

LoopDone:
        cmp     Drive,81h
        jz      J_6
        inc     Drive
        jmp     J_0
J_6:
        xor     ax,ax                   ; all is well
        jmp     Done
Err:
        mov     ax,-1
Done:        
        push    ax
        INVOKE  FIL4_CLOSE,Handle
        pop     ax
        ret

DIS4_CREATE_RECOVER   ENDP
;-----------------------------------------------------------------------------
END
