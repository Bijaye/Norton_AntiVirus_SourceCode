include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOCK_LASTFAT                  3/23/93                          bAd
;
;       Returns the cyl, side, sector of a cluster marked bad for the placement
;          of the master boot record during bootlock.  Also returns the logical
;          sector and offset into the FAT of that cluster entry.
;
; DWORD WINAPI DIS4_LOCK_LASTFAT(BYTE Drive,UINT FAR *SectorFAT,
;                                UINT FAR *OffsetFAT,LPSTR Buffer);
;
; Input:
;       1)  Drive to mark a cluster bad (80h or 81h).
;       2)  The logical sector number in the FAT where the cluster was marked bad.
;       3)  Address of the offset into the FAT where cluster entry is.
;       4)  Address of the DOS work buffer of at least 1024 bytes.
;
; Output:
;       1)  (-1) on failure
;       2)  (0) no free sectors found
;       3)  HIWORD - Cylinder
;           LOWORD HIBYTE - Side
;           LOWORD LOBYTE - Sector
;       4)  SectorFAT - LSN of FAT where cluster marked bad
;       5)  OffsetFAT - # of bytes into Sector where cluster marked bad
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_LOCK_LASTFAT PROC USES XBX XCX XSI XDI SDS SES,
        Drive:BYTE,
        SectorFAT:DWORD,
        OffsetFAT:DWORD,
        Buffer:LPSTR,
        
        LOCAL   altDrive:BYTE
        LOCAL   bLogDrive:BYTE
        LOCAL   lDPB:DPB
        LOCAL   Part:PARTENTRY
        LOCAL   LastFAT:DWORD
        LOCAL   FATOffset:WORD
        LOCAL   Cluster:DWORD
        LOCAL   High12:BYTE
        LOCAL   LastFAT_2:DWORD

;----------------------------
; get the bootable partition
; table entry
;
        INVOKE  DIS4_GET_BOOTABLE_PARTITION,Drive,addr Part,Buffer
        cmp     ax, -1
        jz      Err

;----------------------------
; get the Drive Parameter Block
;
        mov     al, Drive               ; convert 80h to 03h and
        add     al, 03h                 ;         81h to 04h
        and     al, 0fh                 ; clear high nybble
        mov     altDrive, al
        dec     al
        mov     bLogDrive, al

        INVOKE  DIS4_GET_DPB, altDrive, addr lDPB
        cmp     ax, -1
        jz      Err
; find the last LSN for first FAT
; LASTFAT = DPBFIRSTFAT + DPBSIZE - 1
        xor     dx, dx
        mov     ax, lDPB.dpbFirstFAT     ; calc last FAT logical sector number
        add     ax, lDPB.dpbFATSize
        adc     dx, 0
        sub     ax, 2                   ; start at second last sector
        sbb     dx, 0
        mov     word ptr LastFAT+2, dx
        mov     word ptr LastFAT, ax

;----------------------------
; search from the end of the
; FAT to the beginning looking
; for a free cluster
;
        mov     cx, lDPB.dpbFATSize
        dec     cx                      ; start at last FULL sector
J_0:
        mov     ax, cx
        sub     ax, lDPB.dpbFirstFAT
        mov     FATOffset, ax           ; sector offset from beginning of FAT

;----------------------------
; compute location of second
; FAT
;
        mov     dx, word ptr LastFAT+2    ; calculate the sector in the second FAT
        mov     ax, word ptr LastFAT
        add     ax, lDPB.dpbFATSize
        adc     dx, 0
        mov     word ptr LastFAT_2+2, dx
        mov     word ptr LastFAT_2, ax

;----------------------------
; read in first FAT
;
        INVOKE  DIS4_READ_LOGICAL, Buffer, bLogDrive, LastFAT,1
        or      ax, ax                  ; read in that sector of the FAT
        jnz     Err

        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax                 ; ES:DI <- MBR
        mov     XDI, word ptr Buffer

        add     XDI, 510                ; point to last entry (word)
        xor     ax, ax
        push    cx                      ; CX = current FAT sector number

        cmp     byte ptr Part.peFileSystem, 01h ; determine partition size
        jz      TwelveBit               ; dealing with 12-bit partition

; SixteenBit:
        mov     cx, 512/2               ; compare the entire sector (in words)
        std                             ; move backwards
        repnz   scasw                   ; look for 0000h           
        jnz     NothingFree             ; no free clusters
        pop     ax                      ; found free cluster, clear stack
        shl     cx, 1                   ; convert offset into bytes
        jmp     FoundIt

TwelveBit:   
        mov     High12, 0               ; begin with low 12 bits
        mov     ax, SES                 ; DS:SI <- ES:DI (MBR)
        mov     SDS, ax
        mov     XSI, XDI
; find the absolute cluster number so we can align correctly on and entry
; that is 1.5 bytes long and is in the high 12 bits
; cluster = FATOffset * 512 bytes per sector + offset
; 12-bit fats are max 4096 clusters
J_1:
        mov     cx, 510                 ; total number of bytes
J_15:
        mov     ax, FATOffset           ; calculate cluster
        mov     bx, 512
        mul     bx
        add     ax, cx                  ; offset into FAT

