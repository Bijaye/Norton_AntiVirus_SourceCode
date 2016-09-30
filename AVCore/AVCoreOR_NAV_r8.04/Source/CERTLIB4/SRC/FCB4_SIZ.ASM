include CTS4.INC

UniqueSegment
;----------------------------------------------------------------------------
; FCB_SIZE                         2/21/93                             JOE
;
;     FCB form of function for anti-stealthing
;
; UINT WINAPI FCB_SIZE(LPSTR lpFCB);
;
; Input:
;       1) LPTSTR to FCB
;
; Output:
;       1) TRUE (1) FCB Relative Record Number field has file size in bytes.
;       2) FALSE(0) Failed
; 
; LIBRARY: {DOS16}.LIB
;
;---------------------------------------------------------------------------
FCB_SIZE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFCBArray:LPSTR
      
                LOCAL uHandle:WORD
;-------------------------------------
        xor     XAX,XAX
        xor     XDX,XDX

        LDS_DX  lpFCB
        mov     word ptr[XSI+0Eh],1    ; Record size to one byte
        mov     ah,23h
        int     21h

        cmp     al,0
        je      OK
;-------------------------------------
ERR:
        xor     ax,ax
        jmp     EXIT
OK:
        mov     ax,1
EXIT:
        ret   
FCB_SIZE   ENDP
;--------------------------------------------------------------------------
END

