include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETLOGGEDIN                     12/21/92                        CAG
;
;       Retrieve users names from NOVELL, comma seperated, NULL,NULL
;
; UINT WINAPI NOV4_GETLOGGEDIN(LPSTR StorageBuffer,LPSTR WorkBuffer);
;
; Input:
;       1)  StorageBuffer = 4800 bytes
;       2)  WorkBuffer    = 128 bytes
; Output:
;       1)  Number of users retrieved.
;           StorageBuffer = "User,User,User",NULL,NULL
;
;       The user 'NOT-LOGGED-IN' will also be returned!
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_GETLOGGEDIN PROC USES XCX SDS SES XSI XDI,
        lpStorage:LPSTR,
        lpWork:LPSTR
        LOCAL   NumUser:WORD

        mov     cx,100                  ; Get 100 user names
        mov     NumUser,word ptr 0
J_4:
        LES_DI  lpWork
        LDS_SI  lpWork
        mov     [XDI].Novell_GetUserName.Request,sGetUserName_Request-2
        mov     [XDI].Novell_GetUserName.Reply,sGetUserName_Reply-2
        mov     [XDI].Novell_GetUserName.id,16h
        mov     [XDI].Novell_GetUserName.ConNumb,cl
        add     XDI,Novell_GetUserName.Reply
        mov     ah,0E3h
        int     21h

        or      al,al
        jnz     J_0

        cmp     [XSI].Novell_GetUserName.ObjType,byte ptr 1
        jne     J_0                     ; Test if a USER NAME

        LES_DI  lpStorage
        LDS_SI  lpWork
        add     XSI,Novell_GetUserName.ObjName

J_3:
        lodsb
        or      al,al
        jz      J_2
        stosb
        jmp     J_3
J_2:
        mov     al,','
        stosb
        inc     NumUser
        mov     WORD32 ptr lpStorage,XDI
J_0:

        loop    J_4

        LES_DI  lpStorage
        mov     [XDI-1],byte ptr 0

        mov     ax,NumUser
        ret

NOV4_GETLOGGEDIN	ENDP
;-----------------------------------------------------------------------------
END
