include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_UNLOCK_BOOT                03/30/93                      bAd
;
;       Un-Bootlocks a physical drive.
;
; WORD WINAPI DIS4_UNMARK_FAT(BYTE Drive,DWORD fSector, 
;                             WORD fOffset, LPSTR Buffer);
;
; Input:
;       1)  Drive to unmark FAT (80h or 81h)
;       2)  Logical sector in FAT where bad cluster is, OR
;            0 if nothing marked bad.
;       3)  Offset (in bytes) into logical sector to where bad cluster is.
;       4)  Workbuffer of at least 1024 bytes.
; Output:
;       1)  (-1) on error
;       2)   (0) if passed
;       3)   (1) if FAT was not marked bad in the first place
;
; LIBRARY: {INTEL}.LIB      
;
;-----------------------------------------------------------------------------
DIS4_UNMARK_FAT PROC USES XCX XDX XDI SES,
        Drive:BYTE,
        fSector:DWORD,
        fOffset:WORD,
        Buffer:LPSTR

        LOCAL   altDrive:BYTE
        LOCAL   FATII:DWORD
        LOCAL   lDPB:DPB
        LOCAL   Cylinder:WORD
        LOCAL   Sector:BYTE
        LOCAL   Side:BYTE
        LOCAL   Cylinder_2:WORD
        LOCAL   Sector_2:BYTE
        LOCAL   Side_2:BYTE


; if FAT sector is zero, than nothing was marked, so exit
        cmp     word ptr fSector+2, 0
        jnz     IsMarked
        cmp     word ptr fSector, 0
        jz      NotMarked
IsMarked:
        mov     al, Drive               ; convert 80h -> 03h and 81h -> 04h
        and     al, 0fh
        add     al, 2
        mov     altDrive, al
;----------------------------
; calculate location of second
; FAT
;
        mov     dl, altDrive
        inc     dl
        INVOKE  DIS4_GET_DPB,dl,addr lDPB ; get drive parm block
        cmp     ax, -1
        jz      Err
        mov     dx, word ptr fSector+2    ; first FAT +...
        mov     ax, word ptr fSector

        add     ax, lDPB.dpbFATSize       ; total FAT size =...
        adc     dx, 0

        mov     word ptr FATII+2, dx      ; location of second FAT
        mov     word ptr FATII, ax

;----------------------------
; read in the FAT sector
;
        INVOKE  DIS4_READ_LOGICAL,Buffer,altDrive,fSector,1
        or      ax, ax                  ; read FAT sector
        jnz     Err

        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err                     ; ES:DI <- current FAT sector
endif
        mov     SES, ax
        mov     XDI, word ptr Buffer
        add     XDI, fOffset            ; go to bad entry

        mov     ax, word ptr XES[XDI]   ; AX <- cluster marked bad

        cmp     ax, 0fff7h              ; 16-bit fat marked bad?
        jz      SixteenBit
        mov     cx, ax                  ; 12-bit fat, low 12 bits?
        and     cx, 0fffh               
        cmp     cx, 0ff7h
        jz      TwelveLow
        mov     cx, ax                  ; 12-bit fat, high 12 bits?
        and     cx, 0fff0h        
        cmp     cx, 0ff70h              
        jnz     Err
        and     ax, 000fh               ; mark it as unused
        jmp     J_0

TwelveLow:
        and     ax, 0f000h              ; mark it as unused
        jmp     J_0

SixteenBit:
        xor     ax, ax                  ; mark it as unused
J_0:
        mov     word ptr XES[XDI], ax   ; replace it in buffer
        INVOKE  DIS4_WRITE_LOGICAL,Buffer,altDrive,fSector,1
        or      ax, ax
        jnz     Err
; synchronize second FAT with first
        INVOKE  DIS4_WRITE_LOGICAL,Buffer,altDrive,FATII,1
        or      ax, ax
        jnz     Err
        jmp     Done

NotMarked:
        mov     ax, 1
        jmp     Done

Err:
        mov     ax, -1
Done:
        ret

DIS4_UNMARK_FAT ENDP
END
