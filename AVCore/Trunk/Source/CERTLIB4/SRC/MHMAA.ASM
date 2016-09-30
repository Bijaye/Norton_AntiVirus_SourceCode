include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_HMA_ALLOCATE                    3/9/93                          CAG
;
;       This is an operation that will allocate (Make available the
;       HMA for use.  It'll return a pointer to memory that
;       we can do a LOCAL allocation of memory in.
;
; DWORD WINAPI MEM4_HMA_ALLOCATE(void);
;
; Output:
;       1)  Address of HMA memory to use to do LOCAL allocates from.
;       2)  NULL (Unable to get HMA)
;
; LIBRARY:DOS16.LIB
; LIBRARY:DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_HMA_ALLOCATE PROC USES XBX XCX SDS SES XSI XDI

        LOCAL   Enabled:BOOL
        LOCAL   lpA20:DWORD
        LOCAL   uSize:WORD

        mov     Enabled,FALSE

; Allocate memory using UNDOCUMENTED INT 2Fh functions
        mov     ax,4A01h                ; Get free HMA, and size
        int     2Fh
        or      bx,bx
        jz      TryOtherMethod

        mov     ax,4A02h                ; Allocate BX bytes of HMA
        int     2Fh                     
        cmp     di,-1                   ; On Error
        je      TryOtherMethod

        mov     uSize,bx

; ES:DI = Memory in HMA to use
        jmp     GotIt

TryOtherMethod:
; Allocate memory using HIMEM.SYS method
        mov     ah,MEM_REQUEST_HMA
        mov     dx,-1                   ; Off of the HMA
        INVOKE  XMS_CALL
        or      ax,ax
        jz      NoHMA

        mov     uSize,0FFFFh

        mov     ax,0FFFFh
        mov     es,ax
        mov     di,10h                  ; HMA = 0FFFF:10h

GotIt:
; Get state of A20 Line
        mov     word ptr lpA20+2,es
        mov     word ptr lpA20,di

        mov     ah,MEM_QUERY_A20
        INVOKE  XMS_CALL
        or      ax,ax
        jnz     AlreadyEnabled

; Enable A20 Line
        mov     Enabled,TRUE
        mov     ah,MEM_LOCALENABLE_A20
        INVOKE  XMS_CALL
        or      ax,ax
        jz      NoHMA

AlreadyEnabled:
; Setup the LOCAL memory allocation skeem
        mov     XES[XDI].MEMLOCAL.lLocked,FALSE
        mov     XES[XDI].MEMLOCAL.lUsed,FALSE
        mov     ax,uSize
        mov     XES[XDI].MEMLOCAL.lSize,ax
        mov     XES[XDI].MEMLOCAL.lNext,0
        mov     XES[XDI].MEMLOCAL.lPrev,0

; Return A20 Line state to what it was
        cmp     Enabled,TRUE
        je      WasEnabled
        mov     ah,MEM_LOCALDISABLE_A20
        INVOKE  XMS_CALL

WasEnabled:

; Return address of allocated memory
        mov     dx,word ptr lpA20+2
        mov     ax,word ptr lpA20
        jmp     Done
NoHMA:
        xor     ax,ax
        xor     dx,dx
Done:

        ret

MEM4_HMA_ALLOCATE ENDP
;-----------------------------------------------------------------------------
END
