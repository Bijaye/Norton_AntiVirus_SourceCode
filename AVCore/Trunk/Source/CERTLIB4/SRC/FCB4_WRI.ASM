include CTS4.INC

UniqueSegment
;----------------------------------------------------------------------------
; FCB4_WRITE                         2/21/93                             JOE
;
;     FCB form of function for anti-stealthing
;
; UINT WINAPI FCB4_WRITE(LPSTR lpFCB_Buffer, WORD uBytes, WORD uMethod);
;
; Input:
;       1) LPTSTR to Buffer with FCB at start
;       2) Number of bytes to read
;       3) Read method:
;                       0 WRITE_SEQUENTIAL
;                       1 WRITE_RANDOM
;                       2 WRITE_RANDOM_BLOCK
;
; Output:
;       1) TRUE (1) Read bytes in buffer after FCB
;       2) FALSE(0) Failed
; 
; LIBRARY: {DOS16}.LIB
;
;---------------------------------------------------------------------------
WRITE_SEQUENTIAL         equ     0
WRITE_RANDOM             equ     1
WRITE_RANDOM_BLOCK       equ     2

FCB4_WRITE      PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFCB:LPSTR,
                uBytes:WORD,
                uMethod:WORD

                LOCAL lpSaveDTA
;-------------------------------------
        xor     XAX,XAX
        xor     XCX,XCX
        xor     XDX,XDX

        mov     ah,2Fh                  ; Get DTA
        int     21h

        mov     word ptr lpSaveDTA,bx   ; Save former DTA
        mov     word ptr lpSaveDTA+2,es

        LDS_DX  lpFCB
        push    dx

        add     dx,25h                  ; New DTA after FCB in buffer
        mov     ah,1Ah                  ; Set DTA
        int     21h

        pop     dx

        mov     si,dx
        mov     cx,uBytes
        mov     word ptr[XSI+0Eh],cx    ; Bytes to read in FCB record size.
        cmp     uMethod,WRITE_SEQUENTIAL
        je      RAND
        cmp     uMethod,WRITE_RANDOM
        je      BLOX
SEQU:
        mov     ah,15h
        jmp     INT21
RAND:
        mov     ah,22h
        jmp     INT21
BLOX:
        mov     ah,28h
        mov     cx,1
INT21:
        int     21h
        cmp     al,0
        jne     ERR
        jmp     OK
;-------------------------------------
ERR:
        xor     ax,ax
        jmp     EXIT
OK:
        mov     ax,1
EXIT:
        push    ax

        LDS_DX  lpSaveDTA               ; Restore DTA
        mov     ah,1Ah
        int     21h

        pop     ax
        ret   
FCB4_WRITE   ENDP
;--------------------------------------------------------------------------
END

