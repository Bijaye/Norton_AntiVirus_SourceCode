include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; FIL4_COPY                            12/23/92                        CAG
;
;       Copy files.
;
; UINT WINAPI FIL4_COPY(LPSTR From,LPSTR To, LPSTR Buffer, UINT SizeBuffer);
;
; Input:
;       1)  Filename from
;       2)  Filename to
;       3)  Buffer to use
;       4)  Size of buffer
; Output:
;       1)  PASSED (0)
;       2)  FAILED (1)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
FIL4_COPY PROC,
        From:LPSTR,
        To:LPSTR,
        Buffer:LPSTR,
        sBuffer:UINT

        LOCAL   hFrom:WORD
        LOCAL   hTo:WORD
        LOCAL   nStatus:WORD
        LOCAL   fDate:WORD
        LOCAL   fTime:WORD
        LOCAL   fAttr:WORD

        mov     hFrom,0
        mov     hTo,0
        mov     nStatus,0

        INVOKE  FIL4_OPEN,From,fREAD
        cmp     ax, -1
        jz      J_ERROR
        mov     hFrom,ax

        INVOKE  FIL4_DATETIME,hFrom,0,0,GET_INFO
        mov     fDate,dx
        mov     fTime,ax

        INVOKE  FIL4_ATTR,From,0,GET_INFO
        xor     ah,ah
        mov     fAttr,ax

        INVOKE  FIL4_OPEN,To,fWRITE OR fCREATE
        cmp     ax, -1
        jz      J_ERROR
        mov     hTo,ax

        INVOKE  FIL4_COPY_HANDLE,hFrom,hTo,Buffer,sBuffer,-1
        mov     nStatus,ax

J_ERROR:
        cmp     hFrom,0
        jz      J_1
        INVOKE  FIL4_CLOSE,hFrom
J_1:
        cmp     hTo,0
        jz      J_2
        INVOKE  FIL4_DATETIME,hTo,fDate,fTime,SET_INFO
        INVOKE  FIL4_ATTR,To,fAttr,SET_INFO
        INVOKE  FIL4_CLOSE,hTo
J_2:
        
        mov     ax,nStatus
        ret

FIL4_COPY ENDP
;-----------------------------------------------------------------------------
END
