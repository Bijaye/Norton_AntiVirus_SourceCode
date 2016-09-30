include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_HARD_PHYSICAL                                                  SAMIZ
;
;     Repairs Hard drive boot where the cyl, head, and sectors values for the
;     original boot are known.
;
; $Header:   S:\certlib4\vcs\rep4_bhp.asv   1.4   29 Nov 1994 13:09:06   DALLEE  $
;
; UINT WINAPI REP4_HARD_PHYSICAL(WORD uDrive, LPSTR lpBuffer);
;
; Input:
;       1) Drive Number with logical in high byte, absolute in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bhp.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:09:06   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:15:08   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:24:10   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:27:14   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   18 May 1993 01:30:18   SZIADEH
;  minor midfications.
;  
;     Rev 1.0   10 May 1993 16:41:16   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_HARD_PHYSICAL     PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR


                        LOCAL uBOOT_Head_SEC:WORD
                        LOCAL uBOOT_Cylinder:WORD

                        LOCAL byCount:BYTE
                        LOCAL uCylinder:WORD
                        LOCAL bySector:BYTE
                        LOCAL byHead:BYTE
                        LOCAL byDrive:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------

        mov     byCount,1
        mov     ax,uDrive
        mov     byDrive,al                           ; 080h, ...

        mov     ax,word ptr[XSI].N30.HardLoc1
        mov     uCylinder,ax                         ; Cylinder

        mov     ax,word ptr[XSI].N30.HardLoc2
        mov     byHead,ah                            ; Head
        mov     bySector,al                          ; Sector

R_WRITE:

        INVOKE DIS4_LOC_BOOTSECTOR,byDrive,lpBuffer  ;get address of boot sector
        cmp     ax, -1                               ; error
        je      R_ERROR

        mov     uBOOT_Cylinder,dx                    ; cylinder
        mov     uBOOT_Head_SEC,ax                    ; head/sec

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

        cmp     word ptr[XSI+510],0AA55h             ; Verify as BR
        jnz     R_ERROR


        mov     ax, uBOOT_Cylinder                   ; cylinder
        mov     uCylinder, ax
        mov     ax, uBOOT_Head_SEC                   ; head/sec
        mov     byHead, ah
        mov     bySector, al


        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR
        mov     ax,1
        jmp     R_EXIT

;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_HARD_PHYSICAL       ENDP

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

