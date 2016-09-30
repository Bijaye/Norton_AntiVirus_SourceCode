include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_GET_DPB                  3/22/93                          bAd
;
;       Fills in the Drive Parameter Block struct for Drive.
;
; WORD WINAPI DIS4_GET_DPB(BYTE Drive,DPB *DPB);
;
; Input:
;       1)  Drive to read drive parameter block(1=A,2=b,etc);.
;       2)  Pointer to DPB struc
;
; Output:
;       1)  (-1) on failure
;       2)  0 on success
;
; LIBRARY:{INTEL}.LIB
;
; NOTE:
;   The Drive Parameter Block changed by one byte at offset 0fh with the
;    introduction of DOS 4.0.
;
;-----------------------------------------------------------------------------
DIS4_GET_DPB PROC USES XBX XCX XDX XSI XDI SDS SES,
        Drive:BYTE,
        lDPB:LPSTR
        
        LOCAL   DOSversion:BYTE

;----------------------------
; get DOS version number
;
        mov     ax, 3001h
        int     21h
        mov     DOSversion, al

        mov     ah, 32h
        mov     dl, Drive        
        int     21h                     ; get DPB to DS:BX
        cmp     al, -1
        jz      Err

        mov     ax, word ptr lDPB+2
        mov     SES, ax
        mov     XDI, word ptr lDPB

        cld
        mov     XSI, bx
        mov     cx, 10h                 ; up to where DOS versions differ
        rep     movsb                   ; copy it

        mov     cx, sDPB - 10h          ; get ready for the last half...
        cmp     DOSversion, 03h         ; DOS < 4.0 has only a byte
        ja      J_0                     ; but we need a word
        xor     al, al                  ; so fill in a zero
        stosb
        dec     cx
J_0:
        rep     movsb

        xor     ax, ax
        jmp     Done

Err:    
        mov     ax, -1
Done:        
        ret

DIS4_GET_DPB ENDP
;-----------------------------------------------------------------------------
END
