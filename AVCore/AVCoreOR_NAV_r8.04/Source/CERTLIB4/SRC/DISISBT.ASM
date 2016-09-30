include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_IS_BOOTLOCKED                  3/8/93                          bAd
;
;       Returns 1 if drive is already bootloked.       
;
; WORD WINAPI DIS4_IS_BOOTLOCKED(BYTE Drive,LPSTR WorkBuffer);
;
; Input:
;       1)  Drive to read.
;       2)  Work buffer of at least 512 bytes.
;
; Output:
;       1)  (-1) on error
;       2)  (1) if bootlocked
;       3)  (0) if not boolocked
;       4)  WorkBuffer contains MBR and next sector.
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_IS_BOOTLOCKED PROC USES XCX XDX XSI XDI SDS SES,
        Drive:BYTE,
        Buffer:LPSTR

        jmp     J_0

cright		db	'Copyright (C) Symantec 1993.'
scright         EQU     $ - cright

;-----------------------------
; read the first TWO sectors of the MBR -
; TWO to circumvent the INT 13h
; handler
;
J_0:

        mov     ax,cs
        mov     es,ax

        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,0,1,0,Drive
        or      ax,ax
        jnz     Err

        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SDS,ax
        mov     XSI, word ptr Buffer
        add     XSI,2                            ; skip the jump instructions
        mov     DI, offset cright
        mov     cx, scright

        cld
        repe    cmpsb                           ; compare copyright
        jcxz    ItsBootlocked
        xor     ax,ax
        jmp     ReallyDone

ItsBootlocked:
        mov     ax,1
        jmp     ReallyDone
Err:
        mov     ax,-1
ReallyDone:
        ret

DIS4_IS_BOOTLOCKED ENDP
;-----------------------------------------------------------------------------
END
