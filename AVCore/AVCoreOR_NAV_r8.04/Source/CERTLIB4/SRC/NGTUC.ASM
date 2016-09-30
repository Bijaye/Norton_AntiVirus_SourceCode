include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETUSERCONNECT                  12/20/92                        CAG
;
;      Get the USERS connection number.
;
; UINT WINAPI NOV_GETUSERCONNECT(LPSTR User,LPSTR Buffer)
;
; Input:
;       1)  User name
;       2)  Buffer to use (157 bytes)
; Output:
;       1)  Connection ID
;       2)  0 If failed
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_GETUSERCONNECT PROC USES XCX SDS SES XSI XDI,
        UserName:LPSTR,
        Buffer:LPSTR

        LES_DI  Buffer
        mov     XES[XDI].Novell_GetUserNumb.Request,sGetUserNumb_Request-2
        mov     XES[XDI].Novell_GetUserNumb.Reply,sGetUserNumb_Reply-2
        mov     XES[XDI].Novell_GetUserNumb.id,15h
        mov     XES[XDI].Novell_GetUserNumb.ObjType,1h
        mov     XES[XDI].Novell_GetUserNumb.ObjType-1,0


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

        LES_DI  Buffer
        mov     XES[XDI].Novell_GetUserNumb.ObjLen,cl

        LDS_SI  Buffer
        LES_DI  Buffer
        add     XDI,Novell_GetUserNumb.Reply
        mov     ah,0E3h
        int     21h
        or      al,al
        jnz     J_ERROR
        xor     ax,ax
        mov     al,[XSI].Novell_GetUserNumb.ConList
        jmp     J_1

J_ERROR:
        xor     ax,ax
J_1:
        ret

NOV4_GETUSERCONNECT	ENDP
;-----------------------------------------------------------------------------
END
