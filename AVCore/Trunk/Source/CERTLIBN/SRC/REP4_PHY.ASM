;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/rep4_phy.asv   1.3   27 Jun 1996 17:35:10   DALLEE  $
;;
;; Description: Defines REP4_PHYSICAL function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/rep4_phy.asv  $
;  
;     Rev 1.3   27 Jun 1996 17:35:10   DALLEE
;  LuigiPlus port:
;  1.68M DMF floppy support -- treat like 1.44 for known repairs.
;  
;     Rev 1.2   10 Apr 1996 15:10:48   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.1   17 Aug 1994 13:26:16   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   25 May 1994 08:46:08   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_PHYSICAL
;
; Repairs boot/mbr by using simple absolute/physical sectors.
;
; DX:AX WINAPI REP4_PHYSICAL ( LPVOID lpN30, BYTE byType );
;
; Input:
;       lpN30: Virus information from Database
;       byType: Drive type (360, 1.2, 720, etc...)
;
; Output:
;       1) (UINT) -1 if FAIL
;       2)  DX: CYLINDER of original MBR/BOOT sector
;           AL: SECTOR
;           AH: HEAD
;
;----------------------------------------------------------------------------
REP4_PHYSICAL  PROC  CODEDISTANCE  USES SDS XSI,
        lpN30:LPBYTE,
        byType:BYTE

        LDS_SI  lpN30

        cmp     byType,0
        je      R_HARD
        cmp     byType,1
        je      R_360
        cmp     byType,2
        je      R_720
        cmp     byType,3
        je      R_12
        cmp     byType,4
        je      R_144
        cmp     byType,6                ; Treat 1.68M DMF 1024 byte as 1.44M
        je      R_144
        cmp     byType,7                ; Treat 1.68M DMF 2024 byte as 1.44M
        je      R_144

        jmp     R_ERROR

R_HARD:
        mov     dx,[XSI].N30.HardLoc1   ; cylinder
        mov     ax,[XSI].N30.HardLoc2   ; head/sector
        jmp     SHORT R_EXIT

R_144:                                  ; for floppies
        mov     dx,[XSI].N30.Flop_144   ; cyl/sec
        jmp     SHORT R_GET_HEAD

R_360:
        mov     dx,[XSI].N30.Flop_360
        jmp     SHORT R_GET_HEAD
R_720:
        mov     dx,[XSI].N30.Flop_720
        jmp     SHORT R_GET_HEAD
R_12:
        mov     dx,[XSI].N30.Flop_12

R_GET_HEAD:
        mov     ax,[XSI].N30.Flop_All   ; get head
        mov     al,dl                   ; al=sec
        xchg    dh,dl                   ; now dl=cyl
        xor     dh,dh                   ; dh=0
        jmp     SHORT R_EXIT

R_ERROR:
        mov     ax,-1

R_EXIT:
        ret

REP4_PHYSICAL  ENDP

UniqueSegmentEnd
END

;----------------------------------------------------
;  CLEAN                MEANING            Physical
;  FIELD               FOR BOOT             hi/lo
;                                          (offset)
;----------------------------------------------------
; wInfSize           - Hard Type             -
; wTagSize           - Flop Type             -
; wHostJmp           - Hard Location 1     cylinder
; wJmpLen            - Hard Location 2     head/sec
; wSS_reg            - Flop 360            cyl/sec
; wSP_reg            - Flop 720            cyl/sec
; wCS_reg            - Flop 1.2            cyl/sec
; wIP_reg            - Flop 1.44           cyl/sec
; wHeader            - Flop 2.88           cyl/sec
; wHeadLen           - Flop All            head/0
; wBookMarkLocation  - Restore Word           -
; wBookMark          - Special Flags          -
;----------------------------------------------------
