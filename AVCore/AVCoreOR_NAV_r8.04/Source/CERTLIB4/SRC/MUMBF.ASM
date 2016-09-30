include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_UMB_FREE                        1/5/93                          CAG
;
;       Free the UMB that was allocated.
;
; WORD WINAPI MEM4_UMB_FREE(WORD Segment);
;
; Input:
;       1)  Segment of UMB to free
; Output:
;       1)  ERROR (0)
;       2)  PASSED(1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
;
;-----------------------------------------------------------------------------
MEM4_UMB_FREE PROC USES XBX XCX XDX,
        lSegment:WORD

        
        LOCAL   UMB_Link:BYTE
        LOCAL   UMB_Strat:WORD
        LOCAL   nStatus:WORD

        mov     dx,lSegment
        mov     ah,MEM_RELEASE_UMB
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
        INVOKE  MEM4_CONV_FREE,lSegment
        mov     nStatus,ax

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

        mov     ax,nStatus
        jmp     J_1

J_ERROR:
        xor     ax,ax
J_1:
        ret

MEM4_UMB_FREE ENDP
;-----------------------------------------------------------------------------
END
