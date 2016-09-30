include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit4.asv   1.5   29 Nov 1994 13:09:16   DALLEE  $
;
; SCN4_COUNT_ALG                       1/15/93                         CAG
;
;       Count the number of ALGs in the database.
;
; UINT WINAPI SCN4_COUNT_ALG(UINT Handle,LPSTR Work);
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
SCN4_COUNT_ALG PROC USES XCX SDS XSI, 
        hHandle:UINT,
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

        cmp     [XSI].N30.wType,ALGtype
        jne     ReadMore
        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_COUNT_ALG ENDP
;-----------------------------------------------------------------------------
END
