;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:\certlibn\vcs\scn_mte.asv   1.3   17 Aug 1994 13:26:28   CAREY  $
;;
;; Description: Defines SCN4_MTE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:\certlibn\vcs\scn_mte.asv  $
;  
;     Rev 1.3   17 Aug 1994 13:26:28   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.2   31 May 1994 16:44:20   RSTANEV
;  Changed return value from UINT to WORD.
;
;     Rev 1.1   31 May 1994 16:39:38   RSTANEV
;  Fixed a small misunderstanding.
;
;     Rev 1.0   31 May 1994 16:36:42   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; SCN4_MTE
;
; Pure Catch MtE code modified only to conform to CERTLIB.
;
; WORD WINAPI SCN4_MTE ( LPSTR lpBuffer, UINT uBuffer,
;                        LPSTR lpDATA, LPSTR lpStartStruct );
;
; Input:
;       1) ptr Buffer to examine
;       2) Size of buffer ***** MUST BE AT LEASE 256 BYTES *******
;       3) ptr to DATA
;       4) ???
;
; Output:
;       1) No Match = 0
;       2) VID from fixed ID in DATA
;----------------------------------------------------------------------------
SCN4_MTE  PROC  CODEDISTANCE  USES XBX XCX XDX XSI XDI SES SDS,
        lpBuffer:LPSTR,
        uBuffer:UINT,
        lpDATA:LPSTR,
        StartStruct:LPSTR

        LOCAL save_DI:UINT
        LOCAL LOCAL_SIZE:WORD

        xor     XAX,XAX
        xor     XBX,XBX
        xor     XCX,XCX
        xor     XDX,XDX
        xor     XSI,XSI
        xor     XDI,XDI

        mov     CX, WORD PTR uBuffer ; set CX to the size of the buffer
        jcxz    @@NOTFOUND     ; nothing to search for, so get out
        cmp     CX, 255        ; optimize by using 256 bytes only
        jb      @@CONT
        mov     CX, 255

@@CONT:

        cld                     ; shift gears just in case
        LDS_SI  lpBuffer        ; initialize pointers to the buffer
        LES_DI  lpBuffer        ; initialize pointers to the buffer
        mov     save_DI, XDI    ; record top of the buffer temporarily
        add     XDI,9           ; Backward JNZ must have at least 9 bytes
        jmp     @@AGAIN         ; go for it

@@NOTFOUND:                     ; exit point that can be reached from only three
			        ; locations in the routine
        sub     AX, AX          ; return FALSE = 0 to indicate failure
        jmp     @@EXIT          ; get out of here

@@AGAIN:

;===========================================================================
; this routine will use the following few instructions to zip
; through the buffer, without even executing the rest, thus
; giving it decent speed in operation.

        jcxz    @@NOTFOUND      ; no more to search, get out
        mov     AL, 75h         ; look for the opcode for JNZ
        repne   scasb           ; as fast as possible on 80x86
        jne     @@NOTFOUND      ; no JNZ, no MtE
			        ; yes, now check the distance value
        test    byte ptr XES[XDI], 80h  ; is it backwards: high-bit set?
        jz      @@AGAIN                ; no, look for another JNZ

        ; we found a backward JNZ, compute distance
        mov     XSI, XDI        ; leave DI alone, we need it above
        sub     XBX, XBX        ; zero out BX
        mov     BL, XES[XDI]    ; get the distance in BL
        neg     BL              ; since it is negative, we need to flip bits
        inc     XSI             ; and adjust our pointer
        sub     XSI, XBX        ; move back the pointer to destination of JNZ
        cmp     XSI, save_DI    ; is it out of region
        jl      @@AGAIN         ; yes, look for another JNZ
;        jge     @@CHECK_FLAG    ; no, we can use it, check other things

