include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit5.asv   1.5   29 Nov 1994 13:05:48   DALLEE  $
;
; SCN4_LOAD_BOOT                        1/15/93                         CAG
;
;       Load the BOOT table from the database.
;
; UINT WINAPI SCN4_LOAD_BOOT(UINT Handle,LPSTR Buffer,LPSTR Work);
;
; Input:
;       1)  Handle of opened file
;       2)  Buffer where to store
;       3)  Work buffer, size of an Database Entry
; Output:
;       1)  Count (?)
;       2)  ERROR (-1)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
SCN4_LOAD_BOOT PROC USES XCX XDX SDS SES XSI XDI,
        hHandle:UINT,
        lpBuffer:LPSTR,
        lpWork:LPSTR


        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

        xor     XCX,XCX
        xor     XDX,XDX
        LDS_SI  lpWork
        LES_DI  lpBuffer

ReadMore:
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        inc     XDX

        cmp     [XSI].N30.wType,BOOtype
        jne     ReadMore
        cmp     [XSI].N30.lpSig,byte ptr 0
        je      ReadMore

        mov     XES[XDI],dx
        inc     XDI
        inc     XDI

        mov     ax,[XSI].N30.wControl
        mov     XES[XDI],ax
        inc     XDI
        inc     XDI

        mov     XAX,XSI
        add     XAX,N30.lpSig
        INVOKE  MEM4_COPY,vSDS XAX,vSES XDI,SIZE_BOOTSIG
        add     XDI,SIZE_BOOTSIG


        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_LOAD_BOOT ENDP
;-----------------------------------------------------------------------------
END
