include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GDESTROY                        1/5/93                          CAG
;
;       Destroy memory, don't care why, just do it.
;
; WORD WINAPI MEM4_GDESTROY(WORD Handle,INIT_MEMORY FAR *Struc);
;
; Input:
;       1)  Initialized handle from INIT
; Output:
;       1)  PASSED(0)
;       2)  ERROR (1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_GDESTROY PROC USES SDS SES XSI XDI,
        lpGMemory:WORD,
        lpIMemory:LPSTR

        MOV_DS  lpGMEMORY
        xor     XSI,XSI
        LES_DI  lpIMEMORY

;--------------------------------------
; Try to free memory in UMB
        mov     bx,[XSI].GMEMORY.lpUMB
        or      bx,bx
        jz      NoUMB
        INVOKE  MEM4_UMB_FREE,bx

        INVOKE  MEM4_UMB_FREE,lpGMemory ; Also destroy HEADER

NoUMB:

;--------------------------------------
; Try to free memory in EMS
        mov     bx,[XSI].GMEMORY.hEMS
        or      bx,bx
        jz      NoEMS
        INVOKE  MEM4_EMS_FREE,bx
NoEMS:

;--------------------------------------
; Try to free memory in CONV
        mov     bx,[XSI].GMEMORY.lpCONV
        or      bx,bx
        jz      NoCONV

        cmp     XES[XDI].INIT_MEMORY.memlpCONV,0
        jnz     NoCONV
        sub     bx,sGMEMORY/16+1
        INVOKE  MEM4_CONV_FREE,bx

NoCONV:

;--------------------------------------
; Close the handle to file and delete
        cmp     [XSI].GMEMORY.hSwapFile,0
        jz      NoFile
        INVOKE  FIL4_CLOSE,[XSI].GMEMORY.hSwapFile
        INVOKE  FIL4_DELETE,XES[XDI].INIT_MEMORY.memlpFilename
NoFile:

        xor     ax,ax
        ret

MEM4_GDESTROY ENDP
;-----------------------------------------------------------------------------
END