@@CHECK_FLAG:                   ; What instruction altered the flags for JNZ?
        mov     al, XES[XDI-3]  ; Get 2nd byte before 75
        cmp     al, 08Bh        ; Is it a legal move?
        je      @@CHECK_REST    ; yes, continue
        cmp     al, 02Bh        ; Is it a legal SUB?
        je      @@CHECK_REST    ; yes, continue
        mov     al, XES[XDI-5]  ; Get 4th byte before 75
        cmp     al, 081h        ; SUB opcode?
        je      @@CHECK_REST    ; yes, continue
        mov     al, XES[XDI-4]  ; Get 3rd byte before 75
        cmp     al, 083h        ; SUB opcode?  Previous 81 is word subtract.
        je      @@CHECK_REST    ; yes, continue  This one is byte opcode.
        mov     al, XES[XDI-2]  ; Get byte before 75
        cmp     al, 043h        ; Is it below?
        jb      @@AGAIN         ; yes, look for another JNZ
        cmp     al, 047h        ; Is it in INC range?
        jbe     @@CHECK_REST    ; yes, continue
        and     al, 0F0h        ; Is it an XCHG?  (0x9x)
        cmp     al, 090h
;        je      @@CHECK_REST    ; yes, continue
        jne     @@AGAIN         ; No, look for another JNZ

;===========================================================================
; at this point we found a backward JNZ that goes to a location within buf
; do more checks to avoid false +

@@CHECK_REST:

;===========================================================================
        ; here is an optimization:
        ;  top:
        ;     MOV  reg16, imm16               -> 3 bytes
        ;     XOR  [reg16 + word_off], reg16  -> 4 bytes
        ;     INC  reg16                      -> 1 byte
        ;     INC  reg16                      -> 1 byte
        ;
        ;     JNZ  top

;===========================================================================

        cmp     XBX, 9          ; should have at least 9 bytes inside the loop
        jb      @@AGAIN         ; look for another JNZ
        mov     AL, [XSI]       ; opcode at the destination of JNZ
			        ; skip if segment override
        test    AL, 1           ; optimize checks for segment override
        jnz     @@OKAY          ; cannot be an override

        cmp     AL, 2Eh         ; cs
        je      @@ADJUST
        cmp     AL, 26h         ; es
        je      @@ADJUST
        cmp     AL, 3Eh         ; ds
        je      @@ADJUST
        cmp     AL, 36h         ; ss
        jne     @@OKAY          ; no need to adjust

 @@ADJUST:

        inc     XSI             ; skip the opcode
        dec     XBX             ; decrement the count of bytes inside the loop

;===========================================================================
; following is not needed since we made sure there is at least 9 above
;       jz    @@ZERO            ; nothing left, skip more checks

        mov     AL, [XSI]       ; get the opcode after the override

@@OKAY:

        test    AL, 80h
        jz      @@ZERO

        ; make sure JNZ xx goes back to a  MOV  reg16, ??
        cmp     AL, 89h                 ; MOV  reg16/mem16, reg16
        je      @@POSSIBLE
        cmp     AL, 8Bh                 ; MOV   reg16, reg16/mem16
        je      @@POSSIBLE

;===========================================================================
; following three are actually redundant but may be added in the
; next release of MtE
;   cmp   AL, 0A1h        ; MOV  AX, mem16
;   je    @@POSSIBLE
;   cmp   AL, 0A3h        ; MOV  reg16/mem16, AX
;   je    @@POSSIBLE
;   cmp   AL, 0C7h        ; MOV  mem16/reg16, imm16
;   je    @@POSSIBLE

; or it could be B8-BF: MOV  reg16, imm16
; there are 8 possible cases

        cmp     AL, 0B8h
        jb      @@ZERO
        cmp     AL, 0BFh
        ja      @@ZERO
		                ; there is no ModRM byte since we have MOV reg,reg
        mov     AH, 0FFh        ; set dest_reg to an invalid value as a flag
        jmp     @@REG16         ; skip over

@@POSSIBLE:

