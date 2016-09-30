include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit6.asv   1.6   29 Nov 1994 13:09:08   DALLEE  $
;
; SCN4_COUNT_BOOT                       1/15/93                         CAG
;
;       Count the number of BOOTs in the database.
;
; UINT WINAPI SCN4_COUNT_BOOT(UINT Handle,LPSTR Work);
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
SCN4_COUNT_BOOT PROC USES XCX SDS XSI, 
        hHandle:WORD,
        lpWork:LPSTR

        xor     XCX,XCX

        LDS_SI  lpWork

        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

ReadMore:
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        cmp     [XSI].N30.wType,BOOtype
        jne     ReadMore
        cmp     [XSI].N30.lpSig,byte ptr 0
        je      ReadMore

        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_COUNT_BOOT ENDP
;-----------------------------------------------------------------------------
END
