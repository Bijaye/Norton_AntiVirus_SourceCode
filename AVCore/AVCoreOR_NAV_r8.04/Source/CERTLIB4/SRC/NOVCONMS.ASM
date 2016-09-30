include         CTS4.INC

UniqueSegment
;---------------------------------------------------------------------------
; NOV4_CONSOLEMESSAGE               5/21/93                      bAd
;
;       Sends MESSAGE to the console.
;
; BYTE WINAPI NOV4_CONSOLEMESSAGE(LPSTR Message,LPSTR Buffer);
;
; Input:
;       1)  Message to send to console.
;       2)  Word buffer (128 bytes).
; Output:
;       2)  0 If failed
;
; LIBRARY:
;
; NOTE: If the message is longer than NOV_MAXMESSLEN bytes, this funcion will
;       send the message in seperate packets.  Netware may only receive the
;       first packet, and throw the immediately following away.  Plus,
;       Netware only buffers two broadcasts, and throws away all broadcasts
;       after them.  The console will receive all packets, though.
;
;---------------------------------------------------------------------------
NOV4_CONSOLEMESSAGE PROC USES XBX XCX XSI XDI SDS SES,
        Message:LPSTR,
        Buffer:LPSTR

        LOCAL   fMoreMess:BYTE
        LOCAL   iCurrentMess:WORD
        LOCAL   iTempStore:WORD

        mov     iCurrentMess, 00h       ; index into message
Send:
        mov     fMoreMess, 00h          ; no more message to send
; send it to the CONSOLE
        LES_DI  Buffer
        mov     XES[XDI].Novell_SendConsole.Request,sSendConsole_Request-2
        mov     XES[XDI].Novell_SendConsole.Reply,sSendConsole_Reply-2
        mov     XES[XDI].Novell_SendConsole.id,09h

        LDS_SI  Message
        add     XSI, iCurrentMess       ; move to current section of message
        add     XDI,Novell_SendConsole.Message
        mov     XBX, XDI
        xor     cx,cx
        cld
J_0:
        inc     cx
        lodsb
        stosb
        cmp     cx, NOV_MAXMESSLEN      ; message can't be longer than this
        jz      MaxMessage
        or      al,al
        jnz     J_0
        dec     cx
        jmp     J_1

MaxMessage:
        mov     iTempStore, cx

LookMore2:
        cmp     XES[XDI],byte ptr ' '   ; Found a nice break place
        je      SendMe

LookMore:
        cmp     XDI, XBX                ; Back to beginning already.
        je      JustSend

        dec     XDI
        dec     cx
        jmp     LookMore2

JustSend:
        mov     cx,iTempStore
        jmp     J_X
SendMe:
        mov     XES[XDI],byte ptr 0
        add     iCurrentMess, 1         ; No space to be sent
J_X:
        mov     fMoreMess, 01h          ; more of the message to follow
        add     iCurrentMess, cx        ; set index to point to message to follow

J_1:        
        LES_DI  Buffer
        mov     XES[XDI].Novell_SendConsole.MessLen,cl
        LDS_SI  Buffer
        add     XDI,Novell_SendConsole.Reply
        mov     ah,0E1h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif

        cmp     fMoreMess, 01h          ; more message to send?
        jz      Send

        ret

NOV4_CONSOLEMESSAGE ENDP
;---------------------------------------------------------------------------
END