;===========================================================================
; now compute the 16-bit register used
; note that this is not necessary if it involves an immediate
; therefore we had the "jmp  @@REG16" above

        mov     AH, [XSI+1]     ; modRM
        and     AH, 38h         ; isolate middle three bits
        shr     AH, 1           ; slide them to least significant three bits
        shr     AH, 1           ; one by
        shr     AH, 1           ; one

@@REG16:

        mov     DX, AX          ; save in DX, since we will use AX down

@@NEXT:                         ; at this point, BX holds the loop count

        lodsb                   ; get a byte

;===========================================================================
; Here is another optimization. 8086 opcodes that have 8 and 16 bit
; versions differ in the first bit. If it is set, then the operation
; is 16-bit, and 8 otherwise

        test    AL, 1           ; is it an opcode for 16-bit access
        jz      @@BOTTOM        ; nope, skip the rest

;===========================================================================
; we found a 16-bit instruction, check for what MtE uses

        test    AL, 40h         ; all opcodes we are interested in have 7th bit set to 0
        jnz     @@BOTTOM        ; nope, skip the rest

;===========================================================================
; let's see if we got something

        cmp     AL, 89h         ; MOV   reg16, mem16
        je      @@MATCH         ; looks good, take it
        cmp     AL, 87h         ; XCHG  reg16, mem16
        je      @@MATCH         ; looks good, take it
        cmp     AL, 33h         ; XOR   reg16, mem16
        je      @@MATCH
        cmp     AL, 31h         ; XOR   mem16, reg16
        je      @@MATCH
        cmp     AL, 2Bh         ; SUB   reg16, mem16
        je      @@MATCH
        cmp     AL, 29h         ; SUB   mem16, reg16
        jne     @@BOTTOM        ; not an expected opcode

@@MATCH:

        mov     AL, [XSI]       ; get ModRM
        mov     AH, AL          ; save a spare copy for later use
        and     AH, 0C7h        ; isolate upper two bits and lower three bits
        cmp     AH, 10000100b   ; is a MOV with 16-bit indexed or based memory access
        jb      @@BOTTOM        ; no, forget it
        cmp     AH, 10000111b   ; is a MOV with 16-bit indexed or based memory access
        ja      @@BOTTOM        ; no, forget it

;===========================================================================
; At this point we got a 16-bit memory access using based or indexed mode
; and one of the possible opcodes matched above. To avoid false positives
; on self-decompressing programs we need to check couple of other things.

        mov     AH, AL          ; find out which 16-bit register
        and     AH, 38h         ; is used as destination
        shr     AH, 1           ; by isolating the middle
        shr     AH, 1           ; three bits
        shr     AH, 1           ; and move them to the least significant three

;===========================================================================
; do it backwards and find out what opcode for MOV reg16,xx would be used
; if it is used

        mov     AL, 0B8h        ; compute a MOV Reg16,xx
        add     AL, AH          ; plus the destination register bits we got above
		                ; now AL has an opcode for a MOV reg16,??

;===========================================================================
; check the flag set above if we are dealing with a memory MOV, not reg,reg

        cmp     DH, 0FFh        ; is there a ModRM byte
        jne     @@check_ModRM   ; yes, check it
        cmp     AL, DL          ; opcode == target using immediate
        je      @@FOUND         ; HIT
        jmp     @@BOTTOM        ; skip

@@check_ModRM:

        cmp     DH, AH          ; mod == dest_ModRM using the same register
        je      @@FOUND         ; HIT
		                ; not this one, never mind
@@BOTTOM:

        dec     XBX             ; reduce the count
        jnz     @@NEXT          ; go up and examine the next byte

@@ZERO:

        jmp     @@AGAIN         ; not inside this loop, try next match on JNZ

@@FOUND:                        ; we got a HIT
        LES_DI  lpDATA
        mov     ax,XES[XDI].HEADER.wFixedIDs + FID_MTE
@@EXIT:
        ret                          ; that's all folks

SCN4_MTE  ENDP

UniqueSegmentEnd
END
