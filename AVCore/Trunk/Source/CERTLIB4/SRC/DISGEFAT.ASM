include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_GET_MARKED_FAT                03/31/93                      bAd
;
;       Fills Sector and Offset with the sector/offset in the FAT of the 
;         cluster that was marked bad by bootlock.
;
; WORD WINAPI DIS4_GET_MARKED_FAT(BYTE Drive,VOID FAR *fSector,VOID FAR *fOffset, LPSTR Buffer);
;
; Input:
;       1)  Drive to unmark (80h or 81h).     
;       2)  returns logical sector of FAT with bad cluster
;       3)  returns byte offset into FAT of bad cluster
;       4)  Workbuffer of at least 1024 bytes.
; Output:
;       1)  (-1) on error
;       2)   (0) if passed
;       3)  Logical sector of FAT with bad cluster
;       4)  Byte offset into FAT of bad cluster
;
; LIBRARY: {INTEL}.LIB      
;
;-----------------------------------------------------------------------------
DIS4_GET_MARKED_FAT PROC USES XCX XSI XDI SDS SES,
        Drive:BYTE,
        fSector:DWORD,
        fOffset:DWORD,
        Buffer:LPSTR

; zero fSector and fOffset out
        LDS_SI  fSector
        xor     ax, ax
        mov     word ptr XDS[XSI+2], ax
        mov     word ptr XDS[XSI], ax
        LDS_SI  fOffset
        mov     word ptr XDS[XSI], ax

;---------------------------
; need to determine if FAT was modified
; so, read our MBR and look for the 
; location of FAT stuff
;
        INVOKE  DIS4_READ_PHYSICAL,Buffer,2,0,1,0,Drive
        or      ax, ax
        jnz     Err

        mov     ax, word ptr Buffer+2
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
        or      ax, ax
        jz      Err
endif
        mov     SES, ax
        mov     XDI, word ptr Buffer
; look for 78 56 34 12
; LSN will be right after it
        mov     cx, 512                         ; search whole sector
        cld
J_0:
        mov     al, 78h
        repnz   scasb
        jcxz    Err
        cmp     byte ptr XES[XDI], 56h
        jnz     J_0
        inc     XDI
        cmp     word ptr XES[XDI], 1234h        ; extra check
        jnz     J_0

        inc     XDI
        inc     XDI                             ; point to LSN of fat


        LDS_SI  fSector
        mov     ax, word ptr XES[XDI+2]
        mov     word ptr XDS[XSI+2], ax
        mov     ax, word ptr XES[XDI]
        mov     word ptr XDS[XSI], ax
        LDS_SI  fOffset
        mov     ax, word ptr XES[XDI+4]
        mov     word ptr XDS[XSI], ax
        
        xor     ax, ax
        jmp     Done
Err:
        mov     ax, -1
Done:
        ret

DIS4_GET_MARKED_FAT ENDP
END
