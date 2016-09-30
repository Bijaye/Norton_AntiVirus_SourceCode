include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_UNIQUE                1/28/93                      JOE
;
;     Repairs floppy by using virus specific code
;  $Header:   S:\certlib4\vcs\rep4_bfu.asv   1.3   29 Nov 1994 13:04:46   DALLEE  $
;
; UINT WINAPI REP4_FLOP_UNIQUE(WORD uDrive, LPSTR lpBuffer, WORD uType);
;
; Input:
;       1) Drive Number with logical in high byte, physcial in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bfu.asv  $
;  
;     Rev 1.3   29 Nov 1994 13:04:46   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.1.1.0   19 Oct 1993 07:21:02   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.1   12 Oct 1993 16:31:44   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.0.2.0   03 Sep 1993 19:29:30   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.0   10 May 1993 16:24:08   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_FLOP_UNIQUE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR,
                        uType:WORD
;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------

;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_FLOP_UNIQUE       ENDP
;--------------------------------------
END
