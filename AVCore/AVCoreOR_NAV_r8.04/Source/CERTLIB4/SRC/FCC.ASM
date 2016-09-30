include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_CLOSE_CACHE                     2/16/93                         CAG
;
;       Close a previously opened cache.
;
; void WINAPI FIL4_CLOSE_CACHE(LPSTR lpCache,UINT uHandle);
;
; Input:
;       1)  Cache to close
;       2)  Handle to close
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_CLOSE_CACHE PROC,
        lpCache:LPSTR,
        uHandle:UINT


        INVOKE  FIL4_CLOSE,uHandle

        ret

FIL4_CLOSE_CACHE ENDP
;-----------------------------------------------------------------------------
END
