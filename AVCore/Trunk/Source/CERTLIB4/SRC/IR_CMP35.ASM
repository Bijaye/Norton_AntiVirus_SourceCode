; Copyright Symantec Corporation 1992-1993
;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
; $Header:   S:\certlib4\vcs\ir_cmp35.asv   1.4   29 Nov 1994 13:05:12   DALLEE  $
;
; Filename:     IR_CMP35.ASM
; Programmer:   Jimmy Kuo
; Assembler:    MASM 6.0+
; Project:      NAV 3.0
;
; Contents:     INOC_REP_CMP35
;
; Note:         Compares two 35 byte memory blocks
;
; $Log:   S:\certlib4\vcs\ir_cmp35.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:05:12   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:16:22   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:25:56   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:27:44   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   20 May 1993 08:14:42   DDREW
;  No change.
;  
;     Rev 1.0   21 Apr 1993 21:13:12   CJKUO
;  Initial revision.
include CTS4.INC

;-----------------------------------------------------------------------------
; INOC_REP_CMP35                        17APR93
;
;       Compare two 35 byte pieces of memory.
;       35 is the length of the inoculation header entry.
;
; WORD WINAPI INOC_REP_CMP35(LPSTR lpString1,LPSTR lpString2);
;
; Input:
;       1)  String1
;       2)  String2
; Output:
;       0       Strings are equal
;       nonzero Strings are different
;
; Note: routine which calls this wants DX unchanged!
;
; LIBRARY:{INTEL}.LIB
;-----------------------------------------------------------------------------

UniqueSegment
INOC_REP_CMP35 PROC USES SDS SES XSI XDI XCX,
        lpString1:LPSTR,
        lpString2:LPSTR

        cld
        LDS_SI  lpString1
        LES_DI  lpString2

        mov     cx,35
        repe    cmpsb
        mov     ax,cx
        ret

INOC_REP_CMP35 ENDP
;-----------------------------------------------------------------------------
END
