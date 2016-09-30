include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_IPX_SEND_PACKET             9/2/92                        bAd
;
;       Sends IPX packet (Event Control Block).
;
; BYTE WINAPI NOV4_IPX_SEND_PACKET(DWORD lpfnIPXRoutine,LPSTR ecbPacket);
;
; Input:
;       1)  Address of routine to handle IPX.  See NOV4_INIT_IPX.
;       2)  Packet in form of ECB to send.
;
; Output:
;       1)  0 on success.
;       2)  Returns ECB.
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_IPX_SEND_PACKET PROC USES XBX XCX XSI SES,
        lpfnIPXRoutine:DWORD,
        ecbPacket:LPSTR

        mov     ax, word ptr ecbPacket+2
        mov     SES,ax
        mov     XSI, word ptr ecbPacket
        mov     bx, 03h
        push    bp
        call    dword ptr lpfnIPXRoutine
        pop     bp

        mov     al, XES[XSI].tagECB.bCompletionCode

        ret

NOV4_IPX_SEND_PACKET ENDP
;-----------------------------------------------------------------------------
END

