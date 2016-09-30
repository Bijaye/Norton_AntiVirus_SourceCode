include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_DELETE                          12/9/92                         CAG
;
;      Delete a message from the CMB.
;
; BOOL WINAPI CMB4_DELETE(UINT Message,UINT Handle,LPSTR Buffer,UINT Size);
;
; Input:
;       1)  Message number
;       2)  Handle
;       3)  Buffer to work with
;       4)  Size of buffer
; Output:
;       1)  FAILED (0)
;       2)  PASSED (1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_DELETE PROC USES XBX XCX SDS XSI,
        uMessage:UINT,
        Filehandle:UINT,
        ptrStorage:LPSTR,
        uSize:UINT


        mov     ax,1
        ret

CMB4_DELETE      ENDP
;-----------------------------------------------------------------------------
END

