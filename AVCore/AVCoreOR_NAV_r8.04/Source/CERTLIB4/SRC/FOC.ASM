include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_OPEN_CACHE                      2/16/93                         CAG
;
;       Open a file, and setup a cache for it.
;
; UINT WINAPI FIL4_OPEN_CACHE(LPSTR lpCache, UINT uCache,
;                               LPSTR Name,WORD OpenMode)
;
; Input:
;       1)  lpCache, where to store the cache, initially NULL
;       2)  Size of cache
;       3)  Filename to open
;       4)  Open mode (fREAD will only work with cache)
; Output:
;       1)  ERROR (0)
;       2)  Filehandle
;
; LIBRARY:{INTEL}.LIB
;
; Note:
;       The cache must start out being nulled.
;
;-----------------------------------------------------------------------------
FIL4_OPEN_CACHE PROC,
        lpCache:LPSTR,
        uCache:UINT,
        lpName:LPSTR,
        uMode:WORD

        INVOKE  FIL4_OPEN,lpName,uMode
        cmp     ax, -1
        jz      Error
        jmp     Done                

Error:
        xor     ax, ax
Done:
        ret

FIL4_OPEN_CACHE ENDP
;-----------------------------------------------------------------------------
END
