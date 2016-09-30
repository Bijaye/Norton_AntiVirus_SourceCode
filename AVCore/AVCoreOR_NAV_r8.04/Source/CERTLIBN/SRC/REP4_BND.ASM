;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/REP4_BND.ASv   1.4   10 Apr 1996 15:10:44   DALLEE  $
;;
;; Description: Defines REP4_PART_EOFD function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/REP4_BND.ASv  $
;  
;     Rev 1.4   10 Apr 1996 15:10:44   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   11 Jan 1996 17:45:20   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.2   17 Aug 1994 13:26:04   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.1   26 May 1994 08:43:24   RSTANEV
;  Changed list of registers to be saved by the function.
;
;     Rev 1.0   26 May 1994 08:41:04   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_PART_EOFD
;
; Repairs MBR from "end of disk" infections like Flip & Tequila.
;
; BOOL WINAPI REP4_PART_EOFD ( LPVOID lpN30, LPVOID lpBuffer, BYTE byDrive );
;
; Input:
;      lpN30            Virus entry information in Database
;      lpBuffer         Work buffer. 1024 bytes minimum
;      byDrive          Physical drive (0, 1, ..., 7f, 080, 81)
;
; Output:
;       TRUE            sucessful
;       FALSE           otherwise
;----------------------------------------------------------------------------
REP4_PART_EOFD  PROC  CODEDISTANCE  USES XBX XCX XDX SDS XSI,
        lpN30:LPBYTE,
        lpBuffer:LPBYTE,
        byDrive:BYTE

        LOCAL uCylinder:WORD
        LOCAL bySector:BYTE
        LOCAL byHead:BYTE

        LOCAL Add_Sect:BYTE
        LOCAL Fix_Boot:BYTE
        LOCAL Fix_Part:BYTE

                                        ; Get add_to_sector value
                                        ; from clean info in buffer
        LDS_SI  lpN30

        mov     ax,[XSI].N30.HardLoc1
        mov     Add_Sect,al

        mov     ax,[XSI].N30.Restore
        mov     Fix_Part,ah
        mov     Fix_Boot,al

                                        ; lpBuffer contain MBR virus
                                        ; Calculate End Sector and add
                                        ; value to point to original MBR
                                        ; sector
        LDS_SI  lpBuffer                ; Look at buffer
        mov     bySector,03Fh           ; bySector=00111111b

        mov     al,[XSI+1C3h]           ; Get End: head
        mov     byHead,al               ; Store Head

        mov     ax,[XSI+1C4h]           ; Get End: Cyl,Sector
        and     bySector,al
        mov     bl,Add_Sect
        add     bySector,bl             ; Add to Sector

        and     al,0C0h                 ; kill first 6 bits (sector)
        mov     cl,6
        shr     al,cl
        xchg    ah,al

        mov     uCylinder,ax

R_WRITE:
        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uCylinder,bySector,byHead,byDrive
        or      XAX,XAX
        jnz     R_ERROR

        cmp     WORD PTR [XSI+510],0AA55h ; Verify as BR
        jnz     R_ERROR

                                        ; Fix Ending Sector value.
        LDS_SI  lpBuffer
        cmp     Fix_Part,0
        je      @F

        xor     ah,ah
        mov     al,Fix_Part
        add     [XSI+1C4h],ax
@@:
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
        or      XAX,XAX
        jnz     R_ERROR

                                        ; If Boot modified, Read the Boot
                                        ; sector and fix total sector value.
R_BOOT:
        cmp     Fix_Boot,0              ; Don't fix boot if 0
        je      R_DONE

        INVOKE  DIS4_LOC_BOOTSECTOR,byDrive,lpBuffer ;get address of boot sector
        or      XAX, -1
        je      R_ERROR

        mov     byHead,ah
        mov     bySector,al
        mov     uCylinder,dx

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uCylinder,bySector,byHead,byDrive
        or      XAX,XAX
        jnz     R_ERROR

        LDS_SI  lpBuffer                ; Look at MBR in buffer
        cmp     BYTE PTR[XSI],0E9h      ; Verify as Boot sector
        je      @F

        cmp     BYTE PTR[XSI],0EBh      ; Verify as Boot sector
        jne     R_EXIT

@@:
        cmp     WORD PTR [XSI+510],0AA55h ; Verify signature
        jnz     R_ERROR

        mov     al,Fix_Boot             ; Fix Total sector value
        add     BYTE PTR[XSI+13],al

        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,uCylinder,bySector,byHead,byDrive
        or      XAX,XAX
        jnz     R_ERROR

R_DONE:
        mov     XAX,TRUE
        jmp     SHORT R_EXIT

R_ERROR:
        xor     XAX,XAX

R_EXIT:
        ret

REP4_PART_EOFD  ENDP

UniqueSegmentEnd
END

;----------------------------------------------------
;  CLEAN                MEANING             EOFD
;  FIELD               FOR BOOT             hi/lo
;                                          (offset)
;----------------------------------------------------
; wInfSize           - Hard Type             -
; wTagSize           - Flop Type             -
; wHostJmp           - Hard Location 1      0/add
; wJmpLen            - Hard Location 2       -
; wSS_reg            - Flop 360              -
; wSP_reg            - Flop 720              -
; wCS_reg            - Flop 1.2              -
; wIP_reg            - Flop 1.44             -
; wHeader            - Flop 2.88             -
; wHeadLen           - Flop All              -
; wBookMarkLocation  - Restore Word        par/boo
; wBookMark          - Special Flags         -
;----------------------------------------------------
