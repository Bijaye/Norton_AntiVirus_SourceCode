include         CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_ALERTBROADCAST                  5/13/92                        bAd
;
;
;
; WORD WINAPI NOV4_ALERTBROADCAST(LPSTR lpszNetFilename,LPSTR lpszMessage,
;                                 LPSTR lpBuffer200,LPSTR lpBufferTwoHun,LPSTR lpBuffer100);
;
; Input:
;       1)  Filename of file to get servers and users from.
;       2)  Message buffer of at least 80 bytes.
;       3)  Buffer of at least 210 bytes
;       4)  Buffer of at least 200 bytes
;       5)  Buffer of at least 100 bytes
; Output:
;       1)  Number of users alerted.
;
; LIBRARY: 
;
;-----------------------------------------------------------------------------
NOV4_ALERTBROADCAST PROC USES XCX XSI XDI SDS SES,
        lpszNetFilename:LPSTR,
        lpszMessage:LPSTR,
        lpBuffer200:LPSTR,
        lpBufferTwoHun:LPSTR,
        lpBuffer100:LPSTR

        LOCAL   uNumUsers:WORD
        LOCAL   bDefaultServer:BYTE
        LOCAL   uHandle:WORD
        LOCAL   stNetItem:tagNETUSERITEM
        LOCAL   dwID:DWORD
        LOCAL   bMoreSegments:BYTE

        mov     uNumUsers, 00h
        mov     bMoreSegments, 01h

; save default server
        INVOKE  NOV4_DEFAULTSERVER
        mov     bDefaultServer, al

; open NETWORK.BIN
        INVOKE  FIL4_OPEN,lpszNetFilename,fREAD
        cmp     ax, -1
        jz      Err
        mov     uHandle, ax

; read user/group entry
J_0:
        mov     ax, ss
        mov     SDS, ax
        lea     XSI, stNetItem

        INVOKE  NOV4_SETSERVER, bDefaultServer ; home server to read file
        INVOKE  FIL4_READ,uHandle,vSDS XSI,stagNETUSERITEM
        cmp     ax, -1
        jnz     J_1
        mov     uNumUsers, -1
        jmp     CloseFile

J_1:
; is it the end of the file?
        cmp     ax, stagNETUSERITEM
        jnz     CloseFile
        
; switch to that server server
        mov     al, byte ptr XDS[XSI].tagNETUSERITEM.szServer
        or      al, al
        jz      DefaultServer
        INVOKE  NOV4_GETSERVER,ADDR XDS[XSI].tagNETUSERITEM.szServer
        cmp     ax, -1
        jz      J_3
        INVOKE  NOV4_SETSERVER,al
        jmp     Broadcast
        
DefaultServer:
        INVOKE  NOV4_SETSERVER, bDefaultServer
Broadcast:
; broadcast message
        ; is it a USER? uFlag = 1(user) or 3(server/user)
        test    XDS[XSI].tagNETUSERITEM.uFlag, 00000001b       
        jnz     BroadUser

; object is group
        ; uFlag = 0(group) or 2(server/group)
        INVOKE  NOV4_GETGROUPUSERS,ADDR XDS[XSI].tagNETUSERITEM.szObject,lpBufferTwoHun,lpBuffer200,bMoreSegments
        or      ax, ax
        jz      CloseFile
        LES_DI  lpBuffer200
        LDS_SI  lpBuffer200
        add     XSI, 132                ; point to unused portion
        inc     XDI                     ; skip buffer length word
        inc     XDI
        mov     cx, 128
        cld
NextUser:
; load a user ID and see if it is valid
        or      cx, cx
        jnz     CheckValid
        jmp     J_3
CheckValid:
        mov     ax, word ptr XES[XDI]
        mov     word ptr dwID+2, ax
        mov     ax, word ptr XES[XDI+2]
        mov     word ptr dwID, ax

        add     XDI, 4                  ; point to next user ID
        sub     cx, 4

        cmp     word ptr dwID+2, 00
        jnz     ValidUser
        cmp     word ptr dwID, 00
        jnz     ValidUser
        jmp     J_3

ValidUser:
        INVOKE  NOV4_IDTONAME,dwID,vSDS XSI
        INVOKE  NOV4_GETCONNECTIONS,vSDS XSI,lpBufferTwoHun
        INVOKE  MEM4_COPY,lpBufferTwoHun,lpBuffer100, 100
        INVOKE  NOV4_SENDCONNMESSAGE,lpBuffer100,lpszMessage,lpBufferTwoHun
        jmp     NextUser

; object is user
BroadUser:
        INVOKE  NOV4_GETCONNECTIONS,ADDR XDS[XSI].tagNETUSERITEM.szObject,lpBufferTwoHun
        INVOKE  MEM4_COPY,lpBufferTwoHun,lpBuffer100, 100
        INVOKE  NOV4_SENDCONNMESSAGE,lpBuffer100,lpszMessage,lpBufferTwoHun
        inc     uNumUsers
; loop to read entry
J_3:
        jmp     J_0    

; switch back to default server
CloseFile:
        INVOKE  FIL4_CLOSE, uHandle
        INVOKE  NOV4_SETSERVER, bDefaultServer
        mov     ax, uNumUsers
        jmp     Done

Err:
        mov     ax, -1
Done:
        ret

NOV4_ALERTBROADCAST        ENDP
;-----------------------------------------------------------------------------
END



