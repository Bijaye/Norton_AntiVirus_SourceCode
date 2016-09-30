include CTS4.INC

SetupLocal      PROTO NEAR,:LPSTR,:WORD

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GINIT                           1/5/93                          CAG
;
;       Initialize memory to be used by dynamic linker.
;
; WORD WINAPI MEM4_GINIT(INIT_MEMORY FAR *Structure);
;
; Input:
;       1)  Structure describing how to set up memory.
; Output:
;       1)  Error (0), no UMB/CONV or Fail to create swap file
;       2)  GHANDLE of Global memory
;
; Allocate memory in UMB
; Allocate memory in EMS
; Allocate memory in HMA
; Allocate memory in CONV
; Allocate memory in DATA
; Allocate memory in XMS or Create temp swap file.
;
; Enter with TOTAL = 1000
;              UMB = 1000
;              EMS = 1000
;             CONV =  400
;              XMS = 1000
;         Filename = Swap.fil
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       The "HANDLE" of the memory that was created must reside in
;       either UMB or CONV memory.  If it can't be created, then
;       this fuction will error.
;
;-----------------------------------------------------------------------------
MEM4_GINIT PROC USES SDS SES XSI XDI,
        lpGMEMORY:LPSTR

        LOCAL   hSWAP:WORD
        LOCAL   hEMS:WORD
        LOCAL   hXMS:WORD
        LOCAL   lpUMB:LPSTR
        LOCAL   lpEMS:LPSTR
        LOCAL   lpCONV:LPSTR
        LOCAL   lpDATA:LPSTR
        LOCAL   lpHMA:LPSTR
        LOCAL   lpXMS:LPSTR

        mov     hEMS,0
        mov     hXMS,0
        mov     hSWAP,0
        mov     word ptr lpUMB+2,0
        mov     word ptr lpUMB,0
        mov     word ptr lpEMS+2,0
        mov     word ptr lpEMS,0
        mov     word ptr lpCONV+2,0
        mov     word ptr lpCONV,0
        mov     word ptr lpDATA+2,0
        mov     word ptr lpDATA,0
        mov     word ptr lpHMA+2,0
        mov     word ptr lpHMA,0
        mov     word ptr lpXMS+2,0
        mov     word ptr lpXMS,0

        LDS_SI  lpGMEMORY
;--------------------------------------
; Allocate entire memory in DATA.
        cmp     [XSI].INIT_MEMORY.memDATA,0
        jz      NoDATA
        mov     ax,[XSI].INIT_MEMORY.memlpDATA
        or      ax,ax
        jnz     NoAllocateDATA

        INVOKE  MEM4_CONV_ALLOCATE,[XSI].INIT_MEMORY.memDATA
        or      ax,ax
        jnz     EnoughDATA

; Can't get what I want, get what I can.
        mov     [XSI].INIT_MEMORY.memDATA,dx    ; What I may get
        INVOKE  MEM4_CONV_ALLOCATE,[XSI].INIT_MEMORY.memDATA
        or      ax,ax
        jz      NoDATA

NoAllocateDATA:
EnoughDATA:
        mov     word ptr lpDATA+2,ax
        mov     word ptr lpDATA,0
        INVOKE  SetupLocal,lpDATA,[XSI].INIT_MEMORY.memDATA
NoDATA:

;--------------------------------------
; Allocate entire memory in HMA.
        cmp     [XSI].INIT_MEMORY.memHMA,0
        jz      NoHMA

; This is an ALL or nothing operation!
        INVOKE  MEM4_HMA_ALLOCATE
        or      dx,dx                   ; Is not there
        jz      NoHMA
        mov     word ptr lpHMA+2,dx
        mov     word ptr lpHMA,ax
NoHMA:

;--------------------------------------
; Allocate the UMB memory we need.
        cmp     [XSI].INIT_MEMORY.memUMB,0
        jz      NoUMB
        INVOKE  MEM4_UMB_ALLOCATE,[XSI].INIT_MEMORY.memUMB
        or      ax,ax
        jnz     EnoughUMB

; Unable to allocate all the UMB wanted, get what I can.
        sub     dx,10h                          ; But less 10 paragraphs
        mov     [XSI].INIT_MEMORY.memUMB,dx     ; What I may get
        INVOKE  MEM4_UMB_ALLOCATE,[XSI].INIT_MEMORY.memUMB
        or      ax,ax
        jz      NoUMB

EnoughUMB:
        mov     word ptr lpUMB+2,ax
        mov     word ptr lpUMB,0
        INVOKE  SetupLocal,lpUMB,[XSI].INIT_MEMORY.memUMB
NoUMB:

;--------------------------------------
; Allocate necessary memory in EMS
        cmp     [XSI].INIT_MEMORY.memEMS,0
        jz      NoEMS
        INVOKE  MEM4_EMS_ALLOCATE,[XSI].INIT_MEMORY.memEMS
        or      ax,ax
        jnz     EnoughEMS

; Can't get what we want, get what we can.
        mov     [XSI].INIT_MEMORY.memEMS,dx     ; What I may get
        INVOKE  MEM4_EMS_ALLOCATE,[XSI].INIT_MEMORY.memEMS
        or      ax,ax
        jz      NoEMS

