include CTS4.INC

if FARCALL
   SCN4_EXEC       PROTO FAR,:WORD,:LPSTR,:LPSTR,:LPSTR
else
   SCN4_EXEC       PROTO NEAR,:WORD,:LPSTR,:LPSTR,:LPSTR
endif

UniqueSegment
;----------------------------------------------------------------------------
; SCN4_BOOT                        12/2/92                             JOE
;
;       Scan sector for a boot/partition virus sig.
;
; UINT WINAPI SCN4_BOOT(LPSTR Buffer, LPSTR Data, BOOL bExecuteCode);
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
; LIBRARY:{INTEL}.LIB
;
;----------------------------------------------------------------------------
; UPDATE                            1/2/93                             CAG
;
;       32 bit, clean.
;
;-----------------------------------------------------------------------------
SCN4_BOOT PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpBuffer:LPSTR,
        lpData:LPSTR,
        bExecuteCode:BOOL

        LOCAL   uCount:WORD
        LOCAL   Skip: UINT
        LOCAL   SigLen: UINT

; ES:DI points to the sig
; DS:SI points to the boot sector in question


        LES_DI  lpData
        mov     ax,XES[XDI].HEADER.uNumALGS
        or      ax,ax                   ;make sure it is not zero
        jz      J_NoVirus
        mov     uCount,ax
        add XDI,sHEADER             ;Size of Data Header

J_Pass:
        mov     Skip, 4
        mov     SigLen, 13

        LDS_SI  lpBuffer                ; viral code
        cld
        mov     xcx,498                 ;how much to scan

        cmp     bExecuteCode,TRUE       ; Should we look for VIRSCAN.DAT path
        jne     @F                      ; and execute code?

        add     XSI ,MAX_PATH_SIZE+1
@@:
        cmp     word ptr XES[XDI+4],0E0E0h; sigs using this function must start
        jne     J_Cont                    ; with 0xdf 0xdf

;----------------------------------------------------------------------
ifdef DOS16

        cmp     byte ptr XES[XDI+6],01h   ; function zero
        jne     @F                        ;

        push    XDI
        Add     XDI, 06                 ;   4 (virus entry and clean info)
                                        ; + 2 (DF, DF)
                                        ; + 1 (reserved)
                                        ;paramteters to SNC4_EXEC:
                                        ;1. Number of signatures in INSCANB
                                        ;2. Work buffer( starts with path/VIRSCAN.DAT
                                        ;3. Sector containing virus.
                                        ;4. position of the two bytes for VIRSPxx.DAT" .
        INVOKE  SCN4_EXEC, uCount, lpBuffer, lpBuffer, vSES XDI
        pop     XDI                     ;
        or      ax, ax                  ;
        jz      J_Next                  ; no virus found
        jmp     J_Match                 ;
endif
;----------------------------------------------------------------------
@@:
        cmp     byte ptr XES[XDI+6],02h   ; function 1
        jne     J_Cont                    ; get next signature

        mov     Skip, 7
        mov     SigLen, 10

;----------------------------------------------------------------------
; look for a byte that matchs the first byte in the current signature.

J_Cont:
        lodsb                           ; Load first byte from viral code
        inc     al                      ; Encrypte byte to match signature by adding 1
        mov     XDX, XDI
        add     XDI, Skip
        cmp     al, byte ptr XES[XDI]   ; Compare with first byte in signature;byte is after 2 WORDs
        mov     XDI, XDX
        loopne  J_Cont                  ; Get next byte in scan buffer

        jcxz    J_Next                  ; no match, get next signature
;----------------------------------------------------------------------
; byte matched first byte in the signature, check the rest of the
; signature

        mov bx,cx                       ;save cx in bx to check match
                                        ;
        push    XDI                     ;save pointers
        push    XSI                     ;
                                        ;
        mov     xcx,SigLen                  ;bytes to compare
        add     XDI,Skip                   ;start checking at next position
J_CMP:                                  ;
        lodsb                           ;load al and inc buffer ptr
        inc     al                      ;encrypte to match signature by adding +1
        inc     XDI                     ;
        cmp     al, byte ptr XES[XDI]   ;is there a match
        loope   J_CMP                   ;continue if yeson if equal
                                        ;
        pop     XSI                     ;
        pop     XDI                     ;
        je      J_Match                 ;if CX==0 We have a winner!
                                        ;
        mov     cx,bx                   ;If loser then go find more
        jmp     J_Cont                  ;
                                        ;
J_Next:
        add     XDI,18                  ;18 = vid,ctl, & sig
        dec     uCount                  ;
        cmp     uCount,0                ;have we exhausted all signatures
        jne     J_Pass                  ;if not, get the next one
        jmp     J_NoVirus               ;otherwise, no virus.
J_Match:                                ;
        mov     ax,word ptr XES[XDI]    ;Get virus entry# in N30
        mov     bx,word ptr XES[XDI+2]  ;Get the clean info.
        jmp     short J_Exit

J_NoVirus:
        xor     ax,ax
J_Exit:
        ret

SCN4_BOOT ENDP
;----------------------------------------------------------------------------
END
