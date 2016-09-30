include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit7.asv   1.9   01 Jun 1995 13:25:36   DSACKING  $
;
; SCN4_LOAD_MEMORY                        1/15/93                         CAG
;
;       Load the MEMORY table from the database.
;
; UINT WINAPI SCN4_LOAD_MEMORY(UINT Handle,LPSTR Buffer,LPSTR Work);
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
SCN4_LOAD_MEMORY PROC USES XCX XDX SDS SES XSI XDI,
        hHandle:WORD,
        lpBuffer:LPSTR,
        lpWork:LPSTR

        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

        xor     XCX,XCX
        xor     XDX,XDX
        LDS_SI  lpWork
        LES_DI  lpBuffer

ReadMore:

ifdef NLM
        pushad
        INVOKE  RelinquishControl       ; relinquish server
        popad
endif

        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        inc     XDX

        cmp     word ptr [XSI].N30.lpMem,word ptr 0
        je      ReadMore

        mov     XES[XDI],dx
        inc     XDI
        inc     XDI

        mov     XAX,XSI
        add     XAX,N30.lpMem
        INVOKE  MEM4_COPY,vSDS XAX,vSES XDI,SIZE_MEM
        add     XDI,SIZE_MEM

        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
        cmp     XAX,0                   ; zero defs is an error
        jne     J_Error

        mov     XAX,-1                  ; return it so

J_Error:
        ret

SCN4_LOAD_MEMORY ENDP
;-----------------------------------------------------------------------------
END
