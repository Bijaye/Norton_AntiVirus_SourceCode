include         CTS4.INC

if FARCALL

NOV4_GETCONNECTIONS_OLD  PROTO FAR,:LPSTR,:LPSTR
NOV4_GETCONNECTIONS_NEW  PROTO FAR,:LPSTR,:LPSTR

else

NOV4_GETCONNECTIONS_OLD  PROTO NEAR,:LPSTR,:LPSTR
NOV4_GETCONNECTIONS_NEW  PROTO NEAR,:LPSTR,:LPSTR

endif


UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETCONNECTIONS                  5/11/93                     CAG.bAd
;
;      Get the USER'S connection numbers.
;
; UINT WINAPI NOV4_GETCONNECTIONS(LPSTR User,LPSTR lpszConList);
;
; Input:
;       1)  User name
;       2)  connection list to use (157 bytes)
; Output:
;       1)  Number of connections
;       2)  0 If failed
;       3)  Connection list, NULL, NULL
;
; LIBRARY:
;
;-----------------------------------------------------------------------------
NOV4_GETCONNECTIONS PROC USES XCX SDS SES XSI XDI,
        UserName:LPSTR,
        lpszConList:LPSTR

        INVOKE  NOV4_GETFILESERVERINFO, lpszConList
        or      al,al
        jnz     IsError
                                        ; Now check the max users
        LES_DI  lpszConList             ; to see if the old function will work
        mov     ax, XES[XDI].Novell_GetServerInformation.MaxCons
        xchg    ah, al                  ; Info in Structure is in a different order
        cmp     ax, 256

        jl      CallOldFunction
        
        INVOKE  NOV4_GETCONNECTIONS_NEW, UserName, lpszConList
        jmp     IsDone

CallOldFunction:
        INVOKE  NOV4_GETCONNECTIONS_OLD, UserName, lpszConList
        jmp     IsDone

IsError:
        xor     ax,ax
IsDone:
        ret
NOV4_GETCONNECTIONS     ENDP



NOV4_GETCONNECTIONS_OLD PROC USES XCX SDS SES XSI XDI,
        UserName:LPSTR,
        lpszConList:LPSTR

        LOCAL   bNumConnects:BYTE

        LES_DI  lpszConList
        mov     XES[XDI].Novell_GetUserNumb.Request,sGetUserNumb_Request-2
        mov     XES[XDI].Novell_GetUserNumb.Reply,sGetUserNumb_Reply-2
        mov     XES[XDI].Novell_GetUserNumb.id,15h
        mov     XES[XDI].Novell_GetUserNumb.ObjType,01h
        mov     XES[XDI].Novell_GetUserNumb.ObjType-1,00h

        LDS_SI  UserName
        add     XDI,Novell_GetUserNumb.ObjName
        xor     cx,cx
J_0:
        inc     cx
        lodsb
        stosb
        or      al,al
        jnz     J_0
        dec     cx

        LES_DI  lpszConList
        LDS_SI  lpszConList

        mov     XES[XDI].Novell_GetUserNumb.ObjLen,cl
        add     XDI,Novell_GetUserNumb.Reply

        mov     ah,0E3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        cmp     al, 0FCh
        jz      J_1
        or      al,al
        jnz     J_ERROR
        xor     ax,ax

; copy the connection list
        add     XSI, Novell_GetUserNumb.Conlist
        LES_DI  lpszConList
        xor     cx, cx
        mov     cl, byte ptr XDS[XSI-1]          ; how many connections
        mov     bNumConnects, cl
        cld
rep     movsb
        xor     al, al
        stosb
        stosb
        
; get number of connections
        xor     ax, ax
        mov     al, bNumConnects
        jmp     J_1

J_ERROR:
        xor     ax,ax
J_1:
        ret

NOV4_GETCONNECTIONS_OLD     ENDP


NOV4_GETCONNECTIONS_NEW PROC USES XCX SDS SES XSI XDI,
        UserName:LPSTR,
        lpszConList:LPSTR

        LOCAL   bNumConnects:BYTE

        LES_DI  lpszConList
        mov     cx, sGetUserNumbNew
        xor     al, al
        rep     stosb                   ; Clear the buffers.

        LES_DI  lpszConList
        mov     XES[XDI].Novell_GetUserNumbNew.Request,sGetUserNumbNew_Request-2
        mov     XES[XDI].Novell_GetUserNumbNew.Reply,sGetUserNumbNew_Reply-2
        mov     XES[XDI].Novell_GetUserNumbNew.id, 1Bh
        mov     XES[XDI].Novell_GetUserNumbNew.ConNumb, 0
        mov     XES[XDI].Novell_GetUserNumbNew.ConNumb+2, 0
        mov     XES[XDI].Novell_GetUserNumbNew.ObjType, 0100h ; Word Swapped!

        LDS_SI  UserName
        add     XDI,Novell_GetUserNumbNew.ObjName
NJ_0:
        lodsb
        stosb
        or      al,al
        jnz     NJ_0

        LES_DI  lpszConList
        LDS_SI  lpszConList

        mov     XES[XDI].Novell_GetUserNumbNew.ObjLen, sObjectNameLen
        add     XDI,Novell_GetUserNumbNew.Reply

        mov     ah,0E3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        cmp     al, 0FCh
        jz      NJ_1
        or      al,al
        jnz     NJ_ERROR
        xor     ax,ax

; copy the connection list
        add     XSI, Novell_GetUserNumbNew.Conlist
        LES_DI  lpszConList
        xor     cx, cx
        mov     cl, byte ptr XDS[XSI-1]          ; how many connections

; First byte will be the connection count.
        mov     al, cl
        stosb                                    ; Add her to the list.

        mov     bNumConnects, cl
        shl     cx, 1                            ; 4 * bytes for dwords
        shl     cx, 1
        cld
rep     movsb
        
; get number of connections
        xor     ax, ax
        mov     al, bNumConnects
        jmp     NJ_1

NJ_ERROR:
        xor     ax,ax
NJ_1:
        ret

NOV4_GETCONNECTIONS_NEW     ENDP




;-----------------------------------------------------------------------------
END

