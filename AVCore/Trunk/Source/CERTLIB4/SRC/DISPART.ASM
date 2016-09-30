include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_GET_BOOTABLE_PARTITION       3/22/93                          bAd
;
;       Fills in the PARTENTRY with bootable partition table of Drive.
;
; WORD WINAPI DIS4_GET_BOOTABLE_PARTITION(BYTE Drive,PARTENTRY *Part,LPSTR Buffer);
;
; Input:
;       1)  Drive to read partition table.
;       2)  Pointer to PARTENTRY struc
;       3)  Work buffer of at least 1024 bytes.
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_GET_BOOTABLE_PARTITION PROC USES XCX XSI XDI SDS SES,
        Drive:BYTE,
        Part:LPSTR,
        Buffer:LPSTR

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

;----------------------------
; read the master boot record
;
        INVOKE  DIS4_READ_MBR,Drive,Buffer
        cmp     ax,-1
        jz      Err

;----------------------------
; find the bootable partition
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SDS, ax
        mov     XSI, word ptr Buffer
        add     XSI, PARTITION_OFFSET
        mov     cx, 4
J_0:
        cmp     byte ptr XDS[XSI], 80h  ; this entry bootable?
        jz      J_1                     ; yep
        add     XSI, 16                 ; look at next entry
        loop    J_0
        jmp     Err                     ; no bootable part entry

;--------------------------
; fill in the Bios Parameter Block
;
J_1:
        LES_DI  Part
        mov     cx, sPARTENTRY
        cld     
        rep     movsb

        xor     ax,ax                   ; all is well
        jmp     Done
Err:    
        mov     ax,-1
Done:        
        ret

DIS4_GET_BOOTABLE_PARTITION ENDP
;-----------------------------------------------------------------------------
END
