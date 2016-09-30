; Copyright 1992-1993 Symanted, Peter Norton Product Group
;-----------------------------------------------------------------------------
;
; $Header:   S:\certlib4\vcs\dbdel.asv   1.5   29 Nov 1994 13:08:36   DALLEE  $
;
; Filname:      DBDEL.ASM
; Contains:     DBC4_DELETE
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dbdel.asv  $
;  
;     Rev 1.5   29 Nov 1994 13:08:36   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.3.1.0   19 Oct 1993 07:26:24   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.3   12 Oct 1993 16:36:44   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.2.2.1   20 Sep 1993 02:12:44   DALLEE
;  Fixed so all indexes are now 1 based.  Corrected error with 
;  dec ax   sbb dx,0   which should be  sub ax,1  sbb dx,0  since
;  dec doesn't set CY.
;  
;     Rev 1.2.2.0   03 Sep 1993 19:28:44   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.2   27 May 1993 13:21:32   DALLEE
;  Removed unused LastChain variable.
;  Fixed handling of entries not found in database.
;  Made sure Chain struct was initialized before use.
;-----------------------------------------------------------------------------

include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_DELETE                         12/31/92                        CAG
;
;      Delete an entry from the database.
;
; UINT WINAPI DBC4_DELETE(DBC4_HEADER FAR *Info, void FAR *LookFor,
;                               void FAR *Work);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  Work buffer for deletion stuff.
; Output:
;       1)  UINT = FALSE(0) Entry not found in database
;                  TRUE(1)  Entry found in database
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
DBC4_DELETE PROC USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPSTR,
        LookFor:LPSTR,
        Work:LPSTR

        LOCAL Index_Addr:DWORD
        LOCAL Data_Addr:DWORD
        LOCAL Last_Rec:DWORD
        LOCAL Depth:WORD
        LOCAL Quick_Value:WORD
        LOCAL Done:WORD
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

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4

        cmp     word ptr Data_Addr,0
        jnz     J_0
        cmp     word ptr Data_Addr+2,0
        jz      J_Error

J_0:
        mov     Depth,0
        mov     Done,FALSE
        mov     word ptr Last_Rec,0
        mov     word ptr Last_Rec+2,0

DoWhile:
        INVOKE  Seek_to_data_record,Info,Data_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,
                          [XSI].DBC4_HEADER.Elem_Size

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE

        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,LookFor,Work
        or      ax,ax
        jnz     NextRecord


        cmp     Depth,0
        jnz     DepthGreaterThanZero

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Chain.Next,4

        cmp     word ptr Chain.Next+2,0
        jnz     ChangeQuick
        cmp     word ptr Chain.Next,0
        jz      NoQuick

ChangeQuick:

        INVOKE  Seek_to_data_record,Info,Chain.Next

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,
                          [XSI].DBC4_HEADER.Elem_Size

        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,Work
        mov     Quick_Value,ax

WriteQuick:
        INVOKE  Seek_to_quick_entry,Info,Index_Addr

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Quick_Value,2
        jmp     UpdateDeleted
NoQuick:
        mov     Quick_Value,0
        jmp     WriteQuick


DepthGreaterThanZero:
        INVOKE  Seek_to_data_chain,Info,Last_Rec

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE
        jmp     UpdateDeleted

NextRecord:
        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2
        mov     word ptr Last_Rec,ax
        mov     word ptr Last_Rec+2,dx

        mov     ax,word ptr Chain.Next
        mov     dx,word ptr Chain.Next+2
        mov     word ptr Data_Addr,ax
        mov     word ptr Data_Addr+2,dx

        inc     Depth

        cmp     word ptr Data_Addr,0
        jnz     DoWhile
        cmp     word ptr Data_Addr+2,0
        jnz     DoWhile
        jmp     J_Error

UpdateDeleted:
        mov     Chain.Active,FALSE
        mov     ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        mov     dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2
        mov     word ptr Chain.Next,ax
        mov     word ptr Chain.Next+2,dx

; Seek to data chain
        INVOKE  Seek_to_data_chain,Info,Data_Addr

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE

        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer,ax
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,dx
        jmp     J_Success

J_Error:
        mov     ax,FALSE
        jmp     J_Done
J_Success:
        mov     ax,TRUE
J_Done:
        ret

DBC4_DELETE ENDP
;-----------------------------------------------------------------------------
END
