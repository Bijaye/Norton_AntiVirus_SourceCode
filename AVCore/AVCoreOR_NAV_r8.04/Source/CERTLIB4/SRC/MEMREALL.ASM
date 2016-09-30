include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GREALLOCATE                     1/5/93                          CAG
;
;       Reallocate memory.
;
; WORD WINAPI MEM4_GREALLOCATE(WORD GHandle,WORD LHandle,WORD Size);
;
; Input:
;       1)  Initialized handle from INIT
;       2)  Handle to reallocate
;       3)  Size to reallocate to (PARAGRAPHS)
;
; Output:
;       1)  LHANDLE to local data, needs to be LOCKED
;       2)  ERROR (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;       Note, with this function we can only DECREASE the size of
;       allocated memory.
;
;-----------------------------------------------------------------------------
MEM4_GREALLOCATE PROC USES XBX XCX SDS SES XSI XDI,
        lpGMemory:WORD,
        uHandle:WORD,
        uSize:WORD

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

;--------------------------------------
; Try reallocating data in HMA

;--------------------------------------
; Try reallocating data in DATA
        mov     bx,[XSI].GMEMORY.lpDATA
        or      bx,bx
        jz      NoDATA

        INVOKE  ReAllocateMemory,uHandle,bx,uSize
        or      ax,ax
        jnz     J_Done
NoDATA:


NoTryData:
;--------------------------------------
; Try to reallocate memory in UMB
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB

        INVOKE  ReAllocateMemory,uHandle,bx,uSize
        or      ax,ax
        jnz     J_Done
NoUMB:
;--------------------------------------
; Try to reallocate memory in EMS
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS

        INVOKE  MEM4_EMS_LOCK,bx
        or      ax,ax
        jz      NoEMS

        mov     cx,ax
        INVOKE  ReAllocateMemory,uHandle,bx,uSize
        push    ax
        INVOKE  MEM4_EMS_UNLOCK,bx
        pop     ax
        or      ax,ax
        jnz     J_Done
NoEMS:
;--------------------------------------
; Try to reallocate memory in CONV
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        INVOKE  ReAllocateMemory,uHandle,bx,uSize
        or      ax,ax
        jnz     J_Done

NoCONV:

;--------------------------------------
J_Done:

        ret

MEM4_GREALLOCATE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; REALLOCATEMEMORY                     1/5/93                          CAG
;
;       The routine that actually does the work.
;
; WORD WINAPI REALLOCATEMEMORY(WORD Handle,WORD Segment,WORD Size);
;
; Input:
;       1)  Handle to reallocate size of
;       2)  Segment to look in
;       3)  Size of memory (PARAGRAPH)
; Output:
;       1)  ERROR (0)
;       2)  HANDLE
;
;-----------------------------------------------------------------------------
REALLOCATEMEMORY PROC USES XBX XCX SDS XSI,
        hHandle:WORD,
        lpSegment:WORD,
        uSize:WORD

        MOV_ES  lpSegment
        xor     XDI,XDI
        mov     bx,hHandle

SearchMore:
        cmp     XES[XDI].LMEMORY.hHandle,bx
        je      FoundIt
        cmp     XES[XDI].LMEMORY.lpNEXT,0
        je      J_Error
        MOV_ES  XES[XDI].LMEMORY.lpNEXT
        jmp     SearchMore

J_Error:
        xor     ax,ax
        jmp     J_Done

FoundIt:
        mov     cx,uSize
        cmp     XES[XDI].LMEMORY.uSize,cx
        jb      J_Error                 ; Can't make larger

        mov     bx,XES[XDI].LMEMORY.uSize ; BX = Original size
        mov     XES[XDI].LMEMORY.uSize,cx

        sub     bx,cx
        xchg    bx,cx                   ; New size in CX

        mov     ax,XES[XDI].LMEMORY.lpNEXT
        MOV_DS  ax                      ; DS = Next Segment
        xor     XSI,XSI

        cmp     [XSI].LMEMORY.hHandle,0 ; Allocated???
        je      NotAllocated

;--------------------------------------
; Just make a hole in memory
        mov     bx,es
        mov     dx,es
        add     bx,sGMEMORY/16 + 1      ; Allow for this header

        mov     di,[XSI].LMEMORY.lpNEXT
        mov     [XSI].LMEMORY.lpNEXT,bx

;--------------------------------------
; Now setup next segment
        mov     ax,[XSI].LMEMORY.hHandle
        MOV_DS  bx
        xor     XSI,XSI
        mov     [XSI].LMEMORY.hHandle,0
        mov     [XSI].LMEMORY.uSize,cx  ; New size left
        mov     [XSI].LMEMORY.lpPREV,dx ; Me
        mov     [XSI].LMEMORY.lpNEXT,di ; What I had in next
        jmp     J_Done

NotAllocated:
        mov     ax,[XSI].LMEMORY.uSize  ; Original size
        mov     di,[XSI].LMEMORY.lpNext

; cx = New size
; dx = Prev
; di = Next
; es = Reallocating segment
        mov     cx,XES[XDI].LMEMORY.uSize
        mov     bx,es
        mov     dx,es
        add     bx,sGMEMORY/16 + 1      ; Allow for this header
        add     bx,cx                   ; Add new size
        mov     XES[XDI].LMEMORY.lpNEXT,bx

        add     cx,ax                   ; New original size

;--------------------------------------
; Now setup next segment
        mov     ax,[XSI].LMEMORY.hHandle; My Handle of allocated to return
        MOV_DS  bx
        xor     XSI,XSI
        mov     [XSI].LMEMORY.hHandle,0
        mov     [XSI].LMEMORY.uSize,cx  ; New size left
        mov     [XSI].LMEMORY.lpPREV,dx ; Me
        mov     [XSI].LMEMORY.lpNEXT,di ; What I had in next

J_Done:
        ret

REALLOCATEMEMORY ENDP
;-----------------------------------------------------------------------------
END
