include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_MCB_CHAIN                   1/18/93                         CAG
;
;       Return a segment of the MCB chain.
;
; WORD WINAPI SYS4_GET_MCB_CHAIN(WORD Flag or Segment);
;
; Input:
;       1)  To get first entry in DOS, 0
;       2)  To get first entry in UMB, 1
;       3)  To get next entry pass in last segment value
; Output:
;       1)  Segment of MCB
;       2)  Error (0), not found or was last one
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
SYS4_GET_MCB_CHAIN PROC USES XBX XCX XDX SDS SES XSI XDI,
        wWORD:WORD

        LOCAL   UMB_Link:BYTE
        LOCAL   UMB_Strat:WORD
        
        cmp     wWORD,0
        jz      FirstEntryDOS
        cmp     wWORD,1
        jz      FirstEntryUMB

NextEntry:
        mov     ax,wWORD
        mov     ds,ax
        cmp     XDS[0],byte ptr 'Z'
        je      Error
        mov     ax,wWORD
        add     ax,XDS[3]
        inc     ax
        jmp     Done

FirstEntryUMB:
        mov     ax,9FFFh
        mov     ds,ax
        cmp     ds:[0],byte ptr 'Z'
        je      Done
        cmp     ds:[0],byte ptr 'M'
        je      Done
        jmp     Error

FirstEntryDOS:
        xor     bx,bx
        mov     es,bx
        mov     ah,52h
        int     21h
        mov     cx,es
        or      cx,bx
        jz      Error
        mov     ax,es:[bx-2]
        jmp     Done
Error:
        xor     ax,ax
Done:

        ret

SYS4_GET_MCB_CHAIN ENDP
;-----------------------------------------------------------------------------
END
