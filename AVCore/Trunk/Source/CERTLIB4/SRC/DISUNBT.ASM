include CTS4.INC
  
UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_UNLOCK_BOOT                03/17/93                      bAd
;
;       Un-Bootlocks a physical drive.
;
; WORD WINAPI DIS4_UNLOCK_BOOT(LPSTR Buffer);
;
; Input:
;       1)  Workbuffer of at least 1024 bytes.
; Output:
;       1)  (-1) on error
;       2)   (0) if passed
;       3)   (1) if already unlocked
;
; LIBRARY: {INTEL}.LIB      
;
;-----------------------------------------------------------------------------
DIS4_UNLOCK_BOOT PROC USES XBX XCX XDX XSI XDI SDS SES,
        Buffer:LPSTR

        LOCAL   CylSec:WORD
        LOCAL   Side:BYTE
        LOCAL   Drive:BYTE
        LOCAL   State:WORD
        LOCAL   fSector:DWORD
        LOCAL   fOffset:WORD

        mov     Drive, 80h              ; we're assuming this for now!

;----------------------------
; Are we bootlocked at all?
;
        INVOKE  DIS4_BOOTLOCKED_STATE,Drive,Buffer
        or      ax, ax
        jz      NotBootlocked
        mov     State, ax

        INVOKE  DIS4_LOC_HIDDEN_MBR,Drive,Buffer ; find where the MBR is hidden
        cmp     ax, -1
        jz      Err
        mov     Side, al
        mov     CylSec, dx
        
        cmp     State, 03               ; MBR & TSR 
        jnz     ReplaceMBR
;----------------------------
; blow away the int 13h handler
; 
        mov     ax, ISR13seg
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
        mov     cx,32
        repne   scasw
        jcxz    Err0                              ; couldn't find- should be there
        cmp     word ptr XES[XDI+0],7703h          ; make sure it's the JA
        jnz     Err
        mov     byte ptr XES[XDI+1],0ebh         ; make it a JMP
        jmp     ReplaceMBR
Err0:   jmp     Err
ReplaceMBR:
;---------------------------
; replace the real MBR to where
; it should be
;
;---------------------------
; if the FAT had a cluster 
; marked BAD, then it must be
; restored
;
        INVOKE  DIS4_GET_MARKED_FAT,Drive,addr fSector,addr fOffset,Buffer
        cmp     ax, -1
        jz      Err

        mov     bx, CylSec                       ; BL <- sector
        mov     dx, bx
        and     bl, 0cfh
        xchg    dh, dl                           ; DX <- cylinder
        mov     cx, 6
        shr     dh, cl

        INVOKE  DIS4_READ_PHYSICAL,Buffer,1,dx,bl,Side,Drive ; read real MBR
        or      ax, ax
        jnz     Err

        INVOKE  DIS4_WRITE_PHYSICAL,Buffer,1,0,1,0,Drive        ; write it to MBR location
        or      ax, ax
        jnz     Err

; if cluster was marked BAD, then mark it UNUSED
        INVOKE  DIS4_UNMARK_FAT,Drive,fSector,fOffset,Buffer
        cmp     ax, -1
        jz      Err
        xor     ax, ax
        jmp     Done                            ; all's well

NotBootlocked:
        mov     ax, 1
        jmp     Done
Err:
        mov     ax, -1
Done:
        ret

DIS4_UNLOCK_BOOT ENDP
;-----------------------------------------------------------------------------
END
