include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; MEM4_GETOBJECT                       12/19/92                        CAG
;
;       Get an object from a memory block.
;
; LPSTR WINAPI MEM4_GETOBJECT(BYTE ObjectID,LPSTR MemoryBlock);
;
; Input:
;       1)  Object ID number
;       2)  Memory block of objects
; Output:
;       1)  Pointer to object
;       2)  NULL if not found
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
MEM4_GETOBJECT PROC USES SDS XSI,
        ObjectID:BYTE,
        Block:LPSTR

        mov     al,ObjectID
        LDS_SI  Block
        xor     XDX,XDX
J_1:
        cmp     [XSI],byte ptr 0
        je      J_2
        cmp     [XSI],al
        je      J_0

        mov     dl,[XSI+1]
        inc     dl
        inc     dl
        add     XSI,XDX                ; Goto next item
        jmp     J_1
J_2:
        xor     ax,ax
        xor     dx,dx
        jmp     J_3

J_0:
        inc     XSI
        inc     XSI
if OS_16
        mov     dx,SDS
endif
        mov     XAX,XSI
J_3:
        ret

MEM4_GETOBJECT ENDP
;-----------------------------------------------------------------------------
END
