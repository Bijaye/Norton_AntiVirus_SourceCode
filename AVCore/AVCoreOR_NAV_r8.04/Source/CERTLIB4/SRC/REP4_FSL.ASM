include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_FLOP_SAVE_LOGICAL          4/1/93                              JOE/SAMIZ
;
; $Header:   S:\certlib4\vcs\rep4_fsl.asv   1.9   29 Nov 1994 13:05:04   DALLEE  $
;
;     Repairs floppy by using saved logical sector method.
;
; UINT WINAPI REP4_FLOP_SAVE_LOGICAL(WORD uDrive, LPSTR lpBuffer, WORD uType);
;
; Input:
;       1) Drive Number with logical in high byte, physical in low.
;       4) Ptr to 512k (minimum) buffer with N30 entry at its beginning.
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\rep4_fsl.asv  $
;  
;     Rev 1.9   29 Nov 1994 13:05:04   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.7.1.0   19 Oct 1993 07:22:44   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.7   12 Oct 1993 16:33:10   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.6.2.0   03 Sep 1993 19:30:04   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.6   17 Jun 1993 10:29:00   BRADR
;  Added commented out code for call to DIS4_LOGICAL_TO_ABSOLUTE. This
;  function requires another buffer.
;  
;     Rev 1.5   03 Jun 1993 11:57:54   CGUC
;  Removed buffer.
;  
;     Rev 1.4   12 May 1993 12:46:24   SZIADEH
;  
;     Rev 1.3   12 May 1993 12:43:50   SZIADEH
;  
;     Rev 1.2   11 May 1993 12:48:12   SZIADEH
;  
;  Replaced logical read with physical read. 
;  
;     Rev 1.1   10 May 1993 16:15:56   SZIADEH
;-----------------------------------------------------------------------------

REP4_FLOP_SAVE_LOGICAL       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR,
                        uType:WORD

                        LOCAL uSectorPtr:WORD
                        LOCAL byDrive:BYTE
                        LOCAL dwSector:DWORD
                        LOCAL bySecAdd:BYTE

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
        mov     uSectorPtr,ax           ; Save ptr to word
        mov     ax,[XSI].N30.Flop_All   ; add to logical sector
        mov     bySecAdd, al            ;

        mov     word ptr dwSector,0
        mov     word ptr dwSector+2,0

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,0,1,0,byDrive ; read boot sec
        or      ax,ax                   ; return ZERO if succ
        jnz     R_ERROR

        LDS_SI  lpBuffer                ;
        mov     ax,uSectorPtr           ; load offset
        add     si,ax                   ; Point si to word in virus
        mov     ax,word ptr[XSI]        ; store logical sector in ax
        add     al, bySecAdd            ; adjust field
        mov     word ptr dwSector,ax    ; store logical sector for read
                                        ; in low word

;-----------------------------------------------------------------------------
; PLEASE USE THIS CODE INSTEAD OF DIS4_LOG_TO_ABS.
; ALL THIS REQUIRES IS lpBuffer AND IT WILL BE HAPPY.
; .BradR
;-----------------------------------------------------------------------------
;        INVOKE  DIS4_GET_BPB,byDrive,ADDR stBPB,lpBuffer
;        INVOKE  DIS4_LOGICAL_TO_ABSOLUTE,dwSector,stBPB.SecPerTrack,stBPB.Heads
;-----------------------------------------------------------------------------
        INVOKE  DIS4_LOG_TO_ABS, dwSector, byDrive ; convert to abs
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
        or      ax,ax                   ;return ZERO if succ
        jnz     R_ERROR

        mov     ax,1
        jmp     R_EXIT
;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret

REP4_FLOP_SAVE_LOGICAL       ENDP
;--------------------------------------
END
