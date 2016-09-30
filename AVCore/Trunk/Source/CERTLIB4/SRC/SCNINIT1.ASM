include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; $Header:   S:\certlib4\vcs\scninit1.asv   1.6   29 Nov 1994 13:10:26   DALLEE  $
;
; SCN4_LOAD_CRC                        1/15/93                         CAG
;
;       Load the CRC table from the database.
;
; UINT WINAPI SCN4_LOAD_CRC(UINT Handle,LPSTR Buffer,LPSTR Work);
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
SCN4_LOAD_CRC PROC USES XCX SDS SES XSI XDI,
        hHandle:WORD,
        lpBuffer:LPSTR,
        lpWork:LPSTR

        xor     XCX,XCX

        LDS_SI  lpWork
        LES_DI  lpBuffer

        INVOKE  FIL4_SEEK,hHandle,uSizeVirscanHeader,fSEEK_BEG

ReadMore:
        INVOKE  FIL4_READ,hHandle,lpWork,sN30
        cmp     ax,-1
        je      J_Error
        cmp     ax,sN30
        jne     Done

        cmp     [XSI].N30.wType,CRCtype
        jne     ReadMore

        mov     ax,[XSI].N30.wCRC
        mov     XES[XDI],ax
        inc     XDI
        inc     XDI
        inc     XCX
        jmp     ReadMore

Done:
        mov     XAX,XCX
J_Error:
        ret

SCN4_LOAD_CRC ENDP
;-----------------------------------------------------------------------------
END
