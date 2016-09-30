include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DLL4_GETPROCADDRESS                  1/9/93                          CAG
;
;       Return the address of a procedure loaded from a DLL.
;
; DWORD WINAPI DLL4_GETPROCADDRESS(WORD LibHandle,LPSTR Name,WORD MemHandle);
;
; Input:
;       1)  Library handle to load from
;       2)  Name of function to load (or ordial value)
;       3)  Memory handle to load to
; Output:
;       1)  FAR Pointer to code (AX:DX)
;       2)  ERROR <32
;               1 = Not found RESIDENT_Name table
;               2 = Not a movable segment, must be fixed
;               3 = Unable to load the segment
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_GETPROCADDRESS PROC USES XBX XCX SDS SES XSI XDI,
        LibHandle:WORD,
        lpName:LPSTR,
        MemHandle:WORD

        LOCAL   nStatus:WORD
        LOCAL   bySegmentIndex:BYTE
        LOCAL   uOffset:WORD

;--------------------------------------
; Test if we have the ORIDAL number or LPSTR
        cmp     word ptr lpName+2,0     ; If 0 Segment must have ordial
        jne     GetOrdial
        mov     ax,word ptr lpName      ; Ordial value
        jmp     HaveOrdial

GetOrdial:
        mov     nStatus,1
        INVOKE  DLL4_GETORDIAL,lpName,LibHandle
        or      ax,ax
        jz      J_Error
HaveOrdial:
        
;--------------------------------------
; We now have the ordial value, now need to
; get the SEGMENT index, and OFFSET to function

        mov     nStatus,2
        INVOKE  DLL4_GETMOVABLEENTRY,ax,LibHandle
        or      ax,ax
        jz      J_Error

        mov     bySegmentIndex,al
        mov     uOffset,dx


;--------------------------------------
; Now load the segment and all its relocations
        
        mov     nStatus,3
        INVOKE  DLL4_LOADSEGMENT,bySegmentIndex,LibHandle,MemHandle
        or      ax,ax
        jz      J_Error

        mov     dx,uOffset
        jmp     J_Done


J_Error:
        mov     ax,nStatus
J_Done:
        ret

DLL4_GETPROCADDRESS ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; DLL4_GETORDIAL                       1/9/93                          CAG
;
;       Search thru the RESIDENT_NAME table for this function name.	
;
;
; WORD WINAPI DLL4_GETORDIAL(LPSTR Name,WORD LibHandle);
;
; Input:
;       1)  Name of function to look for
;       2)  Library handle
; Output:
;       1)  ERROR (0)
;       2)  Ordial Number of function
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
DLL4_GETORDIAL PROC USES SDS XSI XDI,
        lpName:LPSTR,
        LibHandle:WORD

        mov     ax,LibHandle
        mov     ds,ax
        xor     XSI,XSI

        LDS_SI  [XSI].DYNA_HEADER.lpResident
; Table format: Length(BYTE),String[Length],Ordial[WORD]
        
More:
        cmp     [XSI],byte ptr 0        ; End of table
        je      J_Error

        xor     XCX,XCX
        mov     cl,[XSI]

; cmp ds:si+1,lpname

        LES_DI  lpName
        push    XSI
        push    XCX
        inc     XSI
        repe    cmpsb
        pop     XCX
        pop     XSI
        jne     LookMore
        cmp     XES[XDI],byte ptr 0
        jne     LookMore

        add     XSI,XCX
        inc     XSI
        mov     ax,[XSI]
        jmp     J_Done

LookMore:
        add     XSI,XCX
        add     XSI,3
        jmp     More


J_Error:
J_Done:
        ret

