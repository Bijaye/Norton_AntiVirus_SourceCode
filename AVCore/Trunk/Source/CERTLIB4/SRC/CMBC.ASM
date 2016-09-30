include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_CLOSE                           12/9/92                         CAG
;
;      Close the CMB database.
;
; BOOL WINAPI CMB4_CLOSE(UINT Handle);
;
; Input:
;       1)  Handle
; Output:
;       1)  FAILED (0)
;       2)  PASSED (1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_CLOSE PROC USES XBX XCX SDS XSI,
        Filehandle:UINT


        INVOKE  FIL4_CLOSE,Filehandle

        mov     ax,1
        ret

CMB4_CLOSE      ENDP
;-----------------------------------------------------------------------------
END


