include CTS4.INC


UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GLOCK                           1/5/93                          CAG
;
;       Lock the memory for use, so it won't be moved on us.
;
; WORD WINAPI MEM4_GLOCK(WORD Handle,WORD LHANDLE);
;
; Input:
;       1)  Initialized handle from INIT
;       2)  LHANDLE Local memory handle, from Allocate
; Output:
;       1)  FAR pointer to locked memory.
;       2)  ERROR (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GLOCK PROC USES XBX XCX SDS SES XSI XDI,
        lpGMEMORY:WORD,
        uLHandle:WORD

        LOCAL   bHMA:BYTE

        mov     bHMA,FALSE

        MOV_DS  lpGMEMORY
        xor     XSI,XSI

;--------------------------------------
; Search thru HMA for handle to lock
        cmp     word ptr [XSI].GMEMORY.lpHMA+2,0
        je      NoHMA

        INVOKE  MEM4_HMA,HMA_STATUS     ; Get HMA Status
        or      ax,ax
        jnz     IsON
        INVOKE  MEM4_HMA,HMA_ON
        mov     bHMA,TRUE               ; Was turned on, must turn off
IsON:

        INVOKE  LocalHMA,uLHandle,[XSI].GMEMORY.lpHMA

        cmp     bHMA,TRUE               ; Was turned on, must turn off
        jne     NotOn
        push    dx
        push    ax
        INVOKE  MEM4_HMA,HMA_OFF
        pop     ax
        pop     dx
NotOn:

        or      dx,dx
        jnz     J_Done
NoHMA:

;--------------------------------------
; Search thru DATA for handle to lock
        mov     bx,[XSI].GMEMORY.lpDATA
        or      bx,bx
        jz      NoDATA

        INVOKE  LockMemory,bx,uLHandle
        or      dx,dx
        jnz     J_Done
NoDATA:

;--------------------------------------
; Search thru UMB for handle to lock
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB

        INVOKE  LockMemory,bx,uLHandle
        or      dx,dx
        jnz     J_Done
NoUMB:

;--------------------------------------
; Search thru CONV for handle to lock
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        INVOKE  LockMemory,bx,uLHandle
        or      dx,dx
        jnz     J_Done
NoCONV:

;--------------------------------------
; Search thru EMS for handle to lock
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS

        INVOKE  MEM4_EMS_LOCK,bx
        or      ax,ax
        jz      NoEMS

        mov     cx,ax
        INVOKE  LockMemory,cx,uLHandle
        or      dx,dx
        jnz     J_Done

NoEMS:

J_Done:

        ret

MEM4_GLOCK ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; LOCKMEMORY                           1/5/93                          CAG
;
;       The routine that really does the work.
;
; DWORD WINAPI LOCKMEMORY(WORD Segment,WORD Handle);
;
; Input:
;       1)  Segment to look in
;       2)  Handle to look for
; Output:
;       1)  Far pointer found at. DX:AX
;       2)  ERROR (0)
;       
;-----------------------------------------------------------------------------
LOCKMEMORY PROC USES XBX SDS SES XSI XDI,
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
        xor     ax,ax
        xor     dx,dx
        jmp     J_Done

FoundIt:
        or      XES[XDI].LMEMORY.uFlags,MEM_LOCKED
        mov     dx,es
        add     dx,sGMEMORY/16 + 1      ; Allow for this header
        xor     ax,ax
J_Done:
        ret

LOCKMEMORY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; LOCALHMA                            3/9/93                          CAG
;
;       Lock memory in the HMA for our use.
;
;-----------------------------------------------------------------------------
LOCALHMA PROC USES SDS XSI,
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
        add     ax,sMEMLOCAL
        mov     dx,ds
        mov     [XSI].MEMLOCAL.lLocked,TRUE
        jmp     Done

NotHere:
        xor     ax,ax
        xor     dx,dx
Done:
        ret

LOCALHMA ENDP
;-----------------------------------------------------------------------------
END
