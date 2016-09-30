include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GALLOCATE                       1/5/93                          CAG
;
;       Allocate memory, don't care where, just get it.
;
; WORD WINAPI MEM4_GALLOCATE(WORD Handle,WORD Size,WORD Flags);
;
; Input:
;       1)  Initialized handle from INIT
;       2)  Size of memory requested (PARAGRAPHS)
;       3)  Flags of type of memory.
;               MEM_CODE
;               MEM_DATA
; Output:
;       1)  LHANDLE to local data, needs to be LOCKED
;       2)  ERROR (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GALLOCATE PROC USES XBX XCX SDS SES XSI XDI,
        lpGMemory:WORD,
        uSize:WORD,
        uFlags:WORD

        LOCAL   bHMA:BYTE

        mov     bHMA,FALSE

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

        test    uFlags,MEM_CODE         ; If code, must not be in DATA or HMA
        jnz     NoTryData

;--------------------------------------
; Try allocating data in HMA
        test    uFlags,MEM_FIXED        ; If FIXED it could be in the HMA
        jz      NotFixed
        cmp     word ptr [XSI].GMEMORY.lpHMA+2,0
        jz      NoHMA

        INVOKE  MEM4_HMA,HMA_STATUS     ; Get HMA Status
        or      ax,ax
        jnz     IsON
        INVOKE  MEM4_HMA,HMA_ON
        mov     bHMA,TRUE               ; Was turned on, must turn off
IsON:

        mov     dx,uSize
        shl     dx,1
        shl     dx,1
        shl     dx,1
        shl     dx,1
        INVOKE  MEM4_LocalAllocate,lpGMEMORY,[XSI].GMEMORY.lpHMA,dx

        cmp     bHMA,TRUE               ; Was turned on, must turn off
        jne     NotOn
        push    ax
        INVOKE  MEM4_HMA,HMA_OFF
        pop     ax
NotOn:

        or      ax,ax
        jnz     J_Done
NoHMA:
        jmp     NoTryData

;--------------------------------------
; Try allocating data in DATA
NotFixed:
        mov     bx,[XSI].GMEMORY.lpDATA
        or      bx,bx
        jz      NoDATA

        INVOKE  AllocateMemory,lpGMEMORY,bx,uSize,uFlags
        or      ax,ax
        jnz     J_Done
NoDATA:


NoTryData:
;--------------------------------------
; Try to allocate memory in UMB
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB

        INVOKE  AllocateMemory,lpGMEMORY,bx,uSize,uFlags
        or      ax,ax
        jnz     J_Done
NoUMB:
;--------------------------------------
; Try to allocate memory in EMS
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS

        INVOKE  MEM4_EMS_LOCK,bx
        or      ax,ax
        jz      NoEMS

        mov     cx,ax
        INVOKE  AllocateMemory,lpGMEMORY,cx,uSize,uFlags
        push    ax
        INVOKE  MEM4_EMS_UNLOCK,bx
        pop     ax
        or      ax,ax
        jnz     J_Done
NoEMS:
;--------------------------------------
; Try to allocate memory in CONV
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        INVOKE  AllocateMemory,lpGMEMORY,bx,uSize,uFlags
        or      ax,ax
        jnz     J_Done

NoCONV:
;--------------------------------------
; Swap memory from UMB to XMS or FILE

;--------------------------------------
; Swap memory from EMS to XMS or FILE

;--------------------------------------
; Swap memory from CONV to XMS or FILE

;--------------------------------------
; Retry if necessary

J_Done:

        ret

MEM4_GALLOCATE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; ALLOCATEMEMORY                       1/5/93                          CAG
;
;       The routine that actually does the work.
;
; WORD WINAPI ALLOCATEMEMORY(WORD Segment,WORD Handle,WORD Size,WORD Flags)
;
; Input:
;       1)  Segment of global structure
;       2)  Segment to look in
;       3)  Size of memory (PARAGRAPH)
;       4)  Flags to set
; Output:
;       1)  ERROR (0)
;       2)  HANDLE
;
;-----------------------------------------------------------------------------
ALLOCATEMEMORY PROC USES XBX SDS XSI,
        lpGlobal:WORD,
        lpSegment:WORD,
        uSize:WORD,
        uFlags:WORD

        MOV_DS  lpSegment               ; DS:SI = pointer for memory
        xor     XSI,XSI
        xor     bx,bx                   ; Use for previous handle

SearchMore:
        cmp     [XSI].LMEMORY.hHandle,0
        jz      FoundFree

SearchNext:
        mov     bx,ds

        cmp     [XSI].LMEMORY.lpNext,0
        jz      NoFree
        MOV_DS  [XSI].LMEMORY.lpNext

        jmp     SearchMore

