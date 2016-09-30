include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; STR4_DWORD_CONV                   2/21/93                          bAd
;
;       Converts a DWORD to both a DWORD and LPSTR in a different base.
;
; DWORD WINAPI STR4_DWORD_CONV(DWORD,WORD,LPSTR,WORD);
;
; Input:
;       1)  Source DWORD to convert from.
;       2)  Source's number base (0Ah or 10h).
;       3)  Destination string
;       4)  Destination number base (0Ah or 10h).
; Output:
;       1)  Source WORD to convert from.
;       2)  Source's number base.
;       3)  Destination string
;       4)  Destination number base.
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;
;-----------------------------------------------------------------------------
STR4_DWORD_CONV PROC USES XBX XCX SES XDI,
        Storage:DWORD,
        FromBase:WORD,
        wTo:LPSTR,
        ToBase:WORD

        LOCAL   wWord:DWORD


;--------------------------------------
; convert SOURCE DWORD to DEST DWORD
;
        xor     cx,cx                   ; digit counter at 0
        mov     word ptr wWord+2,00h    ; dest word
        mov     word ptr wWord,00h      ; dest word
        mov     dx,word ptr Storage+2   ; DX <- hi source word
        mov     ax,word ptr Storage     ; AX <- lo source word
J_0:
        mov     bx,ToBase               ; BX <- dest number base
        div     bx
        xchg    ax, dx                  ; AX <- remainder
        inc     cx                      ; inc digit counter

        push    dx
        xor     dx,dx
        push    cx
        dec     cx
        jcxz    J_2
J_1:
        mov     bx,FromBase
        mul     bx
        loop    J_1
J_2:
        add     word ptr wWord+2,dx
        add     word ptr wWord,ax
        pop     cx
        pop     dx

        mov     ax, dx
        xor     dx, dx
        or      ax, ax                  ; if zero then done
        jnz     J_0
                                        
        cmp     word ptr wTo+2,00h      ; Null string?
        jne     J_3
        jmp     DoWord

;-----------------------------------
; DEST DWORD to DEST ASCII string
;
J_3:
        mov     es, word ptr wTo+2      ; es:di - dest string
        mov     di, word ptr wTo

        cmp     word ptr wWord+2,00h    ; if hi == 00, skip it
        jz      J_4

        INVOKE  STR4_WORD_TO_ASCIIHEX,word ptr wWord+2,vSES XDI

        INVOKE  STR4_LENGTH,vSES XDI    ; find the length of the hi word
        add     di,ax                   ; start the low word after it

J_4:
        INVOKE  STR4_WORD_TO_ASCIIHEX,word ptr wWord,vSES XDI

DoWord:
        mov     dx,word ptr wWord+2
        mov     ax,word ptr wWord
        ret

STR4_DWORD_CONV ENDP
;-----------------------------------------------------------------------------
END
