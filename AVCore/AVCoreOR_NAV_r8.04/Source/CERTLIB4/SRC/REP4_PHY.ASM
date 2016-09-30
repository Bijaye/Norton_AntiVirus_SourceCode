include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_PHYSICAL                                              SAMIZ
;     Repairs boot/mbr by using simple absolute/physical sectors.
;
; $Header:   S:\certlib4\vcs\rep4_phy.asv   1.3   29 Nov 1994 13:06:14   DALLEE  $
;
; int WINAPI REP4_PHYSICAL(LPSTR, byType)
;
; Input:
;   lpN30: Virus information from Database
;   byType: Drive type (360, 1.2, 720, etc...)
; Output:
;       1) -1 if FAIL
;       2)  DX: CYLINDER of original MBR/BOOT sector
;           AL: SECTOR
;           AH: HEAD
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_phy.asv  $
;  
;     Rev 1.3   29 Nov 1994 13:06:14   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:22:28   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:32:58   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:30:00   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   14 Jun 1993 14:28:20   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_PHYSICAL      PROC USES XBX XCX SDS SES XSI XDI,
                        lpN30: LPSTR,
                        byType:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------

        xor     XAX, XAX
        xor     XDX, XDX

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

        jmp     R_ERROR
R_HARD:

        mov     dx,word ptr[XSI].N30.HardLoc1           ; cylinder
        mov     ax,word ptr[XSI].N30.HardLoc2           ; head/sector
        jmp     R_EXIT
R_144:                                                  ; for floppies
        mov     dx,word ptr[XSI].N30.Flop_144           ; cyl/sec
        jmp     short R_GET_HEAD

R_360:
        mov     dx,[XSI].N30.Flop_360
        jmp     short R_GET_HEAD
R_720:
        mov     dx,[XSI].N30.Flop_720
        jmp     short R_GET_HEAD
R_12:
        mov     dx,[XSI].N30.Flop_12

R_GET_HEAD:
        mov     ax,word ptr[XSI].N30.Flop_All       ; get head
        mov     al,dl                               ; al=sec
        xchg    dh,dl                               ; now dl=cyl
        xor     dh,dh                               ; dh=0;
        jmp     short R_EXIT
;--------------------------------------
R_ERROR:
        mov     ax, -1
;--------------------------------------
R_EXIT:
        ret

REP4_PHYSICAL      ENDP
;--------------------------------------
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
