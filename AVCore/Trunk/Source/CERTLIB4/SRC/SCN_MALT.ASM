include CTS4.INC

UniqueSegment
;===========================================================================
; SCN4_MALTA                       3/28/93                            JOE
;
;       Pure Catch MtE code modified only to conform to Certlib4.
;
; UINT WINAPI SCN4_MALTA(LPSTR lpBuffer, LPSTR lpDATA)
;
; Input:
;       1) ptr Buffer to examine
;       2) ptr to DATA
;
; Output:
;       1) No Match = 0
;       2) VID from fixed ID in DATA
;=========================================================================
SCN4_MALTA     PROC USES XBX XCX XDX XSI XDI SES SDS,
                lpBuffer:LPSTR,
                uBuffer:WORD,
                lpDATA:LPSTR,
                StartStruct:LPSTR
                
;------------------------------------------------------------------------
        xor     XDI,XDI
        xor     XSI,XSI
        xor     XCX,XCX
        xor     XBX,XBX

        LES_DI  lpBuffer        
        mov     si,di
        mov     cx,10
        mov     al,9Ch
        cld
        repne   scasb
        jcxz    NoMalt
        mov     cx,4
        mov     al,50h
        repne   scasb
        jcxz    NoMalt
Malt:
        xor     bx,bx
        mov     ax,0E48Ah
        call    INCBX
        mov     ax,0C08Ah
        call    INCBX
        mov     ax,0D28Ah
        call    INCBX
        mov     ax,0C98Bh
        call    INCBX
        mov     ax,0C08Bh
        call    INCBX
        mov     ax,0DB8Bh
        call    INCBX
        mov     ax,09191h
        call    INCBX
        cmp     bx,5
        jb      NoMalt 

        LDS_SI  lpDATA                             
        mov     ax,[XSI].HEADER.wFixedIDs + FID_MALTESE
        jmp     IsMalt
NoMalt: 
        xor     ax,ax
IsMalt:
        ret

SCN4_MALTA      ENDP
;------------------------------------------------------------------------
INCBX   proc    ;routine to locate mutation pads

        cld
        mov     di,si
        mov     cx,48
IBX:
        cmp     ax,word ptr XES[XDI]
        jne     IBX2
        inc     bx
IBX2:
        inc     di
        loop    IBX
        ret

INCBX   endp
;------------------------------------------------------------------------
        END

