include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETALLUSERS                     12/21/92                        CAG
;
;       Retrieve users names from NOVELL, comma seperated, NULL,NULL
;       Logged in or not.
;
; UINT WINAPI NOV4_GETALLUSERS(LPSTR StorageBuffer,LPSTR WorkBuffer);
;
; Input:
;       1)  StorageBuffer = 4800 bytes
;       2)  WorkBuffer    = 128 bytes
; Output:
;       1)  Number of users retrieved.
;           StorageBuffer = "User,User,User",NULL,NULL
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
;-----------------------------------------------------------------------------
NOV4_GETALLUSERS PROC USES XCX SDS SES XSI XDI,
        lpStorage:LPSTR,
        lpWork:LPSTR
        LOCAL   NumUser:WORD
        LOCAL   LastID:DWORD

        mov     cx,100                  ; Get 100 user names
        mov     NumUser,word ptr 0
        mov     word ptr LastID,-1
        mov     word ptr LastID+2,-1
J_4:
        LES_DI  lpWork
        LDS_SI  lpWork
        mov     XES[XDI].Novell_ScanBindery.Request,sScanBindery_Request-2
        mov     XES[XDI].Novell_ScanBindery.Reply,sScanBindery_Reply-2
        mov     XES[XDI].Novell_ScanBindery.id,37h
        mov     XES[XDI].Novell_ScanBindery.ObjType-1,0h
        mov     XES[XDI].Novell_ScanBindery.ObjType,1h
        mov     XES[XDI].Novell_ScanBindery.ObjLen,1h
        mov     XES[XDI].Novell_ScanBindery.ObjName,'*'
        mov     ax,word ptr LastID
        mov     word ptr XES[XDI].Novell_ScanBindery.LastObj,ax
        mov     ax,word ptr LastID+2
        mov     word ptr XES[XDI].Novell_ScanBindery.LastObj+2,ax
        add     di,Novell_ScanBindery.Reply
        mov     ah,0E3h
        int     21h

        cmp     al,0FCh                 ; End of list
        je      J_EOL
        or      al,al                   ; Test for error
        jnz     J_EOL

        mov     ax,word ptr [XSI].Novell_ScanBindery.ObjID
        mov     word ptr LastID,ax
        mov     ax,word ptr [XSI].Novell_ScanBindery.ObjID+2
        mov     word ptr LastID+2,ax

        cmp     [XSI].Novell_ScanBindery.rType,byte ptr 1
        jne     J_0                     ; Test if a USER NAME

        les     di,lpStorage            ; Where to store name
        LES_DI  lpStorage
        LDS_SI  lpWork
        add     XSI,Novell_ScanBindery.rText
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
        mov     word ptr lpStorage,di
J_0:
        loop    J_4

J_EOL:
        LES_DI  lpStorage
        mov     XES[XDI-1],byte ptr 0

        mov     ax,NumUser
        ret

NOV4_GETALLUSERS        ENDP
;-----------------------------------------------------------------------------
END
