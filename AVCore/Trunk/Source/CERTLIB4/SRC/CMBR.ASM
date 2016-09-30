include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; CMB4_READ                            12/9/92                         CAG
;
;      Read the message from a CMB.
;
; UINT WINAPI CMB4_READ(UINT Message,UINT Handle,LPSTR ptr,UINT BuffSiz);
;
; Input:
;       1)  Message number
;       2)  Handle
;       3)  Where to store
;       4)  Size of buffer
; Output:
;       1)  Bytes Read
;       2)  FAILED (0)
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
CMB4_READ PROC USES XBX XCX SDS XSI,
        uMessage:UINT,
        Filehandle:UINT,
        ptrStorage:LPSTR,
        BuffSize:UINT

        mov     XAX,uMessage
        dec     XAX
        mov     cl,sMESSAGE_HEADER      ; Caculate where to load from header
        mul     cl

;--------------------------------
        inc     XAX                     ; But seek past first word (Number
        inc     XAX                     ;  of messages
        mov     XBX,XAX
        INVOKE  FIL4_SEEK,FileHandle,XBX,fSEEK_BEG

;--------------------------------
        INVOKE  FIL4_READ,FileHandle,ptrStorage,sMESSAGE_HEADER
        or      ax,ax
        jz      J_Done

;--------------------------------
        LDS_SI  ptrStorage
        INVOKE  FIL4_SEEK,FileHandle,[XSI].MESSAGE_HEADER.POINTER,fSEEK_BEG

;--------------------------------
        INVOKE  FIL4_READ,FileHandle,ptrStorage,[XSI].MESSAGE_HEADER.MESSAGE_SIZE

;--------------------------------
J_Done:
        ret

CMB4_READ      ENDP
;-----------------------------------------------------------------------------
END
