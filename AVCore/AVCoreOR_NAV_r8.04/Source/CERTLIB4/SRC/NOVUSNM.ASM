include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_USERNAME                 3/12/93                         CAG
;
;       Get the USER that is connected to this machine.
;
; BOOL WINAPI NOV4_USERNAME(LPSTR lpBuffer);
;
; Input:
;       1)  lpBuffer to store name, 64 bytes max
; Output:
;       1)  TRUE, Got name
;       2)  FALSE, Invalid name
;
; LIBRARY:DOS16.LIB     
; LIBRARY:DOS32.LIB     
; LIBRARY:WIN16.LIB     
; LIBRARY:WIN32.LIB     
;
;-----------------------------------------------------------------------------
NOV4_USERNAME PROC USES XCX SDS SES XSI XDI,
        lpBuffer:LPSTR

        INVOKE  NOV4_USER,lpBuffer
        ret

NOV4_USERNAME ENDP
;-----------------------------------------------------------------------------
END

