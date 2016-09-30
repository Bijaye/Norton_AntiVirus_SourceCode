include CTS4.INC

CACHE_HEADER    STRUCT
        Extension       db      4 dup(0)
        CacheSize       dw      0
CACHE_HEADER    ENDS
sCACHE_HEADER   EQU SIZE CACHE_HEADER

CACHE_ELEMENT   STRUCT
        Filename        db      78 dup(0)
        Handle          dw      0                
        RealHandle      dw      0
        FilePosition    dw      0
        FileSize        dw      0
        NextElement     dw      0
        Status          db      0
CACHE_ELEMENT   ENDS
sCACHE_ELEMENT  EQU SIZE CACHE_ELEMENT

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_INSTALLCACHE                    2/24/93                         CAG
;
;       Install a cache for the file system.
;
; DWORD WINAPI FIL4_INSTALLCACHE(LPSTR lpExt,LPSTR Memory,WORD uMemory);
;
; Input:
;       1)  Extension of files to cache.
;       2)  Memory block to use
;       3)  Size of memory block
; Output:
;       1)  Handle of cache
;       2)  ERROR (0)
;
; Note:
;       This function will hook the INT 21h, and use a cache of
;       not larger then 64k.
;
;-----------------------------------------------------------------------------
PREV_Cache21h   dd      0
CachePointer    dd      0
InMe            db      FALSE
;-----------------------------------------------------------------------------
Cache21h        PROC    FAR

        cmp     cs:InMe,TRUE
        je      AlreadyMe
        mov     cs:InMe,TRUE

        cmp     ax,3D00h                ; Open for READ, Cache it!
        jne     NotOpen

        call    AddCache
        jc      NotMe
        jmp     WasMe
NotOpen:
        cmp     ah,3Eh                  ; Close this file
        jne     NotClose
        call    CloseCache
        jc      NotMe
        jmp     WasMe

NotClose:
        cmp     ah,3Fh                  ; Read this file
        jne     NotRead
        call    ReadCache
        jc      NotMe
        jmp     WasMe
NotRead:
        cmp     ah,42h                  ; Seek this file
        jne     NotSeek
        call    SeekCache
        jc      NotMe
        jmp     WasMe
NotSeek:

NotMe:
        mov     cs:InMe,FALSE
AlreadyMe:
        jmp     dword ptr cs:PREV_Cache21h

WasMe:
        mov     cs:InMe,FALSE
        ret     2

Cache21h        ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
FIL4_INSTALLCACHE PROC USES XBX SDS XSI,
        lpExt:LPSTR,
        lpMemory:LPSTR,
        uMemory:WORD

; Clear the cache
        INVOKE  MEM4_SET,lpMemory,0,uMemory

; Place extension in cache to monitor
        INVOKE  STR4_COPY,lpExt,lpMemory

        LDS_SI  lpMemory                ; Size of cache - header
        mov     ax,uMemory
        sub     ax,sCACHE_HEADER
        mov     [XSI].CACHE_HEADER.CacheSize,ax

        mov     ax,ds
        mov     word ptr cs:CachePointer+2,ax
        mov     word ptr cs:CachePointer,si

; Hook INT 21h
        mov     dx,cs
        lea     bx,Cache21h
        INVOKE  SYS4_VECTOR,21h,dx::bx,ADDR PREV_Cache21h

        mov     dx,word ptr lpMemory+2  ; Return pointer to cache
        mov     ax,word ptr lpMemory    ; This is the handle
        ret

FIL4_INSTALLCACHE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; FIL4_DESTROYCACHE                    2/24/93                         CAG
;
;       Destroy the cache.
;
; BOOL WINAPI FIL4_DESTROYCACHE(DWORD Handle);
;
; Input:
;       1)  Handle to cache
; Output:
;       1)  TRUE (1) Cache destroyed
;       2)  FALSE (0) Error
;
;-----------------------------------------------------------------------------
FIL4_DESTROYCACHE PROC USES XBX SDS XSI,
        lpCache:LPSTR

        LDS_SI  lpCache
        mov     dx,[XSI].CACHE_HEADER.CacheSize
        add     dx,sCACHE_HEADER

        INVOKE  MEM4_SET,lpCache,0,dx

        mov     dx,cs
        lea     bx,Cache21h
        INVOKE  SYS4_UNVECTOR,21h,dx::bx,ADDR PREV_Cache21h
        ret

