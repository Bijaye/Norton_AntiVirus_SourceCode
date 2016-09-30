include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_UMB_ALLOCATE                     1/5/92                          CAG
;
;      Allocate memory in the Upper Memory Block, use the standard
;      XMS driver method, if this fails, use the DOS 5 method.
;
; DWORD WINAPI MEM4_UMB_ALLOCATE(WORD Paragraphs);
;
; Input:
;       1)  Number of paragraphs I want to allocate
; Output:
;       1)  LOWORD ERROR (0) HIWORD Paragraphs available
;       2)  LOWORD Segment of UMB
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_UMB_ALLOCATE PROC USES XBX SDS XSI,
        uParagraphs:WORD

        LOCAL   UMB_Link:BYTE
        LOCAL   UMB_Strat:WORD
        LOCAL   UMB_Segment:WORD
        LOCAL   UMB_Avail:WORD

        mov     UMB_Segment,0
        mov     UMB_Avail,0
;---------------------------------
; Try old method to allocate first

        mov     dx,uParagraphs
        mov     ah,MEM_REQUEST_UMB      ; 10h
        INVOKE  XMS_CALL
        or      ax,ax
        jz      J_0
        mov     ax,bx
        jmp     J_1

;---------------------------------
J_0:                                    ; Try DOS 5 method
        mov     ax,5802h                ; Get and store UMB link state
        int     21h
        jc      J_ERROR
        mov     UMB_Link,al

;---------------------------------
        mov     ax,5803h                ; Set link state=ENABLED
        mov     bx,1
        int     21h
        jc      J_ERROR

;---------------------------------
        mov     ax,5800h                ; Get and store Memory Strategy
        int     21h
        jc      J_2
        mov     UMB_Strat,ax                  

        mov     ax,5801h                ; Set Allocation strategy=
        mov     bx,41h                  ;  BEST_FIT_HIGHONLY
        int     21h
        jc      J_2

;---------------------------------
        mov     ah,48h                  ; Allocate memory from UMB
        mov     bx,uParagraphs
        int     21h
        jc      J_NoGo
        mov     UMB_SEGMENT,ax
J_NoGo:
        mov     UMB_Avail,bx
;---------------------------------
        mov     ax,5801h                ; Return Memory Strategy
        mov     bx,UMB_Strat
        int     21h

;---------------------------------
J_2:
        mov     ax,5803h                ; Return UMB Link State
        xor     bh,bh
        mov     bl,UMB_Link
        int     21h
        mov     ax,UMB_SEGMENT
        mov     dx,UMB_Avail
        cmp     ax,0
        jz      J_Error

        dec     ax                      ;  mark MEMORY_ARENA with PSP
        mov     ds,ax                   ;  =-1 for now
        mov     XSI,1
        mov     XDS[XSI],word ptr -1
        mov     ax,UMB_SEGMENT
        jmp     J_1

J_ERROR:
        xor     ax,ax
J_1:
        ret

MEM4_UMB_ALLOCATE ENDP
;-----------------------------------------------------------------------------
XMS_CALL PROC USES SDS SES XDI XSI

        LOCAL   CallMe:DWORD

; Test if a memory manager is installed.
        push    ax
        push    bx
        push    cx
        push    dx
        mov     ax,4300h
        int     2Fh
        cmp     al,80h
        je      J0
J_Error:
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        xor     ax,ax
        jmp     J_Done
J0:
; Get address of entry point.
        mov     ax,4310h
        int     2Fh                     ; Get address of entry point
        mov     word ptr CallMe,bx
        mov     word ptr CallMe+2,es

        pop     dx
        pop     cx
        pop     bx
        pop     ax

        call    DWORD PTR CallMe
        jnc     J_Done
        xor     ax,ax
J_Done:
        ret

XMS_CALL ENDP
;-----------------------------------------------------------------------------
END

