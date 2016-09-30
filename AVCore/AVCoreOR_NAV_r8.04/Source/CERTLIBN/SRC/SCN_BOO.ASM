;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scn_boo.asv   1.6   01 Feb 1995 20:14:02   SZIADEH  $
;;
;; Description: Defines SCN4_BOOT function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scn_boo.asv  $
;  
;     Rev 1.6   01 Feb 1995 20:14:02   SZIADEH
;  cleared the upper word of uCount
;  
;     Rev 1.5   31 Jan 1995 15:32:40   SZIADEH
;  made sure the return value is UINT.
;  
;     Rev 1.4   21 Dec 1994 19:52:06   DALLEE
;  Commented out calls to SCN4_EXEC (again!) until someone uncomments the 
;  actual function and tells me it's working.
;  
;     Rev 1.3   16 Dec 1994 22:16:10   SZIADEH
;  added 3.05 changes.
;  
;     Rev 1.2   22 Aug 1994 18:52:44   DALLEE
;  Comment out SCN4_EXEC.  Doesn't compile for .DX platform.
;  No longer using this update scheme (hopefully).
;  
;     Rev 1.1   17 Aug 1994 13:26:22   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   25 May 1994 12:18:42   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_BOOT
;
; Scan sector for a boot/partition virus sig.
;
; WORD WINAPI SCN4_BOOT ( LPSTR Buffer, LPSTR Data, BOOL bExecuteCode );
;
; Input:
;       1)  Buffer to scan for virus.
;       2)  Data to scan with
;       3)  Flag to load executable code from VIRSCAN.DAT
;               TRUE = Buffer has path to VIRSCAN.DAT at start and
;                          Boot sector data at start + MAX_PATH_SIZE + 1;
;               FALSE = Don't use SCN4_EXEC
; Output:
;       1)  AX = No Virus = 0 or Virus ID=?
;       2)  BX = Control Flags?????
;
;----------------------------------------------------------------------------
SCN4_BOOT  PROC  CODEDISTANCE  USES XBX XCX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        bExecuteCode:BOOL

        LOCAL   uCount:UINT
        LOCAL   Skip: UINT
        LOCAL   SigLen: UINT

        xor     XAX, XAX
        LES_DI  lpData
        mov     ax,XES[XDI].HEADER.uNumALGS
        or      ax,ax                   ; make sure it is not zero
        jz      J_NoVirus
        mov     uCount, XAX
        add     XDI,sHEADER             ; Size of Data Header
J_Pass:
        mov     Skip, 4
        mov     SigLen, 13

        LDS_SI  lpBuffer                ; viral code
        cld
        mov     XCX,498                 ; how much to scan

        cmp     bExecuteCode,TRUE       ; Should we look for VIRSCAN.DAT path
        jne     @F                      ; and execute code?

        add     XSI, MAX_PATH_SIZE + 1
@@:
        cmp     WORD PTR XES[XDI+4],0E0E0h; sigs using this function must start
        jne     J_Cont                    ; with 0xdf 0xdf

;----------------------------------------------------------------------
;&? Commented this out (again).  Should be IF bSYM_DOS and IFNDEF SYM_DOSX.
;   However, SCN4_EXEC is still commented out (and possibly broken) -DALLEE
;ifdef DOS16
IF 0

        cmp     BYTE PTR XES[XDI+6],01h ; function zero
        jne     @F

        push    XDI
        add     XDI, 06                 ;   4 (virus entry and clean info)
                                        ; + 2 (DF, DF)
                                        ; + 1 (reserved)
                                        ;paramteters to SNC4_EXEC:
                                        ;1. Number of signatures in INSCANB
                                        ;2. Work buffer( starts with path/VIRSCAN.DAT
                                        ;3. Sector containing virus.
                                        ;4. position of the two bytes for VIRSPxx.DAT" .
        INVOKE  SCN4_EXEC, uCount, lpBuffer, lpBuffer, vSES XDI
        pop     XDI
        or      ax, ax
        jz      J_Next                  ; no virus found
        jmp     SHORT J_Match
endif
;----------------------------------------------------------------------
@@:
        cmp     byte ptr XES[XDI+6],02h ; function 1
        jne     J_Cont                  ; get next signature

        mov     Skip, 7
        mov     SigLen, 10


;---------------------------------------;
J_Cont:                                 ; look for a byte that matchs
                                        ; the first byte in the current
                                        ; signature.

        lodsb                           ; Load first byte from viral code
        inc     al                      ; Encrypte byte to match signature by adding 1
        mov     XDX, XDI
        add     XDI, Skip
        cmp     al, byte ptr XES[XDI]   ; Compare with first byte in signature;byte is after 2 WORDs
        mov     XDI, XDX
        loopne  J_Cont                  ; Get next byte in scan buffer

        jcxz    J_Next                  ; no match, get next signature

                                        ; byte matched first byte in the
                                        ; signature, check the rest of the
                                        ; signature
        mov     XBX,XCX                 ; save cx in bx to check match

        push    XDI                     ;save pointers
        push    XSI

        mov     XCX,SigLen                  ;bytes to compare
        add     XDI,Skip                   ;start checking at next position
J_CMP:
        lodsb                           ; load al and inc buffer ptr
        inc     al                      ; encrypte to match signature
        inc     XDI
        cmp     al, BYTE PTR XES[XDI]   ; is there a match
        loope   J_CMP                   ; continue if yeson if equal

        pop     XSI
        pop     XDI
        je      J_Match                 ; if CX==0 We have a winner!

        mov     XCX,XBX                 ; If loser then go find more
        jmp     SHORT J_Cont

J_Next:
        add     XDI,18                  ; 18 = vid,ctl, & sig
        dec     uCount
        jnz     J_Pass
        jmp     SHORT J_NoVirus
J_Match:                                ; this is to guarentee the upper
        xor     XAX, XAX                ; word is cleared
        xor     XBX, XBX
        mov     ax,WORD PTR XES[XDI]    ;Get virus entry# in N30
        mov     bx,WORD PTR XES[XDI+2]  ;Get the clean info.
        jmp     SHORT J_Exit

J_NoVirus:
        xor     XAX, XAX
J_Exit:
        ret

SCN4_BOOT  ENDP

UniqueSegmentEnd
END
