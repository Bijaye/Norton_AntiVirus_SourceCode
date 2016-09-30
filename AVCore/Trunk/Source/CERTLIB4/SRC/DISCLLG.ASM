include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_CLUS_TO_LOG              2/25/93                          bAd
;
;       Convert a CLUSTER to a LOGICAL SECTOR NUMBER.
;
; DWORD WINAPI DIS4_CLUS_TO_LOG(WORD Cluster,BYTE Drive);
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
DIS4_CLUS_TO_LOG PROC USES XBX XCX XDI SES,
        Cluster:WORD,
        Drive:BYTE

        LOCAL   lDPB:DPB

; LSN = (CLUSTER-2)SecPerCluster + LSN2

        INVOKE  DIS4_GET_DPB,Drive,addr lDPB ; DS:BX + 0Bh = LSN2
        cmp     ax, -1                       ; DS:BX + 04h = SecPerClus-1
        jz      J_ERROR

        mov     dx, ss
        lea     ax, lDPB
        mov     SES, dx
        mov     XDI, ax
        
        mov     cx, XES[XDI].DPB.dpbFirstSector ; Logical Sector Number for data start
        mov     bl, XES[XDI].DPB.dpbClusterMask ; SecPerCluster-1
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
        mov     ax, -1
        mov     dx, ax
J_Done:

        ret

DIS4_CLUS_TO_LOG ENDP
;-----------------------------------------------------------------------------
END
