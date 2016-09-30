;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/dis4_typ.asv   1.8   27 Jun 1996 15:54:02   DALLEE  $
;;
;; Description: Defines DIS4_ANALYZE_FLOP function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/dis4_typ.asv  $
;  
;     Rev 1.8   27 Jun 1996 15:54:02   DALLEE
;  LugPlus port:
;  Added two new internal disk types for repairs:
;  1.68M DMF formatted diskettes, 1024 and 2048 bytes per cluster.
;  These are treated as 1.44M for most things (same media descriptor),
;  but generic repair has pre-canned BPB's for these diskettes.
;  
;     Rev 1.7   08 Mar 1996 14:56:38   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.6   23 Aug 1994 20:39:08   RSTANEV
;  Using INC file with a prototype so that the function goes public.
;
;     Rev 1.5   17 Aug 1994 13:25:40   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.4   26 May 1994 14:17:22   RSTANEV
;  Minor changes.
;
;     Rev 1.3   25 May 1994 12:57:22   RSTANEV
;  Don't preserve general registers anymore.
;
;     Rev 1.2   25 May 1994 12:55:58   RSTANEV
;  Converted back to DIS4_READ_PHYSICAL.
;
;     Rev 1.1   25 May 1994 08:18:34   RSTANEV
;  Need to preserve XCX and XDX because calling C function.
;
;     Rev 1.0   25 May 1994 08:17:32   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DIS4_ANALYZE_FLOP
;
; Analyzes floppy type by looking for FATs.
;
; BYTE WINAPI REP4_ANALYZE_TYPE ( BYTE byDrive, LPSTR lpBuffer );
;
; Input:
;       1) Drive Number, logical as BYTE.
;       2) Ptr to 512 (minimum) buffer
;
; Output:
;       1) FALSE(0) Failed on reads or unknown type.
;       2) Diskette Type verified as:
;
;               1 = 360k
;               2 = 720k
;               3 = 1.2m
;               4 = 1.44m
;               5 = 2.88m
;               6 = 1.68m DMF 1024 byte clusters
;               7 = 1.68m DMF 2048 byte clusters
;
;       3) Buffer has first directory sector in it for bootable testing.
;----------------------------------------------------------------------------
DIS4_ANALYZE_FLOP  PROC  CODEDISTANCE  USES SDS XSI,
        byDrive:BYTE,
        lpBuffer:LPBYTE,

        LOCAL   Sector:BYTE
        LOCAL   MediaID:BYTE
        LOCAL   TypeIs:BYTE

        LDS_SI  lpBuffer

                                        ; Read 1st FAT
        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,1,0,2,0,byDrive
        or      XAX,XAX
        jnz     R_ERROR

        mov     al,BYTE PTR[XSI]        ; Get and store
        mov     MediaID,al              ; Media ID byte

        cmp     al,0FDh                 ; Is it 360k?
        je      IS_360

        cmp     al,0F9h                 ; Is it 720k or 1.2m?
        je      IS_720

        cmp     al,0F0h                 ; Is it 1.44m?
        je      IS_144                  ; Could also be a DMF type.

        cmp     al,0FFh                 ; Is it 2.88m?
        je      IS_288

        jmp     R_ERROR                 ; Type non-standard or corrupted.

IS_360:
        mov     Sector,4                ; Second FAT at logical sector 3
        mov     TypeIs,BYTE PTR 1
        jmp     SHORT R_READ

IS_720:
        mov     Sector,5                ; Second FAT at logical sector 4
        mov     TypeIs,BYTE PTR 2
        jmp     SHORT R_READ

IS_144:
        mov     Sector,11               ; Second FAT at logical sector 10
        mov     TypeIs,BYTE PTR 4
        jmp     SHORT R_READ

IS_288:
        mov     Sector,11               ; Second FAT at logical sector 10
        mov     TypeIs,BYTE PTR 5

R_READ:
        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,1,0,Sector,0,byDrive
        or      XAX,XAX
        jnz     R_ERROR                 ; Exit on read error

        mov     al,MediaID              ; Get Media ID from 1st FAT
        cmp     [XSI],al                ; Cmp Media ID from 2nd FAT.
        je      R_OK

        cmp     TypeIs, 4               ; If 144 failed, check for DMF.
        je      Check_DMF

        cmp     TypeIs,2                ; If 720k check now try 1.2m
        jne     R_ERROR
        mov     Sector,9                ; Second FAT at logical sector 8
        mov     TypeIs,BYTE PTR 3       ; TypeIs <- 1.2
        jmp     SHORT R_READ            ; try again

Check_DMF:
                                        ; Look for multiple sectors per cluster
                                        ; in BPB to identify DMF formats.
        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,1,0,1,0,byDrive
        or      XAX,XAX
        jnz     R_ERROR
        cmp     [XSI+0Bh].BPB_struc.SecPerClust, 2
        jne     Check_DMF2048
        mov     Sector, 7
        mov     TypeIs, 6               ; Try DMF 1024 byte cluster
        jmp     R_READ

Check_DMF2048:
        cmp     [XSI+0Bh].BPB_struc.SecPerClust, 4
        jne     R_ERROR
        mov     Sector, 5
        mov     TypeIs, 7               ; Try DMF 2048 byte cluster
        jmp     R_READ

R_ERROR:
        xor     al,al
        jmp     SHORT R_EXIT
R_OK:
        mov     al,TypeIs               ; Put type into al for return.
R_EXIT:
        ret

DIS4_ANALYZE_FLOP  ENDP

UniqueSegmentEnd
END
