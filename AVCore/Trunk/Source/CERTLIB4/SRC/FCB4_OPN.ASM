include CTS4.INC

UniqueSegment
;----------------------------------------------------------------------------
; FCB4_OPEN                         2/21/93                             JOE
;
;     FCB form of function for anti-stealthing
;
; UINT WINAPI FCB4_OPEN(LPSTR lpFCB);
;
; Input:
;       1) LPTSTR to FCB
;
; Output:
;       1) TRUE (1) Opened FCB
;       2) FALSE(0) Failed
; 
; LIBRARY: {DOS16}.LIB
;
;---------------------------------------------------------------------------
FCB4_OPEN       PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFCBArray:LPSTR
      
                LOCAL uHandle:WORD
;-------------------------------------
        xor     XAX,XAX
        xor     XDX,XDX
        LDS_DX  lpFCB
        mov     ah,0Fh
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
FCB4_OPEN   ENDP
;--------------------------------------------------------------------------
END

