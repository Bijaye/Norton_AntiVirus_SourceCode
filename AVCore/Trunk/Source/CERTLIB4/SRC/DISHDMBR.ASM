include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOC_HIDDEN_MBR               3/18/93                          bAd
;
;       Returns the cyl, side, sector of the real MBR.
;
; DWORD WINAPI DIS4_LOC_HIDDEN_MBR(BYTE Drive,LPSTR Buffer);
;
; Input:
;       1)  Bootable drive (80h or 81h)
;       3)  Work buffer of at least 1024 bytes.
;
; Output:
;       1)  (-1) on failure
;       2)  HIWORD Cyl/Sec
;       3)  LOWORD, HIBYTE Drive
;       4)  LOWORD, LOBYTE side
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_LOC_HIDDEN_MBR PROC USES XBX XCX XSI XDI SDS SES,
        Drive:BYTE,
        Buffer:LPSTR

        LOCAL   CylSec:WORD
        LOCAL   Side:BYTE

;----------------------------
; are we bootlocked at all?
;
        INVOKE  DIS4_BOOTLOCKED_STATE,Drive,Buffer
        or      ax, ax
        jz      NotBootlocked
        cmp     ax, 3                   ; MBR replaced & TSR in memory
        jz      J_0
        jmp     NoTSR                   ; only MBR replaced
J_0:
;----------------------------
; search the int 13h handler
; for the real MBR's location
;
        mov     ax, ISR13seg
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax
        xor     XDI, XDI
        jmp     J_1
NoTSR:
;----------------------------
; must get the disk location 
; from the Bootcode already
; on disk
        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,0,1,0,Drive
        or      ax, ax
        jnz     Err
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax
        mov     XDI, word ptr Buffer
        cld
        mov     cx, 100h
        mov     al, 0FCh                ; CMP
Search:
; look for "MOV AH"
        repnz   scasb
        cmp     byte ptr XES[XDI], 03h  ; AH
        jcxz    Err                     ; should have found it
        jnz     Search
J_1:    
; look for "MOV CX"
        cld
        mov     cx, 32
        mov     al, 0b9h                ; MOV CX
        repnz   scasb
        jcxz    Err                     ; should have found it
        mov     ax, word ptr XES[XDI+0] ; Cyl/Sec component
        mov     CylSec, ax
        mov     al, byte ptr XES[XDI+3] ; Side component
        mov     Side, al

        mov     ah, Drive
        mov     al, Side
        mov     dx, CylSec
        jmp     Done

NotBootlocked:
        mov     ah, Drive               ; return the location of the real MBR
        xor     al, al
        mov     dx, 0001h
        jmp     Done

Err:
        mov     ax, -1
        mov     dx, ax
Done:
        ret

DIS4_LOC_HIDDEN_MBR ENDP
;-----------------------------------------------------------------------------
END
