include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_READ_PROPERTY_VALUE             9/2/92                        bAd
;
;       Gets the value of object's property.       
;
; BYTE WINAPI NOV4_READ_PROPERTY_VALUE(WORD wObjectType,LPSTR lpszObjectName,
;                                      BYTE bSegmentNumber, 
;                                      LPSTR lpszPropertyName,LPSTR
;                                      lpReadPropertyValueReply);
;
; Input:
;       1)  Object type.
;       2)  Object name.
;       3)  Segment number (01h if first call).
;       4)  Property Name
;       5)  Reply buffer of at least 210 bytes.
;
; Output:
;       1)  0 on success
;       2)  Reply Buffer
;
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_READ_PROPERTY_VALUE PROC USES XCX XSI XDI SDS SES,
        wObjectType:WORD,
        lpszObjectName:LPSTR,
        bSegmentNumber:BYTE,
        lpszPropertyName:LPSTR,
        lpReadPropertyValueReply:LPSTR

        LOCAL   bObjectNameLength:BYTE

        LDS_SI  lpszObjectName
        LES_DI  lpReadPropertyValueReply
        add     XDI, sReadPropertyValueRequest
        add     XDI, ReadPropertyValueRequest.sObjectName
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
        mov     bObjectNameLength, al

        mov     al, bSegmentNumber
        stosb
        push    XDI                     ; point to PropertyNameLength
        inc     XDI
        LDS_SI  lpszPropertyName
        mov     cx, 15
        cld
J_1:
        lodsb
        or      al, al
        jz      EndString2
        stosb
        loop    J_1

EndString2:
        mov     ax, 15
        sub     ax, cx
        pop     XDI
        mov     byte ptr XES[XDI], al

        LDS_SI  lpReadPropertyValueReply
        add     XSI, sReadPropertyValueRequest

        mov     al, bObjectNameLength
        mov     XDS[XSI].ReadPropertyValueRequest.bObjectNameLength, al
        mov     XDS[XSI].ReadPropertyValueRequest.wReadPropertyValueRequestLength,sReadPropertyValueRequest-2
        mov     XDS[XSI].ReadPropertyValueRequest.bReserved, 3dh

        mov     al, byte ptr wObjectType
        mov     byte ptr XDS[XSI].ReadPropertyValueRequest.wObjectType+1, al
        mov     al, byte ptr wObjectType+1
        mov     byte ptr XDS[XSI].ReadPropertyValueRequest.wObjectType, al

        LES_DI  lpReadPropertyValueReply
        mov     XES[XDI].ReadPropertyValueReply.wReadPropertyValueReplyLength, sReadPropertyValueReply-2

        mov     ah,0E3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif

        ret

NOV4_READ_PROPERTY_VALUE ENDP   
;-----------------------------------------------------------------------------
END

