include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_OPEN_GLOBAL                     1/15/93                         CAG
;
;       Open a file that is located in the GLOBAL data.
;
; UINT WINAPI FIL4_OPEN_GLOBAL(BYTE Id,WORD Mode, 
;                               LPSTR GlobalData,LPSTR Buffer);
;
; Input:
;       1)  ID of file to open.
;       2)  Open mode (FIL4_OPEN)
;       3)  Pointer to global data to search
;       4)  A work buffer
; Output:
;       1)  ERROR (0)
;       2)  Handle (?)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_OPEN_GLOBAL PROC USES XBX XCX XDX SDS XSI,
        byID:BYTE,
        uMode:WORD,
        lpGlobal:LPSTR,
        lpBuffer:LPSTR

        LOCAL   lpTemp:LPSTR

        INVOKE  MEM4_GETOBJECT,ObjectPath,lpGlobal
        or      ax,ax
        jz      NotHere

        INVOKE  STR4_COPY,dx::ax,lpBuffer

        LDS_SI  lpBuffer
SearchMore:
        lodsb
        cmp     al,0
        jne     SearchMore
        dec     XSI
        cmp     [XSI-1],byte ptr 5Ch
        je      FoundSlash
        mov     [XSI],byte ptr 5Ch
        inc     XSI
FoundSlash:

        INVOKE  MEM4_GETOBJECT,byID,lpGlobal
        or      ax,ax
        jz      NotHere

        mov     word ptr lpTemp+2,dx
        mov     word ptr lpTemp,ax

        INVOKE  STR4_SRCH_CHAR,lpTemp,05Ch
        or      ax,ax
        jz      NoSlash

        mov     dx,word ptr lpTemp+2
        mov     ax,word ptr lpTemp
        mov     word ptr lpBuffer+2,dx
        mov     word ptr lpBuffer,ax
        jmp     DoOpen

NoSlash:
        INVOKE  STR4_COPY,lpTemp,vSDS XSI
DoOpen:
        INVOKE  FIL4_OPEN,lpBuffer,uMode
        cmp     ax, -1
        jz      J_File_Error
        jmp     NotHere

J_File_Error:
        xor     ax, ax
NotHere:
        ret
       
FIL4_OPEN_GLOBAL ENDP
;-----------------------------------------------------------------------------
END


