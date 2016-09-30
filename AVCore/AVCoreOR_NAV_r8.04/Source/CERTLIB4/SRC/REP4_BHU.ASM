include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_HARD_UNIQUE                              JOE/SAMIZ
;
;     Repairs HD where original is stored at end of track 1 (Disk Killer)
; $Header:   S:\certlib4\vcs\rep4_bhu.asv   1.3   29 Nov 1994 13:08:08   DALLEE  $
;
; UINT WINAPI REP4_HARD_UNIQUE(WORD uDrive, LPSTR lpBuffer);
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
; $Log:   S:\certlib4\vcs\rep4_bhu.asv  $
;  
;     Rev 1.3   29 Nov 1994 13:08:08   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:21:16   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:31:54   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:29:34   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   10 May 1993 16:41:18   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_HARD_UNIQUE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR

                        LOCAL byCount:BYTE
                        LOCAL uCylinder:WORD
                        LOCAL bySector:BYTE
                        LOCAL byHead:BYTE
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        LDS_SI  lpBuffer

;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_HARD_UNIQUE       ENDP
;--------------------------------------
END

