include CTS4.INC


UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GUNLOCK                         1/5/93                          CAG
;
;       Unlock the memory to allow for it to move.
;
; WORD WINAPI MEM4_GUNLOCK(WORD Handle,WORD LHANDLE);
;
; Input:
;       1)  Initialized handle from INIT
;       2)  LHANDLE Local memory handle, from Allocate
; Output:
;       1)  PASSED(0)
;       2)  ERROR (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GUNLOCK PROC USES XBX XCX SDS XSI,
        lpGMEMORY:WORD,
        uLHandle:WORD

        LOCAL   bHMA:BYTE

        mov     bHMA,FALSE

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

;--------------------------------------
; Search thru HMA for handle to unlock
        cmp     word ptr [XSI].GMEMORY.lpHMA+2,0
        je      NoHMA

        INVOKE  MEM4_HMA,HMA_STATUS     ; Get HMA Status
        or      ax,ax
        jnz     IsON
        INVOKE  MEM4_HMA,HMA_ON
        mov     bHMA,TRUE               ; Was turned on, must turn off
IsON:

        INVOKE  UnLocalHMA,uLHandle,[XSI].GMEMORY.lpHMA

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
; Search thru DATA for handle to unlock
        mov     bx,[XSI].GMEMORY.lpDATA
        or      bx,bx
        jz      NoDATA

        INVOKE  UnlockMemory,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoDATA:

;--------------------------------------
; Search thru UMB for handle to unlock
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB

        INVOKE  UnlockMemory,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoUMB:

;--------------------------------------
; Search thru CONV for handle to unlock
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        INVOKE  UnlockMemory,bx,uLHandle
        or      ax,ax
        jz      J_Done
NoCONV:

;--------------------------------------
; Search thru EMS for handle to unlock
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS

        INVOKE  MEM4_EMS_GETPAGE
        or      ax,ax
        jz      NoEMS

        mov     cx,ax
        INVOKE  UnlockMemory,cx,uLHandle
        push    ax
        INVOKE  MEM4_EMS_UNLOCK,bx
        pop     ax

NoEMS:

J_Done:

        ret

MEM4_GUNLOCK ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; UNLOCKMEMORY                           1/5/93                          CAG
;
;       The routine that really does the work.
;
; WORD WINAPI UNLOCKMEMORY(WORD Segment,WORD Handle);
;
; Input:
;       1)  Segment to look in
;       2)  Handle to look for
; Output:
;       1)  PASSED(0)
;       2)  ERROR (1)
;       
;-----------------------------------------------------------------------------
UNLOCKMEMORY PROC USES XBX SDS SES XSI XDI,
        lpSegment:WORD,
        hHandle:WORD

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
        mov     ax,1
        jmp     J_Done

FoundIt:                                ; Toggle back to 0
        xor     XES[XDI].LMEMORY.uFlags,MEM_LOCKED
        xor     ax,ax
J_Done:
        ret

UNLOCKMEMORY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; UNLOCALHMA                          3/9/93                          CAG
;
;       Unlock memory in the HMA for our use.
;
;-----------------------------------------------------------------------------
UNLOCALHMA PROC USES SDS XSI,
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
        mov     [XSI].MEMLOCAL.lLocked,FALSE
        xor     ax,ax
        jmp     Done

NotHere:
        mov     ax,1
Done:
        ret

UNLOCALHMA ENDP
;-----------------------------------------------------------------------------
END

