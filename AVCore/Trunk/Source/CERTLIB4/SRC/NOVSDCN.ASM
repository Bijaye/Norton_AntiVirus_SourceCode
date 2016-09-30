include         CTS4.INC


if FARCALL

NOV4_SENDCONNMESSAGE_OLD  PROTO FAR,:LPSTR,:LPSTR,:LPSTR
NOV4_SENDCONNMESSAGE_NEW  PROTO FAR,:LPSTR,:LPSTR,:LPSTR

else

NOV4_SENDCONNMESSAGE_OLD  PROTO NEAR,:LPSTR,:LPSTR,:LPSTR
NOV4_SENDCONNMESSAGE_NEW  PROTO NEAR,:LPSTR,:LPSTR,:LPSTR

endif


UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_SENDCONNMESSAGE                 5/11/93                     CAG.bAd
;
;       Send a MESSAGE to CONNECTIONS on a network.
;
; UINT WINAPI NOV4_SENDCONNMESSAGE(LPSTR lpConList,LPSTR Message,LPSTR Buffer);
;
; Input:
;       1)  Connections to send message to.
;       2)  Message to send. 
;       3)  Buffer to use (128 bytes) ; New function needs at least 132 bytes
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?)
;
; LIBRARY:
;
; NOTE: If the message is longer than NOV_MAXMESSLEN bytes, this funcion will
;       send the message in seperate packets.  Netware may only receive the
;       first packet, and throw the immediately following away.  Plus,
;       Netware only buffers two broadcasts, and throws away all broadcasts
;       after them.  The console will receive all packets though.
;
;-----------------------------------------------------------------------------
NOV4_SENDCONNMESSAGE PROC USES XBX XCX SDS SES XSI XDI,
        lpConList:LPSTR,
        Message:LPSTR,
        Buffer:LPSTR

        INVOKE  NOV4_GETFILESERVERINFO, Buffer
        or      al,al
        jnz     IsError
                                        ; Now check the max users
        LES_DI  Buffer                  ; to see if the old function will work
        mov     ax, XES[XDI].Novell_GetServerInformation.MaxCons
        xchg    ah, al                  ; Info in Structure is in a different order
        cmp     ax, 256

        jl      CallOldFunction
        
        INVOKE  NOV4_SENDCONNMESSAGE_NEW, lpConList, Message, Buffer
        jmp     IsDone

CallOldFunction:
        INVOKE  NOV4_SENDCONNMESSAGE_OLD, lpConList, Message, Buffer
        jmp     IsDone

IsError:
        xor     ax,ax
IsDone:
        ret
NOV4_SENDCONNMESSAGE ENDP



NOV4_SENDCONNMESSAGE_OLD PROC USES XBX XCX SDS SES XSI XDI,
        lpConList:LPSTR,
        Message:LPSTR,
        Buffer:LPSTR

        LOCAL   fMoreMess:BYTE
        LOCAL   iCurrentMess:WORD

        xor     bx, bx                  ; index into connection list
; send it to users
J_0:
        mov     iCurrentMess, 00h       ; index into message
        LDS_SI  lpConList
        add     XSI, bx
        mov     al, byte ptr [XSI]      ; are there more??
        or      al, al  
        jnz     J_1
        mov     ah, byte ptr [XSI+1]
        or      ah, ah
        jz      Done
J_1:
        inc     bx        
Send:
        mov     fMoreMess, 00h          ; no more message to send

        LES_DI  Buffer
        mov     XES[XDI].Novell_SendMessage.UserID,al
        mov     XES[XDI].Novell_SendMessage.Request,sSendMessage_Request-2
        mov     XES[XDI].Novell_SendMessage.Reply,sSendMessage_Reply-2
        mov     XES[XDI].Novell_SendMessage.id, 0h
        mov     XES[XDI].Novell_SendMessage.Count,1h

        LDS_SI  Message
        add     XSI, iCurrentMess       ; index to current point of message
        add     XDI,Novell_SendMessage.Message
        cld
        xor     cx,cx
J_2:
        inc     cx                      ; calculate message length
        lodsb
        stosb
        cmp     cx, NOV_MAXMESSLEN      ; Message has max length
        jz      MaxMessage    
        or      al,al
        jnz     J_2
        dec     cx
        jmp     J_3

MaxMessage:                             ; only has to happen when we
        mov     fMoreMess, 01h          ; more of the message to follow
        add     iCurrentMess, cx        ; set index to point to message to follow

J_3:
        LES_DI  Buffer
        LDS_SI  Buffer
        mov     XES[XDI].Novell_SendMessage.MessLen,cl
        add     XDI,Novell_SendMessage.Reply
        push    bx                      ; save index into con list
        mov     ah,0E1h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        pop     bx                      ; restore index into con list

        cmp     fMoreMess, 01h          ; more of this message to send?
        jz      Send
        jmp     J_0

Done:
        xor     ax,ax
        ret

NOV4_SENDCONNMESSAGE_OLD ENDP



NOV4_SENDCONNMESSAGE_NEW PROC USES XBX XCX SDS SES XSI XDI,
        lpConList:LPSTR,
        Message:LPSTR,
        Buffer:LPSTR

        LOCAL   fMoreMess:BYTE
        LOCAL   iCurrentMess:WORD

        mov     iCurrentMess, 00h       ; index into message

SendN:
        mov     fMoreMess, 0            ; If this isn't fixed, it will
                                        ; repeatedly send the second part.
        LES_DI  Buffer
        mov     XES[XDI].Novell_SendMessageNew.Request,sSendMessageNew_Request-2
        mov     XES[XDI].Novell_SendMessageNew.id, 0Ah

                                        ; Now copy in the Connection List
        LDS_SI  lpConList               ; Get list from here.
        xor     ah, ah                  ; GetConnectionList returns a byte
        lodsb                           ; Get the count in al
        mov     XES[XDI].Novell_SendMessageNew.Count, ax
        mov     cx, ax
        shl     cx, 1                   ; Connection numbers are DWORDs
        shl     cx, 1
                                        ; This is where the conlist goes
        add     XDI, Novell_SendMessageNew.ConList 
        rep     movsb                   ; Copy it in

        push    di                      ; Save this position to put message
                                        ; length here.
        inc     di                      ; Skip over this spot
        LDS_SI  Message                 ; Now add the message
        add     XSI, iCurrentMess       ; index to current point of message
        cld
        xor     cx,cx
J_2N:
        inc     cx                      ; calculate message length
        lodsb
        stosb
        cmp     cx, NOV_MAXMESSLEN      ; Message has max length
        jz      MaxMessageN
        or      al,al
        jnz     J_2N
        dec     cx
        jmp     J_3N

MaxMessageN:                            ; only has to happen when we
        mov     fMoreMess, 01h          ; more of the message to follow
        add     iCurrentMess, cx        ; set index to point to message to follow

J_3N:
        pop     di                      ; Restore message length pointer
        LDS_SI  Buffer
        mov     [XDI], cl               ; Save the message length here.
        mov     ah,0E1h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        cmp     fMoreMess, 01h          ; more of this message to send?
        jz      SendN

DoneN:
        xor     ax,ax
        ret

NOV4_SENDCONNMESSAGE_NEW ENDP
;-----------------------------------------------------------------------------
END

