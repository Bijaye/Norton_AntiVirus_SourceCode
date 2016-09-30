include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DIS4_CLUSTER_TO_ABSOLUTE                  3/2/93                          bAd
;
;      Converts a logical cluster to absolute cylinder, sector, side.
;
; DWORD WINAPI DIS4_CLUSTER_TO_ABSOLUTE(BYTE Drive,DWORD Cluster,LPSTR 
;                                       lpBuffer);
;
; Input:
;       1)  Drive converting on (0=Default,1=A,2=B,3=C,etc.)
;       2)  Cluster to convert 
;       3)  Buffer of at least 512 bytes.
; Output:
;       1)  LOWORD (-1) on failure
;       2)  HIWORD, cylinder
;       3)  LOWORD HIBYTE, side
;       4)  LOWORD LOWBYTE, sector
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
DIS4_CLUSTER_TO_ABSOLUTE PROC USES XBX,
        Drive:BYTE,
        Cluster:DWORD,
        lpBuffer:LPSTR

        LOCAL   stBPB:BPB_struc
        LOCAL   logical:DWORD

        INVOKE  DIS4_CLUS_TO_LOG,word ptr Cluster,Drive
        cmp     ax,-1
        jz      Err
        mov     word ptr logical+2,dx
        mov     word ptr logical,ax

;------------------------------------------
; convert from C=3,D=4 to C=80h,D=81h
;
        mov     bl,Drive
        cmp     bl, 3
        jb      J_0
        shr     bl,1
        shr     bl,1
        shr     bl,1
        or      bl,80h
        jmp     J_1
J_0:
        dec     bl
J_1:
        INVOKE  DIS4_GET_BPB,bl,ADDR stBPB,lpBuffer
        INVOKE  DIS4_LOGICAL_TO_ABSOLUTE,logical,bl,stBPB.SecPerTrack,stBPB.Heads

        jmp     Done
Err:
        mov     ax,-1
Done:        
        ret

DIS4_CLUSTER_TO_ABSOLUTE   ENDP
;-----------------------------------------------------------------------------
END
