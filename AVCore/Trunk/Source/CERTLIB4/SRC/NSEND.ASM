include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_SENDMESSAGE                     12/20/92                        CAG
;
;       Send a MESSAGE to a USER on a network.
;
; UINT WINAPI NOV_SENDMESSAGE(LPSTR ToUser, LPSTR Message, LPSTR Buffer)
;
; Input:
;       1)  User to send to
;       2)  Message to send (55 bytes)
;       3)  Buffer to use (128 bytes)
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_SENDMESSAGE PROC USES XCX SDS SES XSI XDI,
        UserName:LPSTR,
        Message:LPSTR,
        Buffer:LPSTR


        LDS_SI  UserName
        cmp     [XSI],word ptr 'OC'
        jne     J_2
        cmp     [XSI+2],word ptr 'SN'
        jne     J_2
        cmp     [XSI+4],word ptr 'LO'
        jne     J_2
        cmp     [XSI+6],byte ptr 'E'
        jne     J_2
        cmp     [XSI+7],byte ptr 0
        jne     J_2

        LES_DI  Buffer
        mov     XES[XDI].Novell_SendConsole.Request,sSendConsole_Request-2
        mov     XES[XDI].Novell_SendConsole.Reply,sSendConsole_Reply-2
        mov     XES[XDI].Novell_SendConsole.id,09h

        LDS_SI  Message
        add     XDI,Novell_SendConsole.Message
        xor     cx,cx
J_0:
        inc     cx
        lodsb
        stosb
        or      al,al
        jnz     J_0
        dec     cx

        LES_DI  Buffer
        mov     XES[XDI].Novell_SendConsole.MessLen,cl
        LDS_SI  Buffer
        LES_DI  Buffer
        add     XDI,Novell_SendConsole.Reply
        mov     ah,0E1h
        int     21h
        jmp     J_1


J_2:
        INVOKE  NOV4_GetUserConnect,UserName,Buffer
        or      ax,ax
        jz      J_ERROR

        LES_DI  Buffer
        mov     XES[XDI].Novell_SendMessage.UserID,al
        mov     XES[XDI].Novell_SendMessage.Request,sSendMessage_Request-2
        mov     XES[XDI].Novell_SendMessage.Reply,sSendMessage_Reply-2
        mov     XES[XDI].Novell_SendMessage.id,0h
        mov     XES[XDI].Novell_SendMessage.Count,1h

        LDS_SI  Message
        add     XDI,Novell_SendMessage.Message
        xor     cx,cx
J_3:
        inc     cx
        lodsb
        stosb
        or      al,al
        jnz     J_3
        dec     cx

        LES_DI  Buffer
        LDS_SI  Buffer
        mov     XES[XDI].Novell_SendMessage.MessLen,cl
        add     XDI,Novell_SendMessage.Reply
        mov     ah,0E1h
        int     21h
        jmp     J_1

J_ERROR:
        mov     ax,1
J_1:
        xor     ah,ah
        ret

NOV4_SENDMESSAGE ENDP
;-----------------------------------------------------------------------------
END