EnoughEMS:
        mov     hEMS,ax                 ; To store later

        INVOKE  MEM4_EMS_LOCK,hEMS
        or      ax,ax
        jz      EmsFail

        mov     word ptr lpEMS+2,ax
        mov     word ptr lpEMS,0
        INVOKE  SetupLocal,lpEMS,[XSI].INIT_MEMORY.memEMS

        INVOKE  MEM4_EMS_UNLOCK,hEMS
        jmp     YesEMS
EmsFail:
        INVOKE  MEM4_EMS_FREE,hEMS
        mov     hEMS,0
YesEMS:
NoEMS:

;--------------------------------------
; Allocate necessary memory in CONV
        cmp     [XSI].INIT_MEMORY.memCONV,0
        jz      NoCONV
        mov     ax,[XSI].INIT_MEMORY.memlpCONV
        or      ax,ax
        jnz     NoAllocateCONV

; Can't get what we what, get what we can
        INVOKE  MEM4_CONV_ALLOCATE,[XSI].INIT_MEMORY.memCONV
        or      ax,ax
        jnz     EnoughCONV

        mov     [XSI].INIT_MEMORY.memCONV,dx    ; What I may get
        INVOKE  MEM4_CONV_ALLOCATE,[XSI].INIT_MEMORY.memCONV
        or      ax,ax
        jz      NoCONV

NoAllocateCONV:
EnoughCONV:
        mov     word ptr lpCONV+2,ax
        mov     word ptr lpCONV,0
        INVOKE  SetupLocal,lpCONV,[XSI].INIT_MEMORY.memCONV
NoConv:

;--------------------------------------
; Allocate necessary swap memory in XMS or in file
        cmp     [XSI].INIT_MEMORY.memXMS,0
        jz      NoWantedXMS
        INVOKE  MEM4_XMS_ALLOCATE,[XSI].INIT_MEMORY.memXMS
        or      ax,ax
        jz      NoXMS
        mov     hXMS,ax
NoXMS:
        cmp     word ptr [XSI].INIT_MEMORY.memlpFilename,0
        jz      NoSwap
        INVOKE  FIL4_OPEN,[XSI].INIT_MEMORY.memlpFilename,fCREATE
        cmp     ax, -1
        jz      NoSwap
        mov     hSWAP,ax
NoWantedXMS:
NoSwap:

;--------------------------------------
; Create GMEMORY handle in UMB or CONV memory
        cmp     word ptr lpUMB+2,0
        jnz     UMB_GMEMORY
        cmp     word ptr lpCONV+2,0
        jz      J_ERROR
        LES_DI  lpCONV
        add     word ptr lpCONV+2,sGMEMORY/16+2
        mov     ax,[XSI].INIT_MEMORY.memCONV
        sub     ax,sGMEMORY/16+2
        INVOKE  SetupLocal,lpCONV,ax
        jmp     HaveGMEMORY
UMB_GMEMORY:
        LES_DI  lpUMB
        add     word ptr lpUMB+2,sGMEMORY/16+2
        mov     ax,[XSI].INIT_MEMORY.memUMB
        sub     ax,sGMEMORY/16+2
        INVOKE  SetupLocal,lpUMB,ax
HaveGMEMORY:

        mov     XES[XDI].GMEMORY.uLastHandle,0
        mov     XES[XDI].GMEMORY.uFreeHandle,0
        mov     ax,hXMS
        mov     XES[XDI].GMEMORY.hXMS,ax
        mov     ax,hSWAP
        mov     XES[XDI].GMEMORY.hSwapFile,ax
        mov     ax,hEMS
        mov     XES[XDI].GMEMORY.hEMS,ax
        mov     ax,word ptr lpUMB+2
        mov     XES[XDI].GMEMORY.lpUMB,ax
        mov     ax,word ptr lpCONV+2
        mov     XES[XDI].GMEMORY.lpCONV,ax
        mov     ax,word ptr lpDATA+2
        mov     XES[XDI].GMEMORY.lpDATA,ax
        mov     dx,word ptr lpHMA+2
        mov     ax,word ptr lpHMA
        mov     word ptr XES[XDI].GMEMORY.lpHMA+2,dx
        mov     word ptr XES[XDI].GMEMORY.lpHMA,ax

        mov     ax,es
        jmp     J_Done

J_ERROR:
        xor     ax,ax
J_Done:
        ret

MEM4_GINIT ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; SETUPLOCAL                           2/18/93                         CAG
;
;       Setup the local header describing the data segment it contains.
;
; VOID WINAPI SETUPLOCAL(LPSTR Allocated, WORD ParaSize);
;
; Input:
;       1)  The memory initialize to be used, needs a header added to it
;       2)  Paragraph size of the entire memory block
;
;-----------------------------------------------------------------------------
SetupLocal PROC NEAR USES SES XDI,
        lpSegment:LPSTR,
        lpParaSize:WORD

        LES_DI  lpSegment

        mov     XES[XDI].LMEMORY.hHandle,0      ; Clear the HANDLE
        mov     ax,lpParaSize
        sub     ax,sGMEMORY/16+2
        mov     XES[XDI].LMEMORY.uSize,ax       ; Set the size - HEADERSIZE
        mov     XES[XDI].LMEMORY.uFlags,MEM_NORMAL
        mov     XES[XDI].LMEMORY.lpNEXT,0       ; Set next pointer
        mov     XES[XDI].LMEMORY.lpPREV,0       ; Set prev pointer
        ret

SetupLocal ENDP
;-----------------------------------------------------------------------------
END
