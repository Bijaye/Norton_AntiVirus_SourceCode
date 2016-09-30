;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/rep4_bun.asv   1.5   07 Aug 1996 15:40:38   DALLEE  $
;;
;; Description: Defines REP4_UNIQUE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/rep4_bun.asv  $
;  
;     Rev 1.5   07 Aug 1996 15:40:38   DALLEE
;  *** &? LOOK back on rev or ONE HALF repair code! ***
;  
;  Removed One-Half specific repair.  This is now handled by NAVEX.
;  
;     Rev 1.4   10 Apr 1996 15:10:46   DALLEE
;  LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.3   16 Dec 1994 19:43:42   SZIADEH
;  included 1half.inc
;  
;     Rev 1.2   19 Aug 1994 22:11:58   RSTANEV
;  Fixed a long jump.
;
;     Rev 1.1   17 Aug 1994 13:26:04   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;
;     Rev 1.0   25 May 1994 14:54:04   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC
INCLUDE REPAIRN.INC
INCLUDE 1HALF.INC

UniqueSegment

;----------------------------------------------------------------------------
; REP4_UNIQUE
;
; Unique virus repairs go here.
;
; BOOL WINAPI REP4_UNIQUE ( LPVOID lpN30, LPVOID lpBuffer,
;                           BYTE byDrive, BYTE byType );
;
; Input:
;       lpN30           Virus entry
;       lpBuffer        work buffer
;       byDrive         physical drive number
;       byType          ?
; Output:
;       TRUE            successful
;       FALSE           fail
;----------------------------------------------------------------------------
        ECHO Warning: Fix the checks for function results (AL vs AX vs XAX).

REP4_UNIQUE  PROC  CODEDISTANCE  USES XBX XCX SDS XSI,
        lpN30:LPBYTE,
        lpBuffer:LPBYTE,
        byDrive:BYTE,
        byType:BYTE

        LOCAL uCyl:WORD
        LOCAL bySec:BYTE
        LOCAL byHead:BYTE

        cmp     byDrive,07Fh
        ja      Hard_Hard

        INVOKE  REP4_BOOT_GENERIC,lpBuffer,byDrive,byType
        or      XAX,XAX
        jz      R_ERROR
        jmp     R_EXIT

Hard_Hard:
        LDS_SI  lpN30
        mov     bx,[XSI].RepairStruct.xN30.wFixedID
        cmp     bx,FID_MONKEY
        je      MONKEY_REPAIR

        jmp     SHORT R_ERROR

MONKEY_REPAIR:
        mov     ax,[XSI].N30.HardLoc1   ; cylinder
        mov     uCyl,ax
        mov     ax,[XSI].N30.HardLoc2   ; head/sector
        mov     bySec,al
        mov     byHead,ah
        mov     bx,[XSI].N30.Restore    ; encryption key

        LDS_SI  lpBuffer
        INVOKE  DIS4_READ_PHYSICAL,vSDS XSI,1,uCyl,bySec,byHead,byDrive
        push    XSI
        mov     XCX,200h                ; 512 bytes

DecryptNextByte:
        xor     [XSI],bl                ; bl == decryption key
        inc     XSI
        loop    DecryptNextByte
        pop     XSI
        cmp     WORD PTR [XSI+510],0AA55h
        jne     R_ERROR
        INVOKE  DIS4_WRITE_PHYSICAL,lpBuffer,1,0,1,0,byDrive
        or      XAX,XAX
        jnz     R_ERROR
        mov     XAX,TRUE                ; TRUE
        jmp     SHORT R_EXIT

R_ERROR:
        xor     XAX,XAX
R_EXIT:
        ret

REP4_UNIQUE  ENDP

UniqueSegmentEnd
END
