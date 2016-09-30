include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_SCAN_BINDERY                     9/2/92                        bAd
;
;       Returns 0 if found object in bindery.
;
; BYTE WINAPI NOV4_SCAN_BINDERY(DWORD dwObjectID,WORD wObjectType,
;                               LPSTR lpszOjectName,LPSTR 
;                               lpScanBinderyReply);
; Input:
;       1)  If first object - 0FFFFFFFFh
;       2)  Object type (ex. 0FFFF for WILD)
;       3)  Object name (1-47 bytes)
;       4)  Reply buffer of at least 200 bytes.
; Output:
;       1)  0 on found
;       2)  Reply buffer
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_SCAN_BINDERY PROC USES XCX XSI XDI SDS SES,
        dwObjectID:DWORD,
        wObjectType:WORD,
        lpszObjectName:LPSTR,
        lpScanBinderyReply:LPSTR


        LDS_SI  lpszObjectName
        LES_DI  lpScanBinderyReply
        add     XDI, sScanBinderyRequest
        add     XDI, ScanBinderyRequest.sObjectName
        mov     cx, 47
        cld
J_0:
        lodsb
        or      al, al
        jz      EndString
        stosb
        loop    J_0
        
EndString:
        mov     ax, 47
        sub     ax, cx
        LDS_SI  lpScanBinderyReply
        add     XSI, sScanBinderyRequest
        mov     XDS[XSI].ScanBinderyRequest.bObjectNameLength, al

        mov     XDS[XSI].ScanBinderyRequest.wScanBinderyRequestLength,sScanBinderyRequest-2
        mov     XDS[XSI].ScanBinderyRequest.bReserved,37h

        mov     ax,word ptr dwObjectID
        mov     word ptr XDS[XSI].ScanBinderyRequest.dwLastObjectID,ax
        mov     ax,word ptr dwObjectID+2
        mov     word ptr XDS[XSI].ScanBinderyRequest.dwLastObjectID+2,ax

        mov     al, byte ptr wObjectType
        mov     byte ptr XDS[XSI].ScanBinderyRequest.wObjectType+1, al
        mov     al, byte ptr wObjectType+1
        mov     byte ptr XDS[XSI].ScanBinderyRequest.wObjectType, al

        LES_DI  lpScanBinderyReply
        mov     XES[XDI].ScanBinderyReply.wScanBinderyReplyLength, sScanBinderyReply-2

        mov     ah,0E3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif

        ret

NOV4_SCAN_BINDERY        ENDP
;-----------------------------------------------------------------------------
END

