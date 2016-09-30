include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; SYS4_GET2_ORIG13                     1/22/93                         CAG
;
;       Get original interrupt 13h using search for INT 40h method.
;
; DWORD WINAPI SYS4_GET2_ORIG13(VOID);
;
; Output:
;       1)  Segment:Offset of where found
;       2)  Not found (0)
;
; LIBRARY:DOS16.LIB	
; LIBRARY:DOS32.LIB	
; LIBRARY:WIN16.LIB	
; LIBRARY:WIN32.LIB	
;
;-----------------------------------------------------------------------------
SYS4_GET2_ORIG13 PROC

        LOCAL   lpLookHere:DWORD
        LOCAL   MyLook[4]:BYTE
        LOCAL   MyLook2[3]:BYTE

        mov     MyLook[0],80h
        mov     MyLook[1],0FAh
        mov     MyLook[2],80h
        mov     MyLook[3],0h

        mov     MyLook2[0],0CDh
        mov     MyLook2[1],040h
        mov     MyLook2[2],0
        
        mov     ax, 0F000h
ifdef WIN16
        INVOKE  MEM4_SEGTOSEL, ax
endif
        mov     word ptr lpLookHere+2,ax

        INVOKE  MEM4_SRCH_STRING,lpLookHere,ADDR MyLook,-1
        or      ax,ax
        jz      NotHere
        mov     word ptr lpLookHere,ax

        INVOKE  MEM4_SRCH_STRING,lpLookHere,ADDR MyLook2,40
        or      ax,ax
        jnz     IsHere

        add     word ptr lpLookHere,3
        mov     ax,word ptr lpLookHere
        neg     ax
        mov     dx,ax
        INVOKE  MEM4_SRCH_STRING,lpLookHere,ADDR MyLook,dx
        or      ax,ax
        jz      NotHere
        mov     word ptr lpLookHere,ax

        INVOKE  MEM4_SRCH_STRING,lpLookHere,ADDR MyLook2,40
        or      ax,ax
        jz      NotHere

IsHere:
        mov     ax,word ptr lpLookHere
        mov     dx,0F000h
        jmp     Found

NotHere:
        xor     ax,ax
        xor     dx,dx
Found:
        ret

SYS4_GET2_ORIG13 ENDP
;-----------------------------------------------------------------------------
END
