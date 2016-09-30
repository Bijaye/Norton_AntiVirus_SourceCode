include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_LOADLIBRARY                     1/8/93                          CAG
;
;       Load a DLL library for DOS to use.
;
; WORD WINAPI DLL4_LOADLIBRARY(LPSTR Libname);
;
; Input:
;       1)  Name of library to load
;       2)  Buffer to work with
; Output:
;       1)  Handle to this library to work with
;       2)  ERROR <32
;               1 = File not found
;               2 = Not enough memory to load
;               3 = Invalid format of a DLL
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_LOADLIBRARY PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpLibname:LPSTR

        LOCAL   nStatus:WORD
        LOCAL   hHandle:WORD
        LOCAL   lpMemory:DWORD
        LOCAL   lpDyna:DWORD
        LOCAL   uDynaSize:WORD
        LOCAL   uTemp:WORD


        mov     word ptr lpDyna+2,0
        mov     word ptr lpDyna,0
        mov     word ptr lpMemory+2,0
        mov     word ptr lpMemory,0
        mov     word ptr hHandle,0

;--------------------------------------
; Open this library - ERROR 1
        mov     nStatus,1
        INVOKE  FIL4_OPEN,lpLibname,fREAD
        cmp     ax, -1
        jz      J_Error
        mov     hHandle,ax

;--------------------------------------
; Allocate memory to read header - ERROR 2
        mov     nStatus,2
        INVOKE  MEM4_CONV_ALLOCATE,(sNEHEADER/16)+10h
        or      ax,ax
        jz      J_Error
        mov     word ptr lpMemory+2,ax
        mov     word ptr lpMemory,0

;--------------------------------------
; Read the header into this memory
        mov     nStatus,3
        INVOKE  FIL4_READ,hHandle,lpMemory,sEXELARGE
        cmp     ax,-1
        je      J_Error
        cmp     ax,sEXELARGE
        jne     J_Error

;--------------------------------------
; Get location of NE header
        LDS_SI  lpMemory
        cmp     [XSI].EXEHEADER.exRelocTable,40h
        jl      J_Error

        mov     dx,[XSI].EXEHEADER.exExtendOffset
        mov     uTemp,dx
        INVOKE  FIL4_SEEK,hHandle,dx,fSEEK_BEG
        cmp     ax,-1
        je      J_Error

;--------------------------------------
; Now read the NE header
        INVOKE  FIL4_READ,hHandle,lpMemory,sNEHEADER
        cmp     ax,-1
        je      J_Error
        cmp     ax,sNEHEADER
        jne     J_Error

        mov     dx,[XSI].NEHEADER.neSegTabOff
        add     dx,uTemp
        INVOKE  FIL4_SEEK,hHandle,dx,fSEEK_BEG

;--------------------------------------
; Verify that this is an NE header
        cmp     [XSI].NEHEADER.neSignature,454Eh
        jne     J_Error

;--------------------------------------
; Calculate number of bytes we need to allocate
        mov     ax,word ptr [XSI].NEHEADER.neNonresOff
        sub     ax,uTemp                ; Make relitive to beg of NE
        sub     ax,[XSI].NEHEADER.neSegTabOff
        add     ax,[XSI].NEHEADER.neNonresNumb

;--------------------------------------
; Allocate memory for header + dll stuff
        mov     nStatus,2
        mov     uDynaSize,ax
        add     ax,(sDYNA_HEADER/16)+10h
        shr     ax,1
        shr     ax,1
        shr     ax,1
        shr     ax,1
        inc     ax
        inc     ax                      ; Allow for two extra paragraphs
        INVOKE  MEM4_CONV_ALLOCATE,ax
        or      ax,ax
        jz      J_Error
        mov     word ptr lpDyna+2,ax

;--------------------------------------
; Move NE stuff to dyna header
        LES_DI  lpDyna

        mov     word ptr XES[XDI].DYNA_HEADER.lpSegment+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpResource+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpResident+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpModuleRef+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpImport+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpEntry+2,ax
        mov     word ptr XES[XDI].DYNA_HEADER.lpNonres+2,ax

        mov     ax,sDYNA_HEADER
        mov     word ptr XES[XDI].DYNA_HEADER.lpSegment,ax
        add     ax,[XSI].NEHEADER.neResourceOff
        sub     ax,[XSI].NEHEADER.neSegTabOff
        mov     word ptr XES[XDI].DYNA_HEADER.lpResource,ax

        add     ax,[XSI].NEHEADER.neResidentOff
        sub     ax,[XSI].NEHEADER.neResourceOff
        mov     word ptr XES[XDI].DYNA_HEADER.lpResident,ax

        add     ax,[XSI].NEHEADER.neModRefOff
        sub     ax,[XSI].NEHEADER.neResidentOff
        mov     word ptr XES[XDI].DYNA_HEADER.lpModuleRef,ax

        add     ax,[XSI].NEHEADER.neImportOff
        sub     ax,[XSI].NEHEADER.neModRefOff
        mov     word ptr XES[XDI].DYNA_HEADER.lpImport,ax

        add     ax,[XSI].NEHEADER.neEntryOffset
        sub     ax,[XSI].NEHEADER.neImportOff
        mov     word ptr XES[XDI].DYNA_HEADER.lpEntry,ax

        add     ax,word ptr [XSI].NEHEADER.neNonresOff
        sub     ax,uTemp
        sub     ax,[XSI].NEHEADER.neEntryOffset
        mov     word ptr XES[XDI].DYNA_HEADER.lpNonres,ax


        mov     word ptr XES[XDI].DYNA_HEADER.lpNextDll+2,0
        mov     word ptr XES[XDI].DYNA_HEADER.lpNextDll,0


;--------------------------------------
; Read into the memory the header stuff
        mov     nStatus,3
        add     XDI,sDYNA_HEADER
        INVOKE  FIL4_READ,hHandle,vSES XDI,uDynaSize
        cmp     ax,-1
        je      J_Error
        cmp     ax,uDynaSize
        jne     J_Error

;--------------------------------------
; Save handle into structure
        LDS_SI  lpDyna
        mov     ax,hHandle
        mov     [XSI].DYNA_HEADER.hFilehandle,ax

;--------------------------------------
; Free small buffer
        INVOKE  MEM4_CONV_FREE,word ptr lpMemory+2

        mov     ax,word ptr lpDyna+2
        jmp     J_Done


;--------------------------------------
; Error routine
J_Error:
        cmp     hHandle,0
        je      NoClose
        INVOKE  FIL4_CLOSE,hHandle
NoClose:
        cmp     word ptr lpMemory+2,0
        je      NoSmallBuffer
        INVOKE  MEM4_CONV_FREE,word ptr lpMemory+2
NoSmallBuffer:
        cmp     word ptr lpDyna+2,0
        je      NoDynaBuffer
        INVOKE  MEM4_CONV_FREE,word ptr lpDyna+2
NoDynaBuffer:

        mov     ax,0

;--------------------------------------
J_Done:

        ret

DLL4_LOADLIBRARY ENDP
;-----------------------------------------------------------------------------
END
