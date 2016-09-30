include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_BOOTLOCKED_STATE      03/09/93                      bAd
;
;       Returns the current bootlocked/not bootlocked state of the machine.
;
; WORD WINAPI DIS4_BOOTLOCKED_STATE(BYTE Drive,LPSTR Buffer);
;
; Input:
;       1)  Drive to examine. 
;       2)  Work Buffer of at least 1024 bytes.     
; Output:
;       1) (-1) on error 
;       2) (0000 0000) not bootlocked at all 
;       3) (0000 0001) MBR locked 
;       4) (0000 0010) INT 13h trap in memory
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_BOOTLOCKED_STATE PROC USES XBX XCX XDX XSI XDI SES SDS,
        Drive:BYTE,
        Buffer:LPSTR

        xor     bx,bx        
;------------------------------
; Is the MBR locked?
;
        INVOKE  DIS4_IS_BOOTLOCKED,Drive,Buffer
        or      ax,ax
        jz      NotLocked
        or      bx,1
NotLocked:

ifndef MIRRORS
;------------------------------
; Is INT 13h trapped?
;
        mov     ax,ISR13seg
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax,ax
        jz      Err
endif ; WIN16
        mov     SES,ax
        xor     XDI,XDI
; look for the CMP  AH,03 / JMP
        cld
        mov     ax,0FC80h
        mov     cx,32
        repne   scasw
        jcxz    NotInMem                         ; couldn't find
        cmp     byte ptr XES[XDI+1],0ebh         ; disble JMP there?
        jz      NotInMem
        or      bx,2
NotInMem:        
        jcxz    CleanMachine
        jmp     Done

Err:    
        mov     bx,-1
        jmp     done

CleanMachine:
Done:
endif ; MIRRORS
        mov     ax,bx
        ret

DIS4_BOOTLOCKED_STATE endp
;-----------------------------------------------------------------------------	
END

