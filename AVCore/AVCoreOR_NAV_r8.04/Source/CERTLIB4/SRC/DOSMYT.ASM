include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET_MYTSR                       1/18/93                         CAG
;
;       Get my original TSR that was put into memory.
;
; DWORD WINAPI SYS4_GET_MYTSR(WORD MySegment);
;
; Input:
;       1)  The code segment of my program.
; Output:
;       1)  Return the address of where this program is found in memory
;       2)  Not Found (0)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
; Note:
;       This function will look thru allocated memory for where
;       the original program was placed.  It'll also look thru
;       the UMBs if found.
;
;-----------------------------------------------------------------------------
SYS4_GET_MYTSR PROC USES XCX SDS SES XSI XDI,
        uSegment:WORD

        LOCAL   wMCB:WORD
        LOCAL   uTest:BYTE


        mov     ax,uSegment
        mov     ds,ax

        mov     uTest,0                 ; 0 = CONV, 1 = UMB

        mov     wMCB,0
LookMore:
        INVOKE  SYS4_GET_MCB_CHAIN,wMCB
        or      ax,ax
        jz      J_Error
        mov     wMCB,ax

        add     ax,1                    ; Over MCB
        add     ax,10h                  ; Over PSP
        mov     es,ax
        mov     cx,100                  ; Max test of 100 bytes or till first null
        xor     XDI,XDI
        xor     XSI,XSI

TestMore:
        cmpsb
        jne     LookMore
        cmp     [XSI-1],byte ptr 0
        je      FoundIt
        loop    TestMore
        jmp     LookMore
FoundIt:
        mov     ax,es
        cmp     ax,uSegment
        jne     J_Ok
J_Error:
        cmp     uTest,0                 ; Next to test UMB
        jne     GetOut
        mov     uTest,1
        mov     wMCB,1                  ; Get UMB
        jmp     LookMore
GetOut:
        xor     ax,ax
J_Ok:
        ret

SYS4_GET_MYTSR ENDP
;-----------------------------------------------------------------------------
END
