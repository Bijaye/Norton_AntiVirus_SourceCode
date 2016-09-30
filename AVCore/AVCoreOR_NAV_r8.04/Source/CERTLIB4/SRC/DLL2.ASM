include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_FREELIBRARY                     1/8/93                          CAG
;
;       Free a library previously allocated by LoadLibrary
;
; UINT WINAPI DLL4_FREELIBRARY(WORD Handle);
;
; Input:
;       1)  Handle of library to free
; Output:
;       1)  ERROR (?)
;       2)  PASSED (0)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_FREELIBRARY PROC USES SDS XSI,
        uHandle:WORD


        mov     ds,uHandle
        xor     XSI,XSI

        INVOKE  FIL4_CLOSE,[XSI].DYNA_HEADER.hFilehandle
        INVOKE  MEM4_CONV_FREE,uHandle

        xor     ax,ax
        ret

DLL4_FREELIBRARY ENDP
;-----------------------------------------------------------------------------
END
