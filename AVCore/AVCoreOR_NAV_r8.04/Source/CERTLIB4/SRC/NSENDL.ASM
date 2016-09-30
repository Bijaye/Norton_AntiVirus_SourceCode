include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; NOV4_SEND_LIST                       1/20/93                         CAG
;
;       Send message to list of people.
;
; UINT WINAPI NOV_SEND_LIST(LPSTR ToList, LPSTR Message, LPSTR Buffer);
;
; Input:
;       1)  Users to send to
;       2)  Message to send (55 bytes)
;       3)  Buffer to use (128 bytes)
; Output:
;       1)  PASSED (0)
;       2)  FAILED (?)
;
; LIBRARY: DOS16.LIB
; LIBRARY: DOS32.LIB
; LIBRARY: WIN16.LIB
; LIBRARY: WIN32.LIB
;
; Note:
;       List is comma delimited
;
;-----------------------------------------------------------------------------
NOV4_SEND_LIST PROC USES XBX SDS XSI,
        lpToList:LPSTR,
        lpMessage:LPSTR,
        lpBuffer:LPSTR

        LDS_SI  lpToList
NextUser:
        mov     XBX,XSI
LookMore:
        cmp     [XSI],byte ptr 0
        je      EndOfList
        cmp     [XSI],byte ptr ','
        je      FoundUser
        inc     XSI
        jmp     LookMore

FoundUser:
        mov     byte ptr [XSI],0
        INVOKE  NOV4_SENDMESSAGE,vSDS XBX,lpMessage,lpBuffer
        mov     byte ptr [XSI],','

        or      ax,ax
        jnz     Failed
        inc     XSI
        jmp     NextUser
EndOfList:
        INVOKE  NOV4_SENDMESSAGE,vSDS XBX,lpMessage,lpBuffer

Failed:
        ret

NOV4_SEND_LIST ENDP
;-----------------------------------------------------------------------------
END
