include         CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETFILESERVERINFO         12/21/94        DSACKING
;
;      Get the FileServer's Information.
;
; UINT WINAPI NOV4_GETFILESERVERINFO(LPSTR lpBuffer);
;
; Input:
;       1)  lpBuffer to store name, 131 bytes max
; Output:
;       1)  Server Information in buffer
;       2)  AX non-zero If failed
;
; LIBRARY:DOS16.LIB	
; LIBRARY:DOS32.LIB	
; LIBRARY:WIN16.LIB	
; LIBRARY:WIN32.LIB	
;
;-----------------------------------------------------------------------------
NOV4_GETFILESERVERINFO PROC USES XBX SDS SES XSI XDI,
        lpBuffer:LPSTR

        LES_DI  lpBuffer
        mov     XES[XDI].Novell_GetServerInformation.Reply, sGetServerInformation_Reply-2

        LDS_SI  lpBuffer                ; Request Packet
                                        ; Length of REQUEST buffer
        mov     [XSI].Novell_GetServerInformation.Request, sGetServerInformation_Request-2
                                        ; Request Packet function number
        mov     [XSI].Novell_GetServerInformation.id, 11h

                                        ; Move to the end of the reply packet
        add     XSI, sGetServerInformation_Reply

        mov     ah,0E3h                 ; Call the interrupt
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        xor     ah,ah                   ; Return value is in AL
 
        ret

NOV4_GETFILESERVERINFO ENDP

;-----------------------------------------------------------------------------
END
