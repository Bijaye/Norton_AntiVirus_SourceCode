include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit9.asv   1.6   29 Nov 1994 13:09:12   DALLEE  $
;
; SCN4_COUNT_REM                       1/15/93                         CAG
;
;       Count the number of REMARKED OUT in the database.
;
; UINT WINAPI SCN4_COUNT_REM(UINT Handle,LPSTR Work);
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
SCN4_COUNT_REM PROC USES XCX SDS XSI, 
        hHandle:WORD,
        lpWork:LPSTR

        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

        LDS_SI  lpWork
        xor     XCX,XCX

ReadMore:
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        cmp     [XSI].N30.wType,CRCtype
        je      ReadMore
        cmp     [XSI].N30.wType,ALGtype
        je      ReadMore
        cmp     [XSI].N30.wType,BOOtype
        je      ReadMore
        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_COUNT_REM ENDP
;-----------------------------------------------------------------------------
END