FIL4_DESTROYCACHE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Look at extension, if matches, check cache for existance,
; if in cache, just return that cache handle, set status
; in cache to TRUE
;
;       DS:DX   Filename requested to be opened.
;
; Exit:
;       CLC,AX  In Cache
;       STC     Not In Cache
;-----------------------------------------------------------------------------
AddCache        PROC NEAR USES XBX XCX XDX SDS SES XSI XDI

        LOCAL   lpFilename:LPSTR

        mov     word ptr lpFilename+2,ds
        mov     word ptr lpFilename,dx

        INVOKE  FIL4_IS_AT,lpFilename,CachePointer
        or      ax,ax
        jz      NotInAtList

        LES_DI  CachePointer
        add     XDI,sCache_Header

LookNextElement:
        cmp     byte ptr XES[XDI].CACHE_ELEMENT.Filename,0
        je      NotInCache

        INVOKE  STR4_COMPARE,lpFilename,vSES XDI
        or      ax,ax
        jz      NotInElement

        mov     ax,XES[XDI].CACHE_ELEMENT.Handle
        mov     XES[XDI].CACHE_ELEMENT.Status,TRUE
        mov     XES[XDI].CACHE_ELEMENT.FilePosition,0
        clc
        jmp     WasInCache

NotInElement:
        mov     XDI,XES[XDI].CACHE_ELEMENT.NextElement
        jmp     LookNextElement

NotInCache:
        INVOKE  FIL4_OPEN,lpFilename,fREAD
        cmp     ax, -1
        jz      J_Error
        mov     bx,ax
        INVOKE  FIL4_SIZE,bx
        or      dx,dx                   ; If larger than 64k error
        jnz     NoGo
        mov     cx,ax                   ; CX = Filesize

; ES:DI = Element to store at
        LDS_SI  CachePointer
        add     cx,sCACHE_ELEMENT
        cmp     [XSI].CACHE_HEADER.CacheSize,cx
        jb      NoGo                    ; Can't fit, don't do it.

        INVOKE  STR4_COPY,lpFilename,vSES XDI
        sub     cx,sCACHE_ELEMENT
        mov     XES[XDI].CACHE_ELEMENT.FileSize,cx
        mov     XES[XDI].CACHE_ELEMENT.FilePosition,0
        add     XCX,sCACHE_ELEMENT
        add     XCX,XDI
        mov     XES[XDI].CACHE_ELEMENT.NextElement,cx
        mov     XES[XDI].CACHE_ELEMENT.Status,TRUE
        mov     ax,es
        add     ax,di
        mov     XES[XDI].CACHE_ELEMENT.Handle,ax
        mov     XES[XDI].CACHE_ELEMENT.RealHandle,bx

        mov     cx,XES[XDI].CACHE_ELEMENT.FileSize

        add     XDI,sCACHE_ELEMENT
        INVOKE  FIL4_READ,bx,vSES XDI,cx
        cmp     ax,cx
        je      IsOk

        sub     XDI,sCACHE_ELEMENT
        mov     XES[XDI].CACHE_ELEMENT.NextElement,0
        mov     XES[XDI].CACHE_ELEMENT.Status,FALSE
        mov     XES[XDI].CACHE_ELEMENT.Handle,0
        mov     XES[XDI].CACHE_ELEMENT.RealHandle,0
        mov     XES[XDI].CACHE_ELEMENT.FileSize,0
        mov     XES[XDI].CACHE_ELEMENT.FilePosition,0
        jmp     NoGo
IsOk:
        LDS_SI  CachePointer
        sub     [XSI].CACHE_HEADER.CacheSize,cx

        INVOKE  FIL4_CLOSE,bx

        sub     XDI,sCACHE_ELEMENT
        mov     ax,XES[XDI].CACHE_ELEMENT.Handle
        clc
        jmp     PlacedInCache

NoGo:
        INVOKE  FIL4_CLOSE,bx
J_Error:
NotInAtList:
        stc
        mov     ax,3D02h

