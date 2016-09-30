include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_WRITE                           12/9/92                         CAG
;
;      Write a message to the CMB.
;
; UINT WINAPI CMB4_WRITE(UINT Message,UINT Handle,LPSTR ptr,UINT Size);
;
; Input:
;       1)  Message number
;       2)  Handle
;       3)  What to store
;       4)  Size to store
; Output:
;       1)  FAILED (0)
;       2)  PASSED (Number of bytes written)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_WRITE PROC USES XBX XCX SDS XSI,
        uMessage:UINT,
        Filehandle:UINT,
        ptrStorage:LPSTR,
        uSize:UINT

        LOCAL   CurrentCount:WORD
        LOCAL   SmallBuffer:MESSAGE_HEADER

        INVOKE  FIL4_SIZE,Filehandle    ; Get where to store message
        mov     word ptr SmallBuffer.Pointer,ax
        mov     word ptr SmallBuffer.Pointer+2,dx

        mov     XAX,uSize
        mov     SmallBuffer.Message_Size,XAX

;--------------------------------
; Get current number of messages and add one
        INVOKE  FIL4_SEEK,FileHandle,0,fSEEK_BEG
        INVOKE  FIL4_READ,FileHandle,ADDR CurrentCount,2
        inc     CurrentCount    
        INVOKE  FIL4_SEEK,FileHandle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,FileHandle,ADDR CurrentCount,2


;--------------------------------
        mov     XAX,uMessage
        dec     XAX
        mov     cl,sMESSAGE_HEADER      ; Caculate where to write to header
        mul     cl
;--------------------------------
        inc     XAX                     ; But seek past first word (Number
        inc     XAX                     ;  of messages
        mov     XBX,XAX                 ;  and write pointer to message
        INVOKE  FIL4_SEEK,FileHandle,XBX,fSEEK_BEG
        INVOKE  FIL4_WRITE,FileHandle,ADDR SmallBuffer,sMESSAGE_HEADER

;--------------------------------
        INVOKE  FIL4_SEEK,FileHandle,SmallBuffer.Pointer,fSEEK_BEG
        INVOKE  FIL4_WRITE,FileHandle,ptrStorage,uSize

        ret

CMB4_WRITE      ENDP
;-----------------------------------------------------------------------------
END