DLL4_GETORDIAL ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; DLL4_GETMOVABLEENTRY                 1/9/93                          CAG
;
;       Get the segment index and offset of a movable entry.	
;
; DWORD WINAPI DLL4_GETMOVABLEENTRY(WORD Ordial,WORD LibHandle);
;
; Input:
;       1)  Ordial number
;       2)  Library handle
; Output:
;       1)  LOWORD(LOBYTE) = SegIndex
;           HIWORD = Offset
;       2)  ERROR (0)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_GETMOVABLEENTRY PROC USES XCX SDS XSI,
        uOrdial:WORD,
        LibHandle:WORD

        mov     ax,LibHandle
        mov     ds,ax
        xor     XSI,XSI
        xor     XDX,XDX

        LDS_SI  [XSI].DYNA_HEADER.lpEntry
        mov     bx,1

NextBundle:
        xor     XCX,XCX
        mov     cl,[XSI]                ; Number of entries
        or      cl,cl
        jz      J_Error

        cmp     byte ptr [XSI+1],0FFh   ; Movable
        je      IsMovable
        cmp     byte ptr [XSI+1],0FEh   ; Movable
        je      IsMovable

        mov     dl,[XSI+1]              ; Segment index
        inc     XSI
        inc     XSI
FindIt2:
        cmp     bx,uOrdial
        je      FoundIt2
        add     XSI,sNEENTRY_FIXED
        inc     bx
        loop    FindIt2
        jmp     NextBundle
FoundIt2:
        mov     al,dl
        mov     dx,[XSI].NEENTRY_FIXED.neFixedOffset
        jmp     WasHere

IsMovable:
        inc     XSI
        inc     XSI
FindIt:
        cmp     bx,uOrdial
        je      FoundIt
        add     XSI,sNEENTRY_MOVABLE
        inc     bx
        loop    FindIt
        jmp     NextBundle

J_Error:
        xor     ax,ax
        jmp     NotHere
FoundIt:
        mov     dx,[XSI].NEENTRY_MOVABLE.neMovableOff
        mov     al,[XSI].NEENTRY_MOVABLE.neMovableSeg

NotHere:
WasHere:
        ret

DLL4_GETMOVABLEENTRY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; DLL4_GETSEGMENTENTRY                 1/9/93                          CAG
;
;       Return the pointer to the entry in the segment table.
;
; DWORD WINAPI DLL4_GETSEGMENTENTRY(BYTE SegIndex,WORD LibHandle);
;
; Input:
;       1)  Segment table index
;       2)  Library handle
; Output:
;       1)  Pointer to entry in table.
;       2)  ERROR (0)
;
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_GETSEGMENTENTRY PROC USES SDS XSI,
        bySegIndex:BYTE,
        LibHandle:WORD

        mov     ax,LibHandle
        mov     ds,ax
        xor     XSI,XSI

        LDS_SI  [XSI].DYNA_HEADER.lpSegment

        xor     XAX,XAX
        mov     al,bySegIndex
        dec     XAX
        mov     cl,sNESEGMENT_TABLE
        mul     cl

        add     XAX,XSI
        mov     dx,ds

        ret

DLL4_GETSEGMENTENTRY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; DLL4_LOADSEGMENTENTRY                1/9/93                          CAG
;
;       Load the actual segment into memory.	
;
; WORD WINAPI DLL4_LOADSEGMENTENTRY(LPSTR SegEntry,WORD LibHandle,WORD MemH);
;
; Input:
;       1)  Pointer to segment entry
;       2)  Library handle
;       3)  Memory handle
; Output:
;       1)  Segment where it was loaded
;       2)  ERROR (0)
; 
; LIBRARY: {INTEL}.LIB
;
;-----------------------------------------------------------------------------
DLL4_LOADSEGMENTENTRY PROC USES XBX XCX XDX SDS SES XSI XDI,
        lpSegEntry:LPSTR,
        uLibHandle:WORD,
        uMemHandle:WORD

        LES_DI  lpSegEntry
        mov     ax,uLibHandle
        mov     ds,ax
        xor     XSI,XSI

        mov     ax,XES[XDI].NESEGMENT_TABLE.neSegDataSec
        mov     cx,16
        mul     cx

        mov     cx,ax                           ; DX:CX location
        INVOKE  FIL4_SEEK,[XSI].DYNA_HEADER.hFilehandle,dx::cx,fSEEK_BEG

