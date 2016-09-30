include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_OPEN                            12/9/92                         CAG
;
;      Open the CMB file.
;
; UINT WINAPI CMB4_OPEN(LPSTR Filename);
;
; Input:
;       1)  Name of CMB to open
; Output:
;       1)  FAILED (-1)
;       2)  Handle (?)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_OPEN PROC USES XBX XCX SDS XSI,
        Filename:LPSTR

        INVOKE  FIL4_OPEN,Filename,fREAD_WRITE
        ret

CMB4_OPEN      ENDP
;-----------------------------------------------------------------------------
END


