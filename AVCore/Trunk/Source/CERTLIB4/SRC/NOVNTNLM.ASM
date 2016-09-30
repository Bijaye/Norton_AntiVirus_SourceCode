include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_NOTIFY_NLM                  9/3/92                        bAd
;
;       Sends message to NLM.
;
; WORD WINAPI NOV4_NOTIFY_NLM(LPSTR lpszServer,LPSTR lpszMess,
;                             LPSTR lpWorkBuffer,LPSTR lpSmallBuffer);
;
; Input:
;       1)  Server name to send message.
;       2)  Actual message.
;       3)  Buffer of at least 210 bytes
;       4)  Buffer of at least 100 bytes.
;
; Output:
;       1)  -1 on failure.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_NOTIFY_NLM PROC USES XCX XDX XSI XDI SDS SES,
        lpszServer:LPSTR,
        lpszMess:LPSTR,
        lpWorkBuffer:LPSTR,
        lpSmallBuffer:LPSTR

        LOCAL   lpfnIPX:DWORD
        LOCAL   wTransTime:WORD
        LOCAL   lpBuffer210:DWORD
        LOCAL   lpBuffer10:DWORD
        LOCAL   lpBuffer30:DWORD
        LOCAL   lpBuffer50:DWORD
        LOCAL   szPropertyName[12]:BYTE
        LOCAL   sImmedAddr[6]:BYTE

        mov     word ptr szPropertyName[0], 'EN'
        mov     word ptr szPropertyName[2], '_T'
        mov     word ptr szPropertyName[4], 'DA'
        mov     word ptr szPropertyName[6], 'RD'
        mov     word ptr szPropertyName[8], 'SE'
        mov     byte ptr szPropertyName[10], 'S'
        mov     byte ptr szPropertyName[11], 00h

        mov     ax, word ptr lpWorkBuffer
        mov     word ptr lpBuffer210, ax
        mov     ax, word ptr lpWorkBuffer+2
        mov     word ptr lpBuffer210+2, ax

        mov     ax, word ptr lpSmallBuffer+2
        mov     word ptr lpBuffer10+2, ax
        mov     word ptr lpBuffer30+2, ax
        mov     word ptr lpBuffer50+2, ax

        mov     ax, word ptr lpSmallBuffer
        mov     word ptr lpBuffer10, ax
        add     ax, 10
        mov     word ptr lpBuffer30, ax
        add     ax, 30
        mov     word ptr lpBuffer50, ax

        INVOKE  NOV4_SCAN_BINDERY,0ffffffffh,
                                  NAV_NLM,
                                  lpszServer,
                                  lpBuffer210
        or      al, al
        jnz     Err

        INVOKE  NOV4_READ_PROPERTY_VALUE,NAV_NLM,    
                                         lpszServer,
                                         01h,
                                         ADDR szPropertyName,
                                         lpBuffer210
        or      al, al
        jnz     Err

        INVOKE  NOV4_INIT_IPX
        or      dx, dx
        jz      Err
        mov     word ptr lpfnIPX, ax
        mov     word ptr lpfnIPX+2, dx

        LES_DI  lpBuffer210

        INVOKE  NOV4_IPX_GET_LOCAL_TARGET,lpfnIPX,
                                          ADDR XES[XDI+2],
                                          ADDR XES[XDI+6],
                                          ADDR XES[XDI+12],
                                          lpBuffer30,
                                          ADDR wTransTime
        or      al, al
        jnz     Err

        ; copy from the reply buffer 6 bytes - the local target
        push    SES
        push    XDI
        LDS_SI  lpBuffer30
        mov     ax, ss
        mov     SES, ax
        lea     XDI, sImmedAddr
        mov     cx, 6
        rep     movsb
        pop     XDI
        pop     SES

        INVOKE  NOV4_BUILD_IPX_HEADER,lpBuffer30,
                                      ADDR XES[XDI+2],
                                      ADDR XES[XDI+6],
                                      ADDR XES[XDI+12]

        INVOKE  NOV4_BUILD_ECB,lpBuffer50,
                               lpszMess,
                               lpBuffer30,
                               ADDR sImmedAddr

        INVOKE  NOV4_IPX_SEND_PACKET,lpfnIPX,
                                     lpBuffer50
        jmp     Done
Err:
        mov     ax, -1
Done:
        ret

NOV4_NOTIFY_NLM ENDP
;-----------------------------------------------------------------------------
END

