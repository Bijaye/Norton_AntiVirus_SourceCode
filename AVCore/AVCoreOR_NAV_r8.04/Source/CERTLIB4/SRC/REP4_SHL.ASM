include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_HARD_SAVE_LOGICAL                1/28/93                      JOE/SAMIZ
;
;     Repairs HD where original is stored as logical in virus (Ping Pong)
;
; $Header:   S:\certlib4\vcs\rep4_shl.asv   1.8   29 Nov 1994 13:05:34   DALLEE  $
;
; UINT WINAPI REP4_HARD_SAVE_LOGICAL(WORD uDrive, LPSTR lpBuffer);
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
; $Log:   S:\certlib4\vcs\rep4_shl.asv  $
;  
;     Rev 1.8   29 Nov 1994 13:05:34   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.6.1.0   19 Oct 1993 07:22:30   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.6   12 Oct 1993 16:33:00   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.5.2.0   03 Sep 1993 19:30:00   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.5   17 Jun 1993 10:27:36   BRADR
;  Added commented out code for call to DIS4_LOGICAL_TO_ABSOLUTE.  This 
;  function will need another buffer to support it.
;  
;     Rev 1.4   03 Jun 1993 11:58:10   CGUC
;  Removed Buffer.
;  
;     Rev 1.3   18 May 1993 01:30:30   SZIADEH
;  minor midfications.
;  
;     Rev 1.2   12 May 1993 12:36:50   SZIADEH
;  
;     Rev 1.1   12 May 1993 12:34:04   SZIADEH
;  
;  Replace logical sector reads with physical.
;  
;     Rev 1.0   10 May 1993 16:41:20   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------

REP4_HARD_SAVE_LOGICAL       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        uDrive:WORD,
                        lpBuffer:LPSTR
                        
                        LOCAL uSectorPtr:WORD
                        LOCAL byDrive:BYTE
                        LOCAL dwSector:DWORD

                        LOCAL byBootSecOff:BYTE
                        LOCAL byBootHeadOff:BYTE
                        LOCAL uBootCylOff:WORD

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
        mov     byDrive,al              ; physical/absolute

        LDS_SI  lpBuffer
        mov     ax,[XSI].N30.HardLoc1   ;
        mov     uSectorPtr,ax           ; Save ptr to word

        mov     ax,[XSI].N30.Restore    ; value to add to logical sector
        mov     bySecAdd, al

        mov     word ptr dwSector,0     ;
        mov     word ptr dwSector+2,0   ;


        INVOKE  DIS4_LOC_BOOTSECTOR,byDrive,lpBuffer  ;get address of boot sector
        cmp     ax, -1                               ; error
        je      R_ERROR

        mov     byBootSecOff, al
        mov     byBootHeadOff, ah
        mov     uBootCylOff, dx

        INVOKE  DIS4_READ_PHYSICAL,lpBuffer,1,uBootCylOff,byBootSecOff,byBootHeadOff,byDrive
        or      ax,ax                   ; return ZERO if succ
        jnz     R_ERROR

        LDS_SI  lpBuffer
        mov     ax,uSectorPtr           ;
        add     si,ax                   ; Point si to word in virus
        mov     ax,word ptr[XSI]        ; store logical sector in ax
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
        INVOKE  DIS4_LOG_TO_ABS, dwSector, byDrive
        or      ax,ax
        jz      R_ERROR

        add     al, bySecAdd
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
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,uBootCylOff,byBootSecOff,byBootHeadOff,byDrive
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
REP4_HARD_SAVE_LOGICAL       ENDP
;--------------------------------------
END