; divide by 1.5 OR fraction 3/2 OR mult by 2 div by 3
; first divide by 3
        mov     bx, 2                   ; divide offset by 1.5 to get
        mul     bx                      ; cluster
        mov     bx, 3
        div     bx

        or      dx, dx                  ; look for no remainder
        jz      J_2                     ; aligned correctly
        jcxz    NothingFree             ; sector is full
        dec     XSI                     ; move back a byte
        dec     cx                      ; and check for proper
        jmp     J_15                    ; cluster alignment again
J_2:               
                                        ; look for free cluster
        mov     ax, word ptr XDS[XSI]   ; AX <- DS:[SI]
        mov     bx, cx                  ; save word count in BX
        mov     dx, XSI
        cmp     High12, 1               ; should we look at high or low  
        jz      doHigh                  ;  12-bits?
        inc     High12
        and     ax, 0fffh               ; clear out high 4 bits
        dec     bx                      ; word counter to next word
        dec     bx                      ; word counter to next word
        dec     dx                      ; word ptr to next word
        dec     dx                      ; word ptr to next word
        jmp     J_3
doHigh:
        dec     High12
        push    cx                      ; CX = current word
        mov     cx, 4                   ; shift high bits over
        shr     ax, cl
        pop     cx                      ; restore current word
        dec     bx                      ; word counter to next word
        dec     dx
J_3:       
        or      ax, ax                  ; check for free cluster
        jnz     J_4
        pop     ax                      ; clear stack
        jmp     FoundIt
J_4:
        pop     cx                      ; restore FAT sector number
        jcxz    NothingFree             ; no more FAT sectors
        push    cx                      ; save it
        mov     cx, bx                  ; move next word counter into current word
        mov     XSI, dx                 ; next word
        jmp     J_2                     ; go get the next word

;------------------------------                                 
NothingFree:
        cld     
        mov     dx, word ptr LastFAT+2  ; move back one sector
        mov     ax, word ptr LastFAT
        sub     ax, 1
        sbb     dx, 0
        mov     word ptr LastFAT+2, dx
        mov     word ptr LastFAT, ax

        pop     cx
        dec     cx
        jcxz    J_5
        push    cx
        jmp     J_0
J_5:
; no free clusters
        xor     ax, ax
        mov     dx, dx
        jmp     Done

FoundIt:
; 16-bit CLUSTER = (FATOFFSET * 512 + OFFSET_INTO_BUFFER) / 2
; 12-bit CLUSTER = (FATOFFSET * 512 + OFFSET_INTO_BUFFER) / 1.5
; copy the offset into the sector to OffsetFAT
        push    SES
        push    XDI
        LES_DI  OffsetFAT
        mov     word ptr XES[XDI], cx   ; cx is offset into buffer
; copy the lsn of the FAT to SectorFAT
        LES_DI  SectorFAT
        xor     dx, dx                  ; compute logical sector number in FAT
        mov     ax, FATOffset
        add     ax, lDPB.dpbFirstFat
        adc     dx, 0
        mov     word ptr XES[XDI+2], dx
        mov     word ptr XES[XDI], ax
        pop     XDI
        pop     SES

        mov     ax, FATOffset           ; get cluster number of this free cluster
        mov     bx, 512
        mul     bx
        add     ax, cx                  ; cx is offset into buffer
        adc     dx, 0
        cmp     byte ptr Part.peFileSystem, 01h ; 12-bit or 16-bit
        jz      J_6
; 16-bit so divide by 2
        mov     bx, 2
        div     bx                      ; this is a 16-bit
        jmp     J_7
J_6:    
; 12-bit so divide by 1.5 OR fraction 3/2 OR mult by 2 div by 3
        mov     bx, 2
        mul     bx
        mov     bx, 3
        div     bx     
        or      dx, dx                  ; no remainder, it's good
        jz      J_7
        xor     dx, dx                  ; add one to the cluster
        inc     ax
J_7:
        mov     word ptr Cluster+2, dx
        mov     word ptr Cluster, ax

; mark it bad with a 0fff7h for 16-bit and 0ff7h for 12-bit
        cmp     byte ptr Part.peFileSystem, 01h
        jnz     J_10
; 12-bit
        mov     ax, word ptr XDS[XSI]        
        cmp     High12, 0
        jz      J_8
        or      ax, 0ff7h
        jmp     J_9
J_8:
        or      ax, 0ff70h
J_9:
        mov     word ptr XDS[XSI], ax
        jmp     J_11
J_10:
; 16-bit
        add     XDI, 2
        mov     ax, 0fff7h
        stosw
J_11:
; write modified FAT back 
        INVOKE  DIS4_WRITE_LOGICAL,Buffer, bLogDrive, LastFAT, 1
        or      ax, ax
        jnz     Err

; sync the second FAT with the first
        INVOKE  DIS4_WRITE_LOGICAL,Buffer, bLogDrive, LastFAT_2, 1
        or      ax, ax
        jnz     Err

; determine physical location of the 'bad' sector
        INVOKE  DIS4_CLUSTER_TO_ABSOLUTE,altDrive,Cluster,Buffer
        cmp     ax, -1
        jz      Err
        jmp     Done

Err:    
        mov     ax, -1
        mov     dx, ax
Done:      
        cld     
        ret

DIS4_LOCK_LASTFAT ENDP
;-----------------------------------------------------------------------------
END
