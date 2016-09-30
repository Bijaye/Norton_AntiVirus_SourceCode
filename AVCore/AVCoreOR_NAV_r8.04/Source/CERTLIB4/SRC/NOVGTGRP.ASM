include         CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_GETGROUPUSERS                5/14/93                     CAG.bAd
;
;       Gets all USERS in GROUP.
;
; UINT WINAPI NOV4_GETGROUPUSERS(LPSTR Group,LPSTR lpBuffer200,LPSTR 
;                                lpszUsersList,BYTE bMoreSegments)
;
; Input:
;       1)  Group name
;       2)  Buffer to use (at least 200 bytes)
;       3)  Users list (200 bytes)
;       4)  Segment number (bMoreSegments = 1).
; Output:
;       1)  Number of users
;       2)  0 If failed
;       3)  4 byte user ID list (128 bytes).
;
; LIBRARY:
;
;-----------------------------------------------------------------------------
NOV4_GETGROUPUSERS PROC USES XCX SDS SES XSI XDI,
        GroupName:LPSTR,
        lpBuffer200:LPSTR,
        lpszUsersList:LPSTR,
        bMoreSegments:BYTE

        LOCAL   bObjectLength:BYTE
        LOCAL   bPropertyNameLength:BYTE
        LOCAL   szGroupMembers[14]:BYTE
        
        mov     word ptr szGroupMembers[0],"RG"
        mov     word ptr szGroupMembers[2],"UO"
        mov     word ptr szGroupMembers[4],"_P"
        mov     word ptr szGroupMembers[6],"EM"
        mov     word ptr szGroupMembers[8],"BM"
        mov     word ptr szGroupMembers[10],"RE"
        mov     word ptr szGroupMembers[12],"S"
        mov     word ptr szGroupMembers[13], 00h

; set up static part of structure
        LES_DI  lpBuffer200
        mov     XES[XDI].NOVELLREQ_E33D.bCmd, 3dh
        mov     XES[XDI].NOVELLREQ_E33D.bObjectType, 00h
        mov     XES[XDI].NOVELLREQ_E33D.bObjectType+1, 02h
        
; copy object name into structure
        LDS_SI  GroupName
        add     XDI,NOVELLREQ_E33D.bObjectName
        xor     cx,cx
        cld
; calculate length of object name
J_0:
        inc     cx
        lodsb
        stosb
        or      al,al
        jnz     J_0
        dec     cx
        mov     bObjectLength, cl
        
; put segment number immediately following object name
        mov     al, bMoreSegments
        mov     byte ptr XES[XDI-1], al
        inc     XDI

; save length of object in static portion of buffer
        LDS_SI  lpBuffer200
        mov     XDS[XSI].NOVELLREQ_E33D.bObjectLength,cl
        
; copy property name into structure after segment number and its length
        push    XDI                     ; first byte of property name
        mov     ax, ss
        mov     SDS, ax
        lea     XSI, szGroupMembers
        xor     cx, cx
J_1:
        inc     cx
        lodsb
        stosb   
        or      al, al
        jnz     J_1
        dec     cx        
        mov     bPropertyNameLength, cl

; save the length of the property name
        pop     XDI
        mov     XES[XDI-1], cl

;---------------------------------------------------------------------
; The request buffer length is equal to the size of the allocated buffer, 
; minus two, minus the max length of the parameters strings, 
; plus the actual string length of the object name string.
;---------------------------------------------------------------------
        mov     ax, sNOVELLREQ_E33D
        sub     ax, 2+47+15
        xor     cx, cx
        mov     cl, bObjectLength 
        add     ax, cx 
        mov     cl, bPropertyNameLength
        add     ax, cx 
        LDS_SI  lpBuffer200
        mov     word ptr XDS[XSI].NOVELLREQ_E33D.wBuffLen, ax

; set up the reply buffer
        LES_DI  lpszUsersList
        mov     XES[XDI], word ptr sNOVELLREP_E33D-2

        mov     ah,0E3h
ifndef WIN16
        int     21h
else
        INVOKE  DPMI21h
endif
        cmp     al, 0FCh
        jz      Done
        or      al,al
        jnz     J_ERROR

; calculate number in group
        mov     cx, 128
        mov     ax, cx
        inc     XDI                     ; skip buffer length word
        inc     XDI
        cld
repnz   scasb
        sub     ax, cx
        mov     cx, 4
        div     cl
        dec     ax
        jmp     Done

J_ERROR:
        xor     ax,ax
Done:
        ret

NOV4_GETGROUPUSERS     ENDP
;-----------------------------------------------------------------------------
END