; Allocate and lock memory
        mov     cx,XES[XDI].NESEGMENT_TABLE.neSegLength
        add     cx,16
        shr     cx,1
        shr     cx,1
        shr     cx,1
        shr     cx,1                    ; Convert to paragraphs

        INVOKE  MEM4_GALLOCATE,uMemHandle,cx,MEM_CODE
        or      ax,ax
        jz      J_Error

        mov     bx,ax
        INVOKE  MEM4_GLOCK,uMemHandle,bx
        or      dx,dx
        jz      J_Error

        mov     bx,dx
        mov     cx,ax
        INVOKE  FIL4_READ,[XSI].DYNA_HEADER.hFilehandle,bx::cx,
                        XES[XDI].NESEGMENT_TABLE.neSegLength

        cmp     ax,-1
        je      J_Error
        cmp     ax,XES[XDI].NESEGMENT_TABLE.neSegLength
        jne     J_Error
        mov     ax,bx
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

DLL4_LOADSEGMENTENTRY ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; DLL4_LOADSEGMENT                     1/9/93                          CAG
;
;       Load the necessary segment and all its relocations.
;
; WORD WINAPI DLL4_LOADSEGMENT(BYTE SegIndex,WORD LibHandle, WORD MemHandle);
;
; Input:
;       1)  Index of segment to load
;       2)  Library handle
;       3)  Memory handle
; Output:
;       1)  Segment of where code was loaded
;       2)  ERROR (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
DLL4_LOADSEGMENT PROC USES XBX XCX XDX SDS SES XSI XDI,
        bySegmentIndex:BYTE,
        LibHandle:WORD,
        MemHandle:WORD

        LOCAL   lpSegEntry:DWORD
        LOCAL   uSegment:WORD

        LOCAL   RelCount:WORD           ; Number of relocations
        LOCAL   RelStack[4]:WORD        ; 8 bytes of relocation data
        LOCAL   Filepointer:DWORD

;--------------------------------------
; Now get the pointer to the segment entry
        INVOKE  DLL4_GETSEGMENTENTRY,bySegmentIndex,LibHandle
        or      ax,ax
        jz      J_Error
        mov     word ptr lpSegEntry,ax
        mov     word ptr lpSegEntry+2,dx

;--------------------------------------
; Test if we're already loaded, then just return address
        LDS_SI  lpSegEntry
        test    [XSI].NESEGMENT_TABLE.neSTFlags,0004h
        jz      NotLoadedAlready
        mov     ax,[XSI].NESEGMENT_TABLE.neSegDataSec
        jmp     J_Done

NotLoadedAlready:

;--------------------------------------
; Now load this segment into memory
        INVOKE  DLL4_LOADSEGMENTENTRY,lpSegEntry,LibHandle,MemHandle
        or      ax,ax
        jz      J_Error
        mov     uSegment,ax

;--------------------------------------
; Let us know later that we're loaded

        mov     [XSI].NESEGMENT_TABLE.neSegDataSec,ax
        or      [XSI].NESEGMENT_TABLE.neSTFlags,0004h

;--------------------------------------
; Determine if we have relocated data

        LDS_SI  lpSegEntry
        test    [XSI].NESEGMENT_TABLE.neSTFlags,0100h
        jz      NoRelocatedData

        mov     ds,LibHandle
        xor     XSI,XSI

        INVOKE  FIL4_READ,[XSI].DYNA_HEADER.hFilehandle,ADDR RelCount,2
        cmp     ax,-1
        je      J_Error
        cmp     ax,2
        jne     J_Error

MoreFixUps:
        INVOKE  FIL4_READ,[XSI].DYNA_HEADER.hFilehandle,ADDR RelStack,8
        cmp     ax,-1
        je      J_Error
        cmp     ax,8
        jne     J_Error

        INVOKE  FIL4_SEEK,[XSI].DYNA_HEADER.hFilehandle,0,fSEEK_FOR
        mov     word ptr Filepointer,ax
        mov     word ptr Filepointer+2,dx

        push    ss
        pop     ds
        lea     XSI,RelStack

        cmp     [XSI],byte ptr 0        ; Low byte at the specified offset

        cmp     [XSI],byte ptr 2        ; 16 bit selector
        je      J_16Fixup
        cmp     [XSI],byte ptr 3        ; 32 bit pointer
        je      J_32Fixup

        cmp     [XSI],byte ptr 5        ; 16 bit offset
        cmp     [XSI],byte ptr 11       ; 48 bit pointer
        cmp     [XSI],byte ptr 13       ; 32 bit offset

        jmp     J_Error

;--------------------------------------
; Fixup a 16 bit selector
;--------------------------------------
J_16Fixup:
        cmp     [XSI+1],byte ptr 0      ; Is Internal Reference
        jz      Internal16
        cmp     [XSI+1],byte ptr 1      ; Imported ordinal
        cmp     [XSI+1],byte ptr 2      ; Imported name
        cmp     [XSI+1],byte ptr 3      ; OSFIXUP
        jmp     J_Error

Internal16:
        INVOKE  DLL4_LOADSEGMENT,[XSI].NEREL_INT_FIXED.neSegmentNumber,
                                LibHandle,MemHandle
        or      ax,ax
        jz      J_Error

        mov     si,[XSI].NEREL_INT_FIXED.neRelFixMe
        mov     bx,uSegment
        mov     ds,bx
MoreFixMe:
        mov     bx,[XSI]                ; Incase of other fixups
        mov     [XSI],ax                ; Place selector in proper place
        cmp     bx,-1
        je      EndFixup
        mov     si,bx
        jmp     MoreFixMe

;--------------------------------------
; Fixup a 32 bit pointer
;--------------------------------------
J_32Fixup:
        cmp     [XSI+1],byte ptr 0      ; Internal reference
        je      J_Internal32
        cmp     [XSI+1],byte ptr 1      ; Imported ordinal
        cmp     [XSI+1],byte ptr 2      ; Imported name
        cmp     [XSI+1],byte ptr 3      ; OSFIXUP
        jmp     J_Error

J_Internal32:
        INVOKE  DLL4_LOADSEGMENT,[XSI].NEREL_INT_FIXED.neSegmentNumber,
                                LibHandle,MemHandle
        or      ax,ax
        jz      J_Error

; Segment in AX
; Offset in [XSI].NEREL_INT_FIXED.neOffset
        mov     dx,[XSI].NEREL_INT_FIXED.neOffset
; AX:DX where to change the fix to
; Fix uSegment:[XSI].NEREL_INT_FIXED.neRelFixMe
        mov     si,[XSI].NEREL_INT_FIXED.neRelFixMe
        mov     bx,uSegment
        mov     ds,bx
        mov     [XSI],dx
        mov     [XSI+2],ax

;--------------------------------------
; Look for more fixups
;--------------------------------------
EndFixup:
        dec     RelCount
        cmp     RelCount,0
        je      NoMoreFixUps

        mov     ds,LibHandle
        xor     XSI,XSI
        INVOKE  FIL4_SEEK,[XSI].DYNA_HEADER.hFilehandle,FilePointer,fSEEK_BEG
        jmp     MoreFixUps

NoMoreFixUps:
NoRelocatedData:
        mov     ax,uSegment
        jmp     J_Done
J_Error:
        xor     ax,ax
J_Done:
        ret

DLL4_LOADSEGMENT ENDP
;-----------------------------------------------------------------------------
END

