include CTS4.INC

UniqueSegment
;-----------------------------------------------------------------------------
; DBC4_CLOSE                           12/31/92                        CAG
;
; $Header:   S:\certlib4\vcs\dbclose.asv   1.7   27 Apr 1995 16:42:52   DALLEE  $
;
;      Close a HASHED database.
;
; UINT WINAPI DBC4_CLOSE(DBC4_HEADER FAR *Info);
;
; Input:
;       1)  DBC4_HEADER structure for database header.
; Output:
;       1)  UINT = FALSE(0) Error closing
;                  TRUE(1)  Close success
;
; LIBRARY:{INTEL}.LIB
;
; Assembly code was modeled from code Carey wrote in 'C'
;-----------------------------------------------------------------------------
; $Log:   S:\certlib4\vcs\dbclose.asv  $
;  
;     Rev 1.7   27 Apr 1995 16:42:52   DALLEE
;  Fixed 32-bit division problem for 16 bit platforms in find_free_data_entry.
;  Was using remainder of division in DX as high word of assumed 32-bit result.
;  
;     Rev 1.6   29 Nov 1994 13:04:52   DALLEE
;  Reget from Qak2 post NAV.305
;  
;     Rev 1.4.1.0   19 Oct 1993 07:22:12   SCOTTP
;  Branch base for version QAK2
;  
;     Rev 1.4   12 Oct 1993 16:32:44   PATRICKM
;  Baseline from QAK1 branch
;  
;     Rev 1.3.1.1   20 Sep 1993 02:11:28   DALLEE
;  Fixed so all indexes are now 1 based.  Corrected error with 
;  dec ax   sbb dx,0   which should be  sub ax,1  sbb dx,0  since
;  dec doesn't set CY.
;  
;     Rev 1.3.1.0   03 Sep 1993 19:28:34   SCOTTP
;  Branch base for version QAK1
;  
;     Rev 1.3   26 Aug 1993 18:51:22   DALLEE
;  Check for divide by 0 in Find Free Data Entry.
;  
;     Rev 1.2   25 Jun 1993 12:19:40   DALLEE
;  [Fix] FindFreeDataEntry called SEEK_TO_DATA_CHAIN with DWORD data_number
;  with high-low words reversed.  Was mangling the linked list of free entries.
;  
;     Rev 1.1   05 May 1993 22:47:12   DALLEE
;  Seek to correct data entry - not entry + 1.
;-----------------------------------------------------------------------------

DBC4_CLOSE PROC USES SDS XSI,
        Info:LPSTR

        LDS_SI  Info
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,vSDS XSI,sDBC4_HEADER
        INVOKE  FIL4_CLOSE,[XSI].DBC4_HEADER.Filehandle
        mov     ax,1
        ret

DBC4_CLOSE ENDP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
Seek_to_index_entry PROC USES SDS XSI,
        Info:LPSTR,
        IndexEntry:DWORD

        LDS_SI  Info
        mov     ax,word ptr IndexEntry
        mov     dx,word ptr IndexEntry+2

        sub     ax,1
        sbb     dx,0

        mov     cx,4
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.IndexStart
        adc     dx,word ptr [XSI].DBC4_HEADER.IndexStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
        ret

Seek_to_index_entry ENDP
;-----------------------------------------------------------------------------
Seek_to_data_record PROC USES SDS XSI,
        Info:LPSTR,
        Data_Addr:DWORD

        LDS_SI  Info
        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2

        sub     ax,1                    ; First data record is 1 so
        sbb     dx,0                    ; sub 1 to get the offset.

        mov     cx,[XSI].DBC4_HEADER.Total_Elem_Size
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.DataStart
        adc     dx,word ptr [XSI].DBC4_HEADER.DataStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
        ret

Seek_to_data_record ENDP
;-----------------------------------------------------------------------------
Seek_to_data_chain PROC USES SDS XSI,
        Info:LPSTR,
        Last_Rec:DWORD

        LDS_SI  Info
        mov     ax,word ptr Last_Rec
        mov     dx,word ptr Last_Rec+2

        sub     ax,1                    ; First data record is 1 so
        sbb     dx,0                    ; sub 1 to get the offset.

        mov     cx,[XSI].DBC4_HEADER.Total_Elem_Size
        MUL32   cx
        add     ax,[XSI].DBC4_HEADER.Elem_Size
        adc     dx,0
        add     ax,word ptr [XSI].DBC4_HEADER.DataStart
        adc     dx,word ptr [XSI].DBC4_HEADER.DataStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
        ret

Seek_to_data_chain ENDP
;-----------------------------------------------------------------------------
Seek_to_quick_entry PROC USES SDS XSI,
        Info:LPSTR,
        Index_Addr:DWORD

        LDS_SI  Info
        mov     ax,word ptr Index_Addr
        mov     dx,word ptr Index_Addr+2

        sub     ax,1                    ; First record is 1 so
        sbb     dx,0                    ; sub 1 to get the offset.

        mov     cx,2                    ; Multiply Modulus * 2 (WORD size)
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.QuickStart
        adc     dx,word ptr [XSI].DBC4_HEADER.QuickStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
        ret

Seek_to_quick_entry ENDP
;-----------------------------------------------------------------------------
Find_free_data_entry PROC USES SDS XSI XBX,
        Info:LPSTR

        LOCAL   data_num:DWORD
        LOCAL   chain:CHAIN_TYPE

        LDS_SI  Info

        cmp     word ptr [XSI].DBC4_HEADER.Free_Pointer,0
        jne     HasDeleted
        cmp     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,0
        jne     HasDeleted


        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,0,fSEEK_END
        sub     ax,word ptr [XSI].DBC4_HEADER.DataStart
        sbb     dx,word ptr [XSI].DBC4_HEADER.DataStart+2
        mov     cx,[XSI].DBC4_HEADER.Total_Elem_Size

        jcxz    J_Done1                 ; Check for divide by 0

                                        ;=========================
                                        ; Perform 32-bit division...
        push    ax
        mov     ax,dx                   ; Setup high word division
        xor     dx,dx
        div     cx
        mov     bx,ax                   ; Save high word of quotient in BX

        pop     ax                      ; Setup low word division
        div     cx                      ; DX = high remainder already

        mov     dx,bx                   ; DX::AX now 32-bit quotient
                                        ;=========================
J_Done1:
        add     ax,1
        adc     dx,0
        jmp     J_Done

HasDeleted:
        mov     ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        mov     dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2

        INVOKE  Seek_to_data_chain,info,dx::ax

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE

        mov     ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        mov     dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2

        mov     bx,word ptr chain.next
        mov     cx,word ptr chain.next+2

        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer,bx
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,cx
J_Done:
        ret

Find_free_data_entry ENDP
;-----------------------------------------------------------------------------
END


