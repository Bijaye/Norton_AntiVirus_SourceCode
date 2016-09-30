;; Copyright 1993-1994 Symantec Corporation
;;===========================================================================
;;
;; $Header:   S:/CERTLIBN/VCS/DBDEL.ASv   1.5   08 Mar 1996 14:56:32   DALLEE  $
;;
;; Description: Defines DBC4_DELETE function.
;;
;; See Also:
;;
;;---------------------------------------------------------------------------
;; $Log:   S:/CERTLIBN/VCS/DBDEL.ASv  $
;  
;     Rev 1.5   08 Mar 1996 14:56:32   DALLEE
;  Changed LPSTR to LPBYTE for non-string data.
;  
;     Rev 1.4   05 Apr 1995 13:48:04   DALLEE
;  Fixed bug for 32 bit platforms.
;  Calling INVOKE with a WORD in place of a DWORD parameter assembles to
;   PUSH dword ptr 0, PUSH word ptr WORD.  A total of 6 bytes, not the
;   expected 4.
;  This is bad.
;  
;     Rev 1.3   30 Jan 1995 17:35:44   BRAD
;  Changed some functions to take a size record paramater
;  
;     Rev 1.2   19 Dec 1994 16:33:48   BRAD
;  Fixed up modulo function for 32-bit
;  
;     Rev 1.1   17 Aug 1994 13:25:36   CAREY
;  Added UniqueSegmentEnd for VXD platform and bSYM_VXD in proper areas...
;  
;     Rev 1.0   30 May 1994 11:04:30   RSTANEV
;  Initial revision.
;
;;---------------------------------------------------------------------------

INCLUDE CTSN.INC

UniqueSegment

;----------------------------------------------------------------------------
; DBC4_DELETE
;
; Delete an entry from the database.
;
; BOOL WINAPI DBC4_DELETE ( DBC4_HEADER FAR *Info, void FAR *LookFor,
;                           void FAR *Work );
;
; Input:
;       1)  DBC4_HEADER structure for database header.
;       2)  LookFor, what we need to look for.
;       3)  Work buffer for deletion stuff.
; Output:
;       1)  BOOL = FALSE(0) Entry not found in database
;                  TRUE(1)  Entry found in database
;----------------------------------------------------------------------------
DBC4_DELETE  PROC  CODEDISTANCE  USES SDS SES XBX XCX XDX XDI XSI,
        Info:LPBYTE,
        LookFor:LPBYTE,
        Work:LPBYTE

        LOCAL Index_Addr:DWORD
        LOCAL Data_Addr:DWORD
        LOCAL Last_Rec:DWORD
        LOCAL Depth:UINT
        LOCAL Quick_Value:WORD
        LOCAL Chain:CHAIN_TYPE
        LOCAL lpEntry:LPBYTE
        LOCAL uEntrySizeDifference:UINT


        LDS_SI  Info

        xor     XAX,XAX
        mov     ax, [XSI].DBC4_HEADER.Elem_Size
        sub     ax, sFile_Protect
        mov     uEntrySizeDifference,XAX

; Get the hash value (CRC) of what we're looking for
        INVOKE  DBC4_HASH PTR [XSI].DBC4_HEADER.Hash_Func,LookFor
IF OS_16
        xor     dx, dx
        INVOKE  NUM4_MODULUS,dx::ax,[XSI].DBC4_HEADER.IndexSize
        add     ax,1
        adc     dx,0
        mov     word ptr Index_Addr+2,dx
        mov     word ptr Index_Addr,ax
ELSE
        INVOKE  NUM4_MODULUS,XAX,[XSI].DBC4_HEADER.IndexSize
        inc     eax
        mov     Index_Addr,eax
ENDIF

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Data_Addr,4

IF OS_16
        cmp     word ptr Data_Addr,0
        jnz     J_0
        cmp     word ptr Data_Addr+2,0
        jz      J_Error
