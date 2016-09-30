include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_RECOVER                  3/3/93                          bAd
;
;       Recovers the MBR of either hard drive (80h/81h) and/or any number
;         of boot sectors.  Also disables the handler in memory.
;
; WORD WINAPI DIS4_RECOVER(LPSTR Filename,WORD Mode,LPSTR Buffer);
;
; Input:
;       1)  File where recover info is stored.
;       2)  What to recover for each drive:
;               LOBYTE - (first hard drive (80h))
;               HIBYTE - (second hard drive (81h))
;                 1 = master boot record
;                 2 = first boot sector
;                 4 = second boot sector        
;                 8 = third boot sector        
;                16 = fourth boot sector        
;        3)  Work buffer of at least 1024 bytes
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_RECOVER PROC USES XCX XDX XDI SES,
        Filename:LPSTR,
        Mode:WORD,
        Buffer:LPSTR
        
        LOCAL   Handle:UINT
        LOCAL   Drive:BYTE
        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE
        LOCAL   Part:BYTE
        LOCAL   fSector:DWORD
        LOCAL   fOffset:WORD
        LOCAL   dwFileSize:DWORD

;------------------------------
; Blow away the int 13h handler
;
        mov     ax,ISR13seg
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax
        xor     XDI, XDI
; look for the CMP  AH,03 and make the next JA a JUMP
        cld
        mov     ax,0FC80h
        mov     cx,100
        repne   scasw
        jcxz    NotInMem                         ; couldn't find
        cmp     word ptr XES[XDI],7703h          ; make sure it's the JA
        jnz     NotInMem
        mov     byte ptr XES[XDI+1],0ebh         ; make it a JMP
        jmp     J_Sub0
NotInMem:
        INVOKE  DIS4_IS_BOOTLOCKED,80h,Buffer
        or      ax,ax
        jz      NotLocked                        ; BUT recover anyway        
        cmp     ax,-1                                  
        jnz     J_Sub0 
        jmp     Err        
NotLocked:
;        mov     ax,1                           
;        jmp     Done
;----------------------------
; open recover file
;                  
J_Sub0:                        
        INVOKE  FIL4_OPEN,Filename,fREAD
        cmp     ax, -1
        jz      Err
        mov     Handle,ax
        INVOKE  FIL4_SIZE,Handle        ; get file size
        mov     word ptr dwFileSize+2, dx
        mov     word ptr dwFileSize, ax

;----------------------------
; do both drives, 80h & 81h
;
        mov     Drive,80h

J_0:
;-----------------------------
; if the FAT had a Cluster
; marked BAD by bootlock, then 
; it should be restored
;
        INVOKE  DIS4_GET_MARKED_FAT,Drive,addr fSector,addr fOffset,Buffer
        ; an error will indicate that a cluster has not been marked bad, so skip it

;----------------------------
; do the MBR if set
;
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES,ax
        mov     XDI, word ptr Buffer

        INVOKE  FIL4_READ,Handle,vSES XDI,512           ; read MBR from file
        cmp     ax,-1
        jz      Err

        test    Mode,1
        jz      J_1

        INVOKE  DIS4_WRITE_PHYSICAL,Buffer,2,0,1,0,Drive   ; write it to HD
        or      ax,ax
        jnz     Err
        jmp     J_2
J_1:
;        INVOKE  FIL4_SEEK,Handle,512,fSEEK_FOR  ; skip the MBR
;        cmp     ax,-1
;        jz      Err

;----------------------------
; look through all four boot sectors
;
J_2:
        mov     cx,4
        mov     Part,0
J_3:
        INVOKE  DIS4_LOC_BOOTSECTOR_NUM,Drive,Part,Buffer    ; find where boot sector should go    
        cmp     ax,-1
        jz      Err
        mov     Cyl,dx
        mov     Side,ah
        mov     Sector,al

        INVOKE  FIL4_READ,Handle,vSES XDI,512           ; read boot sector from file
        cmp     ax,-1
        jz      Err

;-------------------------------
; make sure this is a valid boot
; sector
;
        cmp     word ptr XES[XDI], 4d62h
        jnz     ValidBootSector
        cmp     word ptr XES[XDI+2], 0072h
        jnz     ValidBootSector
        jmp     J_5                     ; wasn't a valid sector; don't recover

ValidBootSector:
        shr     Mode,1                  ; examine next bit setting
        test    Mode,1
        jz      J_4

        INVOKE  DIS4_WRITE_PHYSICAL,Buffer,1,Cyl,Sector,Side,Drive   ; write it to HD
        or      ax,ax
        jnz     Err

        jmp     J_5
J_4:
;        INVOKE  FIL4_SEEK,Handle,512,fSEEK_FOR  ; skip the MBR
;        cmp     ax,-1
;        jz      Err
J_5:
        inc     Part
        dec     cx
        jcxz    J_6
        jmp     J_3
J_6:
        INVOKE  DIS4_UNMARK_FAT,Drive,fSector,fOffset,Buffer
      
        cmp     Drive,81h
        jz      AlmostDone
        inc     Drive
        mov     ax, word ptr dwFileSize ; is there more in the file?
        cmp     ax, 512*5
        jle     AlmostDone
        jmp     J_0

AlmostErr:
        cmp     Drive, 81h              ; the err is probably because
        jz      AlmostDone              ; there is no other hard drive

Err:
        mov     ax,-1
        jmp     Done
AlmostDone:
        xor     ax, ax                  ; all is well
Done:        
        push    ax
        INVOKE  FIL4_CLOSE,Handle
        pop     ax
        ret

DIS4_RECOVER   ENDP
;-----------------------------------------------------------------------------
END