WasInCache:
PlacedInCache:

        ret

AddCache        ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Check cache handle for a match, if matches then
; set status in cache to FALSE
;-----------------------------------------------------------------------------
CloseCache     PROC NEAR USES SDS XSI

        LDS_SI  CachePointer

        add     XSI,sCACHE_HEADER

LookMore:
        cmp     [XSI].CACHE_ELEMENT.Handle,bx
        je      FoundIt
        cmp     [XSI].CACHE_ELEMENT.Filename,0
        je      NotFound

        mov     XSI,[XSI].CACHE_ELEMENT.NextElement
        jmp     LookMore

FoundIt:
        mov     [XSI].CACHE_ELEMENT.Status,FALSE
        clc
        jmp     Done

NotFound:
        stc
Done:
        ret

CloseCache     ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Read from current position in cache, the number of bytes 
; requested, return number read, monitor end.  If at end or
; beyond end, return 0.
;-----------------------------------------------------------------------------
ReadCache      PROC NEAR USES XBX XCX XDX SDS SES XSI XDI

        LOCAL   lpData:LPSTR

        mov     word ptr lpData+2,ds
        mov     word ptr lpData,dx

        LDS_SI  CachePointer
        LES_DI  CachePointer

        add     XSI,sCACHE_HEADER

LookMore:
        cmp     [XSI].CACHE_ELEMENT.Handle,bx
        je      FoundIt
        cmp     [XSI].CACHE_ELEMENT.Filename,0
        je      NotFound
        mov     XSI,[XSI].CACHE_ELEMENT.NextElement
        jmp     LookMore

FoundIt:
        mov     bx,[XSI].CACHE_ELEMENT.FileSize
        sub     bx,[XSI].CACHE_ELEMENT.FilePosition
        js      ReallyDone
        cmp     bx,cx
        jb      NotEnough

        mov     XDI,XSI
        add     XDI,sCACHE_ELEMENT
        add     XDI,[XSI].CACHE_ELEMENT.FilePosition
        INVOKE  MEM4_COPY,vSES XDI,lpDATA,cx
        mov     ax,cx                   ; Bytes read into AX
        add     [XSI].CACHE_ELEMENT.FilePosition,ax
        clc
        jmp     Done
NotEnough:
        mov     XDI,XSI
        add     XDI,sCACHE_ELEMENT
        add     XDI,[XSI].CACHE_ELEMENT.FilePosition
        INVOKE  MEM4_COPY,vSES XDI,lpDATA,bx
        mov     ax,bx                   ; Bytes read into AX
        add     [XSI].CACHE_ELEMENT.FilePosition,ax
        clc
        jmp     Done
ReallyDone:
        xor     ax,ax
        clc
        jmp     Done
NotFound:
        stc
Done:
        ret

ReadCache      ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Seek the pointer to the proper place in the cache.
;-----------------------------------------------------------------------------
SeekCache       PROC NEAR USES SDS XSI

        LDS_SI  CachePointer

        add     XSI,sCACHE_HEADER

LookMore:
        cmp     [XSI].CACHE_ELEMENT.Handle,bx
        je      FoundIt
        cmp     [XSI].CACHE_ELEMENT.Filename,0
        je      NotFound

        mov     XSI,[XSI].CACHE_ELEMENT.NextElement
        jmp     LookMore

FoundIt:
        cmp     al,0                    ; From beginning
        jne     NotBeg

        mov     [XSI].CACHE_ELEMENT.FilePosition,dx
        mov     ax,dx
        xor     dx,dx
        clc
        jmp     Done
NotBeg:
        cmp     al,1                    ; From current
        jne     NotCurrent
        add     [XSI].CACHE_ELEMENT.FilePosition,dx
        xor     dx,dx
        mov     ax,[XSI].CACHE_ELEMENT.FilePosition
        clc
        jmp     Done
NotCurrent:
        add     [XSI].CACHE_ELEMENT.FilePosition,dx
        xor     dx,dx
        mov     ax,[XSI].CACHE_ELEMENT.FilePosition
        clc
        jmp     Done
NotFound:
        stc
Done:
        ret

SeekCache       ENDP
;-----------------------------------------------------------------------------
END
