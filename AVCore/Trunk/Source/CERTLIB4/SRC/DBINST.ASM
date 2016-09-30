include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_INSERT                         12/31/92                        CAG
;
; $Header:   S:\certlib4\vcs\dbinst.asv   1.8   29 Nov 1994 13:10:44   DALLEE  $
;
;      Delete an entry from the database.
;
; UINT WINAPI DBC4_INSERT(DBC4_HEADER *Info, void FAR *AddThis,
;                               void FAR *Work);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  AddThis, what we need to add
;       3)  Work buffer for add stuff.
; Output:
;       1)  UINT = FALSE(0) Entry not added.
;                  TRUE(1)  Entry added.
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dbinst.asv  $
;  
;     Rev 1.8   29 Nov 1994 13:10:44   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.1   15 Jun 1994 15:33:30   DALLEE
;  Update quick lookup info if the 1st entry is updated.
;  
;     Rev 1.4.1.0   19 Oct 1993 07:25:16   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:35:40   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.2.2   20 Sep 1993 02:12:46   DALLEE
;  Fixed so all indexes are now 1 based.  Corrected error with 
;  dec ax   sbb dx,0   which should be  sub ax,1  sbb dx,0  since
;  dec doesn't set CY.
;  
;     Rev 1.3.2.1   17 Sep 1993 04:07:18   CGUC
;  Added test code so we don't crash adding data to a drive
;  with 0 bytes free.
;  
;     Rev 1.3.2.0   03 Sep 1993 19:28:14   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   22 Jun 1993 00:44:34   CJKUO
;  Check return values from FIL4_WRITE/READ.
;  
;     Rev 1.2   13 May 1993 12:45:44   CJKUO
;  Assert USES every register because they do callbacks to C compiled code.
;  Also verified that the routines are written such that they don't
;  have registers that would be destroyed by the callbacks.
;  
;     Rev 1.1   19 Apr 1993 12:26:24   DALLEE
;  Check high word of Data_Addr before we conclude it is an empty entry.
;  Also, commented out int 3 (Should be in debug builds only).
;-----------------------------------------------------------------------------
DBC4_INSERT PROC USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPSTR,
        AddThis:LPSTR,
        Work:LPSTR

        LOCAL Index_Addr:DWORD
        LOCAL Data_Addr:DWORD
        LOCAL data_num:DWORD
        LOCAL new_addr:DWORD
        LOCAL Retry:WORD
        LOCAL Depth:WORD
        LOCAL Quick_Data:WORD
        LOCAL Chain:CHAIN_TYPE
        LOCAL dwFilePointer:DWORD

; int     3h

;--------------------------------------
; If we run out of room on the hard drive,
; we must not allow any thing to modify
; the header or we will corrupt the database
; That is why this code is here, we test
; if the current record can be written
; to the database, if it can't we'll
; error out.
;--------------------------------------

; Seek to end of file
; Get pointer for truncate
        LDS_SI  Info
        mov     bx,[XSI].DBC4_HEADER.FileHandle
        INVOKE  FIL4_SEEK,bx,0,fSEEK_END
        mov     word ptr dwFilePointer+2,dx
        mov     word ptr dwFilePointer,ax

; Seek size of new record.
        mov     ax,[XSI].DBC4_HEADER.Elem_Size
        xor     dx,dx
        INVOKE  FIL4_SEEK,bx,dx::ax,fSEEK_FOR

; Attempt to write chain size
        INVOKE  FIL4_WRITE,bx,ADDR Chain,sCHAIN_TYPE

; On error don't allow it to happen
; Truncate
        push    ax
        INVOKE  FIL4_SEEK,bx,dwFilePointer,fSEEK_BEG
        INVOKE  FIL4_WRITE,bx,ADDR Chain,0
        pop     ax

        cmp     ax,sCHAIN_TYPE          ; Q: Did we write correct amount
        jne     J_Error

;--------------------------------------
; End of new code.
;--------------------------------------

        LDS_SI  Info
; Get the hash value (CRC) of what we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,AddThis
        mov     bx,ax
        INVOKE  NUM4_MODULUS,bx,[XSI].DBC4_HEADER.IndexSize
        add     ax,1
        adc     dx,0
        mov     word ptr Index_Addr+2,dx
        mov     word ptr Index_Addr,ax

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4
        cmp     XAX,4
        jne     J_Error

        cmp     word ptr Data_Addr,0
        jnz     NotFirstEntry
        cmp     word ptr Data_Addr+2,0
        jnz     NotFirstEntry
FirstEntry:

; Save index
        INVOKE  Find_free_data_entry,info
        mov     word ptr data_num,ax
        mov     word ptr data_num+2,dx
        INVOKE  Seek_to_index_entry,info,index_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR data_num,4
        cmp     XAX,4
        jne     J_Error

; Save quick look up
        INVOKE  Seek_to_quick_entry,info,index_addr
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,AddThis
        mov     Quick_Data,ax
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR Quick_Data,2
        cmp     XAX,2
        jne     J_Error

; Save the data record
        INVOKE  Seek_to_data_record,info,data_num
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,
                           [XSI].DBC4_HEADER.Elem_Size
        cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

; Save the chain info
        mov     chain.active,TRUE
        mov     word ptr chain.next,0
        mov     word ptr chain.next+2,0

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error
        jmp     J_Success


NotFirstEntry:
        mov     Retry,50
        mov     Depth,0

dowhile:
        INVOKE  Seek_to_data_record,info,data_addr
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,
                          [XSI].DBC4_HEADER.Elem_Size
        cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error
        
        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,Work,AddThis
        or      ax,ax
        jnz     NotSameKeyField

        INVOKE  Seek_to_data_record,info,data_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,
                           [XSI].DBC4_HEADER.Elem_Size
        cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error
        
        cmp     Depth,0                 ; If we replace the top entry,
        jne     J_Success               ; we need to update the Quick lookup.
        INVOKE  Seek_to_quick_entry,info,index_addr
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,AddThis
        mov     Quick_Data,ax
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR Quick_Data,2
        cmp     XAX,2
        jne     J_Error
        jmp     J_Success

NotSameKeyField:
        INVOKE  Seek_to_data_chain,info,data_addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error

        cmp     word ptr chain.next,0
        jnz     NextChain
        cmp     word ptr chain.next+2,0
        jnz     NextChain

        INVOKE  Find_free_data_entry,info
        mov     word ptr new_addr,ax
        mov     word ptr new_addr+2,dx
        mov     word ptr chain.next,ax
        mov     word ptr chain.next+2,dx

; Save new link
        INVOKE  Seek_to_data_chain,info,data_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error

; Save data record
        INVOKE  Seek_to_data_record,info,new_addr
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,AddThis,
                           [XSI].DBC4_HEADER.Elem_Size
        cmp     XAX,[XSI].DBC4_HEADER.Elem_Size
        jne     J_Error

; Save chain record
        mov     chain.active,TRUE
        mov     word ptr chain.next,0
        mov     word ptr chain.next+2,0

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        jne     J_Error
        jmp     J_Success

NextChain:
        mov     ax,word ptr chain.next
        mov     dx,word ptr chain.next+2
        mov     word ptr data_addr,ax
        mov     word ptr data_addr+2,dx

        inc     depth

        dec     retry
        cmp     retry,0
        jnz     DoWhile

J_Error:
        mov     ax,FALSE
        jmp     J_Done
J_Success:
        mov     ax,TRUE
J_Done:
        ret

DBC4_INSERT ENDP
;-----------------------------------------------------------------------------
END

