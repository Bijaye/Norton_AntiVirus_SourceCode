include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_PART_UNIQUE              4/1/93                              JOE/SAMIZ
;
;     Special MBR repairs.
; $Header:   S:\certlib4\vcs\rep4_bpu.asv   1.3   29 Nov 1994 13:05:28   DALLEE  $
;
; UINT WINAPI REP4_PART_UNIQUE(WORD uDrive, LPSTR lpBuffer);
;
; Input:
;       1) Drive Number with logical in high byte, absolute in low.
;       2) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bpu.asv  $
;  
;     Rev 1.3   29 Nov 1994 13:05:28   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:21:12   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:31:50   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:29:32   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   10 May 1993 16:35:36   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_PART_UNIQUE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR

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
        LDS_SI  lpBuffer





;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret
REP4_PART_UNIQUE       ENDP
;--------------------------------------
END