FoundFree:
        mov     cx,uSIZE
        cmp     [XSI].LMEMORY.uSIZE,cx
        jbe     SearchNext

        MOV_ES  lpGlobal
        xor     XDI,XDI

        cmp     XES[XDI].GMEMORY.uFreeHandle,0
        jz      NoFreed
        mov     ax,XES[XDI].GMEMORY.uFreeHandle
        mov     XES[XDI].GMEMORY.uFreeHandle,0
        jmp     HaveHandle
NoFreed:
        mov     ax,XES[XDI].GMEMORY.uLastHandle
        inc     ax
        mov     XES[XDI].GMEMORY.uLastHandle,ax
HaveHandle:
        
        mov     [XSI].LMEMORY.hHandle,ax
        mov     [XSI].LMEMORY.lpPREV,bx
        mov     bx,uFlags
        mov     [XSI].LMEMORY.uFlags,bx
        mov     bx,uSize
        mov     cx,[XSI].LMEMORY.uSize  ; Current size in CX
        mov     [XSI].LMEMORY.uSize,bx
        add     bx,sGMEMORY/16 + 1      ; Allow for this header
        sub     cx,bx
        mov     dx,ds
        add     bx,dx
        mov     di,[XSI].LMEMORY.lpNEXT
        mov     [XSI].LMEMORY.lpNEXT,bx


;--------------------------------------
; Now setup next segment
        MOV_DS  bx
        xor     XSI,XSI
        mov     [XSI].LMEMORY.hHandle,0
        mov     [XSI].LMEMORY.uSize,cx  ; New size left
        mov     [XSI].LMEMORY.lpPREV,dx ; Me
        mov     [XSI].LMEMORY.lpNEXT,di ; What I had in next
        jmp     J_Done
NoFree:
        xor     ax,ax

J_Done:
        ret

ALLOCATEMEMORY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; MEM4_LOCALALLOCATE            3/10/93                                 CAG
;
;       Used only to allocate memory from a LOCAL HEAP in the HMA
;       or where a Local HEAP is allocated.
;
; WORD WINAPI MEM4_LOCALALLOCATE(WORD Segment,FAR *LocalHeap,UINT uSize);
;
; Input:
;       1)  Far pointer to the local heap to allocate from
;       2)  Size of memory to allocate (BYTES)
; Output:
;       1)  Error (0)
;       2)  Handle of memory to allocate
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;-----------------------------------------------------------------------------
MEM4_LOCALALLOCATE PROC USES XDX SDS SES XSI XDI,
        lpGlobal:WORD,
        lpLocalHeap:LPSTR,
        uSize:WORD

        mov     dx,uSize
        LDS_SI  lpLocalHeap

LookMore:
        cmp     [XSI].MEMLOCAL.lUsed,TRUE
        je      IsUsed

        cmp     [XSI].MEMLOCAL.lSize,dx
        jl      ToSmall

        mov     [XSI].MEMLOCAL.lUsed,TRUE
        mov     ax,[XSI].MEMLOCAL.lSize
        mov     [XSI].MEMLOCAL.lSize,dx
        sub     ax,dx

        add     dx,si
        mov     [XSI].MEMLOCAL.lNext,dx

        mov     dx,si
        add     dx,sMEMLOCAL

        mov     si,[XSI].MEMLOCAL.lNext
        mov     [XSI].MEMLOCAL.lLocked,FALSE
        mov     [XSI].MEMLOCAL.lUsed,FALSE
        mov     [XSI].MEMLOCAL.lSize,ax
        mov     [XSI].MEMLOCAL.lNext,0
        mov     [XSI].MEMLOCAL.lPrev,dx

        MOV_ES  lpGlobal                ; Get a handle for this memory
        xor     XDI,XDI

        cmp     XES[XDI].GMEMORY.uFreeHandle,0
        jz      NoFreed
        mov     ax,XES[XDI].GMEMORY.uFreeHandle
        mov     XES[XDI].GMEMORY.uFreeHandle,0
        jmp     HaveHandle
NoFreed:
        mov     ax,XES[XDI].GMEMORY.uLastHandle
        inc     ax
        mov     XES[XDI].GMEMORY.uLastHandle,ax
HaveHandle:
        mov     [XSI].MEMLOCAL.lHandle,ax
        jmp     GotIt
IsUsed:
        cmp     [XSI].MEMLOCAL.lNext,0
        je      IsError
        mov     si,[XSI].MEMLOCAL.lNext
        jmp     LookMore
ToSmall:
        cmp     [XSI].MEMLOCAL.lNext,0
        je      IsError
        mov     si,[XSI].MEMLOCAL.lNext
        jmp     LookMore

IsError:
        mov     ax,FALSE
GotIt:

        ret

MEM4_LOCALALLOCATE ENDP
;-----------------------------------------------------------------------------
END