ELSE
        cmp     Data_Addr,0
        jz      J_Error
ENDIF

J_0:
        mov     Depth,0
IF OS_16
        mov     word ptr Last_Rec,0
        mov     word ptr Last_Rec+2,0
ELSE
        mov     Last_Rec,0
ENDIF

DoWhile:
        INVOKE  Seek_to_data_record,Info,Data_Addr

        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,XAX

        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE

        INVOKE  DBC4_COMP PTR [XSI].DBC4_HEADER.Comp_Func,LookFor,Work
        or      ax,ax
        jnz     NextRecord


        cmp     Depth,0
        jnz     DepthGreaterThanZero

        INVOKE  Seek_to_index_entry,Info,Index_Addr

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Chain.Next,4

IF OS_16
        cmp     word ptr Chain.Next+2,0
        jnz     ChangeQuick
        cmp     word ptr Chain.Next,0
        jz      NoQuick
ELSE
        cmp     Chain.Next,0
        jz      NoQuick
ENDIF

ChangeQuick:

        INVOKE  Seek_to_data_record,Info,Chain.Next

        movuw   XAX,[XSI].DBC4_HEADER.Elem_Size
        INVOKE  FIL4_READ,[XSI].DBC4_HEADER.Filehandle,Work,XAX

IF OS_16
        mov     ax, word ptr Work
        mov     dx, word ptr Work+2
        add     ax, uEntrySizeDifference
        mov     word ptr lpEntry, ax
        mov     word ptr lpEntry+2, dx
ELSE
        mov     eax, Work
        add     eax, uEntrySizeDifference
        mov     lpEntry, eax
ENDIF
        INVOKE  DBC4_QUICK PTR [XSI].DBC4_HEADER.Quick_Func,lpEntry
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

IF OS_16
        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2
        mov     word ptr Last_Rec,ax
        mov     word ptr Last_Rec+2,dx

        mov     ax,word ptr Chain.Next
        mov     dx,word ptr Chain.Next+2
        mov     word ptr Data_Addr,ax
        mov     word ptr Data_Addr+2,dx
ELSE
        mov     eax,Data_Addr
        mov     Last_Rec,eax

        mov     eax,Chain.Next
        mov     Data_Addr,eax
ENDIF
        inc     Depth

IF OS_16
        cmp     word ptr Data_Addr,0
        jnz     DoWhile
        cmp     word ptr Data_Addr+2,0
        jnz     DoWhile
ELSE
        cmp     Data_Addr,0
        jnz     DoWhile
ENDIF
        jmp     J_Error

UpdateDeleted:
        mov     Chain.Active,FALSE

IF OS_16
        mov     ax,word ptr [XSI].DBC4_HEADER.Free_Pointer
        mov     dx,word ptr [XSI].DBC4_HEADER.Free_Pointer+2
        mov     word ptr Chain.Next,ax
        mov     word ptr Chain.Next+2,dx
ELSE
        mov     eax,[XSI].DBC4_HEADER.Free_Pointer
        mov     Chain.Next,eax
ENDIF

                                        ; Seek to data chain
        INVOKE  Seek_to_data_chain,Info,Data_Addr

        INVOKE  FIL4_WRITE,[XSI].DBC4_HEADER.Filehandle,ADDR Chain,sCHAIN_TYPE

IF OS_16
        mov     ax,word ptr Data_Addr
        mov     dx,word ptr Data_Addr+2
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer,ax
        mov     word ptr [XSI].DBC4_HEADER.Free_Pointer+2,dx
ELSE
        mov     eax,Data_Addr
        mov     [XSI].DBC4_HEADER.Free_Pointer,eax
ENDIF
        jmp     J_Success

J_Error:
        xor     XAX,XAX
        jmp     SHORT J_Done
J_Success:
        mov     XAX,TRUE
J_Done:
        ret

DBC4_DELETE  ENDP

UniqueSegmentEnd
END
