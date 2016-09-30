include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GCONV                           1/15/93                         CAG
;
;       Resize the Conventional memory to what it used.
;
; WORD WINAPI MEM4_GCONV(WORD MemoryHandle);
;
; Input:
;       1)  Handle to initialized memory
; Output:
;       1)  ERROR (0) No conventional
;       2)  Segment that would be next allocated (HEADER OF)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GCONV PROC USES XBX SDS XSI,
        lpGMEMORY:WORD


        MOV_DS  lpGMEMORY
        xor     XSI,XSI

;--------------------------------------
; Search thru CONV for handle to lock
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        MOV_DS  bx
        xor     XSI,XSI

SearchMore:
        cmp     [XSI].LMEMORY.hHandle,0
        jz      FoundFreeMaybe
        cmp     [XSI].LMEMORY.lpNext,0
        jz      NoCONV
NotFree:
        MOV_DS  [XSI].LMEMORY.lpNext
        jmp     SearchMore

FoundFreeMaybe:
        cmp     [XSI].LMEMORY.lpNext,0
        jnz     NotFree

        mov     ax,ds                   ; Segment of free one
        mov     bx,[XSI].LMEMORY.lpPrev
        MOV_DS  bx                      ; Set the last next to 0
        mov     [XSI].LMEMORY.lpNext,word ptr 0
        jmp     J_Done

NoCONV:
        xor     ax,ax

J_Done:
        ret

MEM4_GCONV ENDP
;-----------------------------------------------------------------------------
END
