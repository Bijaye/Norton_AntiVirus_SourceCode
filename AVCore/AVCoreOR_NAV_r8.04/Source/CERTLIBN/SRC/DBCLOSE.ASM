;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBCLOSE.ASv   1.6   08 Mar 1996 14:56:30   DALLEE  $
;;
;; Description: Defines DBC4_CLOSE function.
;;              Defines some positioning functions.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBCLOSE.ASv  $
;  
;     Rev 1.6   08 Mar 1996 14:56:30   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.5   11 Jan 1996 17:57:20   JWORDEN
;  Port changes from Quake 8
;  
;     Rev 1.4.1.1   13 Jul 1995 16:47:50   DALLEE
;  Modified find_free_data_entry to perform the space check and only if 
;  we're adding a new entry.  Allows us to remove the seek back and truncate
;  which causes problems with Win 95's NWREDIR.
;  (They've promised to fix, but only in the first in-line).
;  
;     Rev 1.4.1.0   12 Jul 1995 18:58:38   MPYE
;  Branch base for version QAK8
;  
;     Rev 1.4   27 Apr 1995 16:37:28   DALLEE
;  Fixed 32-bit division in find_free_data_entry for 16 bit platforms.
;  Was using remainder of division in DX as high word of assumed 32-bit result.
;  
;     Rev 1.3   27 Apr 1995 02:49:08   SZIADEH
;  zero out the components of DBC4_HEADER structure that are not
;  constants (meaning could be different between DOS and WINDOWS)
;  
;     Rev 1.2   19 Dec 1994 15:15:20   BRAD
;  Find_free_data_Entry() incorrectly converted to 32-bit
;  
;     Rev 1.1   17 Aug 1994 13:25:32   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   30 May 1994 10:24:48   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment


COPY_MEM PROC USES XCX SDS SES XSI XDI,
        lpFrom:LPBYTE,
        lpTo:LPBYTE,
        uCount:UINT

        cld
        LDS_SI  lpFrom
        LES_DI  lpTo
        mov     XCX,uCount
        rep     movsb

        ret

COPY_MEM ENDP

;----------------------------------------------------------------------------
; DBC4_CLOSE
;
; Close a HASHED database.
;
; BOOL WINAPI DBC4_CLOSE ( DBC4_HEADER FAR *Info );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
; Output:
;       1)  BOOL = FALSE(0) Error closing
;                  TRUE(1)  Close success
;----------------------------------------------------------------------------
DBC4_CLOSE  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE


        LOCAL   dbcHeader: DBC4_HEADER


        INVOKE  COPY_MEM,  Info, ADDR dbcHeader, sDBC4_HEADER

        mov     XBX, dbcHeader.DBC4_HEADER.FileHandle
        mov     dbcHeader.DBC4_HEADER.FileHandle,0

IF OS_16
        mov     word ptr dbcHeader.DBC4_HEADER.Comp_Func,0
        mov     word ptr dbcHeader.DBC4_HEADER.Comp_Func+2,0
        mov     word ptr dbcHeader.DBC4_HEADER.Hash_Func,0
        mov     word ptr dbcHeader.DBC4_HEADER.Hash_Func+2,0
        mov     word ptr dbcHeader.DBC4_HEADER.Quick_Func,0
        mov     word ptr dbcHeader.DBC4_HEADER.Quick_Func+2,0
ELSE
        mov     dbcHeader.DBC4_HEADER.Comp_Func, 0
        mov     dbcHeader.DBC4_HEADER.Hash_Func, 0
        mov     dbcHeader.DBC4_HEADER.Quick_Func,0

ENDIF

        INVOKE  FIL4_SEEK,XBX,0,fSEEK_BEG
        INVOKE  FIL4_WRITE,XBX,ADDR dbcHeader,sDBC4_HEADER
        INVOKE  FIL4_CLOSE,XBX
        mov     XAX,TRUE
        ret

DBC4_CLOSE  ENDP

;----------------------------------------------------------------------------
; ???
;----------------------------------------------------------------------------
Seek_to_index_entry  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE,
        IndexEntry:DWORD

        LDS_SI  Info
IF OS_16
        mov     ax,word ptr IndexEntry
        mov     dx,word ptr IndexEntry+2

        sub     ax,1
        sbb     dx,0

        mov     cx,4
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.IndexStart
        adc     dx,word ptr [XSI].DBC4_HEADER.IndexStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
ELSE
        mov     eax,4
        dec     IndexEntry
        mul     IndexEntry
        add     eax,[XSI].DBC4_HEADER.IndexStart
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,eax,fSEEK_BEG
ENDIF
        ret

Seek_to_index_entry  ENDP

;----------------------------------------------------------------------------
; ???
;----------------------------------------------------------------------------
Seek_to_data_record  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE,
        Data_Addr:DWORD

        LDS_SI  Info
IF OS_16
        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2

        sub     ax,1                    ; First data record is 1 so
        sbb     dx,0                    ; sub 1 to get the offset.

        mov     cx,[XSI].DBC4_HEADER.Total_Elem_Size
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.DataStart
        adc     dx,word ptr [XSI].DBC4_HEADER.DataStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
