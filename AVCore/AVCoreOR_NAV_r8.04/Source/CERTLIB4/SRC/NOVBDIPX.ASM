include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_BUILD_IPX_HEADER                  9/3/92                        bAd
;
;       Builds IPXHEader structure for network message.
;
; BYTE WINAPI NOV4_BUILD_IPX_HEADER(LPSTR ipxHeader,
;                                   LPSTR sDestinationNetwork,
;                                   LPSTR sDestinationNode,
;                                   LPSTR sDestinationSocket);
;
; Input:
;       1)  Pointer to header to fill.
;       2)  Destination Network - 4 bytes.
;       3)  Destination Node - 6 bytes.
;       4)  Destination Socket - 2 bytes.
;
; Output:
;       1)   0 on success.
;       2)   Completed structure.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_BUILD_IPX_HEADER PROC USES XCX XSI XDI SES SDS,
        ipxIPXHeader:LPSTR,
        sDestinationNetwork:LPSTR,
        sDestinationNode:LPSTR,
        sDestinationSocket:LPSTR,

        cld
        LDS_SI  sDestinationNetwork
        LES_DI  ipxIPXHeader
        add     XDI, tagIPXHeader.sDestinationNetwork
        mov     cx, 4
        rep     movsb

        LDS_SI  sDestinationNode
        LES_DI  ipxIPXHeader
        add     XDI, tagIPXHeader.sDestinationNode
        mov     cx, 6
        rep     movsb

        LDS_SI  sDestinationSocket
        LES_DI  ipxIPXHeader
        add     XDI, tagIPXHeader.sDestinationSocket
        mov     cx, 2
        rep     movsb

        LES_DI  ipxIPXHeader
        mov     XES[XDI].tagIPXHeader.bPacketType, 04h

        ret

NOV4_BUILD_IPX_HEADER ENDP
;-----------------------------------------------------------------------------
END

