include CTS4.INC

UniqueSegment
;----------------------------------------------------------------------------
; FCB_CLOSE                         2/21/93                             JOE
;
;     FCB form of function for anti-stealthing
;
; UINT WINAPI FCB_CLOSE(LPSTR lpFCB);
;
; Input:
;       1) LPTSTR to FCB
;
; Output:
;       1) TRUE (1) Passed
;       2) FALSE(0) Failed
; 
; LIBRARY: {DOS16}.LIB
;
;---------------------------------------------------------------------------
FCB_CLOSE       PROC USES XBX XCX XDX SDS SES XSI XDI,
                lpFCBArray:LPSTR
      
                LOCAL uHandle:WORD
;-------------------------------------
        xor     XAX,XAX
        xor     XDX,XDX

        LDS_DX  lpFCB
        mov     ah,10h
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
FCB_CLOSE   ENDP
;--------------------------------------------------------------------------
END

