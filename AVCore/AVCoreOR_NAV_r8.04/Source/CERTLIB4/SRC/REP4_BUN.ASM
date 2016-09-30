include CTS4.INC
include REPAIR.INC

UniqueSegment
;-----------------------------------------------------------------------------
; REP4_UNIQUE                                           SAMIZ
;    Unique virus repairs goes here.
;
; $Header:   S:/CERTLIB4/VCS/rep4_bun.asv   1.8   07 Aug 1996 12:39:52   DALLEE  $
;
; UINT WINAPI REP4_UNIQUE  (lpN30:LPSTR,lpBuffer:LPSTR,byDrive:BYTE)
;
; Input:
;   lpN30:  Virus entry
;   lpBuffer: work buffer
;   byDrive: physical drive number
; Output:
;   TRUE  succ
;   FALSE fail
;
; LIBRARY: {INTEL}.LIB
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIB4/VCS/rep4_bun.asv  $
;  
;     Rev 1.8   07 Aug 1996 12:39:52   DALLEE
;  *** &? LOOK back one rev for ONE-HALF specific repair ***
;  
;  Removed call to one-half specific repair - now handled by NAVEX.
;  
;     Rev 1.7   29 Nov 1994 13:04:50   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.5.1.1   23 Oct 1994 18:24:18   SZIADEH
;  added call to one half repair. function is defined in ONEHALF.asm
;  
;     Rev 1.5.1.0   19 Oct 1993 07:22:44   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.5   12 Oct 1993 16:33:12   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.4.1.0   03 Sep 1993 19:30:04   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.4   20 Aug 1993 00:55:00   SZIADEH
;  Corrected return value.
;  
;     Rev 1.3   01 Aug 1993 10:14:00   SZIADEH
;  Added repair for Monkey class of viruses.
;  
;     Rev 1.1   21 Jun 1993 16:58:36   SZIADEH
;  Modified code to work under WINDOWS...
;  
;     Rev 1.0   14 Jun 1993 14:28:18   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
REP4_UNIQUE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                        lpN30:LPSTR,
                        lpBuffer:LPSTR,
                        lpSEG_OFF:LPSTR,
                        byDrive:BYTE,
                        byType :BYTE

                  LOCAL uCyl:WORD
                  LOCAL bySec:BYTE
                  LOCAL byHead:BYTE
;-------------------------------------
       cmp     byDrive, 07fh
       ja      Hard_Hard

       INVOKE  REP4_BOOT_GENERIC, lpBuffer, lpSEG_OFF, byDrive, byType
       or      ax, ax
       je      R_ERROR

       jmp     R_EXIT

Hard_Hard:
       LDS_SI  lpN30
       mov     bx, word ptr[XSI].RepairStruct.xN30.wFixedID
       cmp     bx, FID_MONKEY
       je      MONKEY_REPAIR

       jmp     R_ERROR

MONKEY_REPAIR:
       mov     ax,word ptr[XSI].N30.HardLoc1         ; cylinder
       mov     uCyl, ax
       mov     ax,word ptr[XSI].N30.HardLoc2           ; head/sector
       mov     bySec,  al
       mov     byHead, ah
       mov     bx, word ptr[XSI].N30.Restore            ;encryption key

       INVOKE  DIS4_READ_PHYSICAL,lpSEG_OFF,1,uCyl,bySec,byHead,byDrive
       LDS_SI  lpBuffer
       push    XSI
       mov     cx, 200h         ; 512 bytes

DecryptNextByte:
       xor     byte ptr[XSI], bl  ;bl == decryption key
       inc     XSI
       loop    DecryptNextByte
       pop     XSI
       cmp     word ptr[XSI+510], 0AA55h
       jne     R_ERROR
       INVOKE  DIS4_WRITE_PHYSICAL,lpSEG_OFF,1,0,1,0,byDrive
       or      ax,ax
       jnz     R_ERROR
       mov     ax,01           ; TRUE
       jmp     R_EXIT
;--------------------------------------
R_ERROR:
        xor     ax,ax
;--------------------------------------
R_EXIT:
        ret
REP4_UNIQUE       ENDP
;--------------------------------------
END
