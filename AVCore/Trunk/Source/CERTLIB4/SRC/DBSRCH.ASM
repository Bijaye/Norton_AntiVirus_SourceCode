include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_SEARCH                         12/31/92                        CAG
;
;      Search thru the hashed database for a match.
;
; UINT WINAPI DBC4_SEARCH(DBC4_HEADER FAR *Info, void FAR *LookFor,
;                       void FAR *PutHere);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  PutHere, where we'll put it when found.
; Output:
;       1)  UINT = FALSE(0) Entry not found in database
;                  TRUE(1)  Entry found in database
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
DBC4_SEARCH PROC USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPSTR,
        LookFor:LPSTR,
        PutHere:LPSTR

        LOCAL Index_Addr:DWORD
        LOCAL Data_Addr:DWORD
        LOCAL Chain:CHAIN_TYPE

        LDS_SI  Info
; Get the hash value (CRC) of what we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,LookFor
        mov     bx,ax
        INVOKE  NUM4_MODULUS,bx,[XSI].DBC4_HEADER.IndexSize
        add     ax,1
        adc     dx,0
        mov     word ptr Index_Addr+2,dx
        mov     word ptr Index_Addr,ax

; Seek to index entry
        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4

        cmp     word ptr Data_Addr,0
        jnz     J_0
        cmp     word ptr Data_Addr+2,0
        jz      J_Error
J_0:

; Seek to data record
        INVOKE  Seek_to_data_record,Info,Data_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,PutHere,
                          [XSI].DBC4_HEADER.Elem_Size

        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,LookFor,PutHere
        or      ax,ax
        jz      J_Success

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE
        or      ax,ax
        jz      J_Error

        mov     ax,word ptr Chain.Next
        mov     dx,word ptr Chain.Next+2

        mov     word ptr Data_Addr,ax
        mov     word ptr Data_Addr+2,dx

        cmp     ax,0
        jnz     J_0
        cmp     dx,0
        jnz     J_0

J_Error:
        mov     ax,FALSE
        jmp     J_Done
J_Success:
        mov     ax,TRUE
J_Done:
        ret

DBC4_SEARCH ENDP
;-----------------------------------------------------------------------------
END
