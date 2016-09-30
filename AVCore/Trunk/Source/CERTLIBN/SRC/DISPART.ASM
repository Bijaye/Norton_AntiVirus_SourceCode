include CTSN.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_GET_BOOTABLE_PARTITION       3/22/93                          bAd
;
;       Fills in the PARTENTRY with bootable partition table of Drive.
;
; WORD WINAPI DIS4_GET_BOOTABLE_PARTITION(BYTE Drive,PARTENTRY *Part,LPBYTE Buffer);
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
; $Log:   S:/CERTLIBN/VCS/DISPART.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:36   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;-----------------------------------------------------------------------------

DIS4_GET_BOOTABLE_PARTITION PROC USES XCX XSI XDI SDS SES,
        Drive:BYTE,
        Part:LPBYTE,
        Buffer:LPBYTE

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
ifdef WIN16
        mov     ax, word ptr Buffer+2
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
        mov     word ptr Buffer+2, ax
endif

        LDS_SI  Buffer
        add     XSI, PARTITION_OFFSET
        mov     XCX, 4
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
        mov     XCX, sPARTENTRY
        cld     
        rep     movsb

        xor     XAX, XAX                   ; all is well
        jmp     Done
Err:    
        mov     XAX,-1
Done:        
        ret

DIS4_GET_BOOTABLE_PARTITION ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END

