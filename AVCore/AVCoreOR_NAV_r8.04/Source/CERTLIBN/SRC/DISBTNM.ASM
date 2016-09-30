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
; $Header:   S:/CERTLIBN/VCS/DISBTNM.ASv   1.3   08 Mar 1996 14:56:36   DALLEE  $
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/DISBTNM.ASv  $
;  
;     Rev 1.3   08 Mar 1996 14:56:36   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.2   24 Mar 1995 13:45:16   DALLEE
;  changed the ifdef to correctly include the 32 bit code.
;  
;     Rev 1.1   21 Feb 1995 20:43:56   SZIADEH
;  retun value in eax in 32-bit.
;  
;     Rev 1.0   19 Jan 1995 15:13:50   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
include         CTSN.INC

UniqueSegment

DIS4_LOC_BOOTSECTOR_NUM PROC USES XCX XSI SDS,
        Drive:BYTE,
        Part:UINT,
        Buffer:LPBYTE

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

;--------------------------
; get the location of the MBR
;
        INVOKE  DIS4_LOC_HIDDEN_MBR,Drive,Buffer
        cmp     XAX, -1
        jz      Err

        mov     Side, al
        mov     XAX, XDX
        and     al, 3fh
        mov     Sector, al
        xchg    dh, dl
        mov     XCX, 6
        shr     dh, cl
        mov     Cyl, dx

;--------------------------
; read the MBR
;
        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,Cyl,Sector,Side,Drive
        or      XAX, XAX
        jnz     Err

;--------------------------
; search through partition
; table for bootable sector
;

if  bSYM_WIN16
        mov     ax, word ptr Buffer+2
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
        mov     word ptr Buffer+2, ax
endif

        LDS_SI  Buffer
        add     XSI,PARTITION_OFFSET
        mov     XAX,Part                 ; the table entry number
        mov     XDX,16
        mul     dl
        add     XSI, XAX

        xor     XAX, XAX
        xor     XDX, XDX

        mov     ah,byte ptr [XSI+01h]    ; SIDE

        mov     al,byte ptr [XSI+02h]
        and     al,3fh                  ; SECTOR

        mov     dl, byte ptr [XSI+03h]
        mov     dh, byte ptr [XSI+02h]
        mov     cl,6
        shr     dh,cl                  ; CYLINDER

if OS_32
        mov     cl , 16
        shl     edx, cl                ; move the low edx word (cylinder)
        or      eax, edx               ; to hi word of eax to get 32-bit value
endif

        jmp     AllDone
Err:
        mov     XAX,-1
AllDone:
        ret

DIS4_LOC_BOOTSECTOR_NUM ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
