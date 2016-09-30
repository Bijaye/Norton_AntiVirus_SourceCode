include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_LOGICAL                1/28/93                      JOE/SAMIZ
;
;     Repairs floppy by using simple logical sector method.
;  $Header:   S:\certlib4\vcs\rep4_bfl.asv   1.7   29 Nov 1994 13:11:10   DALLEE  $
;
; UINT WINAPI REP4_FLOP_LOGICAL(WORD uDrive, LPSTR lpBuffer, WORD uType);
;
; Input:
;       1) Drive Number with logical in high byte, physical in low.
;       2) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_bfl.asv  $
;  
;     Rev 1.7   29 Nov 1994 13:11:10   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.0   19 Oct 1993 07:18:58   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:30:22   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.2.0   03 Sep 1993 19:28:50   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   17 Jun 1993 10:35:50   BRADR
;  Added commented out code for the call to DIS4_LOGICAL_TO_ABSOLUTE.  This
;  function requires another buffer.
;  
;     Rev 1.3   03 Jun 1993 11:57:28   CGUC
;  Removed buffers.
;  
;     Rev 1.2   12 May 1993 13:11:26   SZIADEH
;  
;     Rev 1.1   11 May 1993 12:53:30   SZIADEH
;  
;  Replace logical read/write with physical read/writes.
;  
;  
;     Rev 1.0   10 May 1993 16:24:04   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_FLOP_LOGICAL       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR,
                        uType:WORD

                        LOCAL byDrive:BYTE
                        LOCAL dwSector:DWORD

                        LOCAL bySector:BYTE
                        LOCAL byHead:BYTE
                        LOCAL uCylinder:WORD

                        LOCAL stBPB:BPB_struc

;-------------------------------------
IFDEF DEBUG
int     3h
nop
ENDIF
;-------------------------------------
        mov     ax,uDrive
        mov     byDrive,ah              ; Store Logical drive number

        LDS_SI  lpBuffer

        mov     ax,[XSI].N30.wSS_reg    ; Try 360k
        cmp     uType,1
        je      R_WRITE

        mov     ax,[XSI].N30.wSP_reg    ; Try 720k
        cmp     uType,2
        je      R_WRITE

        mov     ax,[XSI].N30.wCS_reg    ; Try 1.2m
        cmp     uType,3
        je      R_WRITE

        mov     ax,[XSI].N30.wIP_reg    ; Try 1.44m
        cmp     uType,4
        jne     R_ERROR

R_WRITE:
        
        mov     word ptr dwSector,ax
        mov     word ptr dwSector + 2,0

        mov     ax,[XSI].N30.Flop_All    ; add field
        add     word ptr dwSector, ax   ;

;-----------------------------------------------------------------------------
; PLEASE USE THIS CODE INSTEAD OF DIS4_LOG_TO_ABS.
; ALL THIS REQUIRES IS lpBuffer AND IT WILL BE HAPPY.
; .BradR
;-----------------------------------------------------------------------------
;        INVOKE  DIS4_GET_BPB,byDrive,ADDR stBPB,lpBuffer
;        INVOKE  DIS4_LOGICAL_TO_ABSOLUTE,dwSector,stBPB.SecPerTrack,stBPB.Heads
;-----------------------------------------------------------------------------
        INVOKE  DIS4_LOG_TO_ABS, dwSector, byDrive
        or      ax,ax
        jz      R_ERROR

        mov     bySector, al
        mov     byHead, ah
        mov     uCylinder, dx
        

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uCylinder,bySector,byHead,byDrive
        or      ax,ax
        jnz     R_ERROR

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

REP4_FLOP_LOGICAL       ENDP
;--------------------------------------
END
