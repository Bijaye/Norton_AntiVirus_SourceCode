include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_IPX_GET_LOCAL_TARGET             9/2/92                        bAd
;
;       Gets 6-byte node address of logical target.
;
; BYTE WINAPI NOV4_IPX_GET_LOCAL_TARGET(DWORD lpfnIPXRoutine,
;                                       LPSTR sDestinationNetwork,
;                                       LPSTR sDestinationNode,
;                                       LPSTR sDestinationSocket,
;                                       LPSTR lpReplyBuffer,
;                                       DWORD lpwTransportTime);
;
; Input:
;       1)  Pointer to IPX routine to call.
;       2)  Destination Network - 4 bytes.
;       3)  Destination Node - 6 bytes.
;       4)  Destination Socket - 2 bytes.
;       5)  Reply buffer of at least 20 bytes.
;       6)  Pointer to return wTransportTime
;
; Output:
;       1)  0 on success.
;       2)  Reply Buffer - 6 bytes is node address.
;       3)  Transport time
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_IPX_GET_LOCAL_TARGET PROC USES XBX XCX XSI XDI SDS SES,
        lpfnIPXRoutine:DWORD,
        sDestinationNetwork:LPSTR,
        sDestinationNode:LPSTR,
        sDestinationSocket:LPSTR,
        lpReplyBuffer:LPSTR,
        lpwTransportTime:DWORD
        

        LES_DI  lpReplyBuffer
        add     XDI, 12

        LDS_SI  sDestinationNetwork
        mov     ax, word ptr XDS[XSI]
        mov     word ptr XES[XDI], ax
        mov     ax, word ptr XDS[XSI+2]
        mov     word ptr XES[XDI+2], ax

        LDS_SI  sDestinationNode
        mov     ax, word ptr XDS[XSI]
        mov     word ptr XES[XDI+4], ax
        mov     ax, word ptr XDS[XSI+2]
        mov     word ptr XES[XDI+6], ax
        mov     ax, word ptr XDS[XSI+4]
        mov     word ptr XES[XDI+8], ax

        LDS_SI  sDestinationSocket
        mov     ax, word ptr XDS[XSI]
        mov     word ptr XES[XDI+10], ax

        mov     ax, word ptr lpReplyBuffer+2
        mov     SES, ax
        mov     XSI, word ptr lpReplyBuffer
        mov     XDI, XSI
        add     XSI, 12

        mov     bx, 02h
        push    bp
        call    dword ptr lpfnIPXRoutine
        pop     bp
        or      al, al
        jnz     Done

        LDS_SI  dword ptr lpwTransportTime
        mov     word ptr XDS[XSI], cx
Done:
        ret

NOV4_IPX_GET_LOCAL_TARGET ENDP
;-----------------------------------------------------------------------------
END

