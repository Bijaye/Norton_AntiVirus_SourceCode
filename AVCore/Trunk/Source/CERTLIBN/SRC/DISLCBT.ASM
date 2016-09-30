;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DISLCBT.ASv   1.6   08 Mar 1996 14:56:38   DALLEE  $
;;
;; Description: Defines DIS4_LOC_BOOTSECTOR function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DISLCBT.ASv  $
;  
;     Rev 1.6   08 Mar 1996 14:56:38   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.5   11 Jan 1996 18:03:26   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.3.1.2   27 Oct 1995 15:46:06   DALLEE
;  Changed a woefully misleading comment.  Work buffer must really be
;  1024 bytes since the first TWO sectors are actually read.
;  
;     Rev 1.3.1.1   13 Jul 1995 23:44:38   SZIADEH
;  
;     Rev 1.3.1.0   12 Jul 1995 18:58:24   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.3   25 May 1995 02:26:04   SZIADEH
;  fixed some 32-bit stuff.
;  
;     Rev 1.2   17 Aug 1994 13:25:42   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   26 May 1994 14:40:32   RSTANEV
;  Minor changes.
;
;     Rev 1.0   26 May 1994 08:04:18   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_LOC_BOOTSECTOR
;
; Returns the cylinder, sector, side of the bootable boot sector
;
; AX:DX WINAPI DIS4_LOC_BOOTSECTOR ( BYTE Drive, LPBYTE WorkBuffer );
;
; Input:
;       1)  Bootable drive (80h or 81h)
;       2)  Work buffer of at least 1024 bytes - dunno why, but it
;           reads the first 2 sectors...
; Output:
;       1)  AX:DX contain the information
;       2)  Work buffer contains MBR
;       3)  AX:DX == -1 on failure
;----------------------------------------------------------------------------
DIS4_LOC_BOOTSECTOR  PROC  CODEDISTANCE USES XCX XSI SDS,
        Drive:BYTE,
        Buffer:LPBYTE

        LOCAL   Cyl:WORD
        LOCAL   Side:BYTE
        LOCAL   Sector:BYTE

        LDS_SI  Buffer
                                        ; set return result for a floppy
        xor     XDX, XDX                ; cylinder
        mov     XAX, 0001               ; head/sector

        cmp     Drive,07Fh              ; floppies?
        jbe     SHORT AllDone

HARD_DRIVE:
                                        ; get the location of the MBR
        INVOKE  DIS4_LOC_HIDDEN_MBR,Drive,vSDS XSI
        cmp     XAX,-1
        je      Err

        mov     Side,al
        mov     XAX, XDX
        and     al,3fh
        mov     Sector,al

        xchg    dh,dl
        mov     XCX,6
        shr     dh,cl
        mov     Cyl,dx

                                        ; read the MBR
        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,2,Cyl,Sector,Side,Drive
        or      XAX,XAX
        jnz     Err

                                        ; search through partition
                                        ; table for bootable sector
        add     XSI,PARTITION_OFFSET
        mov     XCX,04h                 ; look through all 4 table entries
J_1:
        cmp     BYTE PTR [XSI],80h
        jnz     NotBootable

        mov     ah,BYTE PTR [XSI+01h]   ; SIDE

        mov     al,BYTE PTR [XSI+02h]
        and     al,3fh                  ; SECTOR

        mov     dl,BYTE PTR [XSI+03h]
        mov     dh,BYTE PTR [XSI+02h]
        mov     cl,6
        shr     dh,cl                   ; CYLINDER

if OS_32
        mov     cl, 16
        shl     edx, cl
        or      eax, edx
        shr     edx, cl                 ;
endif
        jmp     SHORT AllDone

NotBootable:
        add     XSI,16                  ; move to next table entry
        loop    J_1
Err:
        mov     XAX,-1
        mov     XDX,XAX
AllDone:
        ret

DIS4_LOC_BOOTSECTOR  ENDP

UniqueSegmentEnd
END
