include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_CLUSTER_TO_LOGICAL              1/8/93                          CAG
;
;       Convert a CLUSTER to a LOGICAL SECTOR NUMBER.
;
; DWORD WINAPI DIS4_CLUSTER_TO_LOGICAL(WORD Cluster,BYTE Drive);
;
; Input:
;       1)  Cluster
;       2)  Drive (0=Default, 1=A:, 2=B:, etc)
; Output:
;       1)  Logical SECTOR number
;       2)  ERROR (-1)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Notes:
;         LSN = (CLUSTER-2)SecPerCluster + LSN2
;       *LSN2 = Start of DATA of drive (after ROOT directory)
;
;-----------------------------------------------------------------------------
DIS4_CLUSTER_TO_LOGICAL PROC USES XBX SDS XSI,
        Cluster:WORD,
        Drive:BYTE

; LSN = (CLUSTER-2)SecPerCluster + LSN2

        mov     dl,Drive
        mov     ah,32h                  ; DS:BX + 0Bh = LSN2
        int     21h                     ; DS:BX + 04h = SecPerClus-1

        cmp     al,0FFh
        jz      J_ERROR

        mov     XSI,XBX
        mov     cx,[XSI+0Bh]            ; Logical Sector Number for data start
        mov     bl,[XSI+4h]             ; SecPerCluster-1
        inc     bl

        mov     ax,Cluster
        dec     ax
        dec     ax

        xor     bh,bh
        mul     bx
        add     ax,cx
        adc     dx,0
        jmp     J_Done

J_ERROR:
        mov     ax,-1
        mov     dx,-1
J_Done:

        ret

DIS4_CLUSTER_TO_LOGICAL ENDP
;-----------------------------------------------------------------------------
END