ELSE
        movzx   eax,[XSI].DBC4_HEADER.Total_Elem_Size
        dec     Data_Addr
        mul     Data_Addr
        add     eax,[XSI].DBC4_HEADER.DataStart
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,eax,fSEEK_BEG
ENDIF
        ret

Seek_to_data_record  ENDP

;----------------------------------------------------------------------------
; ???
;----------------------------------------------------------------------------
Seek_to_data_chain  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE,
        Last_Rec:DWORD

        LDS_SI  Info
IF OS_16
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
ELSE
        movzx   eax,[XSI].DBC4_HEADER.Total_Elem_Size
        dec     Last_Rec
        mul     Last_Rec
        movzx   ecx,[XSI].DBC4_HEADER.Elem_Size
        add     eax,ecx
        add     eax,[XSI].DBC4_HEADER.DataStart
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,eax,fSEEK_BEG
ENDIF
        ret

Seek_to_data_chain  ENDP

;----------------------------------------------------------------------------
; ???
;----------------------------------------------------------------------------
Seek_to_quick_entry  PROC  CODEDISTANCE  USES SDS XSI,
        Info:LPBYTE,
        Index_Addr:DWORD

        LDS_SI  Info
IF OS_16
        mov     ax,word ptr Index_Addr
        mov     dx,word ptr Index_Addr+2

        sub     ax,1                    ; First record is 1 so
        sbb     dx,0                    ; sub 1 to get the offset.

        mov     cx,2                    ; Multiply Modulus * 2 (WORD size)
        MUL32   cx
        add     ax,word ptr [XSI].DBC4_HEADER.QuickStart
        adc     dx,word ptr [XSI].DBC4_HEADER.QuickStart+2
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,dx::ax,fSEEK_BEG
ELSE
        mov     eax,Index_Addr
        dec     eax
        shl     eax,1
        add     eax,[XSI].DBC4_HEADER.QuickStart
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.Filehandle,eax,fSEEK_BEG
ENDIF
        ret

Seek_to_quick_entry  ENDP

;----------------------------------------------------------------------------
; Gets the first free data entry from the deleted list, or adds a blank to
; the end of the file (writes the blank entry as a space check).
;
; Returns:
;       Non-zero        1-based index of free entry.
;       0               Error adding blank entry to end of database.
;----------------------------------------------------------------------------
Find_free_data_entry  PROC  CODEDISTANCE  USES SDS XSI XBX,
        Info:LPBYTE

        LOCAL   data_num:DWORD
        LOCAL   chain:CHAIN_TYPE

        LDS_SI  Info
IF OS_16
        cmp     word ptr [XSI].DBC4_HEADER.Free_Pointer,0
        jne     HasDeleted
        cmp     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,0
        jne     HasDeleted


        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,0,fSEEK_END
        mov     word ptr data_num,ax
        mov     word ptr data_num+2,dx

                                        ; Add new record to end.
                                        ; Seek size of new record.
        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,XAX,fSEEK_FOR

                                        ; Attempt to write chain size
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        je      J_HasSpace

                                        ; On error reset size and return 0
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,data_num,fSEEK_BEG
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,0

        xor     XAX,XAX
        xor     XDX,XDX
        jmp     J_Done

  J_HasSpace:
        mov     ax,word ptr data_num
        mov     dx,word ptr data_num+2
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
        jmp     SHORT J_Done

  HasDeleted:
        mov     ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        mov     dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2

        INVOKE  Seek_to_data_chain,info,dx::ax

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE

        mov     ax,word ptr chain.next
        mov     dx,word ptr chain.next+2

        xchg    ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        xchg    dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2
ELSE
        cmp     [XSI].DBC4_HEADER.Free_Pointer,0
        jne     HasDeleted

        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,0,fSEEK_END
        mov     data_num,eax

                                        ; Add new record to end.
                                        ; Seek size of new record.
        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,XAX,fSEEK_FOR

                                        ; Attempt to write chain size
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE
        cmp     XAX,sCHAIN_TYPE
        je      J_HasSpace

                                        ; On error reset size and return 0
        INVOKE  FIL4_SEEK,[XSI].DBC4_HEADER.FileHandle,data_num,fSEEK_BEG
        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.FileHandle,ADDR chain,0

        xor     XAX,XAX
        jmp     J_Done

  J_HasSpace:
        mov     eax,data_num
        sub     eax,[XSI].DBC4_HEADER.DataStart
        movzx   ecx,[XSI].DBC4_HEADER.Total_Elem_Size
        jecxz   J_Done1
        xor     edx,edx
        div     ecx
  J_Done1:
        inc     eax
        jmp     SHORT J_Done

  HasDeleted:
        INVOKE  Seek_to_data_chain,info,[XSI].DBC4_HEADER.Free_Pointer
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.FileHandle,ADDR chain,sCHAIN_TYPE

        mov     eax,chain.next
        xchg    eax,[XSI].DBC4_HEADER.Free_Pointer
ENDIF

  J_Done:
        ret

Find_free_data_entry  ENDP

UniqueSegmentEnd
END
