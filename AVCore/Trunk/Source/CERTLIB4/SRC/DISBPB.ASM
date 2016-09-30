include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_GET_BPB                  3/2/93                          bAd
;
;       Fills in the Bios Parameter Block struct for Drive.
;
; WORD WINAPI DIS4_GET_BPB(BYTE Drive,BPB_struc *BPB,LPSTR Buffer);
;
; Input:
;       1)  Drive to read bios parameter block.
;       2)  Pointer to BPB struc
;       3)  Work buffer of at least 512 bytes.
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_GET_BPB PROC USES XCX XDX XSI XDI SDS SES,
        Drive:BYTE,
        BPB:LPSTR,
        Buffer:LPSTR

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

;--------------------------
; read the bootsector
;
        INVOKE  DIS4_READ_BOOTSECTOR,Drive,Buffer
        cmp     ax,-1
        jz      Err

;--------------------------
; fill in the Bios Parameter Block
;
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SDS, ax
        mov     XSI, word ptr Buffer
        add     XSI, BPB_OFFSET
        mov     cx, sBPB_struc
        cld     
        LES_DI  BPB
        rep     movsb

        xor     ax,ax                   ; all is well
        jmp     Done
Err:    
        mov     ax,-1
Done:        
        ret

DIS4_GET_BPB ENDP
;-----------------------------------------------------------------------------
END
