include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_BUILD_ECB                  9/3/92                        bAd
;
;       Builds ECB structure for network message.
;
; VOID WINAPI NOV4_BUILD_ECB(LPSTR ecbPacket,LPSTR lpszMessage,
;                            LPSTR lpIPXHeader,LPSTR lpImmedAddr);
;
; Input:
;       1)   Structure to fill with information.
;       2)   Message to send over network.
;       3)   Pointer to completed IPXHeader.
;       4)   Pointer to immediate address.
;
; Output:
;       1)   Completed structure.
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_BUILD_ECB PROC USES XCX XSI XDI SDS SES,
        ecbPacket:LPSTR,
        lpszMessage:LPSTR,
        lpIPXHeader:LPSTR,
        lpImmedAddr:LPSTR


        LES_DI  ecbPacket
        xor     ax, ax
        mov     word ptr XES[XDI].tagECB.sLinkAddress, ax
        mov     word ptr XES[XDI].tagECB.sLinkAddress+2, ax
        mov     word ptr XES[XDI].tagECB.sESRAddress, ax
        mov     word ptr XES[XDI].tagECB.sESRAddress+2, ax
        mov     byte ptr XES[XDI].tagECB.fInUseFlag, al

        LDS_SI  lpIPXHeader
        mov     ax, word ptr XDS[XSI].tagIPXHeader.sDestinationSocket
        mov     word ptr XES[XDI].tagECB.wSocketNumber, ax

        ; copy 6-byte node address  ECB.ImmediateAddress
        LDS_SI  lpImmedAddr
        push    XDI
        add     XDI, tagECB.sImmediateAddress
        mov     cx, 6
        cld
        rep     movsb
        pop     XDI

        mov     word ptr XES[XDI].tagECB.wFragmentCount, 0002h

        mov     ax, word ptr lpIPXHeader
        mov     word ptr XES[XDI].tagECB.sFragmentAddress1, ax
        mov     ax, word ptr lpIPXHeader+2
        mov     word ptr XES[XDI].tagECB.sFragmentAddress1+2, ax

        mov     ax, word ptr lpszMessage
        mov     word ptr XES[XDI].tagECB.sFragmentAddress2, ax
        mov     ax, word ptr lpszMessage+2
        mov     word ptr XES[XDI].tagECB.sFragmentAddress2+2, ax

        mov     ax, stagIPXHeader
        mov     XES[XDI].tagECB.wFragmentSize1, ax

        LDS_SI  lpszMessage
        xor     cx, cx
        cld
J_1:
        lodsb
        or      al, al
        jz      EndString
        inc     cx
        jmp     J_1
        
EndString:
        mov     XES[XDI].tagECB.wFragmentSize2, cx

        ret

NOV4_BUILD_ECB ENDP
;-----------------------------------------------------------------------------
END

