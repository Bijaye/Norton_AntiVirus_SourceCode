include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_SETOBJECT                       12/19/92                        CAG
;
;       Set an object in a memory block.
;
; void WINAPI MEM4_SETOBJECT(BYTE ObjectID,LPSTR MemoryBlock,
;                             LPSTR Object,WORD LengthObject);
;
; Input:
;       1)  Object ID number
;       2)  Memory block of objects
;       3)  Pointer to object to store
;       4)  Length of object to store
;
; Memory block in this form:
;       ID,Length,Object
;       ID,Length,Object
;       0
;       Byte,Byte,Byte[length]
;
; LIBRARY:{INTEL}.LIB
;
;-----------------------------------------------------------------------------
MEM4_SETOBJECT PROC USES SDS SES XSI XDI,
        ObjectID:BYTE,
        Block:LPSTR,
        ObjectText:LPSTR,
        ObjectLength:WORD

        LES_DI  Block
        LDS_SI  ObjectText
        xor     XDX,XDX
J_1:
        cmp     XES[XDI],byte ptr 0
        je      J_2

        mov     dl,XES[XDI+1]
        inc     dl
        inc     dl
        add     XDI,XDX                 ; Goto next item
        jmp     J_1
J_2:

        mov     al,ObjectID
        stosb                           ; Store the id in ES:DI
        mov     ax,ObjectLength
        stosb                           ; Store the length
        mov     cx,ax
        rep     movsb                   ; Store the string
        xor     al,al
        stosb                           ; Store the terminate null

        ret

MEM4_SETOBJECT ENDP
;-----------------------------------------------------------------------------
END
