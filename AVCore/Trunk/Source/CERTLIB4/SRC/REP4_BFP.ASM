include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_PHYSICAL                 4/1/93                            JOE
;
;     Repairs floppy by using simple absolute/physical sectors.
;  $Header:   S:\certlib4\vcs\rep4_bfp.asv   1.4   29 Nov 1994 13:10:42   DALLEE  $
;
; UINT WINAPI REP4_FLOP_GENERIC(WORD uDrive, LPSTR lpBuffer, WORD uType);
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
; $Log:   S:\certlib4\vcs\rep4_bfp.asv  $
;  
;     Rev 1.4   29 Nov 1994 13:10:42   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.2.1.0   19 Oct 1993 07:21:00   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.2   12 Oct 1993 16:31:42   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.1.2.0   03 Sep 1993 19:29:30   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.1   18 May 1993 01:29:58   SZIADEH
;  minor midfications.
;  
;     Rev 1.0   10 May 1993 16:24:06   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_FLOP_PHYSICAL      PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR,
                        uType:WORD

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
        mov     ax,uDrive
        mov     byDrive,al              ; Store physical drive number
        mov     byCount ,1

        LDS_SI  lpBuffer

        cmp     uType,1
        je      R_360
        cmp     uType,2
        je      R_720
        cmp     uType,3
        je      R_12
        cmp     uType,4
        jne     R_ERROR
R_144:
        mov     ax,[XSI].N30.Flop_144
        xchg    ah,al
        cbw
        mov     uCylinder,ax
        mov     ax,[XSI].N30.Flop_144
        mov     bySector,al
        jmp     R_WRITE
R_360:
        mov     ax,[XSI].N30.Flop_360
        xchg    ah,al
        cbw
        mov     uCylinder,ax
        mov     ax,[XSI].N30.Flop_360
        mov     bySector,al
        jmp     R_WRITE
R_720:
        mov     ax,[XSI].N30.Flop_720
        xchg    ah,al
        cbw
        mov     uCylinder,ax
        mov     ax,[XSI].N30.Flop_720
        mov     bySector,al
        jmp     R_WRITE
R_12:
        mov     ax,[XSI].N30.Flop_12
        xchg    ah,al
        cbw
        mov     uCylinder,ax
        mov     ax,[XSI].N30.Flop_12
        mov     bySector,al
R_WRITE:
        mov     ax,[XSI].N30.Flop_All
        mov     byHead,ah

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,byCount,uCylinder,bySector,byHead,byDrive
        or      ax,ax                   ;function returns Zero if no errors
        jnz      R_ERROR

        LDS_SI  lpBuffer
        cmp     byte ptr[XSI],0E9h      ; Verify sector as Boot
        jz      @F
        cmp     byte ptr[XSI],0EBh
        jnz     R_ERROR
@@:
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
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

REP4_FLOP_PHYSICAL      ENDP
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
