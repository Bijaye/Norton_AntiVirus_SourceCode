include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_LOGICAL_TO_CLUSTER              1/8/93                          CAG
;
;       Convert a LOGICAL SECTOR NUMBER to a CLUSTER number.
;
; WORD WINAPI DIS4_LOGICAL_TO_CLUSTER(DWORD Sector,BYTE Drive);
;
; Input:
;       1)  Long sector number
;       2)  Drive  (0 Default, 1=A: etc)
; Output:
;       1)  Cluster number
;       2)  ERROR (-1)
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Notes:
;         CLUSTER = ((LSN - LSN2)/SecPerCluster)+2
;       *LSN2 = Start of DATA of drive (after ROOT directory)
;
;-----------------------------------------------------------------------------
DIS4_LOGICAL_TO_CLUSTER PROC USES XBX SDS XSI,
        Sector:DWORD,
        Drive:BYTE

; CLUSTER = ((LSN2 - LSN)/SecPerCluster)+2
        mov     dl,Drive
        mov     ah,32h                  ; DS:BX + 0Bh = LSN2
        int     21h                     ; DS:BX + 04h = SecPerClus-1

        cmp     al,0FFh
        je      J_ERROR

        mov     XSI,XBX
        mov     cx,[XSI+0Bh]            ; Logical Sector Number for data start
        mov     bl,[XSI+4h]             ; SecPerCluster-1
        inc     bl

        sub     word ptr Sector,cx
        sbb     word ptr Sector+2,0
        mov     ax,word ptr Sector
        mov     dx,word ptr Sector+2

        xor     bh,bh
        or      bx,0
        jz      J_ERROR
        div     bx

        inc     ax
        inc     ax
        jmp     J_Done

J_ERROR:
        mov     ax,-1
J_Done:
        ret

DIS4_LOGICAL_TO_CLUSTER ENDP
;-----------------------------------------------------------------------------
END
