;-----------------------------------------------------------------------------
; MEM4_COPY                            1/14/93                         CAG
;
;       Copy a memory block.
;
; VOID WINAPI MEM4_COPY(LPVOID From,LPVOID To,UINT Count);
;
; Input:
;       1)  From Buffer
;       2)  To Buffer
;       3)  Number of bytes
;
; LIBRARY: {INTEL}.LIB
;       
;
;-----------------------------------------------------------------------------
; $Header:   S:/CERTLIBN/VCS/MEMCOPY.ASv   1.1   08 Mar 1996 14:57:14   DALLEE  $
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; $Log:   S:/CERTLIBN/VCS/MEMCOPY.ASv  $
;  
;     Rev 1.1   08 Mar 1996 14:57:14   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.0   19 Jan 1995 15:14:02   SZIADEH
;  Initial revision.
;-----------------------------------------------------------------------------
include CTSN.INC

UniqueSegment

MEM4_COPY PROC USES XCX SDS SES XSI XDI,
        lpFrom:LPBYTE,
        lpTo:LPBYTE,
        uCount:UINT

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        mov     XCX,uCount
        rep     movsb

        ret

MEM4_COPY ENDP
;-----------------------------------------------------------------------------
UniqueSegmentEnd
END
