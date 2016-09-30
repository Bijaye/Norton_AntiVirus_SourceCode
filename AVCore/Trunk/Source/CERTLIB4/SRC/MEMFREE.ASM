include CTS4.INC


UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GFREE                           1/5/93                          CAG
;
;       Free the memory we've allocated.
;
; WORD WINAPI MEM4_GFREE(WORD Handle,WORD LHANDLE);
;
; Input:
;       1)  Initialized handle from INIT
;       2)  LHANDLE Local memory handle, from Allocate
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1) Not there or locked
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GFREE PROC USES XBX XCX SDS XSI,
        lpGMEMORY:WORD,
        uLHandle:WORD

        LOCAL   bHMA:BYTE

        mov     bHMA,FALSE

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

;--------------------------------------
; Search thru HMA for handle to free
        cmp     word ptr [XSI].GMEMORY.lpHMA+2,0
        je      NoHMA

        INVOKE  MEM4_HMA,HMA_STATUS     ; Get HMA Status
        or      ax,ax
        jnz     IsON
        INVOKE  MEM4_HMA,HMA_ON
        mov     bHMA,TRUE               ; Was turned on, must turn off
IsON:

        INVOKE  FreeHMA,uLHandle,[XSI].GMEMORY.lpHMA

        cmp     bHMA,TRUE               ; Was turned on, must turn off
        jne     NotOn
        push    ax
        INVOKE  MEM4_HMA,HMA_OFF
        pop     ax
NotOn:

        or      ax,ax
        jz      J_Done
NoHMA:

;--------------------------------------
; Search thru DATA for handle to free
        mov     bx,[XSI].GMEMORY.lpDATA
        or      bx,bx
        jz      NoDATA

        INVOKE  FreeMemory,lpGMEMORY,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoDATA:

;--------------------------------------
; Search thru UMB for handle to free
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB

        INVOKE  FreeMemory,lpGMEMORY,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoUMB:

;--------------------------------------
; Search thru CONV for handle to free
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        INVOKE  FreeMemory,lpGMEMORY,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoCONV:

;--------------------------------------
; Search thru EMS for handle to free
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS

        INVOKE  MEM4_EMS_LOCK,bx
        or      ax,ax
        jz      NoEMS

        mov     cx,ax
        INVOKE  FreeMemory,lpGMEMORY,cx,uLHandle
        push    ax
        INVOKE  MEM4_EMS_UNLOCK,bx
        pop     ax
        or      ax,ax
        jz      J_Done

NoEMS:
        mov     ax,1
J_Done:

        ret

MEM4_GFREE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; FREEMEMORY                           1/5/93                          CAG
;
;       The routine that actually does the work.
; 
; WORD WINAPI FREEMEMORY(GMEMORY,WORD Seg,WORD Handle);
;
; Input:
;       1)  GMEMORY Structure
;       2)  Segment of memory to search for handle
;       3)  Handle to search for
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
; 
;-----------------------------------------------------------------------------
FREEMEMORY PROC USES XBX SDS SES XSI XDI,
        lpGMEMORY:WORD,
        lpSegment:WORD,
        hHandle:WORD

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

        MOV_ES  lpSegment
        xor     XDI,XDI
        mov     bx,hHandle
        xor     cx,cx                   ; Save for PREV entry

SearchMore:
        cmp     XES[XDI].LMEMORY.hHandle,bx
        je      FoundIt

        mov     cx,es                   ; Save for PREV
        cmp     XES[XDI].LMEMORY.lpNEXT,0
        je      J_Error
        MOV_ES  XES[XDI].LMEMORY.lpNEXT
        jmp     SearchMore
FoundIt:
        test    XES[XDI].LMEMORY.uFlags,MEM_LOCKED
        jnz     J_Error

        mov     XES[XDI].LMEMORY.hHandle,0
        mov     XES[XDI].LMEMORY.lpPREV,cx
        mov     [XSI].GMEMORY.uFreeHandle,bx

; Now detemine if next pointer is also free and add together
        cmp     XES[XDI].LMEMORY.lpNEXT,0
        je      NotAFreeOne
        MOV_DS  XES[XDI].LMEMORY.lpNEXT
        xor     XSI,XSI
        cmp     [XSI].LMEMORY.hHandle,0
        jnz     NotAFreeOne

        mov     bx,[XSI].LMEMORY.uSize  ; Get its size
        add     bx,sGMEMORY/16 + 1      ; Allow for this header
        add     XES[XDI].LMEMORY.uSize,bx

        mov     bx,[XSI].LMEMORY.lpNEXT
        mov     XES[XDI].LMEMORY.lpNEXT,bx

NotAFreeOne:
        xor     ax,ax
        jmp     J_Done
J_Error:
        mov     ax,1
J_Done:

        ret
FREEMEMORY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; FREEHMA
;
;-----------------------------------------------------------------------------
FREEHMA PROC USES SDS XSI,
        hHandle:WORD,
        lpHMA:LPSTR

        LDS_SI  lpHMA

        mov     ax,hHandle
MoreLook:
        cmp     [XSI].MEMLOCAL.lHandle,ax
        je      FoundIt
        mov     XSI,[XSI].MEMLOCAL.lNext
        or      XSI,XSI
        jz      NotHere
        jmp     MoreLook

FoundIt:
        mov     ax,si
        mov     dx,ds
        mov     [XSI].MEMLOCAL.lLocked,TRUE
        mov     [XSI].MEMLOCAL.lUsed,FALSE
        xor     ax,ax
        jmp     Done

NotHere:
        mov     ax,1
Done:
        ret

FREEHMA ENDP
;-----------------------------------------------------------------------------
END
