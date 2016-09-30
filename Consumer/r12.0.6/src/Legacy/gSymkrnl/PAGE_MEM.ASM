; Copyright 1993 Symantec Corporation                                  
;****************************************************************************
; 
; $Header:   S:/SYMKRNL/VCS/PAGE_MEM.ASv   1.0   26 Jan 1996 20:21:52   JREARDON  $  
; 
; Description: Memory page table access routines
; 
; Contains: 
; 
; See Also: 
; 
;****************************************************************************
; $Log:   S:/SYMKRNL/VCS/PAGE_MEM.ASv  $  
;  
;     Rev 1.0   26 Jan 1996 20:21:52   JREARDON
;  Initial revision.
;  
;     Rev 1.0   22 Dec 1994 03:10:48   JMILLARD
;  Initial revision.
;  
;****************************************************************************

include platform.inc

  OPTION  LANGUAGE:PASCAL

include page_mem.inc

;
;       local prototypes
;

CheckMemoryBlock PROTO NEAR,lpData:LPBYTE, lpStartAddress:LPBYTE, dwLen:DWORD

SCAN_BLOCK_SIZE EQU     04000h          ; arbitrary size for server friendly
                                        ; scanning


        .code @Filename
;-----------------------------------------------------------------------------
; CheckPageValid
;
;       Check Page Valid
;
; UINT WINAPI CheckPageValid(LPBYTE StartAddress);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
CheckPageValid PROC USES XBX XCX XSI,
        lpPageAddress:LPBYTE

        mov     cx,  0ffh               ; assume page ok

        mov     eax, cr0                ; test paging enabled
        test    eax, PAGING_ENABLE
        jz      CPV_Done                ; no, then the page is ok

        ; extract the page table indices from the address

        mov     eax, lpPageAddress      
        shr     eax, PAGE_OFFSET_SIZE   ; drop page offset part of address
        mov     ebx, eax        
        and     ebx, PAGE_MASK          ; page PAGE value
        shl     ebx, 2                  ; dw index

        shr     eax, PAGE_PAGE_SIZE     ; page DIR value
        shl     eax, 2                  ; dw index

        mov     esi, cr3                ; get page directory base address
        add     esi, eax                ; physical address of page directory

        mov     eax, [esi]              ; get page directory entry
        or      eax, eax
        jz      CPV_BadPage             ; no entry, page bad

        and     eax, PF_FLAG_MASK       ; convert entry to address by 
                                        ; pitching flags

        add     eax, ebx                ; offset to requested entry
        mov     esi, eax
        mov     eax, [esi]              ; get page table entry

        test    eax, PF_FLAG_PRESENT    ; page present?
        jnz     CPV_Done                ; yes, ok flag already set

CPV_BadPage:
	xor	cx,cx                   ; page invalid

CPV_Done:
        movzx   XAX,cx                  ; return platform sized result
        ret


CheckPageValid        ENDP








;-----------------------------------------------------------------------------
; CountValidPageBytes
;
;       Check Pages Valid - return number of bytes to scan
;
; DWORD WINAPI CountValidPageBytes(LPBYTE StartAddress, DWORD dwMaxBytes);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
CountValidPageBytes PROC USES XBX XCX XSI XDI,
        lpPageAddress:LPBYTE,
        dwMaxBytes:DWORD

        mov     XDI, lpPageAddress
        mov     XCX, dwMaxBytes
        xor     XBX,XBX                 ; clear total byte count

@@:
        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jz      @F                      ; no, end of page set
        add     XDI,1000h               ; yes, count page
        add     XBX,1000h               ; count bytes to return
        sub     XCX,1000h       
        jg      @B                      ; test next page

@@:
        cmp     XBX, dwMaxBytes         ; check overflow
        jb      @F
        mov     XBX,dwMaxBytes
@@:
        mov     XAX,XBX                 ; return valid bytes
        ret

CountValidPageBytes        ENDP


;-----------------------------------------------------------------------------
; BytesToFirstValidPage
;
;       Find first valid page - return bytes to start of first valid page
;
; DWORD WINAPI BytesToFirstValidPage(LPBYTE StartAddress, DWORD dwMaxBytes);
;
;-----------------------------------------------------------------------------
; UPDATE                               1/2/93				CAG
;
;-----------------------------------------------------------------------------
BytesToFirstValidPage PROC USES XBX XCX XSI XDI,
        lpPageAddress:LPBYTE,
        dwMaxBytes:DWORD

        mov     XDI, lpPageAddress
        mov     XCX, dwMaxBytes
        xor     XBX,XBX                 ; clear total byte count

@@:
        INVOKE  CheckPageValid,XDI      ; test page
        or      XAX,XAX                 ; page valid?
        jnz     @F                      ; yes, valid page found
        add     XDI,1000h               ; no, skip page
        add     XBX,1000h               ; count bytes to return
        sub     XCX,1000h       
        jg      @B                      ; test next page

@@:
        cmp     XBX, dwMaxBytes         ; check overflow
        jb      @F
        mov     XBX,dwMaxBytes
@@:
        mov     XAX,XBX                 ; return invalid bytes
        ret

BytesToFirstValidPage        ENDP

        END


