include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit3.asv   1.6   29 Nov 1994 13:06:56   DALLEE  $
;
; SCN4_LOAD_ALG                        1/15/93                         CAG
;
;       Load the ALG table from the database.
;
; UINT WINAPI SCN4_LOAD_ALG(UINT Handle,LPSTR IDs,LPSTR Buffer,LPSTR Work);
;
; Input:
;       1)  Handle of opened file
;       2)  Where the FIXED ids are stored.
;       3)  Buffer where to store
;       4)  Work buffer, size of an Database Entry
; Output:
;       1)  Count (?)
;       2)  ERROR (-1)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
SCN4_LOAD_ALG PROC USES XCX XDX SDS SES XSI XDI,
        hHandle:WORD,
        lpIDs:LPSTR,
        lpBuffer:LPSTR,
        lpWork:LPSTR


        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG


        LES_DI  lpBuffer
        xor     XCX,XCX
        xor     XDX,XDX

ReadMore:
        LDS_SI  lpWork
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        inc     XDX

        cmp     [XSI].N30.wFixedID,0
        je      NoFixedID

        xor     XAX,XAX
        mov     ax,[XSI].N30.wFixedID
        LDS_SI  lpIDs
        shl     XAX,1                   ; Make WORD length
        add     XSI,XAX
        mov     [XSI],dx
        LDS_SI  lpWork

NoFixedID:
        cmp     [XSI].N30.wType,ALGtype
        jne     ReadMore

        mov     XAX,XSI
        add     XAX,N30.lpSig
        INVOKE  MEM4_COPY,vSDS XAX,vSES XDI,SIZE_SIG
        add     XDI,SIZE_SIG
        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_LOAD_ALG ENDP
;-----------------------------------------------------------------------------
END
