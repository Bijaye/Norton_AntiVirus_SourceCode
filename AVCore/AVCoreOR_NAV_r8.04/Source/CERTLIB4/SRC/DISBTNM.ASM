include         CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOC_BOOTSECTOR_NUM               3/2/93                          bAd
;
;       Returns the cylinder, sector, side of the boot sector for the 
;        Nth partition table entry.
;
; DWORD WINAPI DIS4_LOC_BOOTSECTOR_NUM(BYTE Drive,WORD PartitionNumber,
;                                       LPSTR WorkBuffer);
;
; Input:
;       1)  Bootable drive (80h or 81h)
;       2)  Partition table entry number (0,1,2,3).
;       3)  Work buffer of at least 512 bytes.
;
; Output:
;       1)  LOWORD (-1) on failure
;       2)  HIWORD cylinder
;       3)  LOWORD, HIBYTE side
;       4)  LOWORD, LOBYTE sector
;       5)  WorkBuffer contains MBR
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_LOC_BOOTSECTOR_NUM PROC USES XCX XSI SDS,
        Drive:BYTE,
        Part:WORD,
        Buffer:LPSTR

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

;--------------------------
; get the location of the MBR
;
        INVOKE  DIS4_LOC_HIDDEN_MBR,Drive,Buffer
        cmp     ax, -1
        jz      Err
        mov     Side, al
        mov     ax, dx
        and     al, 3fh
        mov     Sector, al
        xchg    dh, dl
        mov     cx, 6
        shr     dh, cl
        mov     Cyl, dx

;--------------------------
; read the MBR
;
        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,Cyl,Sector,Side,Drive
        or      ax, ax
        jnz     Err

;--------------------------
; search through partition
; table for bootable sector
;
        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SDS, ax
        mov     XSI, word ptr Buffer

        add     XSI,PARTITION_OFFSET
        mov     ax,Part                 ; the table entry number
        mov     dx,16
        mul     dl
        add     XSI,ax

        mov     ah,byte ptr [XSI+01h]    ; SIDE

        mov     al,byte ptr [XSI+02h]
        and     al,3fh                  ; SECTOR

        mov     dl, byte ptr [XSI+03h]
        mov     dh, byte ptr [XSI+02h]
        mov     cl,6
        shr     dh,cl                  ; CYLINDER

        jmp     AllDone
Err:
        mov     ax,-1
AllDone:
        ret

DIS4_LOC_BOOTSECTOR_NUM ENDP
;-----------------------------------------------------------------------------
END
